/*  cake++ - a checkers engine
*
*  Copyright (C) 2000-2005 by Martin Fierz
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*  contact: nospam1@fierz.ch
*/

/*
version history

1.20  (16th feb 2005)
--> added book.c and book.h files to the project; cake++ now can read a
book file called xbook.bin
--> changed back rank values slightly

1.19 (16th november 2004)
--> set playnow to zero before calling getmove

1.18 (16th september 2001)
--> after some testing with values for truncation depth
and singleextend parameters i now use 15 for TD, 5 for SE.
--> added some more knowledge to evaluation function:
-> runaways in two are detected as runaways now if the have
a clear run left or right ahead, instead of both sides.
-> more trapped king code: kings trapped in double corner
by two men are recognized, and kings trapped in single corner
by two men are recognized too.
-> and a new cramp formation is recognized.
result:
Cake++ 1.18 - KingsRow 1.08 by Ed Gilbert
+:71 =:171 -:36 unknown:4
1.17 (5th september)
--> saw crashes because MAXMOVES = 24 was too small - is 28 now
--> fixed one more bug (man center value) in eval
--> fixed a bug in truncation code which made much
too much truncations and didn't reexpand correctly
--> moved truncation decision before (d<0) test

1.16 (31st august)
--> fixed quite a number of bugs in evaluation function.
--> added some knowledge to evaluation

1.15 (12th august)
--> window size reduced to 10 -> 10% less nodes
--> implemented ETC -> 10% less nodes
(there is a switch #define ETC to turn this on/off)

1.14 (27th june)
improvements over 1.13:
--> cramp evaluation
--> additional info for database wins/losses
--> static evaluation implemented

*/


#include "switches.h"

#include <stdio.h>
#include <stdlib.h> /* malloc() */
#include <string.h> /* memset() */
#include <time.h>
#ifdef SYS_WINDOWS
#include <conio.h>
#include <windows.h>
#endif/*SYSTEM*/

/* structs.h defines the data structures for cake++ */
#include "structs.h"

/* consts.h defines constants used by cake++ */
#include "consts.h"

/* function prototypes */
#include "cakepp.h"
#include "movegen.h"
#include "db.h"
#include "book.h"
#ifdef OPLIB
void boardtobitboard(int b[8][8], struct pos *position);
#endif

#ifdef SYS_WINDOWS
#define TICKS CLOCKS_PER_SEC
#endif
#ifdef SYS_MACOS
#define TICKS CLOCKS_PER_SEC
#endif
#ifdef SYS_UNIX
#define TICKS CLOCKS_PER_SEC
#endif /*SYSTEM*/

/* globals */
static FILE *cake_fp;
struct pos p;
unsigned int cake_nodes;
static int dblookups;
int logging;
int *play; /*is nonzero if the engine is to play immediately */
static int bm,bk,wm,wk;
static int realdepth, maxdepth;
#ifdef REPCHECK
static struct pos Ghistory[CAKE_MAXDEPTH+HISTORYOFFSET+10]; /*holds the current variation for repetition check*/
#endif

/* create two hashtables: one where all positions with realdepth < DEEP are stored, and
where the bucket size is large, and another where everything is overwritten all the time */

static struct hashentry *deep, *shallow;
static int32  hashxors[2][4][32];
static int32 Gkey,Glock;
static int lastone[256];

#ifdef TABLE8
static int bitsinbyte[256];
#endif

#ifdef TABLE16
static int bitsinword[65536];
#endif

int hashsearch,hashhit,hashstores;
static char *out;
int maxNdb=0; /* the largest number of stones which is still in the database */
static int Gtruncationdepth=TRUNCATIONDEPTH;
double start,t,maxtime; /* time variables */
int searchmode;

/* history table */
int32 history[32][32];

/*----------------------------------interface---------------------------------*/
/* consists of initcake() exitcake() and getmove() */

int32 myrand(void)
{
	/* myrand produces a 32-bit random number */
	/* rand() returns a 15-bit random number (at least under windows), not 16,
	that's the reason i only take tha last 8 bits instead of 16*/
	int32 result=0;
	result+=(rand() & 0xFF);
	result=result<<8;
	result+=(rand() & 0xFF);
	result=result<<8;
	result+=(rand() & 0xFF);
	result=result<<8;
	result+=(rand() & 0xFF);
	return result;
}

int initcake(int log)
{
	int i,j;
	logging=log;
	deep=malloc(HASHSIZEDEEP*sizeof(struct hashentry));
	shallow=malloc(HASHSIZESHALLOW*sizeof(struct hashentry));
	/* initialize xors */
	srand(1);
	for(i=0;i<4;i++)
	{
		for(j=0;j<32;j++)
		{
			hashxors[0][i][j]=(int32)rand();
			hashxors[1][i][j]=(int32)rand();
		}
	}
#ifdef SYS_WINDOWS
	for(i=0;i<4;i++)
	{
		for(j=0;j<32;j++)
		{
			hashxors[0][i][j] = myrand();
			hashxors[1][i][j] = myrand();
		}
	}
#endif
	/* initialize array for "lastone" */
	for(i=0;i<256;i++)
	{
		if(i&BIT0) {lastone[i]=0;continue;}
		if(i&BIT1) {lastone[i]=1;continue;}
		if(i&BIT2) {lastone[i]=2;continue;}
		if(i&BIT3) {lastone[i]=3;continue;}
		if(i&BIT4) {lastone[i]=4;continue;}
		if(i&BIT5) {lastone[i]=5;continue;}
		if(i&BIT6) {lastone[i]=6;continue;}
		if(i&BIT7) {lastone[i]=7;continue;}
	}
	/* initialize bitsinbyte */
#ifdef TABLE8
	for(i=0;i<256;i++)
		bitsinbyte[i]=recbitcount((int32)i);
#endif
#ifdef TABLE16
	for(i=0;i<65536;i++)
		bitsinword[i]=recbitcount((int32)i);
#endif
#ifdef USEDB

	maxNdb=DBInit();
#endif

	/* load book */
	i = initbook();
	printf("\nloaded %i book positions",i);

	if(log & 1)
		/* delete old logfile */
	{
		cake_fp=fopen("cakelog.txt","w");
		fclose(cake_fp);
	}



	return 1;
}

int exitcake(void)

{
	/*   fclose(cake_fp);*/
	/* deallocate memory for the hashtables */
	free(deep);
	free(shallow);
	return 1;
}

void bookmovetomove(int32 bookmove, struct move *best, struct pos *p, int color);

