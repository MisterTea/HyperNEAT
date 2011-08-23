#include <string>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <exception>
#include <stdexcept>
using namespace std;

/* movegen.c is the movegenerator for the bitboard checkers engine		*/
/* the move structure consists of 4 int32's to toggle the position with */
// august 2007: move_gen.c needs extern searchinfo si for ordereval - not thread-safe.

#define MCV 1
#define KCV 2
#define PVVAL 3
#define KINGCAPT 13
#define MANCAPT 10

#include <assert.h>
#include "structs.h"
#include "consts.h"
#include "move_gen.h"
#include "cakepp.h"
#include "switches.h"
#include "cake_misc.h"


/* values used for dynamic move ordering */
#define HASHMOVE 1<<30
#define KILLER 1<<29
/* values used for static move ordering */
#define C4 0x00042000   /*innermost squares */
#define C3 0x00624600   /* center squares */
//#define C3 0x00666600   // alternate definition
#define C1 0xF181818F   /* edge squares */
#define PROM 30     /*20*/    /* a promotion */
#define PROM1 8   /*18 */    /* far down the board */
#define PROM2 3   /*16 */
#define GIVEUPBACK 12 /*12*/
#define MANC3VAL 2
#define MANC4VAL 4
#define KINGC3VAL 5
#define KINGC4VAL 10
#define KINGC1VAL -10
#define CAPT 50
#define HISTORY	300			//300
#define MINHASH 100
#define DEVSINGLE 4
#define GIVEUPOREO 10


#pragma warning( disable : 4146 ) // disable warning that i compute -x for an unsigned int x

int getorderedmovelist(POSITION *p, CAKE_MOVE movelist[MAXMOVES])
	{
	// generates a movelist or a capturelist, and orders it according to static move ordering.

	int i,j,n;
	int values[MAXMOVES];
	SEARCHINFO si; // dummy searchinfo, not used for anything
	int forcefirst = MAXMOVES-1;
	CAKE_MOVE tmpmove;
	int bestvalue;
	int tmpvalue;
	int bestindex; 

	resetsearchinfo(&si);

	//for(i=0;i<MAXMOVES;i++)
	//	values[i] = (p->color==BLACK) ? -MATE:MATE;

	n = makecapturelist(p, movelist, values, forcefirst);
	if(n == 0)
		n = makemovelist(&si, p,movelist,values,forcefirst,0);

	if(n == 0)
		return 0;

	// now order movelist
	for(i = 0; i < n; i++)
		{
		bestvalue = -MATE;
		// find index with highest value
		for(j = i; j<n; j++)
			{
			if(values[i] > bestvalue)
				{
				bestvalue = values[i];
				bestindex = i;
				}
			}
		// move it to front of list
		tmpmove = movelist[i];
		tmpvalue = values[i];
		movelist[i] = movelist[bestindex];
		values[i] = values[bestindex];
		movelist[bestindex] = tmpmove;
		values[bestindex] = tmpvalue;
		}

	// set sentinel move
	movelist[n].bk = 0;
	movelist[n].bm = 0;
	movelist[n].wm = 0;
	movelist[n].wk = 0;
	
	return n;
	}

int numberofmoves(CAKE_MOVE m[MAXMOVES])
	{
	int i;

	for(i = 0; i<MAXMOVES; i++)
		{
		if(m[i].bm == 0 && m[i].bk == 0 && m[i].wm == 0 && m[i].wk == 0)
			return i;
		}

	// we should never arrive here...
	assert(0);

	return MAXMOVES;
	}

#define IS_IN_BOUNDS(X,Y) ((X)>=0&&(Y)>=0&&(X)<8&&(Y)<8)
typedef unsigned char uchar;

bool CheckersCommon_hasMove(
							 uchar b[8][8],
							 int color,
							 int x,
							 int y,
							 int deltax,
							 int deltay
							 )
{
	if (IS_IN_BOUNDS(x+deltax,y+deltay))
	{
		if (b[x+deltax][y+deltay]&(MAN|KING))
		{
			//A piece exists in the square we could move to, can we jump it?

			if (
				IS_IN_BOUNDS(x+deltax*2,y+deltay*2) && //must be in bounds
				(!(b[x+deltax][y+deltay]&color)) && //can't jump your own color
				(!(b[x+deltax*2][y+deltay*2]&(MAN|KING))) //must have an empty square behind
				)
			{
				return true;
			}
		}
		else
		{
			//No piece exists, a move is possible
			return true;
		}
	}

	return false;
}

bool CheckersCommon_hasMove(
							 uchar fromB[8][8],
							 int color,
							 uchar toB[8][8]
)
{
	int moveFromX,moveFromY,moveToX,moveToY;

	for (int x=0;x<8;x++)
	{
		for (int y=(x%2);y<8;y+=2)
		{
			if ( (fromB[x][y]&color)>0 && (toB[x][y]&color)==0 )
			{
				moveFromX = x;
				moveFromY = y;
			}
			else if ( (fromB[x][y]&color)==0 && (toB[x][y]&color)>0 )
			{
				moveToX = x;
				moveToY = y;
			}
		}
	}

	int deltaMoveX = moveToX-moveFromX;
	int deltaMoveY = moveToY-moveFromY;

	return CheckersCommon_hasMove(fromB,color,moveFromX,moveFromY,deltaMoveX,deltaMoveY);
}

void CheckersCommon_getDestination(
							 uchar fromB[8][8],
							 int color,
							 uchar toB[8][8],
                             int &toX,
                             int &toY
)
{
	for (int x=0;x<8;x++)
	{
		for (int y=(x%2);y<8;y+=2)
		{
			if ( (fromB[x][y]&color)==0 && (toB[x][y]&color)>0 )
			{
				toX = x;
				toY = y;
                return;
			}
		}
	}

    throw std::runtime_error("OOPS");
}

void ucharbitboardtoboard(POSITION position,unsigned char b[8][8])
{
    /* return a board from a bitboard */
    int i,board[32];

    for(i=0;i<32;i++)
    {
        if (position.bm & (1<<i))
            board[i]=(BLACK|MAN);
        if (position.bk & (1<<i))
            board[i]=(BLACK|KING);
        if (position.wm & (1<<i))
            board[i]=(WHITE|MAN);
        if (position.wk & (1<<i))
            board[i]=(WHITE|KING);
        if ( (~(position.bm|position.bk|position.wm|position.wk)) & (1<<i))
            board[i]=0;
    }
    /* return the board */
    b[0][0]=board[0];b[2][0]=board[1];b[4][0]=board[2];b[6][0]=board[3];
    b[1][1]=board[4];b[3][1]=board[5];b[5][1]=board[6];b[7][1]=board[7];
    b[0][2]=board[8];b[2][2]=board[9];b[4][2]=board[10];b[6][2]=board[11];
    b[1][3]=board[12];b[3][3]=board[13];b[5][3]=board[14];b[7][3]=board[15];
    b[0][4]=board[16];b[2][4]=board[17];b[4][4]=board[18];b[6][4]=board[19];
    b[1][5]=board[20];b[3][5]=board[21];b[5][5]=board[22];b[7][5]=board[23];
    b[0][6]=board[24];b[2][6]=board[25];b[4][6]=board[26];b[6][6]=board[27];
    b[1][7]=board[28];b[3][7]=board[29];b[5][7]=board[30];b[7][7]=board[31];
}

void CheckersCommon_printBoard(uchar b[8][8])
{
    const char *f = "-    wb  WB      ";
    bool error=false;
    for (int y=0;y<8;y++)
    {
        for (int x=0;x<8;x++)
        {
            if (x==0 && y==1)
            {
                cout << "  ";
                continue;
            }
            if (x==0 && y==3)
            {
                cout << "  ";
                continue;
            }

            cout << " " << f[b[x][y]];
            if (((y+x) % 2 == 1) && f[b[x][y]]!=' ')
            {
                error=true;
            }
        }
        cout << endl;
    }
    cout << endl;
    if (error)
    {
		cout << "ERROR: INVALID BOARD!\n";
    }
}

CheckersAdvisor *checkersAdvisor = NULL;

