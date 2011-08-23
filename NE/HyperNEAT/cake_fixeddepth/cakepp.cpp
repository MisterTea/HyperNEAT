/*
*		cake - a checkers engine			
*															
*		Copyright (C) 2000...2007 by Martin Fierz	
*														
*		contact: checkers@fierz.ch				
*/


#include "switches.h"
#include <stdio.h>
#include <stdlib.h> // malloc() 
#include <string.h> // memset() 
#include <time.h>
#include <assert.h>
#include <algorithm>
using namespace std;

//#include <conio.h>
//#include <windows.h>

// cake-specific includes - structs defines structures, consts defines constants,
// xxx.h defines function prototypes for xxx.c
#include "structs.h"
#include "consts.h"
#include "cakepp.h"
#include "move_gen.h"
#include "dblookup.h"
#include "initcake.h"
#include "cake_misc.h"
#include "cake_eval.h"

#ifndef _MSC_VER
int max(int a,int b)
{
	if(a>b)
		return a;
	else
		return b;
}
#endif

//-------------------------------------------------------------------------------//
// globals below here are shared - even with these, cake *should* be thread-safe //
//-------------------------------------------------------------------------------//

static int iscapture[MAXDEPTH];		// tells whether move at realdepth was a capture

int hashmegabytes = 64;				// default hashtable size in MB if no value in registry is found
int dbmegabytes = 128;				// default db cache size in MB if no value in registry is found
int usethebook = BOOKALLKINDS;					// default: use best moves if no value in the registry is found

static HASHENTRY *hashtable;		// pointer to the hashtable, is allocated at startup
static HASHENTRY *book;				// pointer to the book hashtable, is allocated at startup

int hashsize = HASHSIZE;

#ifdef SPA
static SPA_ENTRY *spatable;
#endif


int maxNdb=0;						// the largest number of stones which is still in the database 


static int cakeisinit = 0;			// is set to 1 after cake is initialized, i.e. initcake has been called

// history table array
#ifdef MOHISTORY
int32 history[32][32];
#endif

// hashxors array.
int32 hashxors[2][4][32];			// this is initialized to constant hashxors stored in the code.

// bit tables for  number of set bits in word 
static unsigned char bitsinword[65536];
static unsigned char bitsinbyte[256];
static unsigned char LSBarray[256];

static int   norefresh;

int bookentries = 0; // number of entries in book hashtable
int bookmovenum = 0; // number of used entries in book

#ifdef HEURISTICDBSAVE
FILE *fp_9pc_black, *fp_9pc_white;
FILE *fp_10pc_black, *fp_10pc_white;
FILE *fp_11pc_black, *fp_11pc_white;
FILE *fp_12pc_black, *fp_12pc_white;
FILE *fp_9pc_blackK, *fp_9pc_whiteK;
FILE *fp_10pc_blackK, *fp_10pc_whiteK;
FILE *fp_11pc_blackK, *fp_11pc_whiteK;
FILE *fp_12pc_blackK, *fp_12pc_whiteK;
#endif

#ifdef THREADSAFEHT
CRITICAL_SECTION hash_access;		// is locked if a thread is accessing the hashtable
#endif
#ifdef THREADSAFEDB
CRITICAL_SECTION db_access;			// is locked if a thread is accessing the database
#endif


/*----------------------------------------------------------------------------*/
/*																			  */
/*                                initialization                              */
/*																			  */
/*----------------------------------------------------------------------------*/

int initcake(char str[1024])
//  initcake must be called before any calls to cake_getmove can be made.
//	it initializes the database and some constants, like the arrays for lastone
//	and bitsinword, plus some evaluation tables 
//	str is the buffer it writes output to
{
	char dirname[256];
	FILE *fp;

	// create a new logfile / delete old logfile 
	sprintf(str,"creating logfile");

	// delete old logfile
	fp = fopen("cakelog.txt","w");
	fclose(fp);

	strcpy(dirname,"./");
	//GetCurrentDirectory(256, dirname);
	logtofile(dirname);

	// do some loads. first, the endgame database.

#ifdef USEDB
	sprintf(str, "initializing database");
	maxNdb = db_init(dbmegabytes,str);
#endif // USEDB

#ifdef BOOK
	// next, load the book 
	sprintf(str,"loading book...");
	book = loadbook(&bookentries, &bookmovenum);
#endif

	// allocate hashtable
	sprintf(str,"allocating hashtable...");
	printf("Allocating hashtable...");
	hashtable = inithashtable(hashsize);
	printf("Done!\n");

	// initialize xors 
	initxors((int*)hashxors);

	// initialize bit-lookup-table 
	initbitoperations(bitsinword, LSBarray);

	initeval();


#ifdef SPA
	// allocate memory for SPA
	initspa();
#endif

#ifdef BOOKHT
	initbookht();
#endif

#ifdef THREADSAFEHT
	InitializeCriticalSection(&hash_access);
#endif
#ifdef THREADSAFEDB
	InitializeCriticalSection(&db_access);
#endif 

	cakeisinit=1;
	return 1;
}

void resetsearchinfo(SEARCHINFO *s)
{
	// resets the search info structure nearly completely, with the exception of
	// the pointer si.repcheck, to which we allocated memory during initialization - 
	// we don't want to create a memory leak here.
	s->aborttime = 0;
	s->allscores = 0;
	s->bk = 0;
	s->bm = 0;
	s->cutoffs = 0;
	s->cutoffsatfirst = 0;
	s->dblookup = 0;
	s->dblookupfail = 0;
	s->dblookupsuccess = 0;
	s->Gbestindex = 0;
	s->hash.key = 0;
	s->hash.lock = 0;
	s->hashlookup = 0;
	s->hashlookupsuccess = 0;
	s->hashstores = 0;
	s->iidnegamax = 0;
	s->leaf = 0;
	s->leafdepth = 0;
	s->matcount.bk = 0;
	s->matcount.bm = 0;
	s->matcount.wk = 0;
	s->matcount.wm = 0;
	s->maxdepth = 0;
	s->maxtime = 0;
	s->negamax = 0;
	s->out = NULL;
	s->play = NULL;
	s->qsearch = 0;
	s->qsearchfail = 0;
	s->qsearchsuccess = 0;
	s->realdepth = 0;
	s->searchmode = 0;
	s->spalookups = 0;
	s->spasuccess = 0;
	s->start = 0;
	s->wk = 0;
	s->wm = 0;
}




int hashreallocate(int x)
{
	// TODO: move to little-used functions file
	// reallocates the hashtable to x MB

	static int currenthashsize;

	HASHENTRY *pointer;
	int newsize;

	hashmegabytes = x;
	newsize = (hashmegabytes)*1024*1024/sizeof(HASHENTRY);	

	// TODO: this must be coerced to a power of 2. luckily this is true
	// with sizeof(HASHENTRY) == 8 bytes, but if i ever change that it will
	// fail

#ifdef WINMEM
	VirtualFree(hashtable, 0, MEM_RELEASE);
	pointer = VirtualAlloc(0, (newsize+HASHITER)*sizeof(HASHENTRY), MEM_COMMIT, PAGE_READWRITE);
#else
	pointer = (HASHENTRY*)realloc(hashtable,(newsize+HASHITER)*sizeof(HASHENTRY));
#endif
	if(pointer != NULL)
	{
		hashsize = newsize;
		hashtable = pointer;
	}
	// TODO: do something if pointer == NULLbut what do we do when pointer == 0 here??
	return 1;
}

int exitcake()
{
	// deallocate memory 
	//free(hashtable);
	db_exit();
	//free(book);
	return 1;
}


