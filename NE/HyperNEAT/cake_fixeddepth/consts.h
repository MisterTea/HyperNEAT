/*		cake++ 1.22 - a checkers engine			*/
/*															*/
/*		Copyright (C) 2001 by Martin Fierz		*/
/*															*/
/*		contact: checkers@fierz.ch					*/

/* consts.h - all definitions for cake++, movegen, captgen */


/* mate value */
#define MATE 2000

/* # of fractions per ply */
#define FRAC 4

/* bitboard masks for moves in various directions */
/* here "1" means the squares in the columns 1357 and "2" in 2468.*/
#define RF1  0x0F0F0F0F
#define RF2  0x00707070
#define LF1  0x0E0E0E0E
#define LF2  0x00F0F0F0
#define RB1  0x0F0F0F00
#define RB2  0x70707070
#define LB1  0x0E0E0E00
#define LB2  0xF0F0F0F0
/* bitboard masks for jumps in various directions */
#define RFJ1  0x00070707
#define RFJ2  0x00707070
#define LFJ1  0x000E0E0E
#define LFJ2  0x00E0E0E0
#define RBJ1  0x07070700
#define RBJ2  0x70707000
#define LBJ1  0x0E0E0E00
#define LBJ2  0xE0E0E000

/* back rank masks */
#define WBR  0xF0000000
#define BBR  0x0000000F
#define NWBR 0x0FFFFFFF
#define NBBR 0xFFFFFFF0

// ranks
#define RANK1 0xF
#define RANK2 0xF0
#define RANK3 0xF00
#define RANK4 0xF000
#define RANK5 0xF0000
#define RANK6 0xF00000
#define RANK7 0xF000000
#define RANK8 0xF0000000

/* center and edge mask */
/*       WHITE
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
#define MCENTER 0x00066000 /* center for men */
#define CENTER 0x00666600 /* the innermost 8 squares */
#define EDGE   0xF181818F /* the 14 edge squares */


/* roaming king masks */
/* where a king has to be so that he's not possibly trapped */
/* possibly make this more strict to 0x00FFFFFF and 0xFFFFFF00*/
/* no! that makes the match result worse! */

#define ROAMINGBLACKKING 0x06FFFFFF
#define ROAMINGWHITEKING 0xFFFFFF60


#define WHITE 1
#define BLACK 2
#define MAN 4
#define KING 8
#define FREE 16
#define CC 3
#define MAXMOVES 28 //28
/* number of moves in the movelist - saw crashes with 24!*/


/* value type definitions for hashtable store*/
#define LOWER 0
#define UPPER 1
#define EXACT 2


#define BIT0 0x00000001
#define BIT1 0x00000002
#define BIT2 0x00000004
#define BIT3 0x00000008
#define BIT4 0x00000010
#define BIT5 0x00000020
#define BIT6 0x00000040
#define BIT7 0x00000080
#define BIT8 0x00000100
#define BIT9 0x00000200
#define BIT10 0x00000400
#define BIT11 0x00000800
#define BIT12 0x00001000
#define BIT13 0x00002000
#define BIT14 0x00004000
#define BIT15 0x00008000
#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000
#define BIT24 0x01000000
#define BIT25 0x02000000
#define BIT26 0x04000000
#define BIT27 0x08000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000

/*       WHITE
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


/* runaway checkers masks: for a black checker on 20 -> RA20 is the mask.
   use RA20&white (has to be 0) to check if its a runaway.
   sometimes a RAxL and RAxR are defined. for these squares, either
   RAxL&white==0 OR RaxR&white==0 is sufficient for a checker to be a runaway*/

/*black*/
#define RA20  0x11000000
#define RA21L 0x32000000
#define RA21R 0x64000000
#define RA22L 0x64000000
#define RA22R 0xC8000000
#define RA23  0xC8000000

#define RA16 0x33100000
#define RA17 0x77300000
#define RA18 0xFE600000
#define RA19 0xECC00000
/*white*/
/*       WHITE
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
#define RA8   0x00000013
#define RA9L  0x00000013
#define RA9R  0x00000026
#define RA10L 0x00000026
#define RA10R 0x0000004C
#define RA11  0x00000088

#define RA12 0x00000337
#define RA13 0x0000067F
#define RA14 0x00000CEE
#define RA15 0x000008CC


/* move macros */

#define lf1(x) ((x&LF1)>>3)
#define lf2(x) ((x&LF2)>>4)
#define rf1(x) ((x&RF1)>>4)
#define rf2(x) ((x&RF2)>>5)
#define lb1(x) ((x&LB1)<<5)
#define lb2(x) ((x&LB2)<<4)
#define rb1(x) ((x&RB1)<<4)
#define rb2(x) ((x&RB2)<<3)

/* database values */
#define DRAW 0
#define WIN 1
#define LOSS 2
#define UNKNOWN 3
#define DB_NOT_LOOKED_UP 4