int makemovelist(SEARCHINFO *si, POSITION *p, CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES], int bestindex, int32 killer)
{

	// generates moves for position p in the movelist - sets ordering values for the moves, adds a high
	// ordering value to bestindex, and searches for the killer move. if it finds the killer move, it
	// also gives that a high value.
	int32 i,n=0,free;
	int32 m,tmp;
	POSITION testpos;

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
	free=~(p->bm|p->bk|p->wm|p->wk);
	if(p->color == BLACK)
	{
		if(p->bk)
		{
			/* moves left forwards */
			/* I: columns 1357 */
			m=((p->bk&LF1)<<3)&free;
			/* now m contains a bit for every free square where a black king can move*/
			while(m)
			{
				tmp=(m&-m); /* least significant bit of m */
				tmp=tmp|(tmp>>3);
				movelist[n].bm=0;
				movelist[n].bk=tmp;
				movelist[n].wm=0;
				movelist[n].wk=0;
				values[n]=0;
				n++;
				m=m&(m-1);   /* clears least significant bit of m */
			}
			/* II: columns 2468 */
			m=((p->bk&LF2)<<4)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp>>4);
				movelist[n].bm=0;
				movelist[n].bk=tmp;
				movelist[n].wm=0;
				movelist[n].wk=0;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
			/* moves right forwards */
			/* I: columns 1357 */
			m=((p->bk&RF1)<<4)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp>>4);
				movelist[n].bm=0;
				movelist[n].bk=tmp;
				movelist[n].wm=0;
				movelist[n].wk=0;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
			/* II: columns 2468 */
			m=((p->bk&RF2)<<5)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp>>5);
				movelist[n].bm=0;
				movelist[n].bk=tmp;
				movelist[n].wm=0;
				movelist[n].wk=0;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
			/* moves left backwards */
			/* I: columns 1357 */
			m=((p->bk&LB1)>>5)&free;
			/* now m contains a bit for every free square where a black man can move*/
			while(m)
			{
				tmp=(m&-m); /* least significant bit of m */
				tmp=tmp|(tmp<<5);
				movelist[n].bm=0;
				movelist[n].bk=tmp;
				movelist[n].wm=0;
				movelist[n].wk=0;
				values[n]=0;
				n++;
				m=m&(m-1);   /* clears least significant bit of m */
			}
			/* II: columns 2468 */
			m=((p->bk&LB2)>>4)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp<<4);
				movelist[n].bm=0;
				movelist[n].bk=tmp;
				movelist[n].wm=0;
				movelist[n].wk=0;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
			/* moves right backwards */
			/* I: columns 1357 */
			m=((p->bk&RB1)>>4)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp<<4);
				movelist[n].bm=0;
				movelist[n].bk=tmp;
				movelist[n].wm=0;
				movelist[n].wk=0;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
			/* II: columns 2468 */
			m=((p->bk&RB2)>>3)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp<<3);
				movelist[n].bm=0;
				movelist[n].bk=tmp;
				movelist[n].wm=0;
				movelist[n].wk=0;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
		}
		/* moves with black stones:*/
		if(p->bm)
		{
			/* moves left forwards */
			/* I: columns 1357: just moves */
			m=((p->bm&LF1)<<3)&free;
			/* now m contains a bit for every free square where a black man can move*/
			while(m)
			{
				tmp=(m&-m); /* least significant bit of m */
				tmp=tmp|(tmp>>3); /* square where man came from */
				movelist[n].bm=tmp&NWBR; /* NWBR: not white back rank */
				movelist[n].bk=tmp&WBR; /*if stone moves to WBR (white back rank) it's a king*/
				movelist[n].wm=0;
				movelist[n].wk=0;
				values[n]=0;
				n++;
				m=m&(m-1);   /* clears least significant bit of m */
			}

			/* II: columns 2468 */
			m=((p->bm&LF2)<<4)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp>>4);
				movelist[n].bm=tmp;
				movelist[n].bk=0;
				movelist[n].wm=0;
				movelist[n].wk=0;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
			/* moves right forwards */
			/* I: columns 1357 :just moves*/
			m=((p->bm&RF1)<<4)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp>>4);
				movelist[n].bm=tmp&NWBR;
				movelist[n].bk=tmp&WBR;
				movelist[n].wm=0;
				movelist[n].wk=0;
				values[n]=0;
				n++;
				m=m&(m-1);
			}

			/* II: columns 2468 */
			m=((p->bm&RF2)<<5)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp>>5);
				movelist[n].bm=tmp;
				movelist[n].bk=0;
				movelist[n].wm=0;
				movelist[n].wk=0;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
		}
#ifndef MOVEORDERING
		return n;
#endif

#ifdef MOHASH
		/* give the forcefirst-move a high eval */
		values[bestindex] += HASHMOVE;
#endif
#ifdef MOKILLER
		if(n>1)
		{
			/* give the forcefirst-move a high eval */
			if(killer)
			{
				for(i=0;i<n;i++)
				{
					if((movelist[i].bm|movelist[i].bk) == killer)
					{
						values[i] += KILLER;
						break;
					}
				}
			}
		}
#endif

		/* do a static evaluation of the moves */
		if(n>1)
			blackorderevaluation(si, p, movelist, values, n);


		for(int i=0;i<int(n);i++)
		{
			domove(testpos,p,movelist[i]);
			testpos.color = p->color^CC;

			unsigned char fromB[8][8];
			ucharbitboardtoboard(*p,fromB);

			unsigned char toB[8][8];
			ucharbitboardtoboard(testpos,toB);

			if(false && CheckersCommon_hasMove(fromB,p->color,toB)==false)
			{
				printf("ERROR: INVALID MOVE:\n");
				printboard(p);
				printf("**********\n");
				printboard(&testpos);
				printf("DOES NOT MATCH:\n");
				CheckersCommon_printBoard(fromB);
				printf("**********\n");
				CheckersCommon_printBoard(toB);
				throw std::runtime_error("Error");
			}
		}

        if(checkersAdvisor && si->realdepth>2)
        {
            float advisorMoveValue[MAXMOVES];

			unsigned char fromB[8][8];
			ucharbitboardtoboard(*p,fromB);

            checkersAdvisor->setBoardPosition(fromB);

            for(int a=0;a<int(n);a++)
            {
			    unsigned char fromB[8][8];
			    ucharbitboardtoboard(*p,fromB);

		        domove(testpos,p,movelist[a]);
		        testpos.color = p->color^CC;

		        unsigned char toB[8][8];
		        ucharbitboardtoboard(testpos,toB);

                int destX,destY;
                CheckersCommon_getDestination(fromB,p->color,toB,destX,destY);

                advisorMoveValue[a] = checkersAdvisor->getBoardValue(destX,destY);
            }

            for(int a=0;a<int(n);a++)
            {
                for(int b=0;b<int(n)-(a+1);b++)
                {
                    if(advisorMoveValue[b]>advisorMoveValue[b+1])
                    {
                        swap(advisorMoveValue[b],advisorMoveValue[b+1]);
                        swap(movelist[b],movelist[b+1]);
                        swap(values[b],values[b+1]);
                    }
                }
            }

            int32 newn = n;
            if(n>2) newn--;

            if(n-newn)
            {
                //printf("%d moves pruned\n",(n-newn));
            }

			for(int a=int(newn);a<int(n);a++)
			{
				movelist[a].bm = movelist[a].bk = movelist[a].wm = movelist[a].wk = 0;
			}

			n = newn;
        }

		return n;
	}
	/* ****************************************************************/
	else     /* color is WHITE */
		/******************************************************************/
	{
		/* moves with white kings:*/
		if(p->wk)
		{
			/* moves left forwards */
			/* I: columns 1357 */
			m=((p->wk&LF1)<<3)&free;
			/* now m contains a bit for every free square where a black man can move*/
			while(m)
			{
				tmp=(m&-m); /* least significant bit of m */
				tmp=tmp|(tmp>>3);
				movelist[n].bm=0;
				movelist[n].bk=0;
				movelist[n].wm=0;
				movelist[n].wk=tmp;
				values[n]=0;
				n++;
				m=m&(m-1);   /* clears least significant bit of m */
			}
			/* II: columns 2468 */
			m=((p->wk&LF2)<<4)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp>>4);
				movelist[n].bm=0;
				movelist[n].bk=0;
				movelist[n].wm=0;
				movelist[n].wk=tmp;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
			/* moves right forwards */
			/* I: columns 1357 */
			m=((p->wk&RF1)<<4)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp>>4);
				movelist[n].bm=0;
				movelist[n].bk=0;
				movelist[n].wm=0;
				movelist[n].wk=tmp;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
			/* II: columns 2468 */
			m=((p->wk&RF2)<<5)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp>>5);
				movelist[n].bm=0;
				movelist[n].bk=0;
				movelist[n].wm=0;
				movelist[n].wk=tmp;
				values[n]=0;
				n++;
				m=m&(m-1);
			}

			/* moves left backwards */
			/* I: columns 1357 */
			m=((p->wk&LB1)>>5)&free;
			/* now m contains a bit for every free square where a black man can move*/
			while(m)
			{
				tmp=(m&-m); /* least significant bit of m */
				tmp=tmp|(tmp<<5);
				movelist[n].bm=0;
				movelist[n].bk=0;
				movelist[n].wm=0;
				movelist[n].wk=tmp;
				values[n]=0;
				n++;
				m=m&(m-1);   /* clears least significant bit of m */
			}
			/* II: columns 2468 */
			m=((p->wk&LB2)>>4)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp<<4);
				movelist[n].bm=0;
				movelist[n].bk=0;
				movelist[n].wm=0;
				movelist[n].wk=tmp;
				values[n]=0;
				n++;
				m=m&(m-1);
			}

			/* moves right backwards */
			/* I: columns 1357 */
			m=((p->wk&RB1)>>4)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp<<4);
				movelist[n].bm=0;
				movelist[n].bk=0;
				movelist[n].wm=0;
				movelist[n].wk=tmp;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
			/* II: columns 2468 */
			m=((p->wk&RB2)>>3)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp<<3);
				movelist[n].bm=0;
				movelist[n].bk=0;
				movelist[n].wm=0;
				movelist[n].wk=tmp;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
		}

		/* moves with white stones:*/
		if(p->wm)
		{
			/* moves left backwards */
			/* II: columns 2468 ;just moves*/
			m=((p->wm&LB2)>>4)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp<<4);
				movelist[n].bm=0;
				movelist[n].bk=0;
				movelist[n].wm=tmp&NBBR;
				movelist[n].wk=tmp&BBR;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
			/* I: columns 1357 */
			m=((p->wm&LB1)>>5)&free;
			/* now m contains a bit for every free square where a white man can move*/
			while(m)
			{
				tmp=(m&-m); /* least significant bit of m */
				tmp=tmp|(tmp<<5);
				movelist[n].bm=0;
				movelist[n].bk=0;
				movelist[n].wm=tmp;
				movelist[n].wk=0;
				values[n]=0;

				n++;
				m=m&(m-1);   /* clears least significant bit of m */
			}

			/* moves right backwards */

			/* II: columns 2468 : just the moves*/
			m=((p->wm&RB2)>>3)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp<<3);
				movelist[n].bm=0;
				movelist[n].bk=0;
				movelist[n].wm=tmp&NBBR;
				movelist[n].wk=tmp&BBR;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
			/* I: columns 1357 */
			m=((p->wm&RB1)>>4)&free;
			while(m)
			{
				tmp=(m&-m);
				tmp=tmp|(tmp<<4);
				movelist[n].bm=0;
				movelist[n].bk=0;
				movelist[n].wm=tmp;
				movelist[n].wk=0;
				values[n]=0;
				n++;
				m=m&(m-1);
			}
		}