#ifdef BOOKGEN
int bookgen(OLDPOSITION *q, int color, int *numberofmoves, int values[MAXMOVES], int how, int depth, double searchtime)
{
	// this function analyzes a position, and returns the number of possible moves (*numberofmoves) and
	// an array with the evaluation of each move. to speed up things, it only looks at "sensible" values,
	// i.e. if a move is 120 points worse than the best move, it stops there and doesn't determine exactly
	// how bad it is. 
	// bookgen is only used by the book generator, not in testcake or checkerboard.
	int playnow=0;
	int d;
	int value=0,lastvalue=0,n,i,j, guess;
	CAKE_MOVE best,last, movelist[MAXMOVES];
	CAKE_MOVE mlarray[MAXMOVES][MAXMOVES];
	REPETITION dummy;
	char Lstr[1024],str[1024];
	int bookfound=1,bookbestvalue=-MATE;
	int bookequals=0;
	int bookdepth=0;
	int bookindex=-1;
	int reset=1;
	int forcedmove = 0; // indicates if a move is forced, then cake++ will play at depth 1.
	char pvstring[1024];
	char valuestring[256];
	char beststring[1024];
	int issearched[MAXMOVES];
	int bestvalue;
	int index;
	int bestnewvalue;
	double t;
	int zeroevals = 0;
	// since this is an alternate entry point to cake.dll, we need to have a searchinfo structure ready here
	SEARCHINFO si;

	POSITION p;

	// initialize module if necessary
	if(!cakeisinit) 
		initcake(str);

	// reset all counters, nodes, database lookups etc 
	resetsearchinfo(&si);

	// allocate memory for repcheck array
	si.repcheck = (REPETITION*)malloc((MAXDEPTH+HISTORYOFFSET) * sizeof(REPETITION));

	si.play = &playnow;
	si.out = str;
	si.aborttime = 40000*searchtime;
	si.maxtime = searchtime;
	si.searchmode = how;

	// change from old position struct to new one, because oplib is using the old one.
	p.bk = q->bk;
	p.bm = q->bm;
	p.wk = q->wk;
	p.wm = q->wm;
	p.color = color;

	// initialize material 
	countmaterial(&p, &(si.matcount));
	// initialize hash key 
	absolutehashkey(&p, &(si.hash));

#ifdef MOHISTORY
	// reset history table 
	memset(history,0,32*32*sizeof(int));
#endif

	printboard(&p);

	// clear the hashtable 
	memset(hashtable,0,(hashsize+HASHITER)*sizeof(HASHENTRY));

	norefresh=0;

	n = makecapturelist(&p, movelist, values, 0);
	if(!n)
		n = makemovelist(&si, &p, movelist, values, 0, 0);
#ifdef REPCHECK
	// initialize history list: holds the last few positions of the current game 
	si.repcheck[HISTORYOFFSET].hash = si.hash.key;
	si.repcheck[HISTORYOFFSET].irreversible = 1;

	dummy.hash = 0;
	dummy.irreversible = 1;

	for(i=0; i<HISTORYOFFSET; i++)  // was i<historyoffset + 8 - doesn't make much sense to me now
		si.repcheck[i] = dummy;
#endif

	si.start = clock();
	guess = 0;

	// initialize mlarray:
	for(i = 0; i<n; i++)
	{
		togglemove((&p), movelist[i]);
		getorderedmovelist(&p, &mlarray[i][0]);
		togglemove((&p), movelist[i]);
	}

	for(d=1; d<MAXDEPTH; d+=2)
	{
		si.leaf = 0;
		si.leafdepth = 0;
		value = -MATE;
		printf("\n");

		for(i=0;i<n;i++)
			issearched[i] = 0;

		bestnewvalue = -MATE;
		for(i=0;i<n;i++)
		{
			// get move with next best value to search, search moves from best to worst
			bestvalue = -MATE;
			for(j=0;j<n;j++)
			{
				if(issearched[j])
					continue;
				if(values[j]>bestvalue)
				{
					index = j;
					bestvalue=values[j];
				}
			}

			issearched[index]=1;

			togglemove((&p),movelist[index]);
			absolutehashkey((&p), &(si.hash));
			countmaterial((&p), &(si.matcount));

#ifndef MTD
			values[index]=-windowsearch(p,FRAC*(d-1), 0 /*guess*/, &best);
			// set value for next search 
			//guess=value[i];	
#endif
#ifdef MTD
			// MTD(F) search 
			values[index] = -bookmtdf(&si, &p, &mlarray[index][0], -values[index],FRAC*(d-1),&best,-bestnewvalue);
			bestnewvalue = max(bestnewvalue,values[index]);
#endif
			value = max(value, values[index]);
			togglemove((&p),movelist[index]);
			movetonotation(&p,&movelist[index],Lstr);
			printf("[%s %i] ",Lstr,values[index]);
			if(value == values[index])
				sprintf(beststring,"%s",Lstr);
		}

		// zeroevals: count how many times we have seen 0, and stop the analysis early if we see too many.
		if(bestnewvalue == 0)
			zeroevals++;
		else
			zeroevals = 0;

		// generate output 
		sprintf(Lstr,"%s",beststring);
		getpv(&si, &p, pvstring);

		// check for clock tick overflow
		// this should not happen - reset the start time
		t = clock();
		if(t-si.start < 0)
			si.start = clock(); 

		sprintf(valuestring, "value=%i", value);
		//searchinfotostring(str, d*FRAC, (t-si.start)/CLK_TCK, valuestring, pvstring, &si);
		//printf("\n%s",str);	
		// iterative deepening loop break conditions: depend on search mode, 'how':
		//	how=0: time mode
		if(d>1)
		{
			//if( si.searchmode == TIME_BASED && ((clock() - si.start)/CLK_TCK>(si.maxtime/2)) ) 
			//break;

			// early break in case of many zeros and enough search depth
			//if( si.searchmode == TIME_BASED && ((clock()- si.start)/CLK_TCK>(si.maxtime/8)) && (zeroevals>4) && (d>22)) 
			//{
			//printf("\n* early break");
			//break;
			//}

			if( si.searchmode == DEPTH_BASED && d>=depth)
				break;

			if(abs(value)>MATE-100) break;
		}

		lastvalue=value;	// save the value for this iteration 
		last=best;			// save the best move on this iteration 
		norefresh=1;
	}

	free(si.repcheck);

	return value;
}


int bookmtdf(SEARCHINFO *si, POSITION *p, CAKE_MOVE movelist[MAXMOVES], int firstguess,int depth, CAKE_MOVE *best, int bestvalue)
{
	int g,lowerbound, upperbound,beta;
	double time;
	char Lstr1[1024],Lstr2[1024];

#define CUTOFF 120

	upperbound=MATE;
	lowerbound=-MATE;

	g=firstguess;
	g=0;			// a bit weird, firstguess should be better, but 0 seems to help

	while(lowerbound<upperbound)
	{
		if(g==lowerbound)
			beta=g+1;
		else beta=g;
		g = firstnegamax(si, p, movelist, depth, beta-1, beta, best);

		if(g<beta)
		{
			upperbound=g;
			sprintf(Lstr1,"value<%i",beta);
		}
		else
		{
			lowerbound=g;
			sprintf(Lstr1,"value>%i",beta-1);
			// termination if this node is much worse than the best so far:
			if(g - CUTOFF > bestvalue)
				return g;
		}

		time = 1;//( (clock()- si->start)/CLK_TCK);
		getpv(si, p, Lstr2);

		sprintf(si->out,"depth %i/%i/%.1f  time %.2fs  %s  nodes %i  %ikN/s  %s", 
			(depth/FRAC),si->maxdepth,(float)si->leafdepth/((float)si->leaf+0.001) , time, Lstr1, 
			si->negamax, (int)((float)si->negamax/time/1000),Lstr2); 
#ifdef FULLLOG
		logtofile(si.out);
#endif		
	}
	logtofile(si->out);

	return g;
}

#endif //bookgen

int analyze(POSITION *p, int color, int how, int depth, double searchtime, int *stat_eval)
{
	// is a direct-access function for external analysis
	int playnow=0;
	int d;
	int value=0,lastvalue=0, guess;
	int dummy;
	CAKE_MOVE best,last;
	char Lstr[1024],str[1024];
	int staticeval;
	// si for the analyze function so that it can reset HT after X runs
	static int analyzedpositions; 
	double t;
	// analyze is an entry point to cake, so we need a searchinfo here
	SEARCHINFO si;
	CAKE_MOVE movelist[MAXMOVES];

	d = getorderedmovelist(p, movelist);

	resetsearchinfo(&si);

	// allocate memory for repcheck array
	si.repcheck = (REPETITION*)malloc((MAXDEPTH+HISTORYOFFSET) * sizeof(REPETITION));

	si.play = &playnow;
	si.out = str;
	analyzedpositions++;

	// set time limits
	si.aborttime = 40000*searchtime;
	si.maxtime = searchtime;
	si.searchmode = how;

	// initialize module if necessary
	if(!cakeisinit) 
		initcake(str);


#ifdef MOHISTORY
	// reset history table 
	memset(history,0,32*32*sizeof(int));
#endif


	// clear the hashtable 
	// no - not in this mode
	// or more precisely, nearly never. reason: after a long time, the hashtable is
	// filled with positions which have a ispvnode attribute.

	//if((analyzedpositions % 100000) == 0)
	//	{
	printf("\nresetting hashtable");
	memset(hashtable,0,(hashsize+HASHITER)*sizeof(HASHENTRY));
	//	}

	norefresh=0;

	// initialize hash key 
	absolutehashkey(p, &(si.hash));
	countmaterial(p, &(si.matcount));

	si.start = clock();
	guess=0;

	staticeval = evaluation(p,&(si.matcount),0,&dummy,0,maxNdb);

	// set return value of static eval.
	*stat_eval = staticeval;
	//printf("\nstatic eval %i",staticeval);

	for(d=1;d<MAXDEPTH;d+=2)
	{
		si.leaf  = 0;
		si.leafdepth = 0;

		// choose which search algorithm to use: 

		// non - windowed search 
		// value = firstnegamax(FRAC*d, -MATE, MATE, &best);


#ifndef MTD
		// windowed search
		value = windowsearch(&si, p, movelist, FRAC*d, guess, &best);
		// set value for next search 
		guess=value;	
#endif

#ifdef MTD
		// MTD(F) search 
		value = mtdf(&si, p, movelist, guess, FRAC*d, &best);
		guess = value;	
#endif

		// generate output 
		movetonotation(p,&best,Lstr);
		t = clock();

		// iterative deepening loop break conditions: depend on search mode, 'how':
		//	how=0: time mode;
		if(d>1)
		{
			//if( si.searchmode == TIME_BASED && ((clock()- si.start)/CLK_TCK>(si.maxtime/2)) ) 
			//break;
			if( si.searchmode == DEPTH_BASED && d>=depth)
				break;
			if(abs(value)>MATE-100) break;
		}

#ifdef PLAYNOW
		if( *(si.play))
		{
			// the search has been aborted, either by the user or by cake++ because 
			// abort time limit was exceeded
			// stop the search. don't use the best move & value because they might be rubbish 
			best=last;
			movetonotation(p,&best,Lstr);
			value=lastvalue;
			sprintf(str,"interrupt: best %s value %i",Lstr,value);
			break;
		}
#endif

		lastvalue=value;	// save the value for this iteration 
		last=best;			// save the best move on this iteration 
		norefresh=1;
	}

	free(si.repcheck);
	return value;
}