int cake_getmove(struct pos *position,char moveNotation[256],int color, int how,double maximaltime, int depthtosearch, int32 maxnodes,
				 char str[255], int *playnow, int log, int reset)
{
	/* cake_getmove is the entry point to cake++
	give a pointer to a position and you get the new position in
	this structure after cake++ has calculated.

	color is BLACK or WHITE and is the side to move.

	how is 0 for time-based search and 1 for depth-based search and 2 for node-based search

	maxtime and depthtosearch and maxnodes are used for these two search modes.

	cake++ prints information in str

	if playnow is set to a value != 0 cake++ aborts the search.

	if (logging&1) cake++ will write information into "log.txt"

	if(logging&2) cake++ will also print the information to stdout.

	if reset!=0 cake++ will reset hashtables and repetition checklist
	*/



	int d;

	int value,lastvalue=0,n,i,j;
	struct move best,last, movelist[MAXMOVES];
	struct pos dummy;
	char tempstr[255];
    char PVstr[256];
	int32 bookmove;

	play=playnow;
	out=str;
	logging=log;
	maxtime=maximaltime;
	searchmode=how;

	p=(*position);
	if(logging & 1)
	{
		cake_fp=fopen("cakelog.txt","a");
		printboardtofile(p);
		fprintf(cake_fp,"\nposition hex:bm%x bk%x wm%x wk%x",p.bm,p.bk,p.wm,p.wk);
	}

	/* search position in book */
	bookmove = booklookup(&p, color);
	if(bookmove != 0)
	{
		if(logging&2)
		{
			printf("\nbook move\n");
			fflush(stdout);
		}
		if(logging&1)
		{
			fprintf(cake_fp,"\nbook move\n");
			fflush(cake_fp);
		}
		/* set the struct move 'best' to the book move */
		bookmovetomove(bookmove, &best, &p, color);
		if(logging & 1)
			printboardtofile(p);
		movetonotation(p,best,moveNotation,color);
		togglemove(best);
		if(logging & 1)
			printboardtofile(p);
		if(logging & 1)
			fclose(cake_fp);
		*position = p;
		return 0;
	}

	/* no book move found, must search */

	/* initialize material */
	bm=bitcount(p.bm);
	bk=bitcount(p.bk);
	wm=bitcount(p.wm);
	wk=bitcount(p.wk);

	/* reset history table */
	for(i=0;i<32;i++)
	{
		for(j=0;j<32;j++)
		{
			history[i][j]=0;
			history[i][j]=0;
		}
	}

	/* set truncation depth setting:
	in endgames do not truncate any more */
	if(bm+bk+wm+wk<=8) 
		Gtruncationdepth=0;
	else 
		Gtruncationdepth=TRUNCATIONDEPTH;

	/* clear the hashtable */
	memset(deep,0,HASHSIZEDEEP*sizeof(struct hashentry));
	/*if(reset!=0)*/
	memset(shallow,0,HASHSIZESHALLOW*sizeof(struct hashentry));

	start=clock();
	cake_nodes=0;
	dblookups=0;
	n=makecapturelist(movelist, color, 0);


	realdepth=0;maxdepth=0;

	/*reset=0;*/

#ifdef REPCHECK
	/*initialize history list */
	Ghistory[HISTORYOFFSET]=p;
	dummy.bm=BIT0;
	dummy.wm=BIT0;
	dummy.bk=BIT0;
	dummy.wk=BIT0;
	if(reset==0)
	{
		for(i=0;i<HISTORYOFFSET-2;i++)
			Ghistory[i]=Ghistory[i+2];
	}
	else
	{
		for(i=0;i<HISTORYOFFSET;i++)
			Ghistory[i]=dummy;
	}
#endif

	hashsearch=0;
	hashhit=0;
	hashstores=0;
	absolutehashkey();

	for(d=1;d<CAKE_MAXDEPTH;d+=2)
	{
		/*do a search with aspiration window*/
		value=firstnegamax(10*d,color,lastvalue-ASPIRATIONWINDOW,lastvalue+ASPIRATIONWINDOW,&best);
		/* check if aspiration holds */
		t=clock();
		movetonotation(p,best,moveNotation,color);
		if(value>=lastvalue+ASPIRATIONWINDOW)
		{
			/*memset(deep,0,HASHSIZEDEEP*sizeof(struct hashentry));*/
			/*memset(shallow,0,HASHSIZESHALLOW*sizeof(struct hashentry));*/
			if(logging&2)
			{
				printf("\nbest: %s depth %i/%i nodes %u value>%i time %3.2fs",moveNotation,d,maxdepth,cake_nodes,value,(t-start)/TICKS);
				fflush(stdout);
			}
			if(logging&1)
			{
				fprintf(cake_fp,"\nbest: %s depth %i/%i nodes %u value>%i time %3.2fs",moveNotation,d,maxdepth,cake_nodes,value,(t-start)/TICKS);
				fflush(cake_fp);
			}
#ifndef ANALYSISMODULE
			sprintf(str,"\nbest: %s depth %i/%i nodes %u value>%i time %3.2fs",moveNotation,d,maxdepth,cake_nodes,value,(t-start)/TICKS);
#endif
			value=firstnegamax(10*d,color,lastvalue,10000,&best);
			if(value<=lastvalue)
				value=firstnegamax(10*d,color,-10000,10000,&best);
			movetonotation(p,best,moveNotation,color);
		}
		if(value<=lastvalue-ASPIRATIONWINDOW)
		{
			/*memset(deep,0,HASHSIZEDEEP*sizeof(struct hashentry));*/
			/*memset(shallow,0,HASHSIZESHALLOW*sizeof(struct hashentry));*/
			if(logging&2)
			{
				printf("\nbest: %s depth %i/%i nodes %u value<%i time %3.2fs",moveNotation,d,maxdepth,cake_nodes,value,(t-start)/TICKS);
				fflush(stdout);
			}
			if(logging&1)
			{
				fprintf(cake_fp,"\nbest: %s depth %i/%i nodes %u value<%i time %3.2fs",moveNotation,d,maxdepth,cake_nodes,value,(t-start)/TICKS);
				fflush(cake_fp);
			}
#ifndef ANALYSISMODULE
			sprintf(str,"\nbest: %s depth %i/%i nodes %u value<%i time %3.2fs",moveNotation,d,maxdepth,cake_nodes,value,(t-start)/TICKS);
#endif
			value=firstnegamax(10*d,color,-10000,lastvalue,&best);
			if(value>=lastvalue)
				value=firstnegamax(10*d,color,-10000,10000,&best);
			movetonotation(p,best,moveNotation,color);
		}


		t=clock();
#ifndef ANALYSISMODULE
		if(t-start>0)
			sprintf(str,"best: %s depth %i/%i nodes %u value %i time %3.2fs %4.0fkN/s db %i",moveNotation,d,maxdepth,cake_nodes,value,(t-start)/TICKS,cake_nodes/1000/(t-start)*TICKS,dblookups);
		else
			sprintf(str,"best: %s depth %i/%i nodes %u value %i time %3.2fs ?kN/s db %i",moveNotation,d,maxdepth,cake_nodes,value,(t-start)/TICKS,dblookups);

#endif
		if(logging&1)
		{
			fprintf(cake_fp,"\n%s",str);
			fflush(cake_fp);
		}
		if(logging&2)
		{
			printf("\n%s",str);
			fflush(stdout);
		}


		if(how==0)
			/* time mode: if time>maxtime/2 stop search. if time>2*maxtime then a
			test in 'negamax' will set *play to 1
			therefore, cake++ returns in the interval [MT/2,2*MT] */
		{if( (clock()-start)/TICKS>(maxtime/2)) break;}
		if(how==1)
		{
			if(d>=depthtosearch)
			{
				break;
			}
		}
		if(how==2)
		{if(cake_nodes>maxnodes) break;}
#ifdef IMMEDIATERETURNONFORCED
		if(n==1) break;
#endif
		if(abs(value)>4500) break;
		if(*play)
		{
			/* stop the search. don't use the best move & value because they are rubbish */
			best=last;
			movetonotation(p,best,moveNotation,color);
			value=lastvalue;
#ifndef ANALYSISMODULE
			sprintf(str,"interrupt: best %s value %i",moveNotation,value);
#endif
			break;
		}
		lastvalue=value; /* save the value for this iteration */
		last=best; /* save the best move on this iteration */
	}
#ifdef REPCHECK
	Ghistory[HISTORYOFFSET-2]=p;
#endif
	getpv(PVstr,color);
#ifndef ANALYSISMODULE
	strcat(str," pv: ");
	strcat(str,PVstr);
	strcpy(tempstr,"");
	strcat(tempstr,str);

	sprintf(str,"%s",tempstr);
#endif
	if(!(*play))
	{togglemove(best);}
	else
	{togglemove(last);}
	if(logging&1)
	{
		fprintf(cake_fp,"\nPV: %s\n",PVstr);
		fflush(cake_fp);
	}
	if(logging&2)
	{
		printf("\nPV: %s\n",PVstr);
		fflush(stdout);
	}
#ifdef REPCHECK
	Ghistory[HISTORYOFFSET-1]=p;
#endif
	*position=p;
	if(logging&1) fclose(cake_fp);
	return value;
}




void bookmovetomove(int32 bookmove, struct move *best, struct pos *p, int color)
{
	/* in the book, the move is stored in a compressed form to save
	space. this routine decompresses the move again */

	int32 occupied, empty;

	occupied = p->bm|p->bk|p->wm|p->wk;
	empty = ~occupied;

	if(color == BLACK)
	{
		/* remove captured pieces */
		best->wm = p->wm & bookmove;
		bookmove ^= best->wm;
		best->wk = p->wk & bookmove;
		bookmove ^= best->wk;
		best->bm = 0;
		best->bk = 0;

		/* now only the from and to square of the black piece are still set */
		if(bookmove & p->bm)
			best->bm = bookmove;
		else
			best->bk = bookmove;
		best->bk |= (best->bm & WBR);
		best->bm &= NWBR;
	}
	else
	{
		/* remove captured pieces */
		best->bm = p->bm & bookmove;
		bookmove ^= best->bm;
		best->bk = p->bk & bookmove;
		bookmove ^= best->bk;
		best->wm = 0;
		best->wk = 0;

		/* now only the from and to square of the black piece are still set */
		if(bookmove & p->wm)
			best->wm = bookmove;
		else
			best->wk = bookmove;
		best->wk |= (best->wm & BBR);
		best->wm &= NBBR;
	}
}

/*-----------------------------------------------------------------------------*/

#ifndef ANALYSISMODULE
int firstnegamax(int d, int color, int alpha, int beta, struct move *best)
{
	int i,value,swap=0;
	static int n;
	int capture;
	static struct move movelist[MAXMOVES];
	int32 l_bm,l_bk,l_wm,l_wk;
	int32 Lkey,Llock;
	int Lalpha=alpha,Lbeta=beta;
	int32 forcefirst=0;
	int32 Lkiller=0;
	static struct pos last;
	struct move tmpmove;
	int values[MAXMOVES]; /* holds the values of the respective moves - use to order */

	if(*play) return 0;
	cake_nodes++;

	/* search the current position in the hashtable */
	hashlookup(&value,&alpha,&beta,d,&forcefirst, color);

	/* check for a capture move */
	capture = testcapture(color);

	if(last.bm==p.bm && last.bk==p.bk && last.wm==p.wm && last.wk==p.wk)
		/* then we are still looking at the same position - no need to
		regenerate the movelist */    ;
	else
	{
		for(i=0;i<MAXMOVES;i++)
			values[i]=color==BLACK?-10000:10000;
		n=makecapturelist(movelist,color,forcefirst);
		capture=n;
		if(n==0)
			n=makemovelist(movelist,color,forcefirst,0);
		if(n==0)
			return -5000+realdepth;
	}
	/* save old hashkey */
	Lkey=Gkey;
	Llock=Glock;

	/* save old material balance */
	l_bm=bm;l_bk=bk;l_wm=wm;l_wk=wk;
	*best=movelist[0];

	for(i=0;i<n;i++)
	{
		/*movetonotation(p,*best,moveNotation,color);
		printf(out,"best: %s depth %i/%i nodes %i value %i ",moveNotation,d,maxdepth,cake_nodes,value);
		movetonotation(p,movelist[i],moveNotation,color);
		strcat(out, moveNotation); */
		togglemove(movelist[i]);
		if(color==BLACK)
		{
			if(capture)
			{
				wm-=recbitcount(movelist[i].wm);
				wk-=recbitcount(movelist[i].wk);
			}
			bk+=isprom(movelist[i].info);
			bm-=isprom(movelist[i].info);
		}
		else
		{
			if(capture)
			{
				bm-=recbitcount(movelist[i].bm);
				bk-=recbitcount(movelist[i].bk);
			}
			wk+=isprom(movelist[i].info);
			wm-=isprom(movelist[i].info);
		}
		realdepth++;
		/*update the hash key*/
		updatehashkey(movelist[i]);

#ifdef REPCHECK
		Ghistory[realdepth+HISTORYOFFSET]=p;
#endif

		/********************recursion********************/
		value=-negamax(d-10,color^CC,-beta,-alpha,&Lkiller,0);
		/*************************************************/
		values[i]=value;
		realdepth--;
		togglemove(movelist[i]);
		/* restore the old hash key*/
		Gkey=Lkey;
		Glock=Llock;
		/* restore the old material balance */
		bm=l_bm;bk=l_bk;wm=l_wm;wk=l_wk;

		if(value>=beta) {*best=movelist[i];alpha=value;swap=i;break;}
		if(value>alpha) {*best=movelist[i];alpha=value;swap=i;}
	}
	/* save the position in the hashtable */
	hashstore(alpha,Lalpha,Lbeta,d,*best,color);
	/* and order the movelist */
	if(swap!=0)
	{
		tmpmove=movelist[swap];
		for(i=swap;i>0;i--)
		{
			movelist[i]=movelist[i-1];
		}
		movelist[0]=tmpmove;
	}
	/* to check how the movelist is doing, print it */
	/*printf("\n");
	for(i=0;i<n;i++)
	{
	movetonotation(p,movelist[i],moveNotation,color);
	printf("%s, ",moveNotation);
	}*/
	/* save position to check it on next entry because of movelist*/
	last=p;
	return alpha;
}

