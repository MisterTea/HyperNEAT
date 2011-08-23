#ifndef __CAKE_CAKEPP_H__
#define __CAKE_CAKEPP_H__

/* cake++.h */

/* prototypes for all functions in cake++ */
int initcake(int logging);
int exitcake(void);
int cake_getmove(struct pos *position,char moveNotation[256],int color, int how,double maxtime, int depthtosearch,int32 maxnodes, char str[255], int *playnow, int logging,int reset);
/* returns the value of the position */
void countmaterial(void);
void initboard(void);
int firstnegamax(int d, int color, int alpha, int beta, struct move *best);
int negamax(int depth, int color, int alpha, int beta, int32 *bestproto, int truncationdepth);
int evaluation(int color, int alpha, int beta);
int fineevaluation(int color);
int bitcount(int32 n);
int recbitcount(int32 n);
int lastbit(int32 x);
void absolutehashkey(void);
void updatehashkey(struct move m);
void hashstore(int value, int alpha, int beta, int depth, struct move best, int color);
int hashlookup(int *value, int *alpha, int *beta, int depth, int32 *best, int color);
void movetonotation(struct pos position,struct move m, char *str, int color);
void getpv(char *str, int color);
int testcapture(int color);
int dbwineval(int color);
int dblosseval(int color);

void printboardtofile(struct pos p);
void printboard(struct pos p);

#endif