int cake_getmove(SEARCHINFO *si, POSITION *p, int how,double maximaltime,
				 int depthtosearch, int32 maxnodes,
				 char str[1024], int *playnow, int log, int info)
{

	/*----------------------------------------------------------------------------*/
	/*
	/*		cake_getmove is the entry point to cake++
	/*		give a pointer to a position and you get the new position in
	/*		this structure after cake++ has calculated.
	/*		color is BLACK or WHITE and is the side to move.
	/*		how is TIME_BASED for time-based search and DEPTH_BASED for depth-based search and 
	/*		NODE_BASED for node-based search
	/*		maximaltime and depthtosearch and maxnodes are used for these three search modes.
	/*		cake++ prints information in str
	/*		if playnow is set to a value != 0 cake++ aborts the search.
	/*		if (log&1) cake will write information into "log.txt"
	/*		if(log&2) cake will also print the information to stdout.
	/*		if reset!=0 cake++ will reset hashtables and repetition checklist
	/*		reset==0 generally means that the normal course of the game was disturbed
	// info currently has uses for it's first 4 bits:
	// info&1 means reset
	// info&2 means exact time level
	// info&4 means increment time level
	// info&8 means allscore search
	/*
	/*----------------------------------------------------------------------------*/

	int d;
	int value=0,lastvalue=0,n,i, guess;
	CAKE_MOVE best,last;
	REPETITION dummy;
	char Lstr[1024];
	int bookfound=1,bookbestvalue=-MATE;
	CAKE_MOVE bookmove;
	int bookequals=0;
	unsigned int bookdepth=0;
	int bookindex=-1,booklookupvalue;
	int values[MAXMOVES];
	int reset=(info&1);
	int forcedmove = 0; // indicates if a move is forced, then cake++ will play at depth 1.
	int zeroes = 0;		// number of zero evals returned
	int winslosses = 0;
	double t;
	char pvstring[1024];
	CAKE_MOVE movelist[MAXMOVES];	// we make a movelist here which we pass on, so that it can be ordered
	// and remain so during iterative deeping.


	//	int dolearn;

	// initialize module if necessary
	if(!cakeisinit) 
		initcake(str);

	resetsearchinfo(si);

	for(i=0;i<MAXDEPTH;i++)
		iscapture[i] = 0;


	*playnow = 0;
	si->play = playnow;
	si->out = str;
	si->searchmode = how;

	// set time limits
	si->maxtime = maximaltime;
	si->aborttime = 4*maximaltime;

	// if exact:
	if(info&2)
		si->aborttime = maximaltime;

	// allscores:
	if(info&8)
		si->allscores = 1;

	//printboardtofile(p,NULL);

	// print current directory to see whether CB is getting confused at some point.
	strcpy(pvstring,"./");
	//GetCurrentDirectory(256, pvstring);

	// initialize material 
	countmaterial(p, &(si->matcount));

#ifdef MOHISTORY
	// reset history table 
	fflush(stdout);
	memset(history,0,32*32*sizeof(int));
#endif

	// clear the hashtable 
	fflush(stdout);
	memset(hashtable,0,(hashsize+HASHITER)*sizeof(HASHENTRY));

	// initialize hash key 
	absolutehashkey(p, &(si->hash));

	// if LEARNUSE is defined, we stuff learned positions in the hashtable
#ifdef LEARNUSE
	stufflearnpositions();
#endif

	// what is this doing at all?
	norefresh=0;

	// what is this doing here?
	n = makecapturelist(p, movelist, values, 0);

#ifdef REPCHECK
	// initialize history list: holds the last few positions of the current game 

	si->repcheck[HISTORYOFFSET].hash = si->hash.key;
	si->repcheck[HISTORYOFFSET].irreversible = 1;

	dummy.hash = 0;
	dummy.irreversible = 1;

	if(reset==0)
	{
		for(i=0;i<HISTORYOFFSET-2;i++)
			si->repcheck[i] = si->repcheck[i+2];
	}
	else
	{
		for(i=0;i<HISTORYOFFSET;i++)
			si->repcheck[i] = dummy;
	}
	//repetitiondraw = 0;
#endif

#ifdef TESTPERF
	for(i=0;i<MAXMOVES;i++)
		cutoffsat[i]=0;
#endif


	// do a book lookup TODO: put this all in a small function
	if(usethebook)
	{
		bookfound=0;
		bookindex=0;

		if(booklookup(p,&booklookupvalue,0,&bookdepth,&bookindex,str))
		{
			// booklookup was successful, it sets bookindex to the index of the move in movelist that it wants to play
			bookfound = 1;
			n = makecapturelist(p, movelist, values, 0);
			if(!n)
				n = makemovelist(si, p,movelist,values,0,0);
			// set best value
			bookmove = movelist[bookindex];
		}

		// if the remaining depth is too small, we dont use the book move 
		if(bookdepth < BOOKMINDEPTH) 
			bookfound = 0;
		if(bookfound)
		{
			movetonotation(p,&bookmove,Lstr);
			//sprintf(str,"found position in book, value=%i, move is %s (depth %i)\n",booklookupvalue,Lstr,bookdepth);
			best=bookmove;
			value=0;
		}
		else
		{
			sprintf(str,"%X %X not found in book\n",si->hash.key,si->hash.lock);
			value=0;
		}
		logtofile(str);
	}
	else
		bookfound=0;

	absolutehashkey(p, &(si->hash));
	countmaterial(p, &(si->matcount));

	// check if the move on the board is forced - if yes, we don't waste time on it.
	forcedmove = isforced(p);

#ifdef SOLVE
	solve(p, color);
	return 0;
#endif

	// get a movelist which we pass around to mtdf() and firstnegamax() - do this here
	// because we want to keep it ordered during iterative deepening.
	n = getorderedmovelist(p, movelist);


	si->start = clock();
	guess=0;


	if(!bookfound)
	{
		// check if this version is set to search to a fixed depth
		for(d=1; d<MAXDEPTH; d++)
		{
			si->leaf=0;
			si->leafdepth=0;

#ifndef MTD
			// windowed search
			value = windowsearch(si, p, movelist, FRAC*d, guess, &best);
			// set value for next search 
			guess=value;	
#endif
#ifdef MTD
			// MTD(F) search 
			if(si->allscores == 0)
			{
				value = mtdf(si, p, movelist, guess, FRAC*d, &best);
				guess = value;	
			}
			else
				value = allscoresearch(si, p, movelist, FRAC*d, &best);
#endif

			// count zero evals
			if(value == 0)
				zeroes++;
			else 
				zeroes = 0;

			// count winning evals
			// do not count evals > 400 (db wins) as this would confuse cake when it's in a db win!
			if(abs(value)>=100 && abs(value)<400)
				winslosses++;
			else
				winslosses=0;

			// generate output 
			movetonotation(p,&best,Lstr);
			getpv(si, p, pvstring);
			t = clock();

			// iterative deepening loop break conditions: depend on search mode
			if(d>1)
			{
				//if( si->searchmode == TIME_BASED && ((clock()- si->start)/CLK_TCK>(si->maxtime/2)) ) 
				//break;

				if( si->searchmode == DEPTH_BASED && d>=depthtosearch)
					break;

				if(si->searchmode == NODE_BASED && si->negamax>maxnodes)
					break;
#ifdef IMMEDIATERETURNONFORCED
				// do not search if only one move is possible 
				if(forcedmove) // this move was forced! 
				{
					// set return value to 0 so no win/loss claims hamper engine matches*/
					value=0;
					break;
				}
#endif
				if(abs(value)>MATE-100) 
					break;
			}
#ifdef PLAYNOW
			if(*(si->play))
			{
				// the search has been aborted, either by the user or by cake++ because 
				// abort time limit was exceeded
				// stop the search. don't use the best move & value because they might be rubbish 
				best=last;
				movetonotation(p,&best,Lstr);
				value=lastvalue;
				sprintf(str,"interrupt: best %s value %i",Lstr,value);
				break;
			}
#endif

			lastvalue=value;	// save the value for this iteration 
			last=best;			// save the best move on this iteration 
			norefresh=1;
		}
	}

#ifdef REPCHECK
	si->repcheck[HISTORYOFFSET-2].hash = si->hash.key;
	si->repcheck[HISTORYOFFSET-2].irreversible = 0;
#endif



	if(*(si->play))
		best = last;

	togglemove(p,best);

	absolutehashkey(p, &(si->hash));

#ifdef REPCHECK
	si->repcheck[HISTORYOFFSET-1].hash = si->hash.key;
	si->repcheck[HISTORYOFFSET-1].irreversible = (best.bm | best.wm);
#endif

	// return value: WIN / LOSS / DRAW / UNKNOWN

	if(value > WINLEVEL)
		return WIN;
	if(value < -WINLEVEL)
		return LOSS;

#ifdef USEDB
	if(isdbpos(p, &(si->matcount)))
	{

		value = dblookup(p, 0);

		if(value == DB_DRAW)
			return DRAW;
	}
#endif

	// TODO: find repetition draws and return DRAW too.

	return UNKNOWN;
}