#else  /* this is the analysis version of cake++ */
int firstnegamax(int d, int color, int alpha, int beta, struct move *best)
{
	int i,j,value,capture,swap=0;
	static int n;
	static struct move movelist[MAXMOVES];
	int32 l_bm,l_bk,l_wm,l_wk;
	int32 Lkey,Llock;
	int Lalpha=alpha,Lbeta=beta;
	/*   char Lstr[256];*/
	int32 forcefirst=0;
	int32 Lkiller=0;
	static struct pos last;
	struct move tmpmove;
	char Lstr[255];
	static int values[MAXMOVES]; /* holds the values of the respective moves - use to order */
	/* and since this is the analysis module, also used to diplay values of
	all moves */


	if(*play) return 0;
	cake_nodes++;
	/* search the current position in the hashtable */
	/*hashlookup(&value,&alpha,&beta,d,&forcefirst, color);  */

	if(last.bm==p.bm && last.bk==p.bk && last.wm==p.wm && last.wk==p.wk)
		/* then we are still looking at the same position - no need to
		regenerate the movelist */  ;
	else
	{
		/*for(i=0;i<MAXMOVES;i++)
		values[i]=color==BLACK?-10000:10000; */
		n=makecapturelist(movelist,color,forcefirst);
		capture=n;
		if(n==0)
			n=makemovelist(movelist,color,forcefirst,0);
		if(n==0)
			return -5000+realdepth;
	}

	/* save old hashkey */
	Lkey=Gkey;
	Llock=Glock;

	/* save old material balance */
	l_bm=bm;l_bk=bk;l_wm=wm;l_wk=wk;
	*best=movelist[0];

	for(i=0;i<n;i++)
	{
		/*movetonotation(p,*best,Lstr,color);
		printf(out,"best: %s depth %i/%i nodes %i value %i ",Lstr,d,maxdepth,cake_nodes,value);
		movetonotation(p,movelist[i],Lstr,color);
		strcat(out, Lstr); */
		togglemove(movelist[i]);
		if(color==BLACK)
		{
			if(capture)
			{
				wm-=recbitcount(movelist[i].wm);
				wk-=recbitcount(movelist[i].wk);
			}
			bk+=isprom(movelist[i].info);
			bm-=isprom(movelist[i].info);
		}
		else
		{
			if(capture)
			{
				bm-=recbitcount(movelist[i].bm);
				bk-=recbitcount(movelist[i].bk);
			}
			wk+=isprom(movelist[i].info);
			wm-=isprom(movelist[i].info);
		}
		realdepth++;
		/*update the hash key*/
		updatehashkey(movelist[i]);

#ifdef REPCHECK
		Ghistory[realdepth+HISTORYOFFSET]=p;
#endif

		/********************recursion********************/
		value=-negamax(d-10,color^CC,-10000,10000,&Lkiller,0);
		/*************************************************/
		values[i]=value;
		/* output new list of moves with values in str */

		realdepth--;
		togglemove(movelist[i]);
		/* restore the old hash key*/
		Gkey=Lkey;
		Glock=Llock;
		/* restore the old material balance */
		bm=l_bm;bk=l_bk;wm=l_wm;wk=l_wk;

		sprintf(out,"d %i: ",(int)d/10);
		for(j=0;j<n;j++)
		{
			movetonotation(p,movelist[j],Lstr,color);
			strcat(out,Lstr);
			strcat(out,":");
			sprintf(Lstr,"%i   ",values[j]);
			strcat(out,Lstr);
		}


		if(value>=beta) {*best=movelist[i];swap=i;/*break;*/}
		if(value>alpha) {*best=movelist[i];swap=i;alpha=value;}
	}
	/* save the position in the hashtable */
	hashstore(alpha,Lalpha,Lbeta,d,*best,color);
	/* and order the movelist with a bubblesort*/
	for(i=0;i<n;i++)
	{
		for(j=0;j<n-1;j++)
		{
			if(values[j]<values[j+1])
			{
				swap=values[j];
				values[j]=values[j+1];
				values[j+1]=swap;
				tmpmove=movelist[j];
				movelist[j]=movelist[j+1];
				movelist[j+1]=tmpmove;
			}
		}
	}
	last=p;
	return alpha;

}
#endif

int negamax(int d, int color, int alpha, int beta, int32 *protokiller, int truncationdepth)
/* negamax sets *protokiller to the best move compressed like in the hashtable */
/* the calling negamax gets a best move in this way. by passing it on in further */
/* calls it can be used as a killer */
{
	int i,n,value,capture,v1,v2;
	int l_bm,l_bk,l_wm,l_wk;
	int32 forcefirst=0;
	int Lalpha=alpha,Lbeta=beta;
#ifdef ETC
	int ETCalpha=alpha,ETCbeta=beta,ETCvalue=0;
	int32 ETCdummy;
#endif
	struct move movelist[MAXMOVES],best;
	int32 Lkey,Llock;
	int32 Lkiller=0;
	int dbresult;
	int allstones;

	/* time check */
	if((cake_nodes & 0xFFFF)==0)
	{
		if(searchmode==0)
			if( (clock()-start)/TICKS>(4*maxtime)) (*play=1);
	}

	/* return if calculation interrupt */
	if(*play) return 0;
	/* stop search if maximal search depth is reached */
	if(realdepth>CAKE_MAXDEPTH) return evaluation(color,alpha,beta);
	cake_nodes++;

	/* search the current position in the hashtable */
	/* only if there is still search depth left! */
	if(d>0)
	{
		if(hashlookup(&value,&alpha,&beta,d,&forcefirst,color))
			return value;
	}
#ifdef REPCHECK
	/* check for repetitions */
	if(bk && wk)
	{
		for(i=realdepth+HISTORYOFFSET-2;i>=0;i-=2)
		{
			if((p.bm^Ghistory[i].bm)) break; /* stop repetition search if move with a man is detected */
			if((p.wm^Ghistory[i].wm)) break;
			if((p.bm==Ghistory[i].bm) && (p.bk==Ghistory[i].bk) && (p.wm==Ghistory[i].wm) && (p.wk==Ghistory[i].wk))
				return 0;                     /* same position detected! */
		}
	}
#endif
	//#ifdef MOKILLER
	/* if there is no best move in the hashtable we try the killer move */
	//   if(forcefirst==0) forcefirst=*protokiller;
	//#endif
#ifdef CHECKCAPTURESINLINE
	if(testcapture(color))
		n=makecapturelist(movelist,color,forcefirst);
#else

	/* uninlined capture test */
	n=makecapturelist(movelist,color,forcefirst);
#endif
	capture=n;

	/* check for database use */
#ifdef USEDB
	allstones=bk+bm+wk+wm;
	if(allstones<=maxNdb)
	{
		if(capture==0)
		{
			if(testcapture(color^CC)==0)
			{
				if( (bk+bm>0) && (wk+wm>0))
				{
					/* found a position which should be in the database */
					/* no captures are possible */
					dblookups++;
					dbresult=DBLookup(p,(color)>>1);
					if(dbresult==DRAW)
						return 0;
					if(dbresult==WIN)
					{
						value=dbwineval(color);
						if(value>=beta) return value;
					}

				}
			}
		}
	}
#endif

	v1=100*bm+130*bk;
	v2=100*wm+130*wk;
	i=v1-v2;                       /*material values*/
	i+=(250*(v1-v2))/(v1+v2);      /*favor exchanges if in material plus*/
	v1= (color==BLACK) ? i:-i;
	/* v1 now contains the material evaluation at this node */
	/* use this to decide about extensions/truncations */

#ifdef DOEXTENSIONS
	/* truncate or re-expand if the material count is outside / inside eval window */
	if(v1<alpha-TRUNCATEVALUE)
	{
		truncationdepth+=Gtruncationdepth;
		d-=Gtruncationdepth; /* forgot G in versions before 2nd september 1.16 and down */
	}
	else
	{
		if(v1>beta+TRUNCATEVALUE)
		{
			truncationdepth+=Gtruncationdepth;
			d-=Gtruncationdepth; /* forgot G in versions before 2nd september 1.16 and down */
		}
		else
		{
			d+=truncationdepth;
			truncationdepth=0;
		}
	}
#endif


	if(n==0)
	{
		if(d<=0)
		{
			if(realdepth>maxdepth) maxdepth=realdepth;
#ifndef EXTENDALL
			return evaluation(color,alpha,beta);
#else
			if(testcapture(color^CC)!=0)
				/* side not to move has a capture */
				/* now we test if the side to move is outside the window */
			{
				if(v1>beta+QLEVEL)
					return evaluation(color,alpha,beta);
				if(v1<alpha-QLEVEL)
					return evaluation(color,alpha,beta);
				/* if the evaluation is in [alpha-QLEVEL,beta+QLEVEL] we extend */
			}
			else
				return evaluation(color,alpha,beta);
#endif
		}
		n=makemovelist(movelist,color,forcefirst,*protokiller);
	}
	if(n==0)
		return -5000+realdepth;


	/* check for single move and extend appropriately */
	if(n==1)
		d+=SINGLEEXTEND;

	/* save old hashkey and old material balance*/
	Lkey=Gkey;
	Llock=Glock;
	l_bm=bm;l_bk=bk;l_wm=wm;l_wk=wk;

	/* for all moves: domove, update hashkey&material, recursion, restore
	material balance and hashkey, undomove, do alphabetatest */
	best=movelist[0];

#ifdef ETC
	/* enhanced transposition cutoffs: do every move and check if the resulting
	position is in the hashtable. if yes, check if the value there leads to a cutoff
	if yes, we don't have to search */
	if(d>ETCDEPTH)
	{
		for(i=0;i<n;i++)
		{
			/* do move */
			togglemove(movelist[i]);
			/* update hashkey */
			updatehashkey(movelist[i]);

			/* do the ETC lookup:
			with reduced depth and changed color */
			ETCalpha=-beta;
			ETCbeta=-alpha;
			ETCvalue=-10000;
			if(hashlookup(&ETCvalue,&ETCalpha,&ETCbeta,d-10,&ETCdummy,(color^CC)))

			{
				/* if one of the values we find is > beta we quit! */
				if( (-ETCvalue)>=beta )
				{
					best=movelist[i];
					/* before we quit: restore all stuff */
					Gkey=Lkey;
					Glock=Llock;
					togglemove(movelist[i]);
					return beta;
				}
			}
			/* restore the hash key*/
			Gkey=Lkey;
			Glock=Llock;
			/* undo move */
			togglemove(movelist[i]);
		}
	}


#endif
	for(i=0;i<n;i++)
	{
		togglemove(movelist[i]);

		/* inline material count */
		if(color==BLACK)
		{
			if(capture)
			{
				wm-=recbitcount(movelist[i].wm);
				wk-=recbitcount(movelist[i].wk);
			}
			bk+=isprom(movelist[i].info);
			bm-=isprom(movelist[i].info);
		}
		else
		{
			if(capture)
			{
				bm-=recbitcount(movelist[i].bm);
				bk-=recbitcount(movelist[i].bk);
			}
			wk+=isprom(movelist[i].info);
			wm-=isprom(movelist[i].info);
		}
		/* end material count */

		/*countmaterial();*/
		realdepth++;
#ifdef REPCHECK
		Ghistory[realdepth+HISTORYOFFSET]=p;
#endif
		/*update the hash key*/
		updatehashkey(movelist[i]);

		/********************recursion********************/
		value=-negamax(d-10,color^CC,-beta,-alpha, &Lkiller,truncationdepth);
		/*************************************************/
		realdepth--;
		togglemove(movelist[i]);
		/* restore the hash key*/
		Gkey=Lkey;
		Glock=Llock;
		/* restore the old material balance */
		bm=l_bm;bk=l_bk;wm=l_wm;wk=l_wk;

		if(value>=beta)
		{
			alpha=value;
			best=movelist[i];
			break;
		}
		if(value>alpha) {alpha=value;best=movelist[i];}
	}
	/* save the position in the hashtable */
	hashstore(alpha,Lalpha,Lbeta,d,best,color);
	/* set the killer move */
#ifdef MOKILLER
	if(color==BLACK)
		*protokiller=best.bm|best.bk;
	else
		*protokiller=best.wm|best.wk;
#endif
	return alpha;
}