#define WINLEVEL 1000

/* exchange bias */
#define EXBIAS 250 

/* max and min macros*/
// these are already defined in stdlib.h, don't need to define them myself.
/*#undef max
#undef min
#define max(x,y) (((x)>(y))?(x):(y))
#define min(x,y) (((x)>(y))?(y):(x))*/

/* square definitions: a piece on square n in normal checkers notation can be accessed with SQn*/
#define SQ1  0x00000008
#define SQ2  0x00000004
#define SQ3  0x00000002
#define SQ4  0x00000001
#define SQ5  0x00000080
#define SQ6  0x00000040
#define SQ7  0x00000020
#define SQ8  0x00000010
#define SQ9  0x00000800
#define SQ10 0x00000400
#define SQ11 0x00000200
#define SQ12 0x00000100
#define SQ13 0x00008000
#define SQ14 0x00004000
#define SQ15 0x00002000
#define SQ16 0x00001000
#define SQ17 0x00080000
#define SQ18 0x00040000
#define SQ19 0x00020000
#define SQ20 0x00010000
#define SQ21 0x00800000
#define SQ22 0x00400000
#define SQ23 0x00200000
#define SQ24 0x00100000
#define SQ25 0x08000000
#define SQ26 0x04000000
#define SQ27 0x02000000
#define SQ28 0x01000000
#define SQ29 0x80000000
#define SQ30 0x40000000
#define SQ31 0x20000000
#define SQ32 0x10000000

/* rows of the checkerboard */
#define ROW1 0x01010101
#define ROW2 0x10101010
#define ROW3 0x02020202
#define ROW4 0x20202020
#define ROW5 0x04040404
#define ROW6 0x40404040
#define ROW7 0x08080808
#define ROW8 0x80808080

/* search modes */
#define TIME_BASED 0
#define DEPTH_BASED 1
#define NODE_BASED 2


#define togglemove(p,m) p->bm^=m.bm;p->bk^=m.bk; p->wm^=m.wm;p->wk^=m.wk;p->color^=CC;
#define domove(q,p,m) q.bm=p->bm^m.bm; q.bk=p->bk^m.bk; q.wm=p->wm^m.wm; q.wk=p->wk^m.wk;

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
#define LEFT 0xEEEEEEEE
#define RIGHT 0x7777777
#define left(x) ( ((x)&LEFT) >> 1)
#define right(x) ( ((x)&RIGHT) << 1)
// funny, the forward(x) macro with 4 terms is faster than the one with 3 terms...
// likewise for the backward macro
#define forward(x) ( (((x)&LF1)<<3) | (((x)&LF2)<<4) | (((x)&RF1)<<4) | (((x)&RF2)<<5) )
//#define forward(x) ( (((x)&LF1)<<3) | ((x&(LF2|RF1))<<4)  | (((x)&RF2)<<5) )
#define leftforward(x) ( (((x)&LF1)<<3) | (((x)&LF2)<<4)  )
	// returns the squares left forward of x
#define rightforward(x) ( (((x)&RF1)<<4) | (((x)&RF2)<<5) )
#define backward(x) ( (((x)&LB1)>>5) | (((x)&LB2)>>4) | (((x)&RB1)>>4) | (((x)&RB2)>>3) )
//#define backward(x) ( (((x)&LB1)>>5) | ((x&(LB2|RB1))>>4) | (((x)&RB2)>>3) )
#define leftbackward(x) ( (((x)&LB1)>>5) | (((x)&LB2)>>4)  )
#define rightbackward(x) ( (((x)&RB1)>>4) | (((x)&RB2)>>3) )
#define TWOLEFTFORWARD 0x00EEEEEE
#define TWORIGHTFORWARD 0x00777777
#define TWOLEFTBACKWARD 0xEEEEEE00
#define TWORIGHTBACKWARD 0x77777700
#define twoleftforward(x)  ( ((x)&TWOLEFTFORWARD) << 7)
#define tworightforward(x) ( ((x)&TWORIGHTFORWARD) << 9)
#define twoleftbackward(x) ( ((x)&TWOLEFTBACKWARD) >> 9)
#define tworightbackward(x) ( ((x)&TWORIGHTBACKWARD) >> 7)
#define twoforward(x) ((x)<<8)
#define twobackward(x) ((x)>>8)
#define fourforward(x) ((x)<<16)
#define fourbackward(x) ((x)>>16)

// pattern matching macros
#define match1(a,b) ( ((a)&(b)) == (b) )
#define match2(a,b,c,d) ( (((a)&(c))|((b)&(d))) ==((c)|(d)))
#define match3(a,b,c,d,e,f) ( (((a)&(d))|((b)&(e))|((c)&(f))) ==((d)|(e)|(f)))



#define BOOKALLKINDS 1
#define BOOKGOOD 2
#define BOOKBEST 3