#ifdef USEDB
int isdbpos(POSITION *p, MATERIALCOUNT *m)
{
	if(m->bm + m->bk + m->wm + m->wk <= maxNdb && max(m->bm + m->bk, m->wm + m->wk) <= MAXPIECE)
	{
		if(testcapture(p))
			return 0;
		p->color ^= CC;
		if(testcapture(p))
		{
			p->color^=CC;
			return 0;
		}
		p->color ^= CC;
		return 1;
	}
	return 0;
}
#endif





/*--------------------------------------------------------------------------
|																			|
|		driver routines for search: MTD(f) and windowsearch					|
|																			|
--------------------------------------------------------------------------*/

int mtdf(SEARCHINFO *si, POSITION *p, CAKE_MOVE movelist[MAXMOVES], int firstguess,int depth, CAKE_MOVE *best)
{
	// todo: test "correct return values", 
	// also look if the best move is always the same as the last fail-high - 
	// perhaps that's what mtdf should return: the fail-high move with the highest
	// value?!
	int g, lowerbound, upperbound, beta;
	double time;
	char Lstr1[1024]="",Lstr2[1024]="";

	g = firstguess;
	/*g=0; */ /* strange - this seems to help?! */
	upperbound = MATE;
	lowerbound = -MATE;
	while(lowerbound<upperbound)
	{
		if(g==lowerbound)
			beta=g+1;
		else beta=g;
		g = firstnegamax(si, p, movelist, depth, beta-1, beta, best);

		if(g<beta)
		{
			upperbound=g;
			//upperbound = beta-1;
			sprintf(Lstr1,"value<%i",beta);
		}
		else
		{
			lowerbound=g;
			//lowerbound = beta;
			sprintf(Lstr1,"value>%i",beta-1);
		}

		time = 1;//( (clock() - si->start)/CLK_TCK);
		getpv(si, p, Lstr2);

		searchinfotostring(si->out, depth, time, Lstr1, Lstr2, si);
#ifdef FULLLOG
		sprintf(Lstr,"\n -> %s",si.out);
		logtofile(Lstr);
#endif	
	}
	// for testing:
	//g = lowerbound;
	sprintf(Lstr1,"value=%i",g);
	searchinfotostring(si->out, depth, time, Lstr1, Lstr2, si);

	logtofile(si->out);
	return g;
}

int windowsearch(SEARCHINFO *si, POSITION *p, CAKE_MOVE movelist[MAXMOVES], int depth, int guess, CAKE_MOVE *best)
{
	int value;
	double time;
	char Lstr1[1024],Lstr2[1024];

	//do a search with aspiration window

	value = firstnegamax(si, p, movelist, depth, guess-ASPIRATIONWINDOW, guess+ASPIRATIONWINDOW, best);

	if(value >= guess+ASPIRATIONWINDOW)
	{
		// print info in status bar and cakelog.txt
		sprintf(Lstr1,"value>%i",value-1);
		time = 1;//( (clock()- si->start)/CLK_TCK);
		getpv(si, p, Lstr2);
		searchinfotostring(si->out, depth, time, Lstr1, Lstr2, si);
		logtofile(si->out);

		value = firstnegamax(si, p, movelist, depth, guess, MATE, best);

		if(value <= guess)
		{
			// print info in status bar and cakelog.txt
			sprintf(Lstr1,"value<%i",value+1);
			time = 1;//( (clock()- si->start)/CLK_TCK);
			getpv(si, p, Lstr2);
			searchinfotostring(si->out, depth, time, Lstr1, Lstr2, si);
			logtofile(si->out);

			value = firstnegamax(si, p, movelist, depth, -MATE, MATE, best);
		}
	}

	if(value <= guess-ASPIRATIONWINDOW)
	{
		// print info in status bar and cakelog.txt
		sprintf(Lstr1,"value<%i",value+1);
		time = 1;//( (clock()- si->start)/CLK_TCK);
		getpv(si, p, Lstr2);
		searchinfotostring(si->out, depth, time, Lstr1, Lstr2, si);
		logtofile(si->out);

		value = firstnegamax(si, p, movelist, depth, -MATE, guess, best);

		if(value >= guess)
		{
			// print info in status bar and cakelog.txt
			sprintf(Lstr1,"value>%i",value-1);
			time = 1;//( (clock()- si->start)/CLK_TCK);
			getpv(si, p, Lstr2);
			searchinfotostring(si->out, depth, time, Lstr1, Lstr2, si);
			logtofile(si->out);

			value = firstnegamax(si, p, movelist, depth, -MATE, MATE, best);
		}
	}
	sprintf(Lstr1,"value=%i",value);
	time = 1;//( (clock()- si->start)/CLK_TCK);
	getpv(si, p, Lstr2);
	searchinfotostring(si->out, depth, time, Lstr1, Lstr2, si);
	logtofile(si->out);

	return value;
}


int allscoresearch(SEARCHINFO *si, POSITION *p, CAKE_MOVE movelist[MAXMOVES], int d, CAKE_MOVE *best)
{
	int i, j, n;
	static int values[MAXMOVES];
	int bestindex = 0;
	char str[256], tmpstr[256], movestr[256];
	int Lkiller;
	int tmpvalue;
	CAKE_MOVE tmpmove;
	MATERIALCOUNT Lmatcount;
	HASH localhash;


#ifdef PLAYNOW
	if(*(si->play)) return 0;
#endif

	// get number of moves in movelist
	n = numberofmoves(movelist);

	// save old hashkey 
	localhash = si->hash;

	// save old material balance 
	Lmatcount = si->matcount;

	*best = movelist[0];

	for(i=0;i<n;i++)
	{
		// domove
		togglemove(p,movelist[i]);
		countmaterial(p, &(si->matcount));
		si->realdepth++;
		updatehashkey(&movelist[i], &(si->hash));

#ifdef REPCHECK
		si->repcheck[si->realdepth + HISTORYOFFSET].hash = si->hash.key;
		si->repcheck[si->realdepth + HISTORYOFFSET].irreversible = movelist[i].bm|movelist[i].wm;
#endif

		/********************recursion********************/

		values[i] = -negamax(si,p,d-FRAC,0, &Lkiller, &Lkiller, 0,0,0);

		/******************* end recursion ***************/

		// undomove
		si->realdepth--;
		togglemove(p,movelist[i]);
		si->hash = localhash;
		si->matcount = Lmatcount;

		// create output string
		sprintf(str,"d%i  ",d/FRAC);
		for(j=0;j<n;j++)		
		{
			if(j<=i)
			{
				movetonotation(p, &movelist[j], movestr);
				sprintf(tmpstr, "%s: %i   ", movestr, values[j]);
				strcat(str, tmpstr);
			}
			else
			{
				movetonotation(p, &movelist[j], movestr);
				sprintf(tmpstr, "%s:(%i)  ", movestr, values[j]);
				strcat(str, tmpstr);
			}
		}
		sprintf(si->out,"%s", str);
	}

	logtofile(str);

	// now, order the list according to values
	for(j=0;j<n;j++)
	{
		for(i=0;i<n-1;i++)
		{
			if(values[i] < values[i+1])
			{
				// swap
				tmpvalue = values[i];
				values[i] = values[i+1];
				values[i+1] = tmpvalue;
				tmpmove = movelist[i];
				movelist[i] = movelist[i+1];
				movelist[i+1] = tmpmove;
			}
		}
	}

	*best = movelist[0];

	//	last = *p;
	return 1;
}


