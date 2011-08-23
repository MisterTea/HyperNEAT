//		cake - a checkers engine			
//															
//		Copyright (C) 2001 -2004 by Martin Fierz		
//															
//		contact: checkers@fierz.ch					


#include "switches.h"

// prototypes for all functions in cakepp.c
void absolutehashkey(POSITION *p, HASH *hash);
int allscoresearch(SEARCHINFO *si, POSITION *p, CAKE_MOVE movelist[MAXMOVES], int d, CAKE_MOVE *best);
int bitcount(int32 n);
static int booklookup(POSITION *p, int *value, int depth, int32 *best, int *bestindex, char str[256]);
int cake_getmove(SEARCHINFO *si, POSITION *p, int how,double maxtime, int depthtosearch,int32 maxnodes, char str[1024], int *playnow, int logging,int reset);
void countmaterial(POSITION *p, MATERIALCOUNT *m);
int exitcake();
static int firstnegamax(SEARCHINFO *si, POSITION *p, CAKE_MOVE movelist[MAXMOVES], int d, int alpha, int beta, CAKE_MOVE *best);
static void getpv(SEARCHINFO *si, POSITION *p, char *str);
int hashlookup(SEARCHINFO *si, int *value, int *valuetype, int depth, int *bestindex, int color, int *ispvnode);
int hashreallocate(int MB);
void hashstore(SEARCHINFO *si, POSITION *p, int value, int alpha, int depth, CAKE_MOVE *best, int32 bestindex);
static void initboard(void);
int initcake(char str[1024]);

int LSB(int32 x);
static int mtdf(SEARCHINFO *si, POSITION *p, CAKE_MOVE movelist[MAXMOVES], int firstguess,int depth, CAKE_MOVE *best);
//static int negamax(SEARCHINFO *si, POSITION *p,int depth, int alpha, int beta, int32 *bestproto, 
//				   int *bestmoveindex, int truncationdepth, int truncationdepth2,int iid);
static int negamax(SEARCHINFO *si, POSITION *p,int depth, int alpha, int *bestproto, 
				   int *bestmoveindex, int truncationdepth, int truncationdepth2,int iid);
int pvhashlookup(SEARCHINFO *si, int *value, int *valuetype, int depth, int32 *forcefirst, int color, int *ispvnode);
void resetsearchinfo(SEARCHINFO *s);
int selfstalemate(POSITION *p);
int testcapture(POSITION *p);
void updatehashkey(CAKE_MOVE *m, HASH *h);
static int windowsearch(SEARCHINFO *si, POSITION *p, CAKE_MOVE movelist[MAXMOVES], int depth, int guess, CAKE_MOVE *best);

#ifdef USEDB
int isdbpos(POSITION *p, MATERIALCOUNT *m);
#endif

#ifdef LEARNUSE
void stufflearnpositions(void);
#endif

#ifdef LEARNSAVE
static void learn(POSITION *p, int value, int depth, int bestindex);
#endif

#ifdef SAFE
int safemoves(POSITION *p);
#endif


#ifdef ETC
int ETClookup(SEARCHINFO *si, POSITION *p, CAKE_MOVE movelist[MAXMOVES], int d, int n, int *bestindex);
#endif

#ifdef BOOKGEN
int bookgen(OLDPOSITION *p, int color, int *numberofmoves, int values[MAXMOVES], int how, int depth, double searchtime);
int bookmtdf(SEARCHINFO *si, POSITION *p, CAKE_MOVE movelist[MAXMOVES], int firstguess,int depth,CAKE_MOVE *best,int bestvaluesofar);
#endif

#ifdef QSEARCH
int qsearch(SEARCHINFO *si, POSITION *p, int alpha, int beta, int qsdepth);
int csearch(SEARCHINFO *si, POSITION *p, int alpha, int beta);
#endif

#ifdef QS_SQUEEZE
int issqueeze(POSITION *p, int color);
#endif