#ifndef MOVEORDERING
		return n;
#endif
#ifdef MOHASH
		/* sort moves first */
		values[bestindex] += HASHMOVE;

#endif
#ifdef MOKILLER
		if(n>1)
		{
			/* give the forcefirst-move a high eval */
			if(killer)
			{
				for(i=0;i<n;i++)
				{
					if((movelist[i].wm|movelist[i].wk) == killer)
					{
						values[i] += KILLER;

						break;
					}
				}
			}
		}
#endif

		// TODO: if we have a hashmove or a killer move, we could defer the orderevaluation part,
		// because all values assigned there are lower than hash/killer.

		/* in this case we have to order the list */
		if(n>1) 
			whiteorderevaluation(si, p, movelist, values, n);

		for(int i=0;i<int(n);i++)
		{
			domove(testpos,p,movelist[i]);
			testpos.color = p->color^CC;

			unsigned char fromB[8][8];
			ucharbitboardtoboard(*p,fromB);

			unsigned char toB[8][8];
			ucharbitboardtoboard(testpos,toB);

			if(false && CheckersCommon_hasMove(fromB,p->color,toB)==false)
			{
				printf("ERROR: INVALID MOVE:\n");
				printboard(p);
				printf("**********\n");
				printboard(&testpos);
				printf("DOES NOT MATCH:\n");
				CheckersCommon_printBoard(fromB);
				printf("**********\n");
				CheckersCommon_printBoard(toB);
				throw std::runtime_error("Error");
			}
		}

        if(checkersAdvisor && si->realdepth>2)
        {
            float advisorMoveValue[MAXMOVES];

			unsigned char fromB[8][8];
			ucharbitboardtoboard(*p,fromB);

            checkersAdvisor->setBoardPosition(fromB);

            for(int a=0;a<int(n);a++)
            {
			    unsigned char fromB[8][8];
			    ucharbitboardtoboard(*p,fromB);

		        domove(testpos,p,movelist[a]);
		        testpos.color = p->color^CC;

		        unsigned char toB[8][8];
		        ucharbitboardtoboard(testpos,toB);

                int destX,destY;
                CheckersCommon_getDestination(fromB,p->color,toB,destX,destY);

                advisorMoveValue[a] = checkersAdvisor->getBoardValue(destX,destY);
            }

            for(int a=0;a<int(n);a++)
            {
                for(int b=0;b<int(n)-(a+1);b++)
                {
                    if(advisorMoveValue[b]>advisorMoveValue[b+1])
                    {
                        swap(advisorMoveValue[b],advisorMoveValue[b+1]);
                        swap(movelist[b],movelist[b+1]);
                        swap(values[b],values[b+1]);
                    }
                }
            }

            int32 newn = n;
            if(n>2) newn--;

            if(n-newn)
            {
                //printf("%d moves pruned\n",(n-newn));
            }

			for(int a=int(newn);a<int(n);a++)
			{
				movelist[a].bm = movelist[a].bk = movelist[a].wm = movelist[a].wk = 0;
			}

			n = newn;
        }

		return n;
	}

}


void blackorderevaluation(SEARCHINFO *si, POSITION *p,CAKE_MOVE ml[MAXMOVES],int values[MAXMOVES],int n)
	{
	int eval;
	int32 from,to;
	int32 black;
	int i;
	extern char blackbackrankeval[256];
	//	eval += (blackbackrankeval[p->bm & 0xFF] - whitebackrankeval[p->wm >> 24]); //2!!


#ifdef MOHISTORY
	extern int32 history[32][32]; /*has entries for how often a move was good */
	//extern SEARCHINFO si;        /* is the number of entries in history list */
#endif
	
	black = p->bm|p->bk;

	for(i=0;i<n;i++)
		{
   		eval=128;
  	
		from=(ml[i].bm|ml[i].bk)&black;
		to=(ml[i].bm|ml[i].bk)&(~black);

#ifdef MOHISTORY
		/* history...*/
		if(si->hashstores>MINHASH) 
			eval+=( (HISTORY*history[LSB(from)][LSB(to)]) / (si->hashstores));  // vtune: if is loopindependent - take out
#endif

#ifdef MOSTATIC
   
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
	if(ml[i].bm)
      	{
        /* man going down the board */
		// add elseifs here, should increase speed.
		if(to&0xF0000000)
			eval+=PROM;
      	else 
			{
			if(to&C3)
      			eval+=MANC3VAL;
   		
			if(to&0x0F000000)
      			eval+=PROM1;
			else
				{
				if(to&0x00F00000)
         			eval+=PROM2;
				else if(to&C4)
      				eval+=MANC4VAL;
				}
			}
      	
		
        // centralization for men 
        if(from & C3)
      		eval -= MANC3VAL;
		else 
			{
			if(from & 0xFF)
				{
				/* man giving up back rank */
				//if(from&0x0000000E)
         		//	eval -= GIVEUPBACK;
				eval -= blackbackrankeval[p->bm & 0xFF];
				eval += blackbackrankeval[ (p->bm ^ ml[i].bm) & 0xFF];
				}
			else
				{
				if(from&C4)
					eval-=MANC4VAL;
				}
			}
        }

	if(ml[i].bk)
      	{
        /* it's a move with a king */
        if(to&C1)
         	eval+=KINGC1VAL;
        else
			{
			if(to&C3)
        		eval+=KINGC3VAL;
			else if(to&C4)
        		eval+=KINGC4VAL;
			}

        if(from&C4)
         	eval-=KINGC4VAL;
		else
			{
			if(from&C3)
         		eval-=KINGC3VAL;
			else if(from&C1)
         		eval-=KINGC1VAL;
			}
        }

   	/*    (white)
   				 37  38  39  40
              32  33  34  35
                28  29  30  31
              23  24  25  26
                19  20  21  22
              14  15  16  17
                10  11  12  13
               5   6   7   8
         (black)   */
#endif
#ifdef MOTESTCAPT
       /* toggle move */
       p->bm^=ml[i].bm;
       p->bk^=ml[i].bk;
	   p->color ^= CC;
       if(testcapture(p)) 
			eval-=CAPT;
#ifdef MOTESTCAPT2
       else
       		{
			p->color^=CC;
			if(testcapture(p)) 
				eval+=CAPT;
			p->color^=CC;
			}
       /* toggle move */
#endif
       p->bm^=ml[i].bm;
       p->bk^=ml[i].bk;
	   p->color ^= CC;
#endif
   		values[i]+=eval;
   		}
   return;
   }

void whiteorderevaluation(SEARCHINFO *si, POSITION *p,CAKE_MOVE ml[MAXMOVES],int values[MAXMOVES],int n)
	{
	int eval;
	int32 from,to;
	int32 white;
	int i;

	extern int32 history[32][32];
	//extern SEARCHINFO si;
	extern char whitebackrankeval[256];
	

	white = p->wm|p->wk;

	for(i=0;i<n;i++)
		{
   		eval=128;
      
		from=( (ml[i].wm)|(ml[i].wk) )&white;
		to=((ml[i].wm)|(ml[i].wk))&(~white);
#ifdef MOHISTORY
		/* history...*/
		if(si->hashstores > MINHASH)
			eval+=( (HISTORY*history[LSB(from)][LSB(to)]) / (si->hashstores));
#endif

#ifdef MOSTATIC
   		if(ml[i].wm)
      		{
         /* man going down the board */
			if(to&0x00000F00)
         		eval+=PROM2;
			else
				{
				if(to&0x000000F0)
      				eval+=PROM1;
				else if(to&0x0000000F)
					eval+=PROM;
				}

			/* centralization for men */
			if(to&C3)
      			eval+=MANC3VAL;
   			else if(to&C4)
      			eval+=MANC4VAL;
      		
			/* man giving up back rank */
			if(from&C3)
      			eval-=MANC3VAL;
			else
				{
      			if(from & 0xFF000000)
					{
					eval -= whitebackrankeval[p->wm >> 24];
					eval += whitebackrankeval[ (p->wm ^ ml[i].wm) >> 24];
					}
				//if(from&0x70000000)
         		//	eval -= GIVEUPBACK;
				else if(from&C4)
      				eval -= MANC4VAL;
				}
			}

		if(ml[i].wk)
      		{
			/* it's a move with a king */
			if(to&C1)
         		eval+=KINGC1VAL;
			else
				{
				if(to&C3)
         			eval+=KINGC3VAL;
				else if(to&C4)
         			eval+=KINGC4VAL;
				}

			if(from&C1)
         		eval-=KINGC1VAL;
			else
				{
				if(from&C3)
         			eval-=KINGC3VAL;
				else if(from&C4)
         			eval-=KINGC4VAL;
				}
			
			}      /* toggle move */
#endif
#ifdef MOTESTCAPT
		p->wm^=ml[i].wm;
		p->wk^=ml[i].wk;
		p->color ^= CC;
		if(testcapture(p)) 
			eval-=CAPT;
#ifdef MOTESTCAPT2
		else
       		{
			p->color ^= CC;
			if(testcapture(p)) 
				eval+=CAPT;
			p->color ^= CC;
			}
#endif
		/* toggle move */
		p->wm^=ml[i].wm;
		p->wk^=ml[i].wk;
		p->color ^= CC;
#endif
		values[i]+=eval;
   		}
   return;
   }