int firstnegamax(SEARCHINFO *si, POSITION *p, CAKE_MOVE movelist[MAXMOVES], int d, int alpha, int beta, CAKE_MOVE *best)
{

	/*--------------------------------------------------------------------------
	|																			|
	|		firstnegamax: first instance of negamax which returns a move		|
	|																			|
	--------------------------------------------------------------------------*/

	int i,value,swap=0,bestvalue=-MATE;
	static int n;
	static CAKE_MOVE ml2[MAXMOVES];
	MATERIALCOUNT Lmatcount;
	int Lalpha=alpha;
	int forcefirst=0;
	int Lkiller=0;
	static POSITION last;
	CAKE_MOVE tmpmove;
	static int values[MAXMOVES];	/* holds the values of the respective moves - use to order */
	int refnodes[MAXMOVES];			/* number of nodes searched to refute a move */
	int statvalues[MAXMOVES];
	int tmpnodes;
	int bestindex=0;
	HASH localhash;

#ifdef PLAYNOW
	if(*(si->play)) return 0;
#endif
	si->negamax++;

	// TODO: will static position last work here? and static movelist??
	// probably i need to make a movelist in mtdf and send it to firstnegamax!

	for(i=0;i<MAXMOVES;i++)
		refnodes[i] = 0;

	// get number of moves in movelist
	n = numberofmoves(movelist);

	if(n==0)
		return -MATE+si->realdepth;

	//save old hashkey 
	localhash = si->hash;

	// save old material balance 
	Lmatcount = si->matcount;
	*best = movelist[0];

	// For all moves do...
	for(i=0;i<n;i++)
	{
		togglemove(p,movelist[i]);
		countmaterial(p, &(si->matcount));
		si->realdepth++;
		updatehashkey(&movelist[i], &(si->hash));
#ifdef REPCHECK
		si->repcheck[si->realdepth + HISTORYOFFSET].hash = si->hash.key;
		si->repcheck[si->realdepth + HISTORYOFFSET].irreversible = movelist[i].bm|movelist[i].wm;
#endif
		tmpnodes = si->negamax;
		/********************recursion********************/

		value = -negamax(si, p,d-FRAC,-beta, &Lkiller, &forcefirst, 0,0,0);

		/******************* end recursion ***************/
		refnodes[i] = si->negamax-tmpnodes;
		values[i] = value;
		si->realdepth--;
		togglemove(p,movelist[i]);
		// restore the old hash key
		si->hash = localhash;
		// restore the old material balance 
		si->matcount = Lmatcount;
		bestvalue = max(bestvalue,value);

		if(value >= beta) 
		{
			*best = movelist[i];/*Lalpha=value;*/
			swap=i;
			break;
		}
		if(value > Lalpha) 
		{
			*best=movelist[i];
			Lalpha=value;
			swap=i;
		}
	}
	/* save the position in the hashtable */
	/* a bit complicated because we need to save best move as index */
	// what if we don't set forcefirst here, i.e. set it to 0?
	n = makecapturelist(p, ml2, statvalues, 0);

	iscapture[si->realdepth] = n;

	if(n==0)
		n = makemovelist(si, p, ml2, statvalues, 0,0);

	// search the best move
	for(i=0; i<n; i++)
	{
		if(ml2[i].bm==best->bm && ml2[i].bk==best->bk && ml2[i].wm==best->wm && ml2[i].wk==best->wk)
			break;
	}
	bestindex = i;


	hashstore(si, p, bestvalue, alpha, d, best, bestindex);

	// set global Gbestindex for learn function
	si->Gbestindex = bestindex;

	// order movelist according to the number of nodes it took
	if(swap!=0)
	{
		tmpmove = movelist[swap];
		tmpnodes = refnodes[swap];
		for(i=swap;i>0;i--)
		{
			movelist[i] = movelist[i-1];
			refnodes[i] = refnodes[i-1];
		}
		movelist[0] = tmpmove;
		refnodes[0] = tmpnodes;
	}
	last=*p;
	return bestvalue;
}




