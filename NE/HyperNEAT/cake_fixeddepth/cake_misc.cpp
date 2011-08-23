#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
//#include <winbase.h>
#include <assert.h>
#include "structs.h"
#include "consts.h"
#include "switches.h"
#include "cakepp.h"
#include "move_gen.h"




void searchinfotostring(char *out, int depth, double time, char *valuestring, char *pvstring, SEARCHINFO *si)
	{
	char percent='%';

	sprintf(out,"depth %i/%i/%.1f  time %.2fs  %s  nodes %u  %ikN/s  db %.0f%c cut %.1f%c %s", 
		(depth/FRAC), si->maxdepth, (float)si->leafdepth/((float)si->leaf+0.01) , time, valuestring, 
		si->negamax, (int)((float)si->negamax/(time+0.01)/1000),
		100*((float)si->dblookupsuccess/(float)(si->dblookupsuccess+si->dblookupfail+1)),
		percent, 100.0*(float)(si->cutoffsatfirst)/(float)(si->cutoffs), percent,	
		pvstring); 
	}


void movetonotation(POSITION *p,CAKE_MOVE *m, char *str)
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
	// TODO: i just wrote a function that does this, i can get rid of this table.

	if(p->color==BLACK)
		{
		if(m->wk|m->wm) c='x';
		else c='-';                      /* capture or normal ? */
		from = (m->bm|m->bk)&(p->bm|p->bk);    /* bit set on square from */
		to = (m->bm|m->bk)&(~(p->bm|p->bk));
		from = LSB(from);
		to = LSB(to);
		from = square[from];
		to = square[to];
		sprintf(str,"%2i%c%2i",from,c,to);
		}
	else
		{
		if(m->bk|m->bm) c='x';
		else c='-';                      /* capture or normal ? */
		from=(m->wm|m->wk)&(p->wm|p->wk);    /* bit set on square from */
		to=  (m->wm|m->wk)&(~(p->wm|p->wk));
		from=LSB(from);
		to=LSB(to);
		from=square[from];
		to=square[to];
		sprintf(str,"%2i%c%2i",from,c,to);
		}
	return;
	}