void hashstore(int value, int alpha, int beta, int depth, struct move best,int color)
{
	/* write the record anyway where the index is*/
	int32 index,minindex;
	int mindepth=1000,iter=0;
	int from,to;

	if(depth<0) return;
	hashstores++;

	/* update history table */
	if(color==BLACK)
	{
		from=(best.bm|best.bk)&(p.bm|p.bk);    /* bit set on square from */
		to=  (best.bm|best.bk)&(~(p.bm|p.bk));
		from=lastbit(from);
		to=lastbit(to);
		if(from<32)
			history[from][to]++;
	}
	else
	{
		from=(best.wm|best.wk)&(p.wm|p.wk);    /* bit set on square from */
		to=  (best.wm|best.wk)&(~(p.wm|p.wk));
		from=lastbit(from);
		to=lastbit(to);
		if(from<32)
			history[from][to]++;
	}

	if(realdepth < DEEPLEVEL)
	{
		/* its in the "deep" hashtable: take care not to overwrite other entries */
		index=Gkey&HASHMASKDEEP;
		minindex=index;
		while(iter<HASHITER)
		{
			if(deep[index].lock==Glock || deep[index].lock==0)
				/* found an index where we can write the entry */
			{
				deep[index].lock=Glock;
				deep[index].info=(int16) (depth&DEPTH);
				if(color==BLACK)
				{
					deep[index].best=best.bm|best.bk;
					deep[index].info|=HASHBLACK;
				}
				else
					deep[index].best=best.wm|best.wk;

				deep[index].value=(sint16)value;
				/* determine valuetype */
				if(value>=beta) {deep[index].info|=LOWER;return;}
				if(value>alpha) {deep[index].info|=EXACT;return;}
				deep[index].info|=UPPER;
				return;
			}
			else
			{
				if( hashdepth(deep[index].info) < mindepth)
				{
					minindex=index;
					mindepth=hashdepth(deep[index].info);
				}
			}
			iter++;
			index++;
			index=index&HASHMASKDEEP;
		}
		/* if we arrive here it means we have gone through all hashiter
		entries and all were occupied. in this case, we write the entry
		to minindex */
		if(mindepth>(depth)) return;
		deep[minindex].lock=Glock;
		deep[minindex].info=(int16) (depth);
		if(color==BLACK)
			deep[minindex].best=best.bm|best.bk;
		else
			deep[minindex].best=best.wm|best.wk;
		deep[minindex].info|=(int16)((color>>1)<<13);
		deep[minindex].value=value;
		/* determine valuetype */
		if(value>=beta) {deep[minindex].info|=LOWER;return;}
		if(value>alpha) {deep[minindex].info|=EXACT;return;}
		deep[minindex].info|=UPPER;
		return;
		/* and return */
	}
	else
	{
		index=Gkey&HASHMASKSHALLOW;
		if( hashdepth(shallow[index].info) <= depth )
			/* replace the old entry if the new depth is larger */
		{
			shallow[index].lock=Glock;
			shallow[index].info=(int16)depth;
			shallow[index].value=(sint16)value;
			if(color==BLACK)
			{
				shallow[index].best=best.bm|best.bk;
				shallow[index].info|=HASHBLACK;
			}
			else
				shallow[index].best=best.wm|best.wk;

			/* determine valuetype */

			if(value>=beta) {shallow[index].info|=LOWER;return;}
			if(value>alpha) {shallow[index].info|=EXACT;return;}
			shallow[index].info|=UPPER;
		}
	}
	return;
}

int hashlookup(int *value, int *alpha, int *beta, int depth, int32 *forcefirst, int color)
{
	int32 index;
	int iter=0;

	hashsearch++;

	if(realdepth<DEEPLEVEL)
		/* a position in the "deep" hashtable - it's important to find it since */
		/* the effect is larger here! */
	{
		hashhit++;
		index=Gkey&HASHMASKDEEP;
		while(iter<HASHITER)
		{
			if(deep[index].lock==Glock && (hashcolor(deep[index].info)>>13)==(color>>1))
			{
				/* we have found the position */
				hashhit++;
				/* move ordering */
				*forcefirst=deep[index].best;
				/* use value if depth in hashtable >= current depth)*/
				if(hashdepth(deep[index].info)>=depth)
				{
					/* if it's an exact value we can use it */
					if(hashvaluetype(deep[index].info) == EXACT)
					{
						*value=deep[index].value;
						return 1;
					}
					/* lower bound */
					if(hashvaluetype(deep[index].info) == LOWER)
					{
						if(deep[index].value>=(*beta)) {*value=deep[index].value;return 1;}
						if(deep[index].value>(*alpha)) {*alpha=deep[index].value;}
						return 0;
					}
					/* upper bound */
					if(hashvaluetype(deep[index].info) == UPPER)
					{
						if(deep[index].value<=*alpha) {*value=deep[index].value;return 1;}
						if(deep[index].value<*beta)   {*beta=deep[index].value;}
						return 0;
					}
				}
			}
			iter++;
			index++;
			index&=HASHMASKDEEP;
		}
		return 0;
	}
	/* use shallow hashtable */
	else
	{
		index=Gkey&HASHMASKSHALLOW;
		if(shallow[index].lock==Glock && (hashcolor(shallow[index].info)>>13)==(color>>1))
		{
			hashhit++;
			/*found the right entry!*/
			*forcefirst=shallow[index].best;
			if(hashdepth(shallow[index].info)>=depth)
			{
				if(hashvaluetype(shallow[index].info) == EXACT)
				{
					*value=shallow[index].value;
					return 1;
				}
				/* lower bound */
				if(hashvaluetype(shallow[index].info) == LOWER)
				{
					if(shallow[index].value>=*beta) {*value=shallow[index].value;return 1;}
					if(shallow[index].value>*alpha) {*alpha=shallow[index].value;}
					return 0;
				}
				/* upper bound */
				if(hashvaluetype(shallow[index].info) == UPPER)
				{
					if(shallow[index].value<=*alpha) {*value=shallow[index].value;return 1;}
					if(shallow[index].value<*beta)   {*beta=shallow[index].value;}
					return 0;
				}
			}
		}
	}
	return 0;
}

void getpv(char *str,int color)
{
	/* retrieve the principal variation from the hashtable */
	struct move movelist[MAXMOVES];
	int32 forcefirst;
	int dummy=0;
	int i,n;
	char Lstr[256];
	struct pos Lp;

	Lp=p;
	absolutehashkey();
	strcpy(str,"");
	for(i=0;i<DEEPLEVEL;i++)
	{
		hashlookup(&dummy,&dummy,&dummy,0, &forcefirst,color);
		n=makecapturelist(movelist,color,forcefirst);
		if(!n)
			n=makemovelist(movelist,color,forcefirst,0);
		if(!n) {p=Lp;return;}

		movetonotation(p,movelist[0],Lstr,color);

		strcat(str,Lstr);
		strcat(str," ");
		togglemove(movelist[0]);
		absolutehashkey();
		color=color^CC;
	}
	p=Lp;
	return;
}