int negamax(SEARCHINFO *si, POSITION *p, int d, int alpha, int *protokiller, int *bestmoveindex, int truncationdepth, int truncationdepth2, int iid)
/*----------------------------------------------------------------------------
|																			  |
|		negamax: the basic recursion routine of cake++						  |
|					returns the negamax value of the current position		  |
|					sets protokiller to a compressed version of a move,		  |
|					if it's black's turn, protokiller = best.bm|best.bk		  |
|					*bestindex is set to the best move index, and is only     |
|					used for IID: there, it returns the best index, and then  |
|					we continue setting forcefirst to that index.             |
----------------------------------------------------------------------------*/
{
	int value;
	int valuetype;
	int hashfound;
	int forcefirst = MAXMOVES-1;
	int n, localeval = MATE;
	int maxvalue = -MATE;
	int delta=0;
#ifdef ETC
	int bestETCvalue;
#endif
	int i,j;
	int index, bestmovevalue;
	int bestindex;
	int Lkiller=0;
	int stmcapture = 0;
	int sntmcapture = 0;
#ifdef USEDB
	int dbresult;
#endif
	int isdbpos = 0;
	int cl = 0;				// conditional lookup - predefined as no. if CLDEPTH>d then it's 1.
	int ispvnode = 0;		// is set to 1 in hashlookup if this position was part of a previous pv.

#ifdef SAFE
	int safemovenum = 0;
#endif

#ifdef STOREEXACTDEPTH
	int originaldepth = d;
#endif
	HASH localhash;
	POSITION q;				// board for copy/make 
	MATERIALCOUNT Lmatcount;
	CAKE_MOVE movelist[MAXMOVES];
	int values[MAXMOVES];



	/* 
	* negamax does the following:
	* 1) abort search if time is elapsed
	* 2) return MATE value if one side has no material
	* 3) abort search if playnow is true
	* 4) return with evaluation if MAXDEPTH is reached (this practically never happens)
	* 5) hashlookup and return if successful
	* 6) IID if hashlookup unsuccessful and remaining depth sufficient
	* 7) check for repetition, return 0 if repetition
	* 8) get capture status of current position (stmcapture, sntmcapture)
	* 9) dblookup if no capture and stones < maxNdb
	* 10) extension and pruning decisions
	* 11) compute # of safe moves in position
	* 12) if depth small enough, no capture, and enough safe moves: return evaluation
	* 13) makemovelist & SINGLEEXTEND if n==1.
	* 14) ETClookup
	* 15) for all moves: domove, recursion, undomove
	* 16) hashstore
	* 17) return value
	*/

	si->negamax++;

	// check material:
	if(p->bm + p->bk == 0)
		return (p->color == BLACK) ? (-MATE+si->realdepth):(MATE-si->realdepth);
	if(p->wm + p->wk == 0)
		return (p->color == WHITE) ? (-MATE+si->realdepth):(MATE-si->realdepth); 

	// return if calculation interrupt is requested
#ifdef PLAYNOW
	if(*(si->play) != 0) 
		return 0;
#endif
	// stop search if maximal search depth is reached - this should basically never happen
	if(si->realdepth > MAXDEPTH) 
	{
		si->leaf++;
		si->leafdepth += si->realdepth;
		return evaluation(p,&(si->matcount),alpha,&delta,0,maxNdb);
	}

	//------------------------------------------------//
	// search the current position in the hashtable   //
	// only if there is still search depth left!      //
	// should test this without d>0 !?                //
	//------------------------------------------------//


	// TODO: check dblookup first!

	if(d >= 0 && !iid)
	{
		si->hashlookup++;

		hashfound = hashlookup(si, &value, &valuetype, d, &forcefirst, p->color, &ispvnode);

		if(hashfound)
		{
			si->hashlookupsuccess++;
			// return value 1: the position is in the hashtable and the depth
			// is sufficient. it's value and valuetype are used to shift bounds or even cutoff 
			if(valuetype == LOWER)
			{
				//if(value >= beta) 
				if(value > alpha) 
					return value;
			}
			else	//if(valuetype==UPPER)		
			{
				if(value <= alpha) 
					return value;
			}
		}
	}




#ifdef REPCHECK
	// check for repetitions 
	// TODO: maybe add repcheck[i-1].irreversible to the break statement.
	if(p->bk && p->wk)
	{
		for(i = si->realdepth + HISTORYOFFSET-2; i >= 0; i-=2)
		{
			// stop repetition search if move with a man is detected 
			// this could be expanded with ....[i+-1]
			if(si->repcheck[i].irreversible)
				break;

			if(si->repcheck[i].hash == si->hash.key)			
				return 0;                      
		}
	}
#endif

	// get info on captures: can side to move or side not to move capture now?
	stmcapture = testcapture(p);

	// get info on capture by opponent (if there is a capture on the board, then the database
	// contains no valid data
	// TODO: do we ever use sntmcapture again? because if no, we could calculate this only
	// if we have a dblookup....
	if(!stmcapture)
	{
		p->color ^= CC;
		sntmcapture = testcapture(p);
		p->color ^= CC;
	}

	if(stmcapture)
		n = makecapturelist(p,movelist,values,forcefirst);
	else
		n = 0;

	iscapture[si->realdepth] = n;

	//--------------------------------------------//
	// check for database use                     // 
	// conditions: -> #of men < maxNdb            //
	//			   -> no capture for either side  //
	//                else result is incorrect    //
	//--------------------------------------------//
#ifdef USEDB
	if(si->matcount.bm + si->matcount.bk + si->matcount.wm + si->matcount.wk <= maxNdb && max(si->matcount.bm + si->matcount.bk, si->matcount.wm + si->matcount.wk) <= MAXPIECE)
	{
		isdbpos = 1;
		if(!(stmcapture|sntmcapture))
		{
			// this position can be looked up!
			si->dblookup++;

			if(d<CLDEPTH*FRAC && !ispvnode)
				cl=1;


			dbresult = dblookup(p,cl);


			// statistics
			if(dbresult == DB_NOT_LOOKED_UP)
				si->dblookupfail++;
			else if(dbresult != DB_UNKNOWN)
			{
				si->dblookupsuccess++;			

				if(dbresult == DB_DRAW)
					return 0;
				if(dbresult == DB_WIN)
				{
					// TODO: we only really need dbwineval  if the initial position
					// is a dbwin/dbloss. i.e. we could set value to + something big, and only if
					// value < localbeta we would get dbwineval.
					// TODO: try the following two lines once
					//if( lbeta < 400)
					//	return 400;
					value = dbwineval(p, &(si->matcount));

					//if(value >= localbeta)
					if(value > alpha)
						return value;
					// TODO check on this: does it ever happen?
					localeval = value;
					//printf("!");
				}
				if(dbresult == DB_LOSS)
				{
					//if (alpha > -400)
					//	return -400;

					p->color ^= CC;
					value = -dbwineval(p, &(si->matcount));
					p->color ^= CC;

					if(value <= alpha)
						return value;
					localeval = value;
				}
			}
		}
	}
#endif // USEDB


	//---------------------------------------------------//
	// check if we can return the evaluation             //
	// depth must be <=0, and sidetomove has no capture  //
	//---------------------------------------------------//

	if(d<=0 && !stmcapture) 
	{
		// depth <= 0 and no capture, return value		
		{
			// first, do some statistics:
			if(si->realdepth > si->maxdepth) 
				si->maxdepth = si->realdepth;
			si->leaf++;
			si->leafdepth += si->realdepth;

			// localeval is initialized to MATE; if we already computed it, we don't have to again:
			if(localeval != MATE)
				value = localeval;
			else
				value = evaluation(p,&(si->matcount),alpha,&delta,0,maxNdb);

			return value;
		}
	}

	// we already made a capture list - if we have moves from there, we don't need to make a movelist.
	if(!n)
		n = makemovelist(si, p, movelist, values, forcefirst, *protokiller);
	if(n==0)  // this can happen if side to move is stalemated.
		return -MATE+si->realdepth;


	// save old hashkey and old material balance
	localhash = si->hash;
	Lmatcount = si->matcount;

	// for all moves: domove, update hashkey&material, recursion, restore
	//	material balance and hashkey, undomove, do alphabetatest
	// set best move in case of only fail-lows 
	for(i=0;i<n;i++)
	{
		index=0;
		bestmovevalue=-1;

		for(j=0; j<n; j++)
		{
			if(values[j]>bestmovevalue)
			{
				bestmovevalue = values[j];
				index = j;
			}
		}
		// TODO: think about swapping: put move at position n-1-i to position index
		// movelist[index] now holds the best move 
		// set values[index] to -1, that means that this move will no longer be considered 
		values[index]=-1;

		// domove 
		domove(q,p,movelist[index]);
		q.color = p->color^CC;

		// if we had no hashmove, we have to set bestindex on the first iteration of this loop
		if(i == 0)		
			bestindex = index;

		// inline material count 
		if(p->color == BLACK)
		{
			if(stmcapture)
			{
				si->matcount.wm -= bitcount(movelist[index].wm);
				si->matcount.wk -= bitcount(movelist[index].wk);
			}
			if(movelist[index].bk && movelist[index].bm) 
			{si->matcount.bk++; si->matcount.bm--;}
		}
		else
		{
			if(stmcapture)
			{
				si->matcount.bm -= bitcount(movelist[index].bm);
				si->matcount.bk -= bitcount(movelist[index].bk);
			}
			if(movelist[index].wk && movelist[index].wm)
			{si->matcount.wk++; si->matcount.wm--;}
		}
		//the above is equivalent to: countmaterial();

		si->realdepth++;
		// update the hash key
		updatehashkey(&movelist[index], &(si->hash));

#ifdef REPCHECK
		si->repcheck[si->realdepth + HISTORYOFFSET].hash = si->hash.key;
		si->repcheck[si->realdepth + HISTORYOFFSET].irreversible = movelist[index].bm | movelist[index].wm;
#endif




		/********************recursion********************/


		value = -negamax(si, &q, d-FRAC,-(alpha+1),&Lkiller, &forcefirst, truncationdepth,truncationdepth2,0);


		/*************************************************/

		//----------------undo the move------------------/
		si->realdepth--;
		si->hash = localhash;
		si->matcount = Lmatcount;
		//----------------end undo move------------------/

		// update best value so far 
		maxvalue = max(value,maxvalue);
		// check cutoff
		if(maxvalue > alpha)
		{
			bestindex = index;
			if(i == 0)
				si->cutoffsatfirst++;
			si->cutoffs++;
			break;
		}

	} // end main recursive loop of forallmoves 

	// set forcefirst in the calling negamax to the index of the best move- used for IID only
	*bestmoveindex = bestindex;

	// save the position in the hashtable 
	// we can/should restore the depth with which negamax was originally entered 
	// since this is the depth with which it would have been compared 

	d = originaldepth;

	if(d>=0)
		hashstore(si, p, maxvalue, alpha, d, &movelist[bestindex], bestindex); 

	// return best value
	return maxvalue;
}



int testcapture(POSITION *p)
{
	// testcapture returns 1 if the side to move has a capture.
	int32 black,white,free,m;

	if (p->color == BLACK)
	{
		black = p->bm|p->bk;
		white = p->wm|p->wk;
		free = ~(black|white);

		m =((((black&LFJ2)<<4)&white)<<3);
		m|=((((black&LFJ1)<<3)&white)<<4);
		m|=((((black&RFJ1)<<4)&white)<<5);
		m|=((((black&RFJ2)<<5)&white)<<4);
		if(p->bk)
		{
			m|=((((p->bk&LBJ1)>>5)&white)>>4);
			m|=((((p->bk&LBJ2)>>4)&white)>>5);
			m|=((((p->bk&RBJ1)>>4)&white)>>3);
			m|=((((p->bk&RBJ2)>>3)&white)>>4);
		}
		if(m & free)
			return 1;
		return 0;
	}
	else
	{
		black = p->bm|p->bk;
		white = p->wm|p->wk;
		free = ~(black|white);
		m=((((white&LBJ1)>>5)&black)>>4);
		m|=((((white&LBJ2)>>4)&black)>>5);
		m|=((((white&RBJ1)>>4)&black)>>3);
		m|=((((white&RBJ2)>>3)&black)>>4);
		if(p->wk)
		{
			m|=((((p->wk&LFJ2)<<4)&black)<<3);
			m|=((((p->wk&LFJ1)<<3)&black)<<4);
			m|=((((p->wk&RFJ1)<<4)&black)<<5);
			m|=((((p->wk&RFJ2)<<5)&black)<<4);
		}
		if(m & free)
			return 1;
		return 0;
	}
}