#ifdef QSEARCH

int makeQSmovelist(POSITION *p, CAKE_MOVE movelist[MAXMOVES])
	{
	int32 n=0,free;
	int32 tmp;
	int32 captures, shots = 0;

	free=~(p->bm|p->bk|p->wm|p->wk);
  
	//     WHITE
	//  28  29  30  31
	//24  25  26  27
	//  20  21  22  23
	//16  17  18  19
	//  12  13  14  15
	// 8   9  10  11
	//   4   5   6   7
	// 0   1   2   3
	//     BLACK
	if(p->color==BLACK)
		{
		// generate only moves that could be shots, no other moves!
		// right forward potential shots
		// find the following  2 patterns sort of simultaneously:
		// the first 5 items from downside are the same, first
		// detect these, then still the 2 different ends above
		// 
		//          fr        fr
		//        wm            wm
		//      wm                wm
		//    fr  wm            fr  wm
		//  b  --   b         b       b
		captures = p->bm & leftbackward(free);
		tmp = leftbackward(p->wm);
		captures &= left(tmp);

		tmp = leftbackward(tmp);
		captures &= tmp;
		captures &= left(left(p->bm|p->bk));
		// now captures contains the 5-stone pattern
		// tmp is set to the white man at the top of the 5-stone pyramid
		// now get pattern on the left:
		shots |= captures & leftbackward(tmp) & twoleftbackward(twoleftbackward(free));
		// and now pattern on the right
		shots |= captures & rightbackward(tmp) & fourbackward(free);
		
		// add more right-forward moving shots here
		
		// next patterns:
		//      fr 
		//    wm  wm
		//  fr  fr  ?
		//    bm  ?
		//      bm
		//    ?   oc
		captures = p->bm & leftbackward(free); // move right forward
	    tmp = leftbackward(p->wm);
		captures &= (leftbackward(tmp) & rightbackward(tmp)); // the two white men there
		captures &= leftforward(p->bm|p->bk);
		captures &= leftbackward(twobackward(free));
		captures &= rightbackward(free);
		captures &= (twoleftforward(~free)|RANK1|RANK2);
		shots |= captures;

		//fr       
		//  w  nw   w
		//    fr  fr  
		//      bm  oc-if-nw-is-w 
		//        b
		//          oc
		captures = p->bm & leftbackward(free); // move right forward
		tmp = rightbackward(free);
		captures &= tmp;
		captures &= tworightbackward(tmp);
	    captures &= leftbackward(leftbackward(p->wm|p->wk));
		captures &= rightbackward(rightbackward(p->wm|p->wk));
		// last but not least: the nw part
		captures &= (twobackward(~(p->wm|p->wk)) | left(~free)  );
		captures &= leftforward(p->bm|p->bk);
		
		captures &= (twoleftforward(~free)|RANK1|RANK2);
		shots |= captures;

		/*if(shots)
			{
			printboard(p, color);
			printint32(shots);
			getch();
			}*/
		

		while(shots)
			{
			tmp = (shots&-shots);
			shots ^= tmp;
			tmp |= rightforward(tmp);
			movelist[n].bm = tmp&NWBR;
			movelist[n].bk = tmp&WBR;
			movelist[n].wm = 0;
			movelist[n].wk = 0;
			n++;
			}

		// left forward moving shots
		// find the following  2 patterns sort of simultaneously:
		// the first 5 items from downside are the same, first
		// detect these, then still the 2 different ends above
		// 
		//  fr                        fr
		//    wm                    wm
		//      wm                wm
		//    wm  fr            wm  fr
		//  b  --   b         b       b
		captures = p->bm & rightbackward(free);
		tmp = rightbackward(p->wm);
		captures &= right(tmp);
		tmp = rightbackward(tmp);
		captures &= tmp;
		captures &= right(right(p->bm|p->bk));
		// now captures contains the 5-stone pattern
		// tmp is set to the white man at the top of the 5-stone pyramid
		// now get pattern on the left:
		shots |= captures & rightbackward(tmp) & tworightbackward(tworightbackward(free));
		// and now pattern on the right
		//shots |= captures & leftbackward(tmp) & leftbackward(leftbackward(rightbackward(rightbackward(free))));
		shots |= captures & leftbackward(tmp) & fourbackward(free);
		
		// add more left-forward moving shots here
				// next patterns:
		//      fr 
		//    wm  wm
		//  ?   fr   fr
		//    ?   bm
		//      bm
		//    oc   ?
		captures = p->bm & rightbackward(free); // move left forward
	    tmp = rightbackward(p->wm);
		captures &= (leftbackward(tmp) & rightbackward(tmp)); // the two white men there
		captures &= rightforward(p->bm|p->bk);
		captures &= twobackward(rightbackward(free));
		captures &= leftbackward(free);
		captures &= (tworightforward(~free)|RANK1|RANK2);
		shots |= captures;

		//             fr
		//  w  nw?   w
		//    fr  fr  
		//  oc? bm  
		//    b
		//  oc
		captures = p->bm & rightbackward(free); // move right forward
		tmp = leftbackward(free);
		captures &= tmp;
		captures &= twoleftbackward(tmp);
	    captures &= tworightbackward(p->wm|p->wk);
		captures &= twoleftbackward(p->wm|p->wk);
		// last but not least: the nw part
		captures &= (twobackward(~(p->wm|p->wk)) | right(~free)  );
		captures &= rightforward(p->bm|p->bk);
		
		captures &= (tworightforward(~free)|RANK1|RANK2);
		shots |= captures;


		/*
		if(shots)
			{
			printboard(p, color);
			printint32(shots);
			getch();
			}*/
		

		while(shots)
			{
			tmp = (shots&-shots);
			shots ^= tmp;
			tmp |= leftforward(tmp);
			movelist[n].bm = tmp&NWBR;
			movelist[n].bk = tmp&WBR;
			movelist[n].wm = 0;
			movelist[n].wk = 0;
			n++;
			}

		}
	else // color is WHITE
		{
		// generate only moves that could be shots, no other moves!
		// right backward potential shots
		// find the following 2 patterns simultaneously:
		// 
		//  wm       w     wm       w
		//    fr  bm         fr  bm
		//      bm             bm
		//        bm         bm
		//          fr     fr
		captures = p->wm & leftforward(free);
		tmp = leftforward(p->bm);
		captures &= left(tmp);
		tmp = leftforward(tmp);
		captures &= tmp;
		//captures &= leftbackward(leftforward(leftbackward(leftforward(p->wm|p->wk))));
		captures &= left(left(p->wm|p->wk));

		/*if(left(left(p->wm|p->wk)) != leftbackward(leftforward(leftbackward(leftforward(p->wm|p->wk)))))
			{
			printint32(p->wm|p->wk);
			printint32(left(left(p->wm|p->wk)));
			printint32(leftbackward(leftforward(leftbackward(leftforward(p->wm|p->wk)))));
			getch();
			}*/

		// captures now contains the 5-man pyramid that is the same in both
		// cases. tmp is lf(lf(p->bm)) now find the left pattern:
		shots |= captures & leftforward(tmp) & twoleftforward(twoleftforward(free));
		// and the right pattern
		shots |= captures & rightforward(tmp) & fourforward(free);
		
		// next patterns:
		//         ?  oc
		//          w 
		//       wm      
		//     fr  fr 
		//       bm  bm
		//         fr
		//  
		captures = p->wm & leftforward(free); // move right backward
	    tmp = leftforward(p->bm);
		captures &= (leftforward(tmp) & rightforward(tmp)); // the two black men there
		captures &= leftbackward(p->wm|p->wk);
		captures &= leftforward(twoforward(free));
		captures &= rightforward(free);
		captures &= (twoleftbackward(~free)|RANK7|RANK8);
		shots |= captures;
		
		
		//fr       
		//  w  nw   w
		//    fr  fr  
		//      bm  oc-if-nw-is-w 
		//        b
		//          oc


		//          oc
		//        w
		//      wm  oc-if-nb
		//    fr  fr
		//   b  nb  b
		// fr

		captures = p->wm & leftforward(free); // move right backward
		tmp = rightforward(free);
		captures &= tmp;
		captures &= tworightforward(tmp);
	    captures &= twoleftforward(p->bm|p->bk);
		captures &= tworightforward(p->bm|p->bk);
		// last but not least: the nw part
		captures &= (twoforward(~(p->bm|p->bk)) | left(~free)  );
		captures &= leftbackward(p->wm|p->wk);
		
		captures &= (twoleftbackward(~free)|RANK7|RANK8);
		shots |= captures;

/*
		if(shots)
			{
			printboard(p, color);
			printint32(shots);
			getch();
			}*/
		
		// add more right-backward moving shots here
		
		while(shots)
			{
			tmp = (shots&-shots);
			shots ^= tmp;
			tmp |= rightbackward(tmp);
			movelist[n].bm = 0;
			movelist[n].bk = 0;
			movelist[n].wm = tmp&NBBR;
			movelist[n].wk = tmp&BBR;
			n++;
			}

		// add left-forward moving shots:
		// find the following 2 patterns simultaneously:
		// 
		//  fr                   fr
		//    wm               wm
		//      wm           wm
		//    wm  fr       wm  fr
		//  bm  --  bm   bm      bm
		
		captures = p->wm & rightforward(free);
		tmp = rightforward(p->bm);
		captures &= right(tmp);
		tmp = rightforward(tmp);
		captures &= tmp;
		captures &= right(right(p->wm|p->wk));
		// captures now contains the 5-man pyramid that is the same in both
		// cases. tmp is rf(rf(p->bm)) now find the left pattern:
		shots |= captures & rightforward(tmp) & tworightforward(tworightforward(free));
		// and the right pattern
		//shots |= captures & leftforward(tmp) & twoleftforward(tworightforward(free));
		shots |= captures & leftforward(tmp) & fourforward(free);
		
		
		// next pattern:
		//        oc  ?
		//          w 
		//           wm  
		//         fr  fr
		//       bm  bm
		//         fr
		//  
		captures = p->wm & rightforward(free); // move left backward
	    tmp = rightforward(p->bm);
		captures &= (rightforward(tmp) & leftforward(tmp)); // the two black men there
		captures &= rightbackward(p->wm|p->wk);
		captures &= rightforward(twoforward(free));
		captures &= leftforward(free);
		captures &= (tworightbackward(~free)|RANK7|RANK8);
		shots |= captures;
		
		//          oc
		//        w
		//      wm  oc-if-nb
		//    fr  fr
		//   b  nb  b
		// fr

		captures = p->wm & rightforward(free); // move left backward
		tmp = leftforward(free);
		captures &= tmp;
		captures &= twoleftforward(tmp);
	    captures &= tworightforward(p->bm|p->bk);
		captures &= twoleftforward(p->bm|p->bk);
		// last but not least: the nw part
		captures &= (twoforward(~(p->bm|p->bk)) | right(~free)  );
		captures &= rightbackward(p->wm|p->wk);
		
		captures &= (tworightbackward(~free)|RANK7|RANK8);
		shots |= captures;

		/*if(shots)
			{
			printboard(p, color);
			printint32(shots);
			getch();
			}*/
		
		// add more left-forward moving shots here
		
		while(shots)
			{
			tmp = (shots&-shots);
			shots ^= tmp;
			tmp |= leftbackward(tmp);
			movelist[n].bm = 0;
			movelist[n].bk = 0;
			movelist[n].wm = tmp&NBBR;
			movelist[n].wk = tmp&BBR;
			n++;
			}
		}

	return n;
	}