/* test because of crash
if(bitcount(p.bm)+bitcount(p.bk)+bitcount(p.wm)+bitcount(p.wk) != bm+bk+wm+wk)
{
printf("\ncannot happen!");
printf("\nsays is bm %i bk %i wm %i wk %i",bm,bk,wm,wk);
printf("\nshould be bm %i bk %i wm %i wk %i",bitcount(p.bm),bitcount(p.bk),bitcount(p.wm),bitcount(p.wk));
printboard(p);
getch();
togglemove(movelist[i]);
printf("\nwas this position before, move %i of %i",i,n);
printboard(p);
togglemove(movelist[i]);

}
end test */
int dbwineval(int color) /* color has a WIN on the board */
{
	int value;

	value = 4000-100*(bm+bk+wm+wk); /*-realdepth*/
	/* new in 1.14: modifiers to encourage the stronger side to
	promote and grab the center */

	if(color==BLACK) /* color is the side which is winning now */
	{
		value+=10*bitcount(p.bk);
		value+=2*bitcount(p.bk&CENTER);
		if(p.bm)
		{
			value+=  bitcount(p.bm&0x000000F0);
			value+=2*bitcount(p.bm&0x00000F00);
			value+=3*bitcount(p.bm&0x0000F000);
			value+=4*bitcount(p.bm&0x000F0000);
			value+=5*bitcount(p.bm&0x00F00000);
			value+=6*bitcount(p.bm&0x0F000000);
		}
	}
	else
	{
		value+=10*bitcount(p.wk);
		value+=2*bitcount(p.wk&CENTER);
		if(p.wm)
		{
			value+=  bitcount(p.wm&0x0F000000);
			value+=2*bitcount(p.wm&0x00F00000);
			value+=3*bitcount(p.wm&0x000F0000);
			value+=4*bitcount(p.wm&0x0000F000);
			value+=5*bitcount(p.wm&0x00000F00);
			value+=6*bitcount(p.wm&0x000000F0);
		}
	}
	return value;
}

int dblosseval(int color) /* color has a WIN on the board */
{
	int value;

	value = -4000+100*(bm+bk+wm+wk); /*-realdepth */
	/* new in 1.14: modifiers to encourage the stronger side to
	promote and grab the center */

	if(color==BLACK) /* color is the side which is winning now */
	{
		value-=10*bitcount(p.wk);
		value-=2*bitcount(p.wk&CENTER);
		if(p.wm)
		{
			value-=  bitcount(p.wm&0x000000F0);
			value-=2*bitcount(p.wm&0x00000F00);
			value-=3*bitcount(p.wm&0x0000F000);
			value-=4*bitcount(p.wm&0x000F0000);
			value-=5*bitcount(p.wm&0x00F00000);
			value-=6*bitcount(p.wm&0x0F000000);
		}
	}
	else
	{
		value-=10*bitcount(p.bk);
		value-=2*bitcount(p.bk&CENTER);
		if(p.bm)
		{
			value-=6*  bitcount(p.bm&0x0F000000);
			value-=5*bitcount(p.bm&0x00F00000);
			value-=4*bitcount(p.bm&0x000F0000);
			value-=3*bitcount(p.bm&0x0000F000);
			value-=2*bitcount(p.bm&0x00000F00);
			value-=bitcount(p.bm&0x000000F0);
		}
	}
	return value;
}


int evaluation(int color, int alpha, int beta)
{
	int eval;
	int v1,v2;
#ifdef USEDB
	int allstones,dbresult;
#endif
#ifdef EVALOFF
	return 0;
#endif
	/************************* material **************************/
	v1=100*bm+130*bk;
	v2=100*wm+130*wk;
	if(v1==0) return(color==BLACK?(-5000+realdepth):5000-realdepth);
	if(v2==0) return(color==BLACK?(5000-realdepth):-5000+realdepth);

#ifdef USEDB
	allstones=bm+bk+wm+wk;
	if(allstones<=maxNdb) /* this position is in the database!*/
	{
		dbresult=DBLookup(p,(color)>>1);
		if(dbresult==DRAW)
			return 0;
		if(dbresult==WIN)
		{
			eval=dbwineval(color);
			return eval;
		}
		if(dbresult==LOSS)
		{
			eval=dblosseval(color);
			return eval;
		}
		/*if(dbresult==UNKNOWN)
		{
		we do nothing - so we don't need this clause;
		}*/
	}
#endif
	eval=v1-v2;                       /*material values*/
	eval+=(250*(v1-v2))/(v1+v2);      /*favor exchanges if in material plus*/

	eval= color==BLACK ?eval:-eval;

#ifdef EVALMATERIALONLY
	return eval;
#endif
	if(( eval>beta+FINEEVALWINDOW) || (eval<alpha-FINEEVALWINDOW) )
		return eval;
	eval+=fineevaluation(color);
#ifdef COARSEGRAINING
	eval=(eval/GRAINSIZE)*GRAINSIZE;
#endif
	return eval;
}