void hashstore(SEARCHINFO *si, POSITION *p, int value, int alpha, int depth, CAKE_MOVE *best, int32 bestindex)
{
	// store position in hashtable
	// based on the search window, alpha&beta, the value is assigned a valuetype
	// UPPER, LOWER or EXACT. the best move is stored in a reduced form 
	// well, i no longer use EXACT, since i'm using MTD(f).

	int32 index,minindex;
	int mindepth=1000,iter=0;
	int from,to;

	si->hashstores++;
	//assert(alpha == beta-1);

#ifdef MOHISTORY
	// update history table 
	if(p->color==BLACK)
	{
		from = (best->bm|best->bk)&(p->bm|p->bk);    /* bit set on square from */
		to   = (best->bm|best->bk)&(~(p->bm|p->bk));
		history[LSB(from)][LSB(to)]++;
	}
	else
	{
		from = (best->wm|best->wk)&(p->wm|p->wk);    /* bit set on square from */
		to   = (best->wm|best->wk)&(~(p->wm|p->wk));
		history[LSB(from)][LSB(to)]++;
	}
#endif

	index = si->hash.key & (hashsize-1);
	minindex = index;


	while(iter<HASHITER)
	{
		if(hashtable[index].lock == si->hash.lock || hashtable[index].lock==0) // vtune: use | instead of ||
			/* found an index where we can write the entry */
		{
			hashtable[index].lock = si->hash.lock;
			hashtable[index].depth =(int16) (depth);
			hashtable[index].best = bestindex;
			hashtable[index].color = (p->color>>1);
			hashtable[index].value =( sint16)value;
			/* determine valuetype */
			if(value > alpha) 
				hashtable[index].valuetype = LOWER;
			else
				hashtable[index].valuetype = UPPER;

			return;
		}
		else
		{
			/* have to overwrite */
			if((int) hashtable[index].depth < mindepth)
			{
				minindex=index;
				mindepth=hashtable[index].depth;
			}
		}
		iter++;
		index++;
	}
	/* if we arrive here it means we have gone through all hashiter
	entries and all were occupied. in this case, we write the entry
	to minindex */

	// if alwaysstore is not defined, and we have found no entry with equal or
	// lower importance, we don't overwrite it.
#ifndef ALWAYSSTORE
	if(mindepth>(depth)) 
		return;
#endif

	hashtable[minindex].lock = si->hash.lock;
	hashtable[minindex].depth=depth;
	hashtable[minindex].best=bestindex;
	hashtable[minindex].color=(p->color>>1);
	hashtable[minindex].value=value;
	/* determine valuetype */
	//if(value>=beta) 
	if(value > alpha)
		hashtable[minindex].valuetype = LOWER;
	else
		hashtable[minindex].valuetype = UPPER;

	return;

}

int hashlookup(SEARCHINFO *si, int *value, int *valuetype, int depth, int *forcefirst, int color, int *ispvnode)
{
	/* searches for a position in the hashtable.
	if the position is found and
	if (the stored depth is >= depth to search),
	hashlookup returns 1, indicating that the value and valuetype have
	useful information.
	else
	forcefirst is set to the best move found previously, and 0 returned
	if the position is not found at all, 0 is returned and forcefirst is
	left unchanged (at MAXMOVES-1). */

	int32 index;
	int iter=0;

	index = si->hash.key & (hashsize-1); // expects that hashsize is a power of 2!

	// TODO: what is this "hashtable[index].lock" good for? 
	while(iter<HASHITER && hashtable[index].lock) 
	{
		if(hashtable[index].lock == si->hash.lock && ((int)hashtable[index].color==(color>>1)))
		{
			// we have found the position 
			*ispvnode=hashtable[index].ispvnode;

			// move ordering 
			*forcefirst=hashtable[index].best;
			// use value if depth in hashtable >= current depth
			if((int)hashtable[index].depth>=depth)
			{
				*value=hashtable[index].value;
				*valuetype=hashtable[index].valuetype;

				return 1;
			}
		}
		iter++;
		index++;
	}

	return 0;
}

int pvhashlookup(SEARCHINFO *si, int *value, int *valuetype, int depth, int32 *forcefirst, int color, int *ispvnode)
{
	// pvhashlookup is like hashlookup, only with the exception that it marks the entry
	// in the hashtable as being part of the PV
	int32 index;
	int iter=0;

	index = si->hash.key & (hashsize-1);

#ifdef THREADSAFEHT
	EnterCriticalSection(&hash_access);
#endif


	while(iter<HASHITER && hashtable[index].lock) // vtune: use & instead
	{
		if(hashtable[index].lock == si->hash.lock && ((int)hashtable[index].color==(color>>1)))
		{
			// here's the only difference to the normal hashlookup!
			hashtable[index].ispvnode=1;

			/* move ordering */
			*forcefirst=hashtable[index].best;
			/* use value if depth in hashtable >= current depth)*/
			if((int)hashtable[index].depth>=depth)
			{
				*value=hashtable[index].value;
				*valuetype=hashtable[index].valuetype;
#ifdef THREADSAFEHT
				LeaveCriticalSection(&hash_access);
#endif				
				return 1;
			}
		}
		iter++;
		index++;
	}
#ifdef THREADSAFEHT
	LeaveCriticalSection(&hash_access);
#endif
	return 0;
}


void getpv(SEARCHINFO *si, POSITION *p, char *str)
{
	//----------------------------------------------------------------------
	// retrieves the principal variation from the hashtable:				|
	// looks up the position, finds the hashtable move, plays it, looks     |
	// up the position again etc.                                           |
	// color is the side to move, the whole PV gets written in *str         |
	// getpv also marks pv nodes in the hashtable, so they won't be pruned! |
	//----------------------------------------------------------------------

	CAKE_MOVE movelist[MAXMOVES];
	int32 forcefirst;
	int dummy=0;
	int i,n;
	char Lstr[1024];
	POSITION Lp;
	int values[MAXMOVES];
	//	int staticeval;
	int capture;

	//return;

	Lp=*p;
	absolutehashkey(&Lp, &(si->hash));
	sprintf(str,"pv ");
	for(i=0;i<MAXDEPTH;i++)
	{
		forcefirst=100;
		// pvhashlookup also stores the fact that these nodes are pv nodes
		// in the hashtable
		pvhashlookup(si, &dummy,&dummy,0, &forcefirst,Lp.color,&dummy);
		if(forcefirst==100)
		{
			break;
		}
		n = makecapturelist(&Lp,movelist,values,forcefirst);
		if(n)
			capture = 1;
		else 
			capture = 0;
		if(!n)
			n = makemovelist(si, &Lp, movelist, values, forcefirst, 0);
		if(!n) 
		{
			absolutehashkey(p,&(si->hash));
			return;
		}


		if(i<MAXPV)
		{

#ifdef FULLLOG
			staticeval = evaluation(&Lp,&(si->matcount),0,&dummy,0,maxNdb);
			if(capture)
				sprintf(Lstr,"[-]",staticeval);
			else
				sprintf(Lstr,"[%i]",staticeval);
			strcat(str,Lstr);
#endif // FULLLOG
			movetonotation(&Lp,&movelist[forcefirst],Lstr);
			strcat(str,Lstr);
			strcat(str," ");
		}

		togglemove((&Lp),movelist[forcefirst]);
		countmaterial(&Lp, &(si->matcount));
		absolutehashkey(&Lp,&(si->hash));
	}

	absolutehashkey(p,&(si->hash));
	countmaterial(p, &(si->matcount));
	return;
}


/* check for mobile black men, however, not all - only those
on rows 3 or more */

/* how to use this stuff: like this! */
/*tmp=p->bm & 0xFFFFF000;
while(tmp) //while black men
{
m= (tmp & (tmp-1))^tmp; // m is the least significant bit of tmp
tmp = tmp&(tmp-1);		// and we peel it away. 

// determine the white attack board without this man on:
free = ~(p->bk|(p->bm^m)|p->wk|p->wm);
wattack=backwardjump((p->wk|p->wm), free) | forwardjump(p->wk, free);

// move this black man forward until he's on the last row,
//	look if there is a way for it to get there 
mobile = 20;
while (m)
{
m=forward(m) & (~(p->wk | p->wm)) & (~wattack);
if(m&0xF0000000)
{
mobileblackmen+=mobile;
break;
}
mobile-=4;
}
}
// only check for rows 2-6 */


/* table-lookup bitcount */
int bitcount(int32 n) // vtune: make this inlined maybe as a macro
// returns the number of bits set in the 32-bit integer n 
{
	return (bitsinword[n&0x0000FFFF]+bitsinword[(n>>16)&0x0000FFFF]);
}