void printint32(int32 x)
	{
	int i;
	int b[32];
	char c[15]="-x";
	for(i=0;i<32;i++)
		{
		if(((x>>i)%2) != 0)
			b[i]=1;
		else
			b[i] = 0;
		}

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



void printboard(POSITION *p)
	{
	int i;
	int free=~(p->bm|p->bk|p->wm|p->wk);
	int b[32];
	char c[15]="-wb      WB";
	for(i=0;i<32;i++)
		{
		if((p->bm>>i)%2)
			b[i]=BLACK;
		if((p->bk>>i)%2)
			b[i]=BLACK|KING;
		if((p->wm>>i)%2)
			b[i]=WHITE;
		if((p->wk>>i)%2)
			b[i]=WHITE|KING;
		if((free>>i)%2)
			b[i]=0;
		}

	printf("\n\n");
	printf("\n %c %c %c %c",c[b[28]],c[b[29]],c[b[30]],c[b[31]]);
	printf("\n%c %c %c %c ",c[b[24]],c[b[25]],c[b[26]],c[b[27]]);
	printf("\n %c %c %c %c",c[b[20]],c[b[21]],c[b[22]],c[b[23]]);
	printf("\n%c %c %c %c ",c[b[16]],c[b[17]],c[b[18]],c[b[19]]);
	printf("\n %c %c %c %c",c[b[12]],c[b[13]],c[b[14]],c[b[15]]);
	printf("\n%c %c %c %c ",c[b[8]],c[b[9]],c[b[10]],c[b[11]]);
	printf("\n %c %c %c %c",c[b[4]],c[b[5]],c[b[6]],c[b[7]]);
	printf("\n%c %c %c %c ",c[b[0]],c[b[1]],c[b[2]],c[b[3]]);

	if(p->color == BLACK)
		printf("\nblack to move");
	else
		printf("\nwhite to move");
	}


void printboardtofile(POSITION *p, FILE *fp)
	{
	int i;
	int free=~(p->bm|p->bk|p->wm|p->wk);
	int b[32];
	char c[15]="-wb      WB";

	FILE *Lfp;
	
	if(fp != NULL)
		Lfp = fp;
	else
		Lfp = fopen("cakelog.txt","a");

	for(i=0;i<32;i++)
		{
		if((p->bm>>i)%2)
			b[i]=BLACK;
		if((p->bk>>i)%2)
			b[i]=BLACK|KING;
		if((p->wm>>i)%2)
			b[i]=WHITE;
		if((p->wk>>i)%2)
			b[i]=WHITE|KING;
		if((free>>i)%2)
			b[i]=0;
		}

	fprintf(Lfp,"\n\n");
	fprintf(Lfp,"\n %c %c %c %c",c[b[28]],c[b[29]],c[b[30]],c[b[31]]);
	fprintf(Lfp,"\n%c %c %c %c ",c[b[24]],c[b[25]],c[b[26]],c[b[27]]);
	fprintf(Lfp,"\n %c %c %c %c",c[b[20]],c[b[21]],c[b[22]],c[b[23]]);
	fprintf(Lfp,"\n%c %c %c %c ",c[b[16]],c[b[17]],c[b[18]],c[b[19]]);
	fprintf(Lfp,"\n %c %c %c %c",c[b[12]],c[b[13]],c[b[14]],c[b[15]]);
	fprintf(Lfp,"\n%c %c %c %c ",c[b[8]],c[b[9]],c[b[10]],c[b[11]]);
	fprintf(Lfp,"\n %c %c %c %c",c[b[4]],c[b[5]],c[b[6]],c[b[7]]);
	fprintf(Lfp,"\n%c %c %c %c ",c[b[0]],c[b[1]],c[b[2]],c[b[3]]);
	fclose(Lfp);
	}


#define USE_LOG_TO_FILE (0)

int logtofile(char *str)
{
#if USE_LOG_TO_FILE
	// log a string to the engine logfile "cakelog.txt"
	FILE *fp;

	printf("%s\n",str);
	fp = fopen("cakelog.txt","a");
	if(fp != NULL)
		{
		fprintf(fp,"\n%s",str);
		fclose(fp);
		return 1;
		}
#endif
	return 0;
}

int isforced(POSITION *p)
// determines if color should move immediately
	{
	int i,n;
	CAKE_MOVE ml1[MAXMOVES];
	POSITION q1,q2;
	int values[MAXMOVES];
	int bestindex=0;

	// forced moves occur if a) only one capture move exist
	// and b) if 2 capture moves exist which transpose with 2 moves, then 4 single captures
	n = makecapturelist(p, ml1, values, bestindex);
	if(n==1)
		return 1;

	if(n!=2)
		return 0;

	// if we arrive here, there are two capture moves.
	// copy position so we dont wreck anything
	q1 = (*p);
	q2=q1;

	togglemove((&q1),ml1[0]);
	togglemove((&q2),ml1[1]);

	for(i=0;i<3;i++)
		{
		n = makecapturelist(&q1, ml1, values, bestindex);
		if(n!=1)
			return 0;
		togglemove((&q1), ml1[0]);
		}

	for(i=0;i<3;i++)
		{
		n = makecapturelist(&q2, ml1, values, bestindex);
		if(n!=1)
			return 0;
		togglemove((&q2), ml1[0]);
		}

	if(q1.bk==q2.bk && q1.bm==q2.bm && q1.wm==q2.wm && q1.wk==q2.wk)
		return 1;

	return 0;
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
	  0   1   2   3                4   3   2    1 etc
	      BLACK
	*/


	// a function to get a board number and return the bit number

int SquareToBit(int square)
{
	int x,y;
	int bit;

	square--;

	x = square % 4;
	y = square/4;

	bit = 4*y + (3-x);

	return bit;
}


/*
int32 myrand(void)
{
// myrand produces a 32-bit random number 
// rand() returns a 15-bit random number, not 16,
//	that's the reason i only take the last 8 bits instead of 16
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
*/