int fineevaluation(int color)
{

	static int br[32]={0,0,2,2,4,6,10,10,
		1,4,16,16,6,10,24,16,
		0,0,2,2,4,10,16,16,
		1,4,16,16,6,10,24,16};

	static int tmod[25]={0,2,2,2,2,1,1,1,1,0,0,0,0,-1,-1,-1,-1,-2,-2,-2,-2,-3,-3,-3,-3};
	/*static int tmod[25]={0,3,3,3,3,2,2,2,2,1,1,0,-1,-1,-1,-1,-2,-2,-3,-3,-4,-4,-5,-5,-5};*/
	int stones=bm+wm;
	int kings=bk+wk;
	int allstones=kings+stones;
	int tempo=0;

	/* constant values for the positional features - end in ...val */
	const int dogholeval=5;
	const int devsinglecornerval=5;
	const int intactdoublecornerval=5;
	const int dykeval=2;
	const int kcval=4;
	const int mcval=1; /*1 is standard */
	const int keval=-4;
	const int turnval=3;
	const int oreoval=5;
	const int crampval=4;
	const int realcrampval=20;
	const int kingtrappedinsinglecornerval=20;
	const int kingtrappedinsinglecornerbytwoval=10;
	const int kingtrappedindoublecornerval=10;

	const int onlykingval=20, roamingkingval=32;
	const int themoveval=2;
	const int promoteinone=27,promoteintwo=24,promoteinthree=18;
	const int unmobileval=8;

	/* and these are the sum of the values of the features, eval=sum of all features */
	int eval=0;
	int n_attacked,n_free,n_all;
	int freebk=bk, freewk=wk;
	int potbk=0, potwk=0;
	int index;
	int brblack,brwhite;
	int doghole=0,devsinglecorner=0,intactdoublecorner=0,dyke=0,balance=0;
	int kc=0,mc=0,ke=0,turn=0,move=0,oreo=0,cramp=0,runaway=0;
	int backrank=0,trappedking=0;
	int onlyking=0,mobility=0;

	int32 white=p.wm|p.wk;
	int32 black=p.bk|p.bm;
	int32 tmp,free,attacked,squares;


	/************************ positional *************************/
	free=~(p.bm|p.bk|p.wm|p.wk);

	/* organize: things to do only if men, things only if kings, */
	/* things to do only if men are on the board */

	if(stones)
	{
		/* back rank */
		index=p.bm&0x0000000F;
		if(p.bm & BIT7) index+=16;

		brblack=br[index];
		backrank+=brblack;
		index=0;
		if(p.wm & BIT31) index++;
		if(p.wm & BIT30) index+=2;
		if(p.wm & BIT29) index+=4;
		if(p.wm & BIT28) index+=8;
		if(p.wm & BIT24) index+=16;

		brwhite=br[index];
		backrank-=brwhite;

		eval+=backrank;
		/* tempo */
		tempo+=  bitcount(p.bm&0x000000F0);
		tempo+=2*bitcount(p.bm&0x00000F00);
		tempo+=3*bitcount(p.bm&0x0000F000);
		tempo+=4*bitcount(p.bm&0x000F0000);
		tempo+=5*bitcount(p.bm&0x00F00000);
		tempo+=6*bitcount(p.bm&0x0F000000);
		tempo-=  bitcount(p.wm&0x0F000000);
		tempo-=2*bitcount(p.wm&0x00F00000);
		tempo-=3*bitcount(p.wm&0x000F0000);
		tempo-=4*bitcount(p.wm&0x0000F000);
		tempo-=5*bitcount(p.wm&0x00000F00);
		tempo-=6*bitcount(p.wm&0x000000F0);
		tempo*=tmod[stones];
		/* wait with tempo evaluation until doghole is done! */

		/* cramping squares */

		if( (p.bm&BIT16) && (p.wm&BIT20) ) cramp+=crampval;
		if( (p.bm&BIT11) && (p.wm&BIT15) ) cramp-=crampval;

		if( (p.bm&BIT15) && ((p.wm&0x00C80000)==0x00C80000) )
			cramp+=crampval;
		if( (p.wm&BIT16) && ((p.bm&0x00001300)==0x00001300) )
			cramp-=crampval;
		/*       WHITE
		28  29  30  31
		24  25  26  27
		20  21  22  23
		16  17  18  19
		12  13  14  15
		8   9  10  11
		4   5   6   7
		0   1   2   3
		BLACK */

		/* real cramp position - new in 1.14*/
		if( ((p.bm&0x00011100)==0x00011100) && ((p.wm&0x02320000)==0x02320000) && ((free&0x11000000)==0x11000000) )
		{
			/* black on 8/12/16 */
			if( ((~(p.bm))&0x00000211) == 0x00000211) /* if a black man is on these squares, cramp loses effectivness */
				cramp+=realcrampval;
		}
		if( ((p.bm&0x00008880)==0x00008880) && ((p.wm&0x004C4000)==0x004C4000) && ((free&0x08800000)==0x08800000) )
		{
			/* black on 7/11/15 */
			if( ((~(p.bm))&0x00000048) == 0x00000048)
				cramp+=realcrampval;
		}
		if( ((p.wm&0x01110000)==0x01110000) && ((p.bm&0x00023200)==0x00023200) && ((free&0x00000110)==0x00000110) )
		{
			/* white on 16/20/24 */
			if( ((~(p.wm))&0x12000000) == 0x12000000)
				cramp-=realcrampval;
		}
		if( ((p.wm&0x00888000)==0x00888000) && ((p.bm&0x00004C40)==0x00004C40) && ((free&0x00000088)==0x00000088) )
		{
			/* white on 15/19/23 */
			if( ((~(p.wm))&0x88400000) == 0x88400000)
				cramp-=realcrampval;
		}
		eval+=cramp;
		/*
		WHITE
		28  29  30  31
		24  25  26  27
		20  21  22  23
		16  17  18  19
		12  13  14  15
		8   9  10  11
		4   5   6   7
		0   1   2   3
		BLACK
		*/
		if(bm==wm)
		{
			/* balance */
			index=-3*bitcount(black&0x01010101)-2*bitcount(black&0x10101010)-bitcount(black&0x02020202)+
				bitcount(black&0x40404040)+2*bitcount(black&0x08080808)+3*bitcount(black&0x80808080);
			balance-=abs(index);
			index=-3*bitcount(white&0x01010101)-2*bitcount(white&0x10101010)-bitcount(white&0x02020202)+
				bitcount(white&0x40404040)+2*bitcount(white&0x08080808)+3*bitcount(white&0x80808080);
			balance+=abs(index);
			eval+=balance;
			/* the move */
			if(color==BLACK)
			{
				if(bitcount((~free)&0x0F0F0F0F)%2) /* the number of stones in blacks system is odd -> he has the move*/
					move=themoveval*(24-allstones)/6;
				else
					move=-themoveval*(24-allstones)/6;
			}
			else
			{
				if(bitcount((~free)&0xF0F0F0F0)%2)
					move=-themoveval*(24-allstones)/6;
				else
					move=themoveval*(24-allstones)/6;
			}
			eval+=move;
		}
		/* center control */
		mc+=mcval*bitcount(p.bm&CENTER);
		mc-=mcval*bitcount(p.wm&CENTER);
		eval+=mc;

		/* doghole */
		if( (p.bm&BIT1) && (p.wm&BIT8))
		{
			/* good for black */
			/* count away the tempo effect: */
			/* only if it's good for black! we don't want white to go in the doghole just
			to save tempo */
			if(tmod[stones]>0) tempo+=5*tmod[stones];
			/* and give a penalty - for sure here*/
			doghole+=dogholeval;
		}
		if( (p.bm&BIT3) && (p.wm&BIT7) )
		{
			/* count away tempo */
			if(tmod[stones]>0) tempo+=6*tmod[stones];
			/* give a penalty or not? - i don't know... */
			doghole+=dogholeval;
		}
		if( (p.bm&BIT24) && (p.wm&BIT28) )
		{
			if(tmod[stones]>0) tempo-=6*tmod[stones];
			doghole-=dogholeval;
		}
		if( (p.bm&BIT23) && (p.wm&BIT30) )
		{
			if(tmod[stones]>0) tempo-=5*tmod[stones];
			doghole-=dogholeval;
		}
		eval+=doghole;
		/* now that doghole has been evaluated, we can do the temp eval too */
		eval+=tempo;
		/*
		WHITE
		28  29  30  31
		24  25  26  27
		20  21  22  23
		16  17  18  19
		12  13  14  15
		8   9  10  11
		4   5   6   7
		0   1   2   3
		BLACK
		*/
		/* oreo */
		if( (p.bm&BIT1) && (p.bm&BIT2) && (p.bm&BIT5) ) oreo+=oreoval;
		if( (p.wm&BIT29) && (p.wm&BIT30) && (p.wm&BIT26) ) oreo-=oreoval;
		eval+=oreo;

		/* developed single corner */
		if( ((~p.bm)&BIT0) && ((~p.bm)&BIT4) ) devsinglecorner+=devsinglecornerval;
		if( ((~p.wm)&BIT31) && ((~p.wm)&BIT27) ) devsinglecorner-=devsinglecornerval;
		eval+=devsinglecorner;
		/* intact double corner */
		if( p.bm&BIT3 )  /* for black */
		{
			if( (p.bm&BIT6) || (p.bm&BIT7) ) intactdoublecorner+=intactdoublecornerval;
			/*if( (p.bm&BIT6)  ) intactdoublecorner+=intactdoublecornerval;  */
		}
		if( p.wm&BIT28 ) /* for white */
		{
			if( (p.wm&BIT24) || (p.wm&BIT25) ) intactdoublecorner-=intactdoublecornerval;
			/*if( (p.wm&BIT25) ) intactdoublecorner-=intactdoublecornerval; */
		}
		eval+=intactdoublecorner;

		/* dyke or g-line */
		dyke+=dykeval*bitcount(p.bm&0x00022400);
		dyke-=dykeval*bitcount(p.wm&0x00244000);
		eval+=dyke;

		/************* runaway checkers **************************/
		/** in one **/
		if( (p.bm) & 0x0F000000)
		{
			if((p.bm)&(free>>4)&BIT24) {runaway+=promoteinone;potbk++;runaway-=6*tmod[stones];}
			if((p.bm)&( (free>>3)|(free>>4) ) & BIT25) {runaway+=promoteinone;potbk++;runaway-=6*tmod[stones];}
			if((p.bm)&( (free>>3)|(free>>4) ) & BIT26) {runaway+=promoteinone;potbk++;runaway-=6*tmod[stones];}
			if((p.bm)&( (free>>3)|(free>>4) ) & BIT27) {runaway+=promoteinone;potbk++;runaway-=6*tmod[stones];}
		}

		if( (p.wm) & 0x000000F0)
		{
			if((p.wm)&(free<<4)&BIT7) 						{runaway-=promoteinone;potwk++;runaway+=6*tmod[stones];}
			if((p.wm)&( (free<<3)|(free<<4) ) & BIT6) {runaway-=promoteinone;potwk++;runaway+=6*tmod[stones];}
			if((p.wm)&( (free<<3)|(free<<4) ) & BIT5) {runaway-=promoteinone;potwk++;runaway+=6*tmod[stones];}
			if((p.wm)&( (free<<3)|(free<<4) ) & BIT4) {runaway-=promoteinone;potwk++;runaway+=6*tmod[stones];}
		}
		/** in two **/
		if( (p.bm) & 0x00F00000)
		{
			if( (p.bm&BIT20) && !(RA20&white) ) {runaway+=promoteintwo;potbk++;runaway-=5*tmod[stones];}
			if( (p.bm&BIT21) && (!(RA21L&white) || !(RA21R&white)) ) {runaway+=promoteintwo;potbk++;runaway-=5*tmod[stones];}
			if( (p.bm&BIT22) && (!(RA22L&white) || !(RA22R&white)) ) {runaway+=promoteintwo;potbk++;runaway-=5*tmod[stones];}
			if( (p.bm&BIT23) && !(RA23&white) ) {runaway+=promoteintwo;potbk++;runaway-=5*tmod[stones];}
		}
		if( (p.wm) & 0x00000F00)
		{
			if( (p.wm&BIT8)  && !(RA8&black) ) {runaway-=promoteintwo;potwk++;runaway+=5*tmod[stones];}
			if( (p.wm&BIT9)  && (!(RA9L&black) || !(RA9R&black))) {runaway-=promoteintwo;potwk++;runaway+=5*tmod[stones];}
			if( (p.wm&BIT10) && (!(RA10L&black) || !(RA10R&black))) {runaway-=promoteintwo;potwk++;runaway+=5*tmod[stones];}
			if( (p.wm&BIT11) && !(RA11&black) ) {runaway-=promoteintwo;potwk++;runaway+=5*tmod[stones];}
		}
		/** in 3 **/
		if( (p.bm) & 0x000F0000)
		{
			if( (p.bm&BIT16) && !(RA16&white) ) {runaway+=promoteinthree;runaway-=4*tmod[stones];}
			if( (p.bm&BIT17) && !(RA17&white) ) {runaway+=promoteinthree;runaway-=4*tmod[stones];}
			if( (p.bm&BIT18) && !(RA18&white) ) {runaway+=promoteinthree;runaway-=4*tmod[stones];}
			if( (p.bm&BIT19) && !(RA19&white) ) {runaway+=promoteinthree;runaway-=4*tmod[stones];}
		}
		if( (p.wm) & 0x0000F000)
		{
			if( (p.wm&BIT12) && !(RA12&black) ) {runaway-=promoteinthree;runaway+=4*tmod[stones];}
			if( (p.wm&BIT13) && !(RA13&black) ) {runaway-=promoteinthree;runaway+=4*tmod[stones];}
			if( (p.wm&BIT14) && !(RA14&black) ) {runaway-=promoteinthree;runaway+=4*tmod[stones];}
			if( (p.wm&BIT15) && !(RA15&black) ) {runaway-=promoteinthree;runaway+=4*tmod[stones];}
		}

		/* bridge situations */
		/* for black */
		if(p.bm&BIT21)
		{
			if( (p.wm&BIT28) && (p.wm&BIT30) && (free&BIT29) )
			{
				/* left side */
				if( (free&BIT24) && (free&BIT25) && (p.bm&BIT20) )
				{runaway+=promoteintwo;runaway-=5*tmod[stones];}
				/* right side */
				if( (free&BIT26) && (free&BIT27) && (p.bm&BIT22) )
				{runaway+=promoteintwo;runaway-=5*tmod[stones];}
			}
		}
		/* for white */
		if(p.wm&BIT10)
		{
			if( (p.bm&BIT1) && (p.bm&BIT3) && (free&BIT2) )
			{
				/* left side */
				if( (free&BIT4) && (free&BIT5) && (p.wm&BIT9) )
				{runaway-=promoteintwo;runaway+=5*tmod[stones];}
				/* right side */
				if( (free&BIT6) && (free&BIT7) && (p.wm&BIT11) )
				{runaway-=promoteintwo;runaway+=5*tmod[stones];}
			}
		}
		eval+=runaway;
	} /* end stuff only with men */


	/************** end runaway checkers *************/
	/*
	WHITE
	28  29  30  31
	24  25  26  27
	20  21  22  23
	16  17  18  19
	12  13  14  15
	8   9  10  11
	4   5   6   7
	0   1   2   3
	BLACK
	*/


	/*************** everything about kings... ********************/
	if(kings)
	{
		/* king center control */
		kc+=kcval*bitcount(p.bk&CENTER);
		kc-=kcval*bitcount(p.wk&CENTER);
		/* kings on edge */
		ke+=keval*bitcount(p.bk&EDGE);
		ke-=keval*bitcount(p.wk&EDGE);
		eval+=kc;
		eval+=ke;
		/* free and trapped kings */
		/* king trapped by one man in single corner */
		if( (p.wk&BIT0) && (p.bm&BIT1))
		{
			if (free&BIT8)
			{
				freewk--;   /* old code takes care of a king trapped by a man */
				trappedking+=kingtrappedinsinglecornerval;
			}
		}
		if( (p.bk&BIT31) && (p.wm&BIT30) )
		{
			if (free&BIT23)
			{
				freebk--;
				trappedking-=kingtrappedinsinglecornerval;
			}
		}
		/*
		WHITE
		28  29  30  31
		24  25  26  27
		20  21  22  23
		16  17  18  19
		12  13  14  15
		8   9  10  11
		4   5   6   7
		0   1   2   3
		BLACK */
		/* king assisted by man but trapped by two men */
		if( p.wk&(BIT0|BIT4) )
		{
			if( p.wm&BIT8)
			{
				if( (p.bm&(BIT1|BIT5)) == (BIT1|BIT5) )
				{
					trappedking+=kingtrappedinsinglecornerbytwoval;
					freewk--;
				}
			}
		}
		if( p.bk&(BIT27|BIT31) )
		{
			if( p.bm&BIT23)
			{
				if( (p.wm&(BIT30|BIT26)) == (BIT30|BIT26) )
				{
					trappedking-=kingtrappedinsinglecornerbytwoval;
					freebk--;
				}
			}
		}

		/* king trapped in double corner by two men */
		if(p.bk & BIT28)
		{
			if((p.wm&BIT24) && (p.wm&BIT29))
			{
				trappedking-=kingtrappedindoublecornerval;
				freebk--;
			}
		}
		if(p.wk & BIT3)
		{
			if( (p.bm&BIT2) && (p.bm&BIT7) )
			{
				trappedking+=kingtrappedindoublecornerval;
				freewk--;
			}
		}
		eval+=trappedking;
		/*
		WHITE
		28  29  30  31
		24  25  26  27
		20  21  22  23
		16  17  18  19
		12  13  14  15
		8   9  10  11
		4   5   6   7
		0   1   2   3
		BLACK
		*/


		/* 'absurd' self-trapping */
		if( (p.bk&BIT31) && (p.wm&BIT30) && (p.bm&BIT27) && (p.bm&BIT23) )
		{
			freebk--;
			eval-=230;
		}
		if( (p.wk&BIT0) && (p.bm&BIT1) && (p.wm&BIT4) && (p.wm&BIT8) )
		{
			freewk--;
			eval+=230;
		}
	} /* end things only if king ! */

	/* only king */
	/* the point of this is to make cake++ sacrifice a man if it gets a king,
	has a strong back rank and gets its king in the center */
	/* sacrificed man=-100
	king -> +30 total -70
	only king +20 total -50
	strong br +18 total -22
	king in center +32 +10
	king in center has such a high value because cake sometimes sacrifices
	a man for a trapped king which isn't so obviously trapped */

	/* single black king */
	if( ((potbk+freebk)>0) && ((freewk+potwk)==0) )
	{
		onlyking+=onlykingval;
		onlyking+=(brblack-brwhite);
		if(bk & CENTER) onlyking+=roamingkingval;
	}
	/* single white king */
	if( ((potwk+freewk)>0) && ((freebk+potbk)==0) )
	{
		onlyking-=onlykingval;
		onlyking+=(brblack-brwhite);
		if(wk & CENTER) onlyking-=roamingkingval;
	}
	eval+=onlyking;

	/* turn */
	if(color==BLACK) turn+=turnval;
	else turn-=turnval;
	eval+=turn;
	/*
	WHITE
	28  29  30  31
	24  25  26  27
	20  21  22  23
	16  17  18  19
	12  13  14  15
	8   9  10  11
	4   5   6   7
	0   1   2   3
	BLACK
	*/
	/* attacked squares - simple mobility check */
	tmp=p.wk|p.wm;
	attacked=lb1(tmp)|lb2(tmp)|rb1(tmp)|rb2(tmp);
	attacked|=lf1(p.wk)|lf2(p.wk)|rf1(p.wk)|rf2(p.wk);
	/* attack contains all squares where white pieces could move to */
	tmp=p.bm|p.bk;
	squares=lf1(tmp)|lf2(tmp)|rf1(tmp)|rf2(tmp);
	squares|=lb1(p.bk)|lb2(p.bk)|rb1(p.bk)|rb2(p.bk);
	/* squares contains all squares where black pieces can go to */
	/* if there are few squares which black pieces can occupy where
	they are not attacked, thats bad */
	n_free=bitcount(squares&(~attacked));
	n_attacked=bitcount(squares&attacked);
	n_all=bitcount(squares);
	mobility+=n_free-n_attacked;
	if(color==BLACK)
	{
		if(n_free<2 && n_all>5) mobility-=unmobileval;
		if(n_free==0) mobility-=unmobileval;
	}
	tmp=p.bk|p.bm;
	attacked=lf1(tmp)|lf2(tmp)|rf1(tmp)|rf2(tmp);
	attacked|=lb1(p.bk)|lb2(p.bk)|rb1(p.bk)|rb2(p.bk);
	/* attack contains all squares where black pieces could move to */
	tmp=p.wm|p.wk;
	squares=lb1(tmp)|lb2(tmp)|rb1(tmp)|rb2(tmp);
	squares|=lf1(p.wk)|lf2(p.wk)|rf1(p.wk)|rf2(p.wk);
	/* squares contains all squares where white pieces can go to */
	/*if there are few squares which black pieces can occupy where
	they are not attacked, thats bad */
	n_free=bitcount(squares&(~attacked));
	n_attacked=bitcount(squares&attacked);
	n_all=bitcount(squares);
	mobility-=n_free-n_attacked;
	if(color==WHITE)
	{
		if(n_free<2 &&n_all>5) mobility+=unmobileval;
		if(n_free==0) mobility+=unmobileval;
	}
	eval+=mobility;
	return color==BLACK ? eval:-eval;
}