int LSB(int32 x)
{
	//-----------------------------------------------------------------------------------------------------
	// returns the position of the least significant bit in a 32-bit word x 
	// or -1 if not found, if x=0.
	// LSB uses "intrinsics" for an efficient implementation
	//-----------------------------------------------------------------------------------------------------

	/*
	if(_BitScanForward(&returnvalue,x))
	return returnvalue;
	else
	return -1;
	*/


	//old, non-intrinsic code
	if(x&0x000000FF)
		return(LSBarray[x&0x000000FF]);
	if(x&0x0000FF00)
		return(LSBarray[(x>>8)&0x000000FF]+8);
	if(x&0x00FF0000)
		return(LSBarray[(x>>16)&0x000000FF]+16);
	if(x&0xFF000000)
		return(LSBarray[(x>>24)&0x000000FF]+24);
	return -1;

}



void updatehashkey(CAKE_MOVE *m, HASH *h)
{
	// given a move m, updatehashkey updates the HASH structure h
	int32 x,y;
	x=m->bm;
	while(x)
	{
		y=LSB(x);
		h->key ^=hashxors[0][0][y];
		h->lock^=hashxors[1][0][y];
		x&=(x-1);
	}
	x=m->bk;
	while(x)
	{
		y=LSB(x);
		h->key ^=hashxors[0][1][y];
		h->lock^=hashxors[1][1][y];
		x&=(x-1);
	}
	x=m->wm;
	while(x)
	{
		y=LSB(x);
		h->key ^=hashxors[0][2][y];
		h->lock^=hashxors[1][2][y];
		x&=(x-1);
	}
	x=m->wk;
	while(x)
	{
		y=LSB(x);
		h->key ^=hashxors[0][3][y];
		h->lock^=hashxors[1][3][y];
		x&=(x-1);
	}
}

void absolutehashkey(POSITION *p, HASH *hash)
{
	/* absolutehashkey calculates the hashkey completely. slower than
	using updatehashkey, but useful sometimes */

	int32 x;

	hash->lock=0;
	hash->key=0;
	x=p->bm;
	while(x)
	{
		hash->key ^=hashxors[0][0][LSB(x)];
		hash->lock^=hashxors[1][0][LSB(x)];
		x&=(x-1);
	}
	x=p->bk;
	while(x)
	{
		hash->key ^=hashxors[0][1][LSB(x)];
		hash->lock^=hashxors[1][1][LSB(x)];
		x&=(x-1);
	}
	x=p->wm;
	while(x)
	{
		hash->key ^=hashxors[0][2][LSB(x)];
		hash->lock^=hashxors[1][2][LSB(x)];
		x&=(x-1);
	}
	x=p->wk;
	while(x)
	{
		hash->key ^=hashxors[0][3][LSB(x)];
		hash->lock^=hashxors[1][3][LSB(x)];
		x&=(x-1);
	}
}


void countmaterial(POSITION *p, MATERIALCOUNT *m)
{
	// TODO: make a MATERIALCOUNT structure and pass that as parameter.
	/* countmaterial initializes the globals bm, bk, wm and wk, which hold
	the number of black men, kings, white men and white kings respectively.
	during the search these globals are updated incrementally */
	m->bm = bitcount(p->bm);
	m->bk = bitcount(p->bk);
	m->wm = bitcount(p->wm);
	m->wk = bitcount(p->wk);
}

// put stuff like this into something like string.c


// TODO: put this into book.c

int booklookup(POSITION *p, int *value, int depth, int32 *remainingdepth, int *best, char str[256])
{
	/* searches for a position in the book hashtable.
	*/
	int32 index;
	int iter=0;
	int bookmoves;
	// todo: change this to bookhashentry!
	HASHENTRY *pointer;
	int bucketsize;
	int size;
	int bookfound = 0;
	int i,j,n;
	int bookcolor;
	int tmpvalue;
	CAKE_MOVE tmpmove,bestmove;
	int dummy[MAXMOVES], values[MAXMOVES],indices[MAXMOVES];
	int depths[MAXMOVES];
	CAKE_MOVE ml[MAXMOVES];
	char Lstr[1024], Lstr2[1024];
	HASH hash;
	SEARCHINFO s;
	resetsearchinfo(&s);

	bucketsize = BUCKETSIZEBOOK;
	pointer = book;
	size = bookentries;

	if(pointer == NULL)
		return 0;

	// now, look up current position
	absolutehashkey(p,&hash);
	index = hash.key % size;

	// harmonize colors between cake and book
	bookcolor = p->color;
	if(p->color==2) 
		bookcolor=0;

	while(iter<bucketsize)
	{
		if(pointer[index].lock == hash.lock && ((int)pointer[index].color==bookcolor)) // use &
		{
			/* we have found the position */
			*remainingdepth = pointer[index].depth;
			*value = pointer[index].value;
			*best = pointer[index].best;
			bookfound = 1;
		}
		iter++;
		index++;
		index %= size;
		if(bookfound) 
			break;
	}

    bool gotValue=false;
	if(bookfound)
	{
		// search all successors in book
		n = makecapturelist(p, ml, dummy, 0);
		if(!n)
			n = makemovelist(&s, p, ml, dummy, 0,0);

		for(i=0;i<MAXMOVES;i++)
		{
			values[i]=-MATE;
			indices[i]=i;
		}
		bestmove = ml[*best];

		// harmonize colors for book and cake
		bookcolor = (p->color^CC);
		if(bookcolor == 2)
			bookcolor=0;

		// for all moves look up successor position in book
		for(i=0;i<n;i++)
		{
			togglemove(p,ml[i]);
			absolutehashkey(p, &hash);
			index = hash.key % size;
			iter = 0;
			depths[i] = 0;
			values[i]=-MATE;
			while(iter<bucketsize)
			{
				if(pointer[index].lock == hash.lock && ((int)pointer[index].color==bookcolor))
				{
					// found position in book
					depths[i] = pointer[index].depth+1;
					values[i] = -pointer[index].value;
					gotValue=true;
				}
				iter++;
				index++;
				index%=size;
			}
			togglemove(p,ml[i]);
		}
	}

	// print book moves. check if we have successors:
	// order moves so we can print them in status line
	if(bookfound && gotValue)
	{
		sprintf(str,"book  ");
		for(i=0;i<n;i++)
		{
			for(j=0;j<n-1;j++)
			{
				if(100*values[j]+depths[j]<100*values[j+1]+depths[j+1])
				{
					tmpvalue = values[j];
					tmpmove = ml[j];
					values[j] = values[j+1];
					values[j+1] = tmpvalue;
					ml[j] = ml[j+1];
					ml[j+1] = tmpmove;
					tmpvalue = depths[j];
					depths[j] = depths[j+1];
					depths[j+1] = tmpvalue;
					tmpvalue = indices[j];
					indices[j] = indices[j+1];
					indices[j+1] = tmpvalue;
				}
			}
		}

		// count number of available book moves and put in variabe bookmoves
		for(i=0;i<n;i++)
		{
			if(values[i] != -MATE)
				bookmoves = i;
		}
		bookmoves++;

		// create ouput string with all moves, values, depths ordered by value
		for(i=0;i<n;i++)
		{	
			if(values[i]==-MATE)
				continue;
			movetonotation(p, &ml[i], Lstr);
			sprintf(Lstr2, " v%i d%i   ", values[i], depths[i]);
			strcat(str,Lstr);
			strcat(str,Lstr2);
		}

		// now, select a move according to value of usethebook 
		// if we have more than one available bookmove
		if(usethebook < BOOKBEST && bookmoves >1)
		{
			bookmoves = 0;
			if(usethebook == BOOKGOOD)
			{
				// select any move equal to the best in value
				for(i=1;i<n;i++)
				{
					if(values[i]==values[0])
						bookmoves = i;
				}
				bookmoves++;
			}
			if(usethebook == BOOKALLKINDS)
			{
				// select any move that is > -30 in value, and within
				// 10 points of the best move
				for(i=1;i<n;i++)
				{
					if(values[i]>values[0]-10 && values[i]>-30)
						bookmoves = i;
				}
				bookmoves++;
			}
			// we have bookmoves equivalent book moves.
			// pick one at random
			if(bookmoves !=0)
			{
				//srand( (unsigned)time( NULL ) );
				i = rand() % bookmoves;
				*remainingdepth = depths[i];
				*value = values[i];
				*best = indices[i];
				//sprintf(Lstr," #%i/%i %i",i,bookmoves,values[i]);
				//strcat(Lstr,str);
				//sprintf(str,"%s",Lstr);
			}
		}
	}
	else if(bookfound)
	{
		// last book move
		movetonotation(p,&bestmove,Lstr);
		sprintf(str,"book move: %s v %i d %i", Lstr,*value, *remainingdepth);
	}

	return bookfound;
}