/* old version pre cakeM 1.07, but improved in version cakeM 1.06
int makeQSmovelist(POSITION *p, CAKE_MOVE movelist[MAXMOVES])
	{
	int32 n=0,free;
	int32 m,tmp;
	int32 captures, safe;
   
   //
	//     WHITE
	//  28  29  30  31
	//24  25  26  27
	//  20  21  22  23
	//16  17  18  19
	//  12  13  14  15
	// 8   9  10  11
	//   4   5   6   7
	// 0   1   2   3
	//     BLACK

   free=~(p->bm|p->bk|p->wm|p->wk);
  
   if(color==BLACK)
   		{
		// find pieces which can potentially move leading to captures
		// for either side.
		// find all black men for which a right-forward move will lead to a capture
		// mark all black men that can move right forward
		safe = p->bm & leftbackward(free);
		captures = safe;
		// remove those that walk into a white man that can capture them
		tmp = safe & leftbackward(leftbackward(p->wm));
		safe ^= tmp;
		// remove those running into a white man that can capture them "diagonally"
		tmp = safe & twobackward(p->wm) & leftforward(leftbackward(free));
		safe ^= tmp;
		// remove those protecting another man
		tmp = safe & rightbackward(p->bm) & rightbackward(rightbackward(p->wm));
		safe ^= tmp;
		captures ^= safe;

		// captures now contains all black men which will lead to a capture when
		// moving right forward
		// moves with black stones:
		while(captures)
			{
			tmp = (captures&-captures);
			captures ^= tmp;
			tmp |= rightforward(tmp);
			movelist[n].bm = tmp&NWBR;
			movelist[n].bk = tmp&WBR;
			movelist[n].wm = 0;
			movelist[n].wk = 0;
			n++;
			}

		// the same for left forward moves:
		safe = p->bm & rightbackward(free);
		captures = safe;
		tmp = safe & rightbackward(rightbackward(p->wm));
		safe ^= tmp;
		tmp = safe & twobackward(p->wm) & rightforward(rightbackward(free));
		safe ^= tmp;
		tmp = safe & leftbackward(p->bm) & leftbackward(leftbackward(p->wm));
		safe ^= tmp;

		captures ^= safe;

		while(captures)
			{
			tmp = (captures&-captures);
			captures ^= tmp;
			tmp |= leftforward(tmp);
			movelist[n].bm = tmp&NWBR;
			movelist[n].bk = tmp&WBR;
			movelist[n].wm = 0;
			movelist[n].wk = 0;
			n++;
			}

		return n;
		}	
            // ***************************************************************
   else     // color is WHITE 
            //****************************************************************
   		{
		// moves with white stones:
		// mark all white men that can move right backward
		safe = p->wm & leftforward(free);
		captures = safe;
		// remove those that walk into a black man that can capture them
		tmp = safe & leftforward(leftforward(p->bm));
		safe ^= tmp;
		// remove those running into a black man that can capture them "diagonally"
		tmp = safe & twoforward(p->bm) & leftforward(leftbackward(free));
		safe ^= tmp;
		// remove those protecting another man
		tmp = safe & rightforward(p->wm) & rightforward(rightforward(p->bm));
		safe ^= tmp;
		captures ^= safe;
		while(captures)
			{
			tmp = (captures&-captures);
			captures ^= tmp;
			tmp |= rightbackward(tmp);
			movelist[n].bm = 0;
			movelist[n].bk = 0;
			movelist[n].wm = tmp&NBBR;
			movelist[n].wk = tmp&BBR;
			n++;
			}

		
		// other direction
		safe = p->wm & rightforward(free);
		captures = safe;
		tmp = safe & rightforward(rightforward(p->bm));
		safe ^= tmp;
		tmp = safe & twoforward(p->bm) & rightforward(rightbackward(free));
		safe ^= tmp;
		tmp = safe & leftforward(p->wm) & leftforward(leftforward(p->bm));
		safe ^= tmp;
		captures ^= safe;

		while(captures)
			{
			tmp = (captures&-captures);
			captures ^= tmp;
			tmp |= leftbackward(tmp);
			movelist[n].bm = 0;
			movelist[n].bk = 0;
			movelist[n].wm = tmp&NBBR;
			movelist[n].wk = tmp&BBR;
			n++;
			}

		return n;
		}
	}*/
#endif // QSEARCH



/******************************************************************************/
/* capture list */
/******************************************************************************/

/* used to be captgen.c */
/* generates the capture moves */


