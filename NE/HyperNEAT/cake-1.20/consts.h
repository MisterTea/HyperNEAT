#ifndef __CAKE_CONSTS_H__
#define __CAKE_CONSTS_H__

/* consts.h - all definitions for cake++, movegen, captgen */


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

/* center and edge mask */
#define CENTER 0x00666600
#define EDGE   0xF181818F

#define WHITE 1
#define BLACK 2
#define MAN 4
#define KING 8
#define FREE 16
#define CC 3
#define MAXMOVES 28
/* number of moves in the movelist - saw crashes with 24!*/


/* masks for move.info */
#define MOVEVAL  0x0000FFFF

#define ISPROM   0x00010000
#define ISCAPT   0x00020000
#define ISEXTEND 0x00040000

#define moveval(a)     (a&MOVEVAL)
#define isprom(a)   ((a&ISPROM)>>16)
#define iscapt(a)   ((a&ISCAPT)>>17)
#define isextend(a) ((a&ISEXTEND)>>18)

/* masks for hashentry.info */
#define DEPTH	0x00001FFF
#define COLOR 0x00002000
#define VALUETYPE 0x0000C000
#define LOWER 0x00004000
#define UPPER 0x00008000
#define EXACT 0x0000C000
#define HASHBLACK 0x00002000
#define HASHWHITE 0x00000000

#define hashdepth(x) (x&DEPTH)
#define hashcolor(x) (x&COLOR)
#define hashvaluetype(x) (x&VALUETYPE)

#define togglemove(m) p.bm^=m.bm;p.bk^=m.bk; p.wm^=m.wm;p.wk^=m.wk;

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

#endif