/* recursive bitcount */
int recbitcount(int32 n)
{
	int r=0;
	while(n)
	{
		n=n&(n-1);
		r++;
	}
	return r;
}
/* recursive bitcount */
/*int bitcount(int32 n)
{
int r=0;
while(n)
{
n=n&(n-1);
r++;
}
return r;
} */

/* table-lookup bitcount */
int bitcount(int32 n)
{
#ifdef TABLE8
	return (bitsinbyte[n&0x000000FF]+bitsinbyte[(n>>8)&0x000000FF]+bitsinbyte[(n>>16)&0x000000FF]+bitsinbyte[(n>>24)&0x000000FF]);
#endif
#ifdef TABLE16
	return (bitsinword[n&0x0000FFFF]+bitsinword[(n>>16)&0x0000FFFF]);
#endif
}
/* hacker's bible bitcount */
/*int bitcount(int32 b)
{
#define m1 ( (int32) 0x55555555)
#define m2 ( (int32) 0x33333333)

const int32 a=b-((b>>1)&m1);
int32 n=(a&m2)+((a>>2)&m2);

n=(n&0x0F0F0F0F)+((n>>4)&0x0F0F0F0F);
n=(n&0xFFFF)+(n>>16);
n=(n&0xFF)+(n>>8);

return n;
}*/

int lastbit(int32 x)
{
	/* returns the position of the last bit in x */

	if(x&0x000000FF)
		return(lastone[x&0x000000FF]);
	if(x&0x0000FF00)
		return(lastone[(x>>8)&0x000000FF]+8);
	if(x&0x00FF0000)
		return(lastone[(x>>16)&0x000000FF]+16);
	if(x&0xFF000000)
		return(lastone[(x>>24)&0x000000FF]+24);
	return 1000;
}

void updatehashkey(struct move m)
{
	int32 x;
	x=m.bm;
	while(x)
	{
		Gkey ^=hashxors[0][0][lastbit(x)];
		Glock^=hashxors[1][0][lastbit(x)];
		x&=(x-1);
	}
	x=m.bk;
	while(x)
	{
		Gkey ^=hashxors[0][1][lastbit(x)];
		Glock^=hashxors[1][1][lastbit(x)];
		x&=(x-1);
	}
	x=m.wm;
	while(x)
	{
		Gkey ^=hashxors[0][2][lastbit(x)];
		Glock^=hashxors[1][2][lastbit(x)];
		x&=(x-1);
	}
	x=m.wk;
	while(x)
	{
		Gkey ^=hashxors[0][3][lastbit(x)];
		Glock^=hashxors[1][3][lastbit(x)];
		x&=(x-1);
	}
}