int makecapturelist(POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES], int32 bestindex)
	{
	int32 free,m,tmp,white,black;
	int n=0;
	CAKE_MOVE partial;
	POSITION q;
	

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
	free=~(p->bm|p->bk|p->wm|p->wk);
	if(p->color==BLACK)
   		{
		if(p->bm)
      		{
			/* captures with black men! */
			white=p->wm|p->wk;
      		/* jumps left forwards with men*/
      		m=((((p->bm&LFJ2)<<4)&white)<<3)&free;
      		/* now m contains a bit for every free square where a black man can move*/
			while(m)
   				{
				/* find a move */
				tmp=(m&-m); /* least significant bit of m */
				partial.bm=(tmp|(tmp>>7))&NWBR;  /* NWBR: not white back rank */
				partial.bk=(tmp|(tmp>>7))&WBR;  /*if stone moves to WBR (white back rank) it's a king*/
				partial.wm=(tmp>>3)&p->wm;
				partial.wk=(tmp>>3)&p->wk;
				/* toggle it */
				q.bm=p->bm^partial.bm;
   				q.bk=p->bk^partial.bk;
   				q.wm=p->wm^partial.wm;
   				q.wk=p->wk^partial.wk;
				/* recursion */
				/* only if black has another capture move! */
				//white2=p->wm|p->wk;
				//free2=~(p->wm|p->wk|p->bm|p->bk);
				if ( (((((tmp&LFJ2)<<4)&white)<<3)&free) | (((((tmp&RFJ2)<<5)&white)<<4)&free))
            		blackmancapture2(&q,movelist,values, &n, &partial,tmp);
				else
            		{
					/* save move */
					
      				movelist[n]=partial;
					values[n]=MANCAPT*bitcount(partial.wm);
					values[n]+=KINGCAPT*bitcount(partial.wk);
					if(tmp&CENTER) 
						values[n]+=MCV;
					if(tmp&WBR)
               			{
						values[n]+=PVVAL;
						}
      				n++;
				}
            
            /* clears least significant bit of m, associated with that move. */
      		m=m&(m-1);
      		}
         m=((((p->bm&LFJ1)<<3)&white)<<4)&free;
      	/* now m contains a bit for every free square where a black man can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.bm=(tmp|(tmp>>7));
            partial.bk=0;
            partial.wm=(tmp>>4)&p->wm;
            partial.wk=(tmp>>4)&p->wk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            

            //white2=p->wm|p->wk;
            //free2=~(p->wm|p->wk|p->bm|p->bk);
            if ( ( ((((tmp&LFJ1)<<3)&white)<<4)&free ) | ( ((((tmp&RFJ1)<<4)&white)<<5)&free ))
            	blackmancapture1(&q,movelist, values,&n,&partial,tmp);
            else
            	{
               /* save move */
               values[n]=MANCAPT*bitcount(partial.wm);
               values[n]+=KINGCAPT*bitcount(partial.wk);
               if(tmp&CENTER) values[n]+=MCV;

      			movelist[n]=partial;
      			n++;
               }

            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         /* jumps right forwards with men*/
         m=((((p->bm&RFJ2)<<5)&white)<<4)&free;
      	/* now m contains a bit for every free square where a black man can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.bm=(tmp|(tmp>>9))&NWBR;
            partial.bk=(tmp|(tmp>>9))&WBR;
            partial.wm=(tmp>>4)&p->wm;
            partial.wk=(tmp>>4)&p->wk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            
            //white2=p->wm|p->wk;
            //free2=~(p->wm|p->wk|p->bm|p->bk);
            if ( ( ((((tmp&LFJ2)<<4)&white)<<3)&free ) | ( ((((tmp&RFJ2)<<5)&white)<<4)&free ))
            	blackmancapture2(&q,movelist, values,&n, &partial,tmp);
            else
            	{
               /* save move */
               values[n]=MANCAPT*bitcount(partial.wm);
               values[n]+=KINGCAPT*bitcount(partial.wk);
               if(tmp&CENTER) values[n]+=MCV;
               if(tmp&WBR)
               	{
                  values[n]+=PVVAL;
               	}
      			movelist[n]=partial;
      			n++;
               }
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         m=((((p->bm&RFJ1)<<4)&white)<<5)&free;
      	/* now m contains a bit for every free square where a black man can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.bm=tmp|(tmp>>9);
            partial.bk=0;
            partial.wm=(tmp>>5)&p->wm;
            partial.wk=(tmp>>5)&p->wk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            

            //white2=p->wm|p->wk;
            //free2=~(p->wm|p->wk|p->bm|p->bk);
            if ( ( ((((tmp&LFJ1)<<3)&white)<<4)&free ) | ( ((((tmp&RFJ1)<<4)&white)<<5)&free ))
            	blackmancapture1(&q,movelist,values, &n, &partial,tmp);
            else
            	{
               /* save move */
               values[n]=MANCAPT*bitcount(partial.wm);
               values[n]+=KINGCAPT*bitcount(partial.wk);
               if(tmp&CENTER) values[n]+=MCV;
      			movelist[n]=partial;
      			n++;
               }

            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
      	}
      if(p->bk)
      	{
         white=p->wm|p->wk;
      	/* jumps left forwards with black kings*/
      	m=((((p->bk&LFJ1)<<3)&white)<<4)&free;
      	/* now m contains a bit for every free square where a black king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.bm=0;
            partial.bk=(tmp|(tmp>>7));
            partial.wm=(tmp>>4)&p->wm;
            partial.wk=(tmp>>4)&p->wk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            
            blackkingcapture1(&q,movelist,values, &n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         m=((((p->bk&LFJ2)<<4)&white)<<3)&free;
      	/* now m contains a bit for every free square where a black king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.bm=0;
            partial.bk=(tmp|(tmp>>7));
            partial.wm=(tmp>>3)&p->wm;
            partial.wk=(tmp>>3)&p->wk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            
            blackkingcapture2(&q,movelist, values,&n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         /* jumps right forwards with black kings*/
      	m=((((p->bk&RFJ1)<<4)&white)<<5)&free;
      	/* now m contains a bit for every free square where a black king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.bm=0;
            partial.bk=tmp|(tmp>>9);
            partial.wm=(tmp>>5)&p->wm;
            partial.wk=(tmp>>5)&p->wk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            
            blackkingcapture1(&q,movelist, values,&n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         m=((((p->bk&RFJ2)<<5)&white)<<4)&free;
      	/* now m contains a bit for every free square where a black king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.bm=0;
            partial.bk=(tmp|(tmp>>9));
            partial.wm=(tmp>>4)&p->wm;
            partial.wk=(tmp>>4)&p->wk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            
            blackkingcapture2(&q,movelist,values, &n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}

      	/* jumps left backwards with black kings*/
      	m=((((p->bk&LBJ1)>>5)&white)>>4)&free;
      	/* now m contains a bit for every free square where a black king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.bm=0;
            partial.bk=(tmp|(tmp<<9));
            partial.wm=(tmp<<4)&p->wm;
            partial.wk=(tmp<<4)&p->wk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            
            blackkingcapture1(&q,movelist, values,&n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         m=((((p->bk&LBJ2)>>4)&white)>>5)&free;
      	/* now m contains a bit for every free square where a black king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.bm=0;
            partial.bk=(tmp|(tmp<<9));
            partial.wm=(tmp<<5)&p->wm;
            partial.wk=(tmp<<5)&p->wk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            
            blackkingcapture2(&q,movelist,values, &n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         /* jumps right backwards with black kings*/
      	m=((((p->bk&RBJ1)>>4)&white)>>3)&free;
      	/* now m contains a bit for every free square where a black king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.bm=0;
            partial.bk=tmp|(tmp<<7);
            partial.wm=(tmp<<3)&p->wm;
            partial.wk=(tmp<<3)&p->wk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            
            blackkingcapture1(&q,movelist, values,&n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         m=((((p->bk&RBJ2)>>3)&white)>>4)&free;
      	/* now m contains a bit for every free square where a black king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.bm=0;
            partial.bk=(tmp|(tmp<<7));
            partial.wm=(tmp<<4)&p->wm;
            partial.wk=(tmp<<4)&p->wk;
            
   			q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            blackkingcapture2(&q,movelist, values,&n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         }



#ifndef MOVEORDERING
		return n;
#endif

#ifdef MOHASH
      values[bestindex] += HASHMOVE;
#endif

		
		return n;
      }
   else /*******************COLOR IS WHITE *********************************/
      {
      if(p->wm)
      	{
         black=p->bm|p->bk;
      	/* jumps left backwards with men*/
      	m=((((p->wm&LBJ1)>>5)&black)>>4)&free;
      	/* now m contains a bit for every free square where a white man can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.wm=(tmp|(tmp<<9))&NBBR;
            partial.wk=(tmp|(tmp<<9))&BBR;
            partial.bm=(tmp<<4)&p->bm;
            partial.bk=(tmp<<4)&p->bk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            /* only if white has another capture move! */
            //black2=p->bm|p->bk;
            //free2=~(p->wm|p->wk|p->bm|p->bk);
            if ( ( ((((tmp&LBJ1)>>5)&black)>>4)&free) | ( ((((tmp&RBJ1)>>4)&black)>>3)&free ))
            	whitemancapture1(&q,movelist,values, &n, &partial,tmp);
            else
            	{
               /* save move */
               values[n]=MANCAPT*bitcount(partial.bm);
               values[n]+=KINGCAPT*bitcount(partial.bk);
               if(tmp&CENTER) values[n]+=MCV;
               if(partial.wk)
               	{
                  values[n]+=PVVAL;
                  }
      			movelist[n]=partial;
      			n++;
               }

            
            m=m&(m-1);   /* clears least significant bit of m */
      		}
         m=((((p->wm&LBJ2)>>4)&black)>>5)&free;
      	/* now m contains a bit for every free square where a white man can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.wm=(tmp|(tmp<<9));
            partial.wk=0;
            partial.bm=(tmp<<5)&p->bm;
            partial.bk=(tmp<<5)&p->bk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
			//black2=p->bm|p->bk;
            //free2=~(p->wm|p->wk|p->bm|p->bk);
            if ( ( ((((tmp&LBJ2)>>4)&black)>>5)&free) | ( ((((tmp&RBJ2)>>3)&black)>>4)&free ))
            	whitemancapture2(&q,movelist, values,&n, &partial,tmp);
            else
            	{
               /* save move */
               values[n]=MANCAPT*bitcount(partial.bm);
               values[n]+=KINGCAPT*bitcount(partial.bk);
               if(tmp&CENTER) values[n]+=MCV;
      			movelist[n]=partial;
      			n++;
               }

            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         /* jumps right backwards with men*/
      	m=((((p->wm&RBJ1)>>4)&black)>>3)&free;
      	/* now m contains a bit for every free square where a white man can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.wm=(tmp|(tmp<<7))&NBBR;
            partial.wk=(tmp|(tmp<<7))&BBR;
            partial.bm=(tmp<<3)&p->bm;
            partial.bk=(tmp<<3)&p->bk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            //black2=p->bm|p->bk;
            //free2=~(p->wm|p->wk|p->bm|p->bk);
            if ( ( ((((tmp&LBJ1)>>5)&black)>>4)&free) | ( ((((tmp&RBJ1)>>4)&black)>>3)&free ))
            	whitemancapture1(&q,movelist,values, &n, &partial,tmp);
            else
            	{
               /* save move */
               values[n]=MANCAPT*bitcount(partial.bm);
               values[n]+=KINGCAPT*bitcount(partial.bk);
               if(tmp&CENTER) values[n]+=MCV;
               if(partial.wk)
               	{
                  values[n]+=PVVAL;
                  }
      			movelist[n]=partial;
      			n++;
               }
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         m=((((p->wm&RBJ2)>>3)&black)>>4)&free;
      	/* now m contains a bit for every free square where a black man can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.wm=(tmp|(tmp<<7));
            partial.wk=0;
            partial.bm=(tmp<<4)&p->bm;
            partial.bk=(tmp<<4)&p->bk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            //black2=p->bm|p->bk;
            //free2=~(p->wm|p->wk|p->bm|p->bk);
            if ( ( ((((tmp&LBJ2)>>4)&black)>>5)&free) | ( ((((tmp&RBJ2)>>3)&black)>>4)&free ))
            	whitemancapture2(&q,movelist,values, &n, &partial,tmp);
            else
            	{
               /* save move */
               values[n]=MANCAPT*bitcount(partial.bm);
               values[n]+=KINGCAPT*bitcount(partial.bk);
               if(tmp&CENTER) values[n]+=MCV;
      			movelist[n]=partial;
      			n++;
               }
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
      	}
      if(p->wk)
      	{
         black=p->bm|p->bk;
      	/* jumps left forwards with white kings*/
      	m=((((p->wk&LFJ1)<<3)&black)<<4)&free;
      	/* now m contains a bit for every free square where a white king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.wm=0;
            partial.wk=(tmp|(tmp>>7));
            partial.bm=(tmp>>4)&p->bm;
            partial.bk=(tmp>>4)&p->bk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            whitekingcapture1(&q,movelist,values, &n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         m=((((p->wk&LFJ2)<<4)&black)<<3)&free;
      	/* now m contains a bit for every free square where a white king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.wm=0;
            partial.wk=(tmp|(tmp>>7));
            partial.bm=(tmp>>3)&p->bm;
            partial.bk=(tmp>>3)&p->bk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            whitekingcapture2(&q,movelist,values, &n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         /* jumps right forwards with white kings*/
      	m=((((p->wk&RFJ1)<<4)&black)<<5)&free;
      	/* now m contains a bit for every free square where a white king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.wm=0;
            partial.wk=tmp|(tmp>>9);
            partial.bm=(tmp>>5)&p->bm;
            partial.bk=(tmp>>5)&p->bk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            whitekingcapture1(&q,movelist,values, &n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         m=((((p->wk&RFJ2)<<5)&black)<<4)&free;
      	/* now m contains a bit for every free square where a white king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.wm=0;
            partial.wk=(tmp|(tmp>>9));
            partial.bm=(tmp>>4)&p->bm;
            partial.bk=(tmp>>4)&p->bk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            whitekingcapture2(&q,movelist, values,&n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}


      	/* jumps left backwards with white kings*/
      	m=((((p->wk&LBJ1)>>5)&black)>>4)&free;
      	/* now m contains a bit for every free square where a white king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.wm=0;
            partial.wk=(tmp|(tmp<<9));
            partial.bm=(tmp<<4)&p->bm;
            partial.bk=(tmp<<4)&p->bk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            whitekingcapture1(&q,movelist,values, &n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         m=((((p->wk&LBJ2)>>4)&black)>>5)&free;
      	/* now m contains a bit for every free square where a white king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.wm=0;
            partial.wk=(tmp|(tmp<<9));
            partial.bm=(tmp<<5)&p->bm;
            partial.bk=(tmp<<5)&p->bk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            whitekingcapture2(&q,movelist, values,&n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         /* jumps right backwards with white kings*/
      	m=((((p->wk&RBJ1)>>4)&black)>>3)&free;
      	/* now m contains a bit for every free square where a white king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.wm=0;
            partial.wk=tmp|(tmp<<7);
            partial.bm=(tmp<<3)&p->bm;
            partial.bk=(tmp<<3)&p->bk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            whitekingcapture1(&q,movelist, values,&n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         m=((((p->wk&RBJ2)>>3)&black)>>4)&free;
      	/* now m contains a bit for every free square where a white king can move*/
         while(m)
   			{
            tmp=(m&-m); /* least significant bit of m */
            partial.wm=0;
            partial.wk=(tmp|(tmp<<7));
            partial.bm=(tmp<<4)&p->bm;
            partial.bk=(tmp<<4)&p->bk;
            q.bm=p->bm^partial.bm;
   			q.bk=p->bk^partial.bk;
   			q.wm=p->wm^partial.wm;
   			q.wk=p->wk^partial.wk;
            whitekingcapture2(&q,movelist,values, &n, &partial,tmp);
            
      		m=m&(m-1);   /* clears least significant bit of m */
      		}
         }
#ifndef MOVEORDERING
		return n;
#endif
#ifdef MOHASH
      values[bestindex] += HASHMOVE;
#endif
		return n;
      }
   }

static void blackmancapture1(POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES],int *n, CAKE_MOVE *partial, int32 square)
	{
	/* partial move has already been executed. seek LFJ1 and RFJ1 */
	int32 m,free,white;
	int found=0;
	CAKE_MOVE next_partial,whole_partial;
	POSITION q;

	// TODO: this computation is unnecessary
	free=~(p->bm|p->bk|p->wm|p->wk);
	white=p->wm|p->wk;
	/* left forward jump */
	m=((((square&LFJ1)<<3)&white)<<4)&free;
	if(m)
   		{
		next_partial.bm=(m|(m>>7));
		next_partial.bk=0;
		next_partial.wm=(m>>4)&p->wm;
		next_partial.wk=(m>>4)&p->wk;
		q.bm=p->bm^next_partial.bm;
		q.bk=p->bk^next_partial.bk;
   		q.wm=p->wm^next_partial.wm;
   		q.wk=p->wk^next_partial.wk;
		whole_partial.bm=partial->bm^next_partial.bm;
		whole_partial.bk=partial->bk^next_partial.bk;
		whole_partial.wm=partial->wm^next_partial.wm;
		whole_partial.wk=partial->wk^next_partial.wk;
		blackmancapture1(&q,movelist,values,n,&whole_partial,m);
	     
		found=1;
		}

   /* right forward jump */
   m=((((square&RFJ1)<<4)&white)<<5)&free;
   if(m)
   	{
      next_partial.bm=(m|(m>>9));
      next_partial.bk=0;
      next_partial.wm=(m>>5)&p->wm;
      next_partial.wk=(m>>5)&p->wk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      blackmancapture1(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   if(!found)
   	{
      /* no continuing jumps - save the move in the movelist */
      /* set info tag in move */
		values[*n]=MANCAPT*bitcount(partial->wm);
      values[*n]+=KINGCAPT*bitcount(partial->wk);
      if(square&CENTER) values[*n]+=MCV;
      movelist[*n]=*partial;
      (*n)++;
      }
   }

static void blackmancapture2(POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES],int *n, CAKE_MOVE *partial, int32 square)
	{
   /* partial move has already been executed. seek LFJ2 and RFJ2 */
   /* additional complication: black stone might crown here */
   int32 m,free,white;
   CAKE_MOVE next_partial,whole_partial;
   int found=0;
	POSITION q;

   free=~(p->bm|p->bk|p->wm|p->wk);
   white=p->wm|p->wk;
   /* left forward jump */
   m=((((square&LFJ2)<<4)&white)<<3)&free;
   if(m)
   	{
      next_partial.bm=(m|(m>>7))&NWBR;
      next_partial.bk=(m|(m>>7))&WBR;
      next_partial.wm=(m>>3)&p->wm;
      next_partial.wk=(m>>3)&p->wk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      blackmancapture2(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* right forward jump */
   m=((((square&RFJ2)<<5)&white)<<4)&free;
   if(m)
   	{
      next_partial.bm=(m|(m>>9))&NWBR;
      next_partial.bk=(m|(m>>9))&WBR;
      next_partial.wm=(m>>4)&p->wm;
      next_partial.wk=(m>>4)&p->wk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      blackmancapture2(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   if(!found)
   	{
      /* no continuing jumps - save the move in the movelist */
      values[*n]=MANCAPT*bitcount(partial->wm);
      values[*n]+=KINGCAPT*bitcount(partial->wk);
      if(square&CENTER) values[*n]+=MCV;
      if(partial->bk)
        	{
         values[*n]+=PVVAL;
         }
      movelist[*n]=*partial;
      (*n)++;
      }
   }


static void blackkingcapture1(POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES],int *n, CAKE_MOVE *partial, int32 square)
	{
   /* partial move has already been executed. seek LFJ1 RFJ1 LBJ1 RBJ1*/
   int32 m,free,white;
   CAKE_MOVE next_partial,whole_partial;
   int found=0;
	POSITION q;

   free=~(p->bm|p->bk|p->wm|p->wk);
   white=p->wm|p->wk;
   /* left forward jump */
   m=((((square&LFJ1)<<3)&white)<<4)&free;
   if(m)
   	{
      next_partial.bm=0;
      next_partial.bk=(m|(m>>7));
      next_partial.wm=(m>>4)&p->wm;
      next_partial.wk=(m>>4)&p->wk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	  q.wm=p->wm^next_partial.wm;
   	  q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      blackkingcapture1(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* right forward jump */
   m=((((square&RFJ1)<<4)&white)<<5)&free;
   if(m)
   	{
      next_partial.bm=0;
      next_partial.bk=(m|(m>>9));
      next_partial.wm=(m>>5)&p->wm;
      next_partial.wk=(m>>5)&p->wk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      blackkingcapture1(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* left backward jump */
   m=((((square&LBJ1)>>5)&white)>>4)&free;
   if(m)
   	{
      next_partial.bm=0;
      next_partial.bk=(m|(m<<9));
      next_partial.wm=(m<<4)&p->wm;
      next_partial.wk=(m<<4)&p->wk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      blackkingcapture1(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* right backward jump */
   m=((((square&RBJ1)>>4)&white)>>3)&free;
   if(m)
   	{
      next_partial.bm=0;
      next_partial.bk=(m|(m<<7));
      next_partial.wm=(m<<3)&p->wm;
      next_partial.wk=(m<<3)&p->wk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      blackkingcapture1(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   if(!found)
   	{
      /* no continuing jumps - save the move in the movelist */
      values[*n]=MANCAPT*bitcount(partial->wm);
      values[*n]+=KINGCAPT*bitcount(partial->wk);
      if(square&CENTER) values[*n]+=KCV;
      movelist[*n]=*partial;
      (*n)++;
      }
   }
static void blackkingcapture2(POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES],int *n, CAKE_MOVE *partial, int32 square)
	{
   /* partial move has already been executed. seek LFJ1 RFJ1 LBJ1 RBJ1*/
   int32 m,free,white;
   CAKE_MOVE next_partial,whole_partial;
   int found=0;
	POSITION q;

   free=~(p->bm|p->bk|p->wm|p->wk);
   white=p->wm|p->wk;
   /* left forward jump */
   m=((((square&LFJ2)<<4)&white)<<3)&free;
   if(m)
   	{
      next_partial.bm=0;
      next_partial.bk=(m|(m>>7));
      next_partial.wm=(m>>3)&p->wm;
      next_partial.wk=(m>>3)&p->wk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      blackkingcapture2(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* right forward jump */
   m=((((square&RFJ2)<<5)&white)<<4)&free;
   if(m)
   	{
      next_partial.bm=0;
      next_partial.bk=(m|(m>>9));
      next_partial.wm=(m>>4)&p->wm;
      next_partial.wk=(m>>4)&p->wk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      blackkingcapture2(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* left backward jump */
   m=((((square&LBJ2)>>4)&white)>>5)&free;
   if(m)
   	{
      next_partial.bm=0;
      next_partial.bk=(m|(m<<9));
      next_partial.wm=(m<<5)&p->wm;
      next_partial.wk=(m<<5)&p->wk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      blackkingcapture2(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* right backward jump */

   m=((((square&RBJ2)>>3)&white)>>4)&free;
   if(m)
   	{
      next_partial.bm=0;
      next_partial.bk=(m|(m<<7));
      next_partial.wm=(m<<4)&p->wm;
      next_partial.wk=(m<<4)&p->wk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      blackkingcapture2(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   if(!found)
   	{
      /* no continuing jumps - save the move in the movelist */
      values[*n]=MANCAPT*bitcount(partial->wm);
      values[*n]+=KINGCAPT*bitcount(partial->wk);
      if(square&CENTER) 
		  values[*n]+=KCV;
      movelist[*n]=*partial;
      (*n)++;
      }
   }

static void whitemancapture1(POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES],int *n, CAKE_MOVE *partial, int32 square)
	{
   /* partial move has already been executed. seek LBJ1 and RBJ1 */
   int32 m,free,black;
   CAKE_MOVE next_partial,whole_partial;
   int found=0;
	POSITION q;

   free=~(p->bm|p->bk|p->wm|p->wk);
   black=p->bm|p->bk;
   /* left backward jump */
   m=((((square&LBJ1)>>5)&black)>>4)&free;
   if(m)
   	{
      next_partial.wm=(m|(m<<9))&NBBR;
      next_partial.wk=(m|(m<<9))&BBR;
      next_partial.bm=(m<<4)&p->bm;
      next_partial.bk=(m<<4)&p->bk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      whitemancapture1(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* right backward jump */
   m=((((square&RBJ1)>>4)&black)>>3)&free;
   if(m)
   	{
      next_partial.wm=(m|(m<<7))&NBBR;
      next_partial.wk=(m|(m<<7))&BBR;
      next_partial.bm=(m<<3)&p->bm;
      next_partial.bk=(m<<3)&p->bk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      whitemancapture1(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   if(!found)
   	{
      /* no continuing jumps - save the move in the movelist */
      values[*n]=MANCAPT*bitcount(partial->bm);
      values[*n]+=KINGCAPT*bitcount(partial->bk);
      if(square&CENTER) values[*n]+=MCV;
      if(partial->wk)
      	{
         values[*n]+=PVVAL;
         }
      movelist[*n]=*partial;
      (*n)++;
      }
   }
static void whitemancapture2(POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES],int *n, CAKE_MOVE *partial, int32 square)
	{
   /* partial move has already been executed. seek LBJ1 and RBJ1 */
   int32 m,free,black;
   CAKE_MOVE next_partial,whole_partial;
   int found=0;
	POSITION q;

   free=~(p->bm|p->bk|p->wm|p->wk);
   black=p->bm|p->bk;
   /* left backward jump */
   m=((((square&LBJ2)>>4)&black)>>5)&free;
   if(m)
   	{
      next_partial.wm=(m|(m<<9));
      next_partial.wk=0;
      next_partial.bm=(m<<5)&p->bm;
      next_partial.bk=(m<<5)&p->bk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      whitemancapture2(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* right backward jump */
   m=((((square&RBJ2)>>3)&black)>>4)&free;
   if(m)
   	{
      next_partial.wm=(m|(m<<7));
      next_partial.wk=0;
      next_partial.bm=(m<<4)&p->bm;
      next_partial.bk=(m<<4)&p->bk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      whitemancapture2(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   if(!found)
   	{
      /* no continuing jumps - save the move in the movelist */
      values[*n]=MANCAPT*bitcount(partial->bm);
      values[*n]+=KINGCAPT*bitcount(partial->bk);
      if(square&CENTER) values[*n]+=MCV;
      movelist[*n]=*partial;
      (*n)++;
      }
   }

static void whitekingcapture1(POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES],int *n, CAKE_MOVE *partial, int32 square)
	{
   /* partial move has already been executed. seek LFJ1 RFJ1 LBJ1 RBJ1*/
   int32 m,free,black;
   CAKE_MOVE next_partial,whole_partial;
   int found=0;
	POSITION q;

   free=~(p->bm|p->bk|p->wm|p->wk);
   black=p->bm|p->bk;
   /* left forward jump */
   m=((((square&LFJ1)<<3)&black)<<4)&free;
   if(m)
   	{
      next_partial.wm=0;
      next_partial.wk=(m|(m>>7));
      next_partial.bm=(m>>4)&p->bm;
      next_partial.bk=(m>>4)&p->bk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      whitekingcapture1(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* right forward jump */
   m=((((square&RFJ1)<<4)&black)<<5)&free;
   if(m)
   	{
      next_partial.wm=0;
      next_partial.wk=(m|(m>>9));
      next_partial.bm=(m>>5)&p->bm;
      next_partial.bk=(m>>5)&p->bk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      whitekingcapture1(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* left backward jump */
   m=((((square&LBJ1)>>5)&black)>>4)&free;
   if(m)
   	{
      next_partial.wm=0;
      next_partial.wk=(m|(m<<9));
      next_partial.bm=(m<<4)&p->bm;
      next_partial.bk=(m<<4)&p->bk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      whitekingcapture1(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* right backward jump */
   m=((((square&RBJ1)>>4)&black)>>3)&free;
   if(m)
   	{
      next_partial.wm=0;
      next_partial.wk=(m|(m<<7));
      next_partial.bm=(m<<3)&p->bm;
      next_partial.bk=(m<<3)&p->bk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      whitekingcapture1(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   if(!found)
   	{
      /* no continuing jumps - save the move in the movelist */
      values[*n]=MANCAPT*bitcount(partial->bm);
      values[*n]+=KINGCAPT*bitcount(partial->bk);
      if(square&CENTER) values[*n]+=KCV;
      movelist[*n]=*partial;
      (*n)++;
      }
   }
static void whitekingcapture2(POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES],int *n, CAKE_MOVE *partial, int32 square)
	{
   /* partial move has already been executed. seek LFJ1 RFJ1 LBJ1 RBJ1*/
   int32 m,free,black;
   CAKE_MOVE next_partial,whole_partial;
   int found=0;
	POSITION q;

   free=~(p->bm|p->bk|p->wm|p->wk);
   black=p->bm|p->bk;
   /* left forward jump */
   m=((((square&LFJ2)<<4)&black)<<3)&free;
   if(m)
   	{
      next_partial.wm=0;
      next_partial.wk=(m|(m>>7));
      next_partial.bm=(m>>3)&p->bm;
      next_partial.bk=(m>>3)&p->bk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      whitekingcapture2(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* right forward jump */
   m=((((square&RFJ2)<<5)&black)<<4)&free;
   if(m)
   	{
      next_partial.wm=0;
      next_partial.wk=(m|(m>>9));
      next_partial.bm=(m>>4)&p->bm;
      next_partial.bk=(m>>4)&p->bk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      whitekingcapture2(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* left backward jump */
   m=((((square&LBJ2)>>4)&black)>>5)&free;
   if(m)
   	{
      next_partial.wm=0;
      next_partial.wk=(m|(m<<9));
      next_partial.bm=(m<<5)&p->bm;
      next_partial.bk=(m<<5)&p->bk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      whitekingcapture2(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   /* right backward jump */
   m=((((square&RBJ2)>>3)&black)>>4)&free;
   if(m)
   	{
      next_partial.wm=0;
      next_partial.wk=(m|(m<<7));
      next_partial.bm=(m<<4)&p->bm;
      next_partial.bk=(m<<4)&p->bk;
      q.bm=p->bm^next_partial.bm;
      q.bk=p->bk^next_partial.bk;
   	q.wm=p->wm^next_partial.wm;
   	q.wk=p->wk^next_partial.wk;
      
      whole_partial.bm=partial->bm^next_partial.bm;
      whole_partial.bk=partial->bk^next_partial.bk;
      whole_partial.wm=partial->wm^next_partial.wm;
      whole_partial.wk=partial->wk^next_partial.wk;
      whitekingcapture2(&q,movelist,values,n,&whole_partial,m);
      
      found=1;
      }

   if(!found)
   	{
      /* no continuing jumps - save the move in the movelist */
      values[*n]=MANCAPT*bitcount(partial->bm);
      values[*n]+=KINGCAPT*bitcount(partial->bk);
      if(square&CENTER) values[*n]+=KCV;
      movelist[*n]=*partial;
      (*n)++;
      }
   }