void absolutehashkey(void)
{
	int32 x;

	Glock=0;
	Gkey=0;
	x=p.bm;
	while(x)
	{
		Gkey ^=hashxors[0][0][lastbit(x)];
		Glock^=hashxors[1][0][lastbit(x)];
		x&=(x-1);
	}
	x=p.bk;
	while(x)
	{
		Gkey ^=hashxors[0][1][lastbit(x)];
		Glock^=hashxors[1][1][lastbit(x)];
		x&=(x-1);
	}
	x=p.wm;
	while(x)
	{
		Gkey ^=hashxors[0][2][lastbit(x)];
		Glock^=hashxors[1][2][lastbit(x)];
		x&=(x-1);
	}
	x=p.wk;
	while(x)
	{
		Gkey ^=hashxors[0][3][lastbit(x)];
		Glock^=hashxors[1][3][lastbit(x)];
		x&=(x-1);
	}

}

/* inline capture :*/
int testcapture(int color)
{
	/* test for captures inline to speed things up:*/
	int32 black,white,free,m;

	if (color==BLACK)
	{
		black=p.bm|p.bk;
		white=p.wm|p.wk;
		free=~(black|white);
		m =((((black&LFJ2)<<4)&white)<<3);
		m|=((((black&LFJ1)<<3)&white)<<4);
		m|=((((black&RFJ1)<<4)&white)<<5);
		m|=((((black&RFJ2)<<5)&white)<<4);
		m&=free;
		if(p.bk)
		{
			m|=((((p.bk&LBJ1)>>5)&white)>>4)&free;
			m|=((((p.bk&LBJ2)>>4)&white)>>5)&free;
			m|=((((p.bk&RBJ1)>>4)&white)>>3)&free;
			m|=((((p.bk&RBJ2)>>3)&white)>>4)&free;
		}
		if(m)
			return 1;
		return 0;
	}
	else
	{
		black=p.bm|p.bk;
		white=p.wm|p.wk;
		free=~(black|white);
		m=((((white&LBJ1)>>5)&black)>>4);
		m|=((((white&LBJ2)>>4)&black)>>5);
		m|=((((white&RBJ1)>>4)&black)>>3);
		m|=((((white&RBJ2)>>3)&black)>>4);
		m&=free;
		if(p.wk)
		{
			m|=((((p.wk&LFJ2)<<4)&black)<<3)&free;
			m|=((((p.wk&LFJ1)<<3)&black)<<4)&free;
			m|=((((p.wk&RFJ1)<<4)&black)<<5)&free;
			m|=((((p.wk&RFJ2)<<5)&black)<<4)&free;
		}
		if(m)
			return 1;
		return 0;
	}
}

void countmaterial(void)
{
	bm=bitcount(p.bm);
	bk=bitcount(p.bk);
	wm=bitcount(p.wm);
	wk=bitcount(p.wk);
}


void movetonotation(struct pos pos,struct move m, char *str, int color)

{

	/* make a notation out of a move */

	/* m is the move, str the string, color the side to move */

	int from, to;

	char c;

	/*

	WHITE
	28  29  30  31           32  31  30  29
	24  25  26  27           28  27  26  25
	20  21  22  23           24  23  22  21
	16  17  18  19           20  19  18  17
	12  13  14  15           16  15  14  13
	8   9  10  11           12  11  10   9
	4   5   6   7            8   7   6   5
	0   1   2   3            4   3   2   1
	BLACK
	*/
	static int square[32]={4,3,2,1,8,7,6,5,
		12,11,10,9,16,15,14,13,
		20,19,18,17,24,23,22,21,
		28,27,26,25,32,31,30,29}; /* maps bits to checkers notation */

	if(color==BLACK)

	{
		if(m.wk|m.wm) c='x';
		else c='-';                      /* capture or normal ? */
		from=(m.bm|m.bk)&(pos.bm|pos.bk);    /* bit set on square from */
		to=  (m.bm|m.bk)&(~(pos.bm|pos.bk));
		from=lastbit(from);
		to=lastbit(to);
		from=square[from];
		to=square[to];
		sprintf(str,"%2i%c%2i",from,c,to);
	}

	else

	{

		if(m.bk|m.bm) c='x';

		else c='-';                      /* capture or normal ? */

		from=(m.wm|m.wk)&(pos.wm|pos.wk);    /* bit set on square from */

		to=  (m.wm|m.wk)&(~(pos.wm|pos.wk));

		from=lastbit(from);

		to=lastbit(to);

		from=square[from];

		to=square[to];

		sprintf(str,"%2i%c%2i",from,c,to);

	}

	return;

}

void printboardtofile(struct pos p)
{
	int i;
	int free=~(p.bm|p.bk|p.wm|p.wk);
	int b[32];
	char c[15]="-wb      WB";

	for(i=0;i<32;i++)
	{
		if((p.bm>>i)%2)
			b[i]=BLACK;
		if((p.bk>>i)%2)
			b[i]=BLACK|KING;
		if((p.wm>>i)%2)
			b[i]=WHITE;
		if((p.wk>>i)%2)
			b[i]=WHITE|KING;
		if((free>>i)%2)
			b[i]=0;
	}
	/*        WHITE
	7  15  23  31
	3  11  19  27
	6  14  22  30
	2  10  18  26
	5  13  21  29
	1   9  17  25
	4  12  20  28
	0   8  16  24
	BLACK     */

	fprintf(cake_fp,"\n\n");
	fprintf(cake_fp,"\n %c %c %c %c",c[b[28]],c[b[29]],c[b[30]],c[b[31]]);
	fprintf(cake_fp,"\n%c %c %c %c ",c[b[24]],c[b[25]],c[b[26]],c[b[27]]);
	fprintf(cake_fp,"\n %c %c %c %c",c[b[20]],c[b[21]],c[b[22]],c[b[23]]);
	fprintf(cake_fp,"\n%c %c %c %c ",c[b[16]],c[b[17]],c[b[18]],c[b[19]]);
	fprintf(cake_fp,"\n %c %c %c %c",c[b[12]],c[b[13]],c[b[14]],c[b[15]]);
	fprintf(cake_fp,"\n%c %c %c %c ",c[b[8]],c[b[9]],c[b[10]],c[b[11]]);
	fprintf(cake_fp,"\n %c %c %c %c",c[b[4]],c[b[5]],c[b[6]],c[b[7]]);
	fprintf(cake_fp,"\n%c %c %c %c ",c[b[0]],c[b[1]],c[b[2]],c[b[3]]);
	fflush(cake_fp);
}

void printboard(struct pos p)
{
	int i;
	int free=~(p.bm|p.bk|p.wm|p.wk);
	int b[32];
	char c[15]="-wb      WB";
	for(i=0;i<32;i++)
	{
		if((p.bm>>i)%2)
			b[i]=BLACK;
		if((p.bk>>i)%2)
			b[i]=BLACK|KING;
		if((p.wm>>i)%2)
			b[i]=WHITE;
		if((p.wk>>i)%2)
			b[i]=WHITE|KING;
		if((free>>i)%2)
			b[i]=0;
	}
	/*        WHITE
	7  15  23  31
	3  11  19  27
	6  14  22  30
	2  10  18  26
	5  13  21  29
	1   9  17  25
	4  12  20  28
	0   8  16  24
	BLACK     */

	printf("\n\n");
	printf("\n %c %c %c %c",c[b[28]],c[b[29]],c[b[30]],c[b[31]]);
	printf("\n%c %c %c %c ",c[b[24]],c[b[25]],c[b[26]],c[b[27]]);
	printf("\n %c %c %c %c",c[b[20]],c[b[21]],c[b[22]],c[b[23]]);
	printf("\n%c %c %c %c ",c[b[16]],c[b[17]],c[b[18]],c[b[19]]);
	printf("\n %c %c %c %c",c[b[12]],c[b[13]],c[b[14]],c[b[15]]);
	printf("\n%c %c %c %c ",c[b[8]],c[b[9]],c[b[10]],c[b[11]]);
	printf("\n %c %c %c %c",c[b[4]],c[b[5]],c[b[6]],c[b[7]]);
	printf("\n%c %c %c %c ",c[b[0]],c[b[1]],c[b[2]],c[b[3]]);
}
#ifdef OPLIB
void boardtobitboard(int b[8][8], struct pos *position)
{
	/* initialize bitboard */
	int i,board[32];
	/*
	WHITE
	28  29  30  31
	24  25  26  27
	20  21  22  23
	16  17  18  19
	12  13  14  15
	8   9  10  11
	4   5   6   7
	0   1   2   3
	BLACK
	*/
	board[0]=b[0][0];board[1]=b[2][0];board[2]=b[4][0];board[3]=b[6][0];
	board[4]=b[1][1];board[5]=b[3][1];board[6]=b[5][1];board[7]=b[7][1];
	board[8]=b[0][2];board[9]=b[2][2];board[10]=b[4][2];board[11]=b[6][2];
	board[12]=b[1][3];board[13]=b[3][3];board[14]=b[5][3];board[15]=b[7][3];
	board[16]=b[0][4];board[17]=b[2][4];board[18]=b[4][4];board[19]=b[6][4];
	board[20]=b[1][5];board[21]=b[3][5];board[22]=b[5][5];board[23]=b[7][5];
	board[24]=b[0][6];board[25]=b[2][6];board[26]=b[4][6];board[27]=b[6][6];
	board[28]=b[1][7];board[29]=b[3][7];board[30]=b[5][7];board[31]=b[7][7];

	(*position).bm=0;
	(*position).bk=0;
	(*position).wm=0;
	(*position).wk=0;

	for(i=0;i<32;i++)
	{
		switch (board[i])
		{
		case BLACK|MAN:
			(*position).bm=(*position).bm|(1<<i);
			break;
		case BLACK|KING:
			(*position).bk=(*position).bk|(1<<i);
			break;
		case WHITE|MAN:
			(*position).wm=(*position).wm|(1<<i);
			break;
		case WHITE|KING:
			(*position).wk=(*position).wk|(1<<i);
			break;
		}
	}
}
#endif
