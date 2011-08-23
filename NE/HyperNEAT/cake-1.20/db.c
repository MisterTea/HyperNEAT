/* chinook database access code for endgame database access written by jonathan schaeffer*/

/* adapted may 29th 2000 by martin fierz */
/* this code is more or less the chinook code, but i modified some things:*/

/* DBlookup takes parameters struct position p and int turn */

/* these are then copied into globals as in the chinook version */

/* db.c looks for a file called db.ini which informs it which files to read,
	that is which database it's supposed to use, and how many buffers to
   allocate.
   db.ini should look like this:
   "db4 db4.idx 4 100"
   <database file> <database index file> <max number of pieces> <number of 1KB buffers>
*/
#include "switches.h"


#define int32 unsigned long
struct pos
	{
   int32 bm;
   int32 bk;
   int32 wm;
   int32 wk;
   } Gposition; /* Gposition does for my program what Locbv does for schaeffers:
   				its the global position vector */

/* Program looks for files "DB4" and "DB4.idx" in the current directory */



/* schaeffer's code below */
#include <stdio.h>
#include <stdlib.h>
#ifndef SYS_WINDOWS
#include <unistd.h>
#endif
#ifdef SYS_UNIX
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif /*SYSTEM*/
/* Look at DB_BUFFERS to set the storage requirements you want to use.  */

/* Code has been modified to reduce storage requirements.  With more	*/
/* storage, you can rewrite to get more efficient code.			*/

/* Note: code as given only gives the correct result for a position	*/
/* where the side to move does not have a capture move, or if the turn 	*/
/* is switched, then a capture move would be present.			*/
/*									*/
/* Incorrect result returned because white can capture:			*/
/* 									*/
/* 8  . - . - . - . -							*/
/* 7  - . - . - . - .							*/
/* 6  . - . - . - . -							*/
/* 5  - . b . b . - .   White to move					*/
/* 4  . w . - . - . -							*/
/* 3  w . - . - . - .							*/
/* 2  . - . - . - . -							*/
/* 1  - . - . - . - .							*/
/* 									*/
/* Incorrect result returned because if the turn were switched to 	*/
/* black, then a capture would be possible.				*/
/* 8  . - . - . - . -							*/
/* 7  - . - . - . - .							*/
/* 6  . - . b . - . -							*/
/* 5  - . b . - . - .   White to move					*/
/* 4  . w . - . - . -							*/
/* 3  - . w . - . - .							*/
/* 2  . - . - . - . -							*/
/* 1  - . - . - . - .							*/

#define	WHITE	0
#define	BLACK	1
#define	KINGS	2
#define	WKING	2
#define	BKING	3

#define	PARTIAL	2
#define	YES	1
#define	NO	0

#define MAX( a, b )	( (a) > (b) ? (a) : (b) )
#define MIN( a, b )	( (a) < (b) ? (a) : (b) )
#define ABS( a )	( (a) >  0  ? (a) : (-a))

#define	MAX_DEPTH_SHIFT	 6
#define	MAX_DEPTH	( 1 << MAX_DEPTH_SHIFT )

/* JS REMOVE */
#define EXP_SHIFT       20
#define EXP_OFFSET      0x3ff
#define NEXT_BIT ( ( (*expptr) >> EXP_SHIFT ) - EXP_OFFSET )

/* Database access */

#define	BICOEF		7 /* 0..7 pieces of any time B/W/b/p */

/* whr.h */
typedef unsigned long	WHERE_T;
typedef WHERE_T		* WHERE_VEC_PTR_T;

#define WHERE_T_SZ	sizeof(WHERE_T)

#define	BPBOARD		0L
#define	WPBOARD		1L
#define	BKBOARD		2L
#define	WKBOARD		3L

/* definitions for database */

typedef unsigned char		DB_ELEM_T;
typedef DB_ELEM_T	*DB_VEC_PTR_T;

#define DB_ELEM_SZ	sizeof(DB_ELEM_T)

typedef struct sidxstr {
	unsigned sidxbase  : 24;
	unsigned sidxindex :  8;
} sidxindex, * sidxindexptr;

typedef struct {
	long		rangeBK;     /* number of Black king configurations */
	long		rangeWK;     /* number of White king configurations */
	long		firstBPIdx;
	long		nextBPIdx;
	long		numPos;
	sidxindexptr	dbptr;
	unsigned char	nWP;
	unsigned char	nBP;
	unsigned char	nBK;
	unsigned char	nWK;
	unsigned char	rankBP;
	unsigned char	rankWP;
} DB_REC_T;

typedef DB_REC_T	*DB_REC_PTR_T;

#define DB_REC_SZ	sizeof(DB_REC_T)

#define IBLOCK(Index)	((Index) >> 2)
#define IOFFSET(Index)	((Index) & 3)

#define GetVal(dbptr, Index)	((dbptr[IBLOCK(Index)] &		\
				  RetrieveMask[IOFFSET(Index)]) >>	\
				  (IOFFSET(Index) << 1))

#define SetVal(dbptr, Index, Val) (dbptr[IBLOCK(Index)] = 		\
				    (dbptr[IBLOCK(Index)] &		\
				     SetMask[IOFFSET(Index)]) |	\
				    (Val << (IOFFSET(Index)<<1)))

DB_REC_PTR_T	dbCreate();
/*unsigned long	dbLocbvToSubIdx();*/
DB_REC_PTR_T	dbValInit();
int32	RotateBoard(int32 x);

/* Values in the database */
#define	TIE	0
#define	WIN	1
#define	LOSS	2
#define	UNKNOWN	3

#define	DB_UNKNOWN	-9999

/* Entry for each sub-database.  */
typedef struct db {
	unsigned char defaulttype;	/* Dominant value in the db */
	unsigned char value;		/* Some dbs are all one value - if
					   so this field is set to that value */
	unsigned short startbyte;	/* Byte where subdatabase starst */
	unsigned long startaddr;	/* Position number at start byte */
	unsigned long endaddr;		/* Ending address of db */
	DB_REC_PTR_T db;		/* Pointer to index info */
} DBENTRY, * DBENTRYPTR;

/* bk X wk X bp X wp */
#define	DBTABLESIZE	( 6 * 6 * 6 * 6 )

/* Given the number w/b kings and checkers (pawns), DBINDEX gives us	*/
/* the index into DBTable.  Speeds up finding things.			*/
#define	DBINDEX( bk, wk, bp, wp )				\
	( (bk * 216) + (wk * 36) + (bp * 6) + wp )

#define Choose( n, k )		( Bicoef[ n ][ k ] )

/* System dependent parameter */
/* Number of bytes to read in at a time.  This can be increased by mult-*/
/* iples of 2, but the bigger they are, the more expensive it is to de-	*/
/* compress.  Recommend leaving as is.					*/
#define DISK_BLOCK		( 1 << 10 ) /* was (1<<10) */

/* Amount of storage for the database.  Should be as big as possible. 	*/
/* Storage is 50 * DISK_BLOCK = 50K - very small.  A few megabytes is	*/
/* better.  You can increase this parameter to whatever size you want,	*/
/* as long as it does not exceed 32MB total (see next comment).		*/
#define	DB_BUFFERS		5000 /*# of DISK_BLOCK buffers */

/* For each block of 1024 bytes, we have a record. of information */

/* Using short implies that I cannot have more than 2**15 - 1	*/
/* buffers.  To have more, change 2 structures below to ints.	*/
typedef struct dbbuf {
	int	forward;	/* In a linked list - fwd pointer */
	int	backward;	/* backward pointer */
	long	block;		/* which block of data is this */
	unsigned char data[ DISK_BLOCK ];	/* block of data */
} dbbuffer, * dbbufferptr;


#include <fcntl.h>

#ifdef SYS_WINDOWS
/* PC specifdic include files */
/*#include <alloc.h>*/
#include <io.h>
#include <sys\stat.h>
#endif /*SYSTEM*/

#include "db.h"

/* Board vector */
long Turn = BLACK;
unsigned long Locbv[3];

/* Each byte in the database contains either 5 values (3**5=243), 	*/
/* accounting for values 0..242 in a byte.  The following table tells	*/
/* values 243..255.  These values the number of consecutive positions	*/
/* whose value is equal to the "dominant" value of the database.  For	*/
/* example, Table[13] corresponds to a byte value of 255 (242+13) and	*/
/* says that the next 1600 values in the db are all the same as the 	*/
/* dominant value.							*/
int Skip[14] = {
	0, 10, 15, 20, 25, 30, 40, 50, 60, 100, 200, 400, 800, 1600
};

/* Divisors to extract values from a byte.  Byte contains 5 position	*/
/* values, each which can be W/L/D.  Div is the number to do a divide/	*/
/* mod to extract a value.						*/
/* The values are stored as ( v1 ) + ( v2 * 3 ) + ( v3 * 3*3 ) +	*/
/*		( v4 * 3*3*3 ) + ( v5 * 3*3*3*3 )			*/
int Div[7] = { 0, 1, 3, 9, 27, 81, 243 };

DBENTRYPTR DBTable[DBTABLESIZE];
int DBFile;
extern int logging; /* global from cake++.c*/
FILE *fp;
/*
 * Table to map between DB board representation and Chinook's
 * Note: the colours are on opposite sides
 *
 *	 Database Board:	 Chinook's Board:
 *	       WHITE		      BLACK
 *	   28  29  30  31	   7  15  23  31
 *	 24  25  26  27  	 3  11  19  27
 *	   20  21  22  23	   6  14  22  30
 *	 16  17  18  19  	 2  10  18  26
 *	   12  13  14  15	   5  13  21  29
 *	  8   9  10  11  	 1   9  17  25
 *	    4   5   6   7	   4  12  20  28
 *	  0   1   2   3  	 0   8  16  24
 *	      BLACK                    WHITE
 */


long Bicoef[33][BICOEF+1];
unsigned long ReverseByte[256];
unsigned long BitPos[32], NotBitPos[32];
unsigned long RetrieveMask[4];

/* This is the number of the biggest database to read.  If you set it 	*/
/* to 5, for example, the program will only use the 2..5 piece database	*/
int DBPieces = 4;

/* Used for stoing parts of the database in memory.  DBTop points to the*/
/* head of a linked list of 1024-byte blocks of the database.  The list	*/
/* is organized so that the oldest parts of the database are the first	*/
/* removed to make room for new parts.					*/
int DBTop;
dbbufferptr DBBuffer;

/* Read in from DB4.idx - index information where in the file to find 	*/
/* each sub-database, and the starting and ending position numbers for	*/
/* each 1024 bytes in the databse.					*/
long  int * DBIndex;
long  int   MaxBlockX;
short int * DBBufPtr;

/*
 * For the complete board we want to use the definitions and indexing
 * procedure below.  But, for now, use a more efficient (though less
 * general) technique.
 */

WHERE_T         BoardPos[4][12];
WHERE_T         *BPK[4];

WHERE_T         BPsave[12];
WHERE_T         *BPKsave;


#define PIECES  nbk, nwk, nbp, nwp, rankbp, rankwp
#define FMTSTR  "About to load %d%d%d%d.%d%d\n"
#define FMTVAR  nbk, nwk, nbp, nwp, rankbp, rankwp

/* function prototypes. */
void sidxCreate(DB_REC_PTR_T p,long nBP,long nWP,long rankBPL,long rankWPL,
                long *numPos,long *firstBPLIdx,long *nextBPLIdx);
long DBrevindex(long *pos,long k);
long DBindex(long *pos,long k);
void InitIndex(long off,long k,long n);
void NextIndex(long off);
void SaveIndex(long off);
void LoadIndex(long off);


/* Initialize the database data strucures */

int DBInit()
{
	int i, j, p, w, b, wk, bk, bp, wp, wr, br, stat;
	long dbidx, dbsize;
	long number, index, deftype, addr, byte, min, limit, blocks;
	char dt, dtt, next;
	long * dbindex, buffers;
	int dbffp;
	FILE * dbifp;
	dbbufferptr a;
	DBENTRYPTR ptr;
	short int * dbbufptr;
   char dbfilename[80],dbindexfilename[80];
   int buffs;

	/* Initialization */
   /* find out which files to use */
   fp=fopen("../cake-1.20/db.ini","r");
   if(fp==NULL)
   	{
      printf("db.ini is missing!\n");
      exit(0);
      }
   fscanf(fp,"%s %s %i %i",dbfilename,dbindexfilename,&DBPieces,&buffs);
   fclose(fp);


   if(logging&1)
   	{
      fp=fopen("dblog.txt","w");
   	fprintf(fp,"read db.ini\n");
   	fprintf(fp,"database file %s\n",dbfilename);
   	fprintf(fp,"database index file %s\n",dbindexfilename);
   	fprintf(fp,"number of stones %i\n",DBPieces);
   	fprintf(fp,"number of buffers %i\n",buffs);
      }
	/* ReverseByte contains the "reverse" of each byte 0..255. For	*/
	/* example, entry 17 (binary 00010001) is a (10001000) - reverse*/
	/* the bits.							*/
	for (i = 0; i < 256; i++)
	{
		ReverseByte[i] = 0;
		for (j = 0; j < 8; j++)
			if (i & (1L << j))
				ReverseByte[i] |= (1L << (7 - j));
	}

	/* Compute binomial coeficients */
	for (i = 0; i <= 32; i++)
	{
		min = MIN(BICOEF, i);
		for (j = 0; j <= min; j++)
		{
			if (j == 0 || i == j)
				Bicoef[i][j] = 1;
			else    Bicoef[i][j] =
				Bicoef[i-1][j] + Bicoef[i-1][j-1];
		}
	}
	/* Needed to make sideindex work properly.  Note that if we ever */
	/* do the 7:1 databases, this won't work!                        */
	Bicoef[0][BICOEF] = 0;

	/* Mask for extracting values */
	for (i = 0; i < DB_ELEM_SZ * 4; i++)
		RetrieveMask[i] = 3L << (i << 1);

	/* Mask for bit positons - just trying to make this more efficient*/
	/* by using tables instead of shifts all the time.		*/
	for (i = 0; i < 32; i++)
	{
		BitPos[i] = 1L << i;
		NotBitPos[i] = ~BitPos[i];
	}

	/* DBTable is used to speedup looking up an entry.  There is one*/
	/* entry for each possible database.  Note that some of the	*/
	/* posible databases do not really exist (e.g. 6 against 0)	*/
	for (i = 0; i < DBTABLESIZE; i++)
		DBTable[i] = (DBENTRYPTR) -1;
if(logging & 1)
	{
   fprintf(fp,"Wait for database loading...\n");
	fprintf(fp,"... creating table\n");  fflush(fp);
   }

if(logging & 2)
	{
   printf("Wait for database loading...\n");
	printf("... creating table\n");
   }

	/* Initalize datastructures for each size of database */
	/* For each combination of wk, bk, wp, bp totalling 6 pieces,	*/
	/* have an entry in DBTable.  Each entry is a pointer to storage*/
	/* For a pure king endgame, need only 1 entry.  For a db with	*/
	/* only 1 side having checkers, need 7 entries (checker can be	*/
	/* on one of 7 ranks).  Other dbs need 49 entries - each side	*/
	/* has a checker on 1 of 7 ranks.				*/
	for (p = 2; p <= DBPieces; p++)
	{
	    for (b = 1; b <= p; b++)
	    {
		w = p - b;
		if (w == 0)
			continue;
		for (bk = b; bk >= 0; bk--)
		{
		    for (wk = w; wk >= 0; wk--)
		    {
			bp = b - bk;
			wp = w - wk;
			if (bp == 0 && wp == 0)
				number = 1;
			else if (bp == 0 || wp == 0)
				number = 7;
			else    number = 49;
			index = DBINDEX(bk, wk, bp, wp);

			/* Allocate storage of 1, 7 or 49 entries */
			ptr = (DBENTRYPTR) malloc(sizeof(DBENTRY) * number);
			if (ptr == NULL)
			{
         if(logging&1)
				{fprintf(fp,"ERROR: malloc failure in DBInit\n"); fflush(fp);}
         if(logging&2)
            printf("ERROR: malloc failure in DBInit\n");
         exit(-1);
			}

			/* Initialize enach entry */
			DBTable[index] = ptr;
			for (i = 0; i < number; i++)
				{
				ptr->value       = UNKNOWN;
				ptr->defaulttype = UNKNOWN;
				ptr->startaddr   = 0;
				ptr->startbyte   = 0;
				ptr->endaddr     = 0;
				ptr->db          = 0;
				ptr++;
				}
         }
		}
   }
	}

	/* Open database file */
if(logging&1)
	{fprintf(fp,"... using database %s\n",dbfilename); fflush(fp);}
if(logging&2)
	printf("... using database %s\n",dbfilename);

#ifdef SYS_MACOS
	DBFile = open( dbfilename, O_BINARY|O_RDONLY);
#endif
#ifdef SYS_UNIX
	DBFile = open( dbfilename, O_RDONLY);
#endif
#ifdef SYS_WINDOWS
/* pc-specific!*/
/* on a unix-machine this is different!? */
	DBFile = open( dbfilename, O_BINARY|O_RDONLY);
#endif
	if( DBFile == 0 )
	{
	if(logging&1)
		{fprintf(fp,"Cannot open %s\n",dbfilename);fflush(fp);}
   if(logging&2)
   	printf("Cannot open %s\n",dbfilename);
		exit(-1);
	}

	/* Read in index files */
	buffers = 0;
	limit = DBPieces;
	DBPieces = 0;

	/* Index file contains all the sub-database - file is in text	*/
	/* each db is in there, and this loop reads in that info.  For	*/
	/* example, here is the head of the file:			*/
	/* BASE1100.00 =	DB 1bk, 1wk, 0bp, 0wp - mostly draws	*/
	/* S      0       0/0	Start position 0 at block 0, byte 0	*/
	/* E    995       0/189	End at position 995, at block 0 byte 189*/
	/*			By defualt, block size is 1024 bytes	*/
	/*								*/
	/*			This says this database of 995 positions*/
	/*			has been compressed to 189 bytes.	*/
	/*								*/
	/* BASE1001.00 +	DB 1bk, 0wk, 0bp, 1wp on rank 0. Wins	*/
	/* S      0       0/189						*/
	/* E    125       0/214						*/
	/*			125 positions in 214-189=25 bytes	*/
	/*			Most  positions in DB are black wins	*/
	/*								*/
	/* BASE1001.01 =	DB 1bk, 0wk, 0bp, 1wp on rank 1.  Draws	*/
	/* S      0       0/214						*/
	/* etc.			Interesting.  With the checker on the	*/
	/*			1st rank most pos are draws, on the 0th,*/
	/*			wins.  Must be because of the move.	*/
	/*								*/
	/* BASE0011.51 ==	DB 0 kings, 1 bp on 5th, 1 wp on 1st	*/
	/*			== means entire db is drawn.  ++ means	*/
	/*			all db is win; -- all is loss.		*/
	/*								*/
	/* Here is a bigger db:						*/
	/* BASE1311.26 -						*/
	/* S      0    2024/783	Position 0 at block 2024, byte 783	*/
	/*			i.e. starts at addr=2024*1024+783	*/
	/* . 327765    2025	Next block (2025), 1st pos is 327,765	*/
	/* .1625795    2026	Next block (2026), 1st pos is 1,625,795	*/
	/* E1753920    2026/143	Ending addr=2026*1024+143		*/
	/*			Num positions in db = 1,753,920		*/
	/*			Bytes used: endaddr-startaddr=1408	*/
	/*			Nice compression ratio!!		*/

		/* Allocate index files */
		/* Need to get size from end of file. */
		dbffp = DBFile;
		dbsize = lseek(dbffp, 0L, 2);
		lseek(dbffp, 0L, 0);
		blocks = (dbsize / DISK_BLOCK);

		/* MaxBlockX contains the number of the last block in the */
		/* file.  I check for read errors.  All reads of db get	*/
		/* DISK_BLOCK bytes - except the last read.		*/
		MaxBlockX = blocks++;
		if(logging&1)
			{fprintf(fp,"... allocating %ld/%ld entry index\n", dbsize, blocks);fflush(fp);}
		if(logging&2)
			printf("... allocating %ld/%ld entry index\n", dbsize, blocks);

		DBIndex = dbindex  = (     long *)
				malloc(blocks * sizeof(      long));
		DBBufPtr = dbbufptr = (short int *)
				malloc(blocks * sizeof(short int));
		if (dbindex == NULL || dbbufptr == NULL)
		{
		if(logging&1)
			{fprintf(fp,"ERROR: dbindex malloc\n");fflush(fp);}
      if(logging&2)
			{printf("ERROR: dbindex malloc\n");}

			exit(-1);
		}

		/* Read index file and build table */
		dbidx = -1;
		dbifp = fopen(dbindexfilename, "r");
		if (dbifp == NULL)
		{
		if(logging&1)
      	{fprintf(fp,"ERROR: cannot open %s\n",dbindexfilename); fflush(fp); }
      if(logging&2)
      	{printf("ERROR: cannot open %s\n",dbindexfilename); }

		exit(-1);
		}

		/* Read the entire index file */
		for (i = 0; ;)
		{
			/* Parse the header line of each sub-database */
			stat = fscanf(dbifp, "BASE%1d%1d%1d%1d.%1d%1d %c%c",
				&bk, &wk, &bp, &wp, &br, &wr, &dt, &dtt);
			if (stat <= 0)
				break;
			if (stat < 8)
			{
			if(logging&1)
				{fprintf(fp,"ERROR: DBInit scanf failure %d\n",stat); fflush(fp); }
         if(logging&2)
				{printf("ERROR: DBInit scanf failure %d\n",stat);}

				exit(-1);
			}

			i = bk + wk + bp + wp;
			if (i > limit)
				break;
			if (i != DBPieces)
			{
			if(logging&1)
				{fprintf(fp,"... initializing %d piece database\n", i);fflush(fp); }
         if(logging&2)
				{printf("... initializing %d piece database\n", i); }

				DBPieces = i;
			}

			/* What is the database? */
			index = DBINDEX(bk, wk, bp, wp);
			ptr = DBTable[index];
			if (ptr == NULL)
			{
			if(logging&1)
         	{
				fprintf(fp,"ERROR: %d %d %d %d %d %d %c dbtab\n",
					bk, wk, bp, wp, br, wr, dt);   fflush(fp); }
			if(logging&2)
         	{
				printf("ERROR: %d %d %d %d %d %d %c dbtab\n",
					bk, wk, bp, wp, br, wr, dt); }
				exit(-1);
			}

			/* Based on who has checkers, index into the	*/
			/* 1/7/49 entries.				*/
			if (wp == 0)
				number = br;
			else    number = br * 7 + wr;
			ptr += number;

			/* If we are not at end of line, we have a db	*/
			/* that is all one value.			*/
			if (dtt != '\n')
			{
				if (dtt == '+')
					ptr->value = WIN;
				else if (dtt == '-')
					ptr->value = LOSS;
				else if (dtt == '=')
					ptr->value = TIE;
				else
            	{
					if(logging&1)
						{fprintf(fp,"ERROR: illegal result %c\n",dtt);fflush(fp);}
					if(logging&2)
						{printf("ERROR: illegal result %c\n",dtt);}

              }
				fscanf(dbifp, "\n");
				continue;
			}

			/* Create the index informaiton for the database */
			ptr->db = dbValInit((long)bk,(long)wk,(long)bp,
					 (long)wp,(long)br,(long)wr);

			/* Set the default type */
			if (dt == '=')
				deftype = TIE;
			else if (dt == '+')
				deftype = WIN;
			else if (dt == '-')
				deftype = LOSS;
			else
         	{
				if(logging&1)
					{fprintf(fp,"ERROR: illegal type %c\n", dt);fflush(fp);}
				if(logging&2)
					{printf("ERROR: illegal type %c\n", dt);}
				exit(-1);
				}
			ptr->defaulttype = deftype;

			/* Read in the starting address */
			next = getc(dbifp);
			if (next != 'S')
			{
         if(logging&1)
            {fprintf(fp,"ERROR: illegal start %c\n", next);fflush(fp); }
         if(logging&2)
            {printf("ERROR: illegal start %c\n", next); }
				exit(-1);
			}
			stat = fscanf(dbifp, "%ld%ld/%ld\n", &index, &addr,
							&byte);
			if (stat != 3)
			{
			if(logging&1)
				{fprintf(fp,"ERROR: illegal start read %d\n",
							stat);fflush(fp);}
			if(logging&2)
				{printf("ERROR: illegal start read %d\n",
							stat);}
				exit(-1);
			}
			ptr->startaddr = addr;
			ptr->startbyte = byte;

			/* If starting address takes us into a new block, */
			/* update table of indexes.			*/
			if (addr != dbidx)
			{
				dbindex [++dbidx] = index;
				dbbufptr[  dbidx] = -1;
			}

			/* Read each line of the database - block number */
			/* and position number.				*/
			for (; ;)
			{
				stat = fscanf(dbifp, "%c", &dt);
				if (stat != 1)
				{
				if(logging&1)
					{fprintf(fp,"ERROR: db read1 %d\n", stat);fflush(fp);}
				if(logging&2)
					{printf("ERROR: db read1 %d\n", stat);}
					exit(-1);
				}
				if (dt == 'E')
					break;
				stat = fscanf(dbifp, "%ld%ld\n", &index, &addr);
				if (stat != 2)
				{
				if(logging&1)
               {fprintf(fp,"ERROR: db read2 %d\n", stat); fflush(fp);  }
				if(logging&2)
               {printf("ERROR: db read2 %d\n", stat);  }
					exit(-1);
				}
				if (addr != dbidx)
				{
					dbindex [++dbidx] = index;
					dbbufptr[  dbidx] = -1;
				}
			}
			stat = fscanf(dbifp, "%ld%ld/%ld\n",
						&index, &addr, &byte);
			if (stat != 3)
			{
			if(logging&1)
            {fprintf(fp,"ERROR: db read3 %d\n", stat); fflush(fp);}
			if(logging&2)
            {printf("ERROR: db read3 %d\n", stat); }

				exit(-1);
			}
			ptr->endaddr = addr;
		}
		fclose(dbifp);

	/* Initialize database buffers - this is the storage used to hold */
	/* portions of the database, assuming you do not have enough memory */
	/* to hold the entire db.  If you do, things are *much simpler* */
	buffers += buffs;
	if(logging&1)
   	{fprintf(fp,"... allocating %ld buffers\n", buffers);fflush(fp);}
	if(logging&2)
   	{printf("... allocating %ld buffers\n", buffers);}
	DBBuffer = (dbbufferptr) malloc(buffers * sizeof(dbbuffer));
	if (DBBuffer == NULL)
	{
	if(logging&1)
      {fprintf(fp,"ERROR: malloc failure on DBBuffer\n");  fflush(fp);}
	if(logging&2)
      {printf("ERROR: malloc failure on DBBuffer\n");}
		exit(-1);
	}
	if(logging&1)
   	{fprintf(fp,"... initializing %d buffers\n", buffs);fflush(fp);}
	if(logging&2)
   	{printf("... initializing %d buffers\n", buffs);}

	for (i = 0; i < buffs; i++)
	{
		a = &DBBuffer[i];
		a->forward  = i + 1;
		a->backward = i - 1;
		a->block    =    -1;
		for (j = DISK_BLOCK -1; j >= 0; j--)
			a->data[j] = 0;
	}
	DBBuffer[i - 1].forward  = 0;
	DBBuffer[0    ].backward = i - 1;
	DBTop = 0;
   return(DBPieces);
}

/* Look up a position in the database */
/* Do not call with one side having 0 pieces */
/* this is the function you call from the checkers program */
long DBLookup(struct pos p, int turn)
{
	long result, diff, back, fwd, use;
	unsigned long index, cindex, c;
	DBENTRYPTR dbentry;
	long start, end, middle, byte, i, def;
	unsigned char *buffer;
	unsigned long *dbindex;
	/*long startx, endx, inc;*/
	short int *dbbufptr;
	int cx;

   /*int32 Locbv[3];*/
   Locbv[0]=p.bm|p.bk;
   Locbv[1]=p.wm|p.wk;
   Locbv[2]=p.wk|p.bk;

   Turn=turn;
   /* set the global board: */
   Gposition=p;
   /*Turn=BLACK;
    * in my version this database is only called with Turn=Black */


	/* Find out its index */
   /* we don't need to hand on position and turn since they're global */
	index = dbLocbvToSubIdx(&dbentry,p,turn);

	/* Check if we already know its value because the db is all one value */
	if ((long)index < 0) {
		if (index == -(WIN+1))
			return((long) WIN);
		if (index == -(LOSS + 1))
			return((long) LOSS);
		if (index == -(TIE + 1))
			return((long) TIE);
		if (index == DB_UNKNOWN)
			return((long) DB_UNKNOWN);
		return(index);
	}

	/* Have to find the value */
	start = dbentry->startaddr;
	end   = dbentry->endaddr;
	byte  = dbentry->startbyte;
	dbindex  = (unsigned long int*)DBIndex;
	dbbufptr = DBBufPtr;

	/* Find the block where the position is located.  Do a binary	*/
	/* on the blocks of the databse to find which block the position*/
	/* must be in.							*/
	while (start < end)
   	{
		middle = (start + end + 1) / 2;
		if (dbindex[middle] <= index)
			start = middle;
		else
      	end = middle - 1;
		}

	if (start != end)
		{
		if(logging&1)
			{
         fprintf(fp,"ERROR: cannot agree %ld %ld \n", start, end);
         fflush(fp);
         }
		if(logging&2)
			{
         printf("ERROR: cannot agree %ld %ld \n", start, end);
         }
		return((long) DB_UNKNOWN);
		}

	middle = start;

	/* Is this block in memory?  If not, read it in */
	if (dbbufptr[middle] == -1)
		{
		/* Need a buffer to read into.  Look at the top one (DBTop)*/
		/* If has some data, mark it as invalid */
		if (      DBBuffer[DBTop].block   >= 0)
		    DBBufPtr[DBBuffer[DBTop].block] = -1;
		use = DBTop;

		/* Seek to the block - block# * 1024 */
		if (lseek(DBFile, (long)(middle * DISK_BLOCK), 0) == -1)
			{
			if(logging&1)
				{fprintf(fp,"ERROR: dblookup seek failed\n"); fflush(fp);}
			if(logging&2)
				{printf("ERROR: dblookup seek failed\n"); }
         return((long) DB_UNKNOWN);
			}

		/* Read in the DISK_BLOCK bytes */
		cx = read(DBFile, (char*)&DBBuffer[use].data[0],DISK_BLOCK);
		if (cx != DISK_BLOCK)
			/* Check for error - should get DISK_BLOCK bytes,*/
			/* except for the last block */
			if (middle != MaxBlockX)
			{
			if(logging&1)
				{fprintf(fp,"ERROR: dblookup read failed %d %ld %ld %ld %ld\n",cx,start,end,middle,MaxBlockX);fflush(fp);}
			if(logging&2)
				{printf("ERROR: dblookup read failed %d %ld %ld %ld %ld\n",cx,start,end,middle,MaxBlockX);}
				return((long) DB_UNKNOWN);
			}
		/* Mark this buffer as in use */
		dbbufptr[middle] = use;

		/* Fix linked list to point to the next oldest entry */
		DBTop = DBBuffer[DBTop].forward;
		DBBuffer[use].block = middle;
		}

	else
   	{
		/* In memory - no i/o necessary.  Adjust linked list so this */
		/* block is moved to the end of the list, since it is the */
		/* most recently used.					*/
		use = dbbufptr[middle];
		if (DBTop == use)
			DBTop = DBBuffer[DBTop].forward;
		else {
			back = DBBuffer[use].backward;
			fwd  = DBBuffer[use].forward;
			DBBuffer[back].forward = fwd;
			DBBuffer[fwd].backward = back;

			back = DBBuffer[DBTop].backward;
			DBBuffer[back].forward = use;
			DBBuffer[use].backward = back;
			DBBuffer[use].forward = DBTop;
			DBBuffer[DBTop].backward = use;
		}
	}

	/* We have the block - now need to find the position value. */
        if( start == dbentry->startaddr )
        {
                i = byte;
                cindex = 0;
        }
        else {
                i = 0;
                cindex = dbindex[ start ];
        }
        def = dbentry->defaulttype;
        buffer = &DBBuffer[ use ].data[ 0 ];

	/* Scan through block until we find the position.  cindex is	*/
	/* current position number; i is the starting byte.		*/
        for( ; i < DISK_BLOCK; i++ )
        {
                c = buffer[ i ] & 0xff;
                if( c > 242 )
                {
			/* Byte is a "skip" byte.  Skip tells us how many*/
			/* positions in a row have the same value.	 */
                        cindex += Skip[ c - 242 ];
                        if( index < cindex )
                        {
				/* If "skip"ing takes us past the pos'n	*/
				/* number, we know the positions value.  */
                                result = def;
                                break;
                        }
                }
                else {
			/* Byte contains 5 values */
                        if( cindex + 5 <= index )
				/* Did not find the position - skip 5 pos */
                                cindex += 5;
                        else {
				/* Found - extract the value */
                                diff = index - cindex;
                                result = c % Div[ 5 - diff + 1 ];
                                result = result / Div[ 5 - diff ];
                                break;
                        }
                }
        }

	/* Safety check */
	if (i < 0 || i >= DISK_BLOCK)
	{
	if(logging&1)
      {fprintf(fp,"ERROR: scanned block and did not find\n");
		fprintf(fp,"INDEX %ld CINDEX %ld\n", index, cindex);
		fprintf(fp,"Start %ld end %ld byte %d = middle %ld\n",
			dbentry->startaddr,
			dbentry->endaddr,
			dbentry->startbyte, middle);   fflush(fp); }
	if(logging&2)
      {
      printf("ERROR: scanned block and did not find\n");
		printf("INDEX %ld CINDEX %ld\n", index, cindex);
		printf("Start %ld end %ld byte %d = middle %ld\n",
			dbentry->startaddr,
			dbentry->endaddr,
			dbentry->startbyte, middle);   }

		return((long) DB_UNKNOWN);
	}

	if (result == WIN)
		return((long) WIN);
	if (result == LOSS)
		return((long) LOSS);
	return((long) TIE);
}


DB_REC_PTR_T dbValInit(PIECES)
	long PIECES;
{
	return(dbCreate(PIECES));
}

/*
 * Allocates memory for a DB_REC_T and initializes its scalars and SIdx.
 */
DB_REC_PTR_T dbCreate(PIECES)
	long PIECES;
{
	DB_REC_PTR_T p;

	p = (DB_REC_PTR_T) malloc(DB_REC_SZ);
	/*
	 * Compute the number of black and white king configurations for this
	 * database slice
	 */
	p->rangeBK = Choose(32 - nbp - nwp, nbk);
	p->rangeWK = Choose(32 - nbp - nwp - nbk, nwk);

	/* Sets p->sidxbase and p->sidxindex */
	sidxCreate(p, nbp, nwp, rankbp, rankwp,
			&(p->numPos), &(p->firstBPIdx), &(p->nextBPIdx));
	p->numPos *= p->rangeBK * p->rangeWK;
	p->nBP = nbp;
	p->nWP = nwp;
	p->nBK = nbk;
	p->nWK = nwk;
	p->rankBP = rankbp;
	p->rankWP = rankwp;
	return(p);
}

#define LeadWP (7 - (BoardPos[WPBOARD][db->nWP-1] >> 2))

/*
 * CountHits - count the number of pieces occupying squares less than the
 * 	       current square
 *
 * Parameters:
 *
 *	XXpos	piece array to be counted
 *	nXX	number of pieces in XXpos
 *	YYpos	piece array to be compared against
 *	nYY	number of pieces in YYpos
 *	XXhits	an array containing the number of YYpos pieces which occupy
 *		squares less that those in XXpos.  XXhits[n] has the number
 * 		of YYpos pieces in lesser squares than the piece XXpos[n].
 *
 * Example:
 *
 *	Suppose there are black kings on squares 1 and 11, and white kings
 * 	on squares 10 and 26.  This routine sets XXhits as follows:
 *
 *	XXpos	26 10		YYpos	11 1		XXhits	2 1
 *	nXX	2		nYY	2
 */
#define CountHits(XXpos, nXX, YYpos, nYY, XXhits) \
{ \
	register long x, y; \
	if (nYY > 0) { \
		for (x = 0; x < nXX; x++) { \
			if (XXpos[x] < YYpos[nYY-1]) \
				break; \
			XXhits[x]++; \
			for (y = nYY - 2; y >= 0; y--) { \
				if (XXpos[x] < YYpos[y]) \
					break; \
				XXhits[x]++; \
			} \
		} \
	} \
}

/*
 * CountRevHits - count the number of pieces occupying squares greater than
 * 	          the current square
 *
 * Parameters:
 *
 *	XXpos	piece array to be counted
 *	nXX	number of pieces in XXpos
 *	YYpos	piece array to be compared against
 *	nYY	number of pieces in YYpos
 *	XXhits	an array containing the number of YYpos pieces which occupy
 *		squares greater that those in XXpos.  XXhits[n] has the number
 * 		of YYpos pieces in greater squares than the piece XXpos[n].
 *
 * This routine is similar to CountHits, except it is used for computing the
 * index of the white checkers (since, unlike the other piece types, they are
 * always placed starting from the top of the board).
 */
#define CountRevHits(XXpos, nXX, YYpos, nYY, XXhits) \
{ \
	register long x, y; \
	if (nYY > 0) { \
		for (x = nXX - 1; x >= 0; x--) { \
			if (XXpos[x] > YYpos[0]) \
				break; \
			XXhits[x]++; \
			for (y = 1; y < nYY; y++) { \
				if (XXpos[x] > YYpos[y]) \
					break; \
				XXhits[x]++; \
			} \
		} \
	} \
}

/*
 * SquishBP - squish the black checker configuration and determine its
 *	      reverse index
 *
 * Parameters:
 *
 *	nXX	number of black checkers
 *	XXpos	array containing square numbers (in descending order)
 *
 * Since the black checkers are placed on the board first, there are no
 * conflicts with other pieces and hence no "squishing" is required to
 * determine its reverse index.  Thus, all that is necessary is to compute
 * its reverse index.
 */
#define SquishBP(nXX, XXpos) \
{ \
	if (nXX) { \
		bpidx = DBrevindex(XXpos, nXX); \
		rankbp = XXpos[0] >> 2; \
	} \
	else { \
		bpidx = 0; \
		rankbp = 0; \
	} \
}

/*
 * SquishWP - squish the white checker configuration and determine its
 *	      reverse index
 *
 * Parameters:
 *
 *	nXX	number of white checkers
 *	nYY	number of black checkers
 *	XXpos	array containing white checker square numbers
 *	YYpos	array containing black checker square numbers
 *
 * Since the white checkers are placed on the board after the black checkers,
 * there may have been possible conflicts with the black checkers during the
 * enumeration process.  As a result, the computation applied directly to the
 * black checker configuration cannot be applied to the white checker
 * configuration without first "squishing" the white checker configuration.
 * This is done by determining how many of the black checkers interfere with
 * each white checker, and then decrementing this amount from each white
 * checker square.  After this is complete, the reverse index is computed.
 */
#define SquishWP(nXX, nYY, XXpos, YYpos) \
{ \
	register long i; \
	if (nXX) { \
		for (i = nXX - 1; i >= 0; i--) \
			XXhits[i] = 0; \
		CountRevHits(XXpos, nXX, YYpos, nYY, XXhits); \
		for (i = nXX - 1; i >= 0; i--) \
			XXhits[i] = 31 - (XXpos[i] + XXhits[i]); \
		wpidx = DBindex(XXhits, nXX); \
		rankwp = (31 - XXpos[nXX - 1]) >> 2; \
	} \
	else { \
		wpidx = 0; \
		rankwp = 0; \
	} \
}

/*
 * SquishBK - squish the black king configuration and determine its
 *	      reverse index
 *
 * Parameters:
 *
 *	nXX	number of black kings
 *	nYY	number of white checkers
 *	nZZ	number of black checkers
 *	XXpos	array containing black king square numbers
 *	YYpos	array containing white checker square numbers
 *	ZZpos	array containing black checker square numbers
 *
 * Since the black kings are placed on the board after the white checkers,
 * there may have been possible conflicts with both the white checkers and
 * the black checkers during the enumeration process.  As with the white
 * checkers, the black king configuration must first be "squished" to
 * determine its correct index.  This is done by determining how many of
 * the white and black checkers interfere with each black king, and then
 * decrementing this amount from each black king square.  After this is
 * complete, the reverse index is computed.
 */
#define SquishBK(nXX, nYY, nZZ, XXpos, YYpos, ZZpos) \
{ \
	register long i; \
	if (nXX) { \
		for (i = nXX - 1; i >= 0; i--) \
			XXhits[i] = 0; \
		CountHits(XXpos, nXX, YYpos, nYY, XXhits); \
		CountHits(XXpos, nXX, ZZpos, nZZ, XXhits); \
		for (i = nXX - 1; i >= 0; i--) \
			XXhits[i] = XXpos[i] - XXhits[i]; \
		bkidx = DBrevindex(XXhits, nXX); \
	} \
	else bkidx = 0; \
}

/*
 * SquishWK - squish the white king configuration and determine its
 *	      reverse index
 *
 * Parameters:
 *
 *	nXX	number of white kings
 *	nYY	number of black kings
 *	nZZ	number of white checkers
 *	nYZ	number of black checkers
 *	XXpos	array containing white king square numbers
 *	YYpos	array containing black king square numbers
 *	ZZpos	array containing white checker square numbers
 *	YZpos	array containing black checker square numbers
 *
 * Since the white kings are placed on the board after the black kings,
 * there may have been possible conflicts with the black kings, white
 * checkers, and the black checkers during the enumeration process.  As
 * with the white checkers and black kings, the white king configuration
 * must first be "squished" to determine its correct index.  This is done
 * by determining how many of the black kings, white checkers, and black
 * checkers interfere with each white king, and then decrementing this
 * amount from each white king square.  After this is complete, the reverse
 * index is computed.
 */
#define SquishWK(nXX, nYY, nZZ, nYZ, XXpos, YYpos, ZZpos, YZpos) \
{ \
	register long i; \
	if (nXX) { \
		for (i = nXX - 1; i >= 0; i--) \
			XXhits[i] = 0; \
		CountHits(XXpos, nXX, YYpos, nYY, XXhits); \
		CountHits(XXpos, nXX, ZZpos, nZZ, XXhits); \
		CountHits(XXpos, nXX, YZpos, nYZ, XXhits); \
		for (i = nXX - 1; i >= 0; i--) \
			XXhits[i] = XXpos[i] - XXhits[i]; \
		wkidx = DBrevindex(XXhits, nXX); \
	} \
	else wkidx = 0; \
}

/*
 * EXTRACT_PIECES - extract pieces into an array
 *
 * Parameters:
 *
 *	vec	a 32 bit unsigned integer describing pieces on squares
 *		according to the database format
 *	nXX	number of pieces extracted
 *	XXpos	array containing square numbers (in descending order)
 *
 * Example: Suppose vec = 0x00000003 (which corresponds to pieces on database
 * squares 0 and 1 (1 and 2 in the checkers literature).  EXTRACT_PIECES will
 * set nXX and XXpos as follows:
 *
 *		nXX	2
 *		XXpos	1 0
 */
#define EXTRACT_PIECES(vec, nXX, XXpos) \
{ \
	nXX = 0; \
	while (vec) { \
		conv.x = vec; \
		XXpos[nXX] = NEXT_BIT; \
		vec ^= (unsigned long)(1L << XXpos[(nXX)++]); \
	} \
}

/*
-- In: a black-to-move board in Locbv[]
-- Out: Returns the index of the required 2-bit value.
*/
/*
 * dbLocbvToSubIdx - Compute a position index
 */
int32 dbLocbvToSubIdx(DBENTRYPTR *dbentry,struct pos p,int turn)
{
	long            PIECES;
	long            bppos[12], wppos[12];
	long            bkpos[12], wkpos[12];
	long            XXhits[12];
	long            bpidx, wpidx, bkidx, wkidx, firstidx;
	unsigned long   vec, Blackbv, Whitebv, Kingbv;
	long            Index;
	sidxindexptr	ptr;
	DB_REC_PTR_T    db;
	union {
		unsigned long    U;
		unsigned char C[4];
	} a, b;
	union {
		double x;
		long s[2];
	} conv;
#ifndef BIGEND
	long            *expptr = &conv.s[1];
#else
	long            *expptr = &conv.s[0];
#endif


	/* If it's white to move, reverse the board & look up black to move. */
	if (Turn == WHITE) {

   	/* additional code by MF*/
      Locbv[WHITE]=Gposition.wm|Gposition.wk;
      Locbv[BLACK]=Gposition.bm|Gposition.bk;
      Locbv[KINGS]=Gposition.bk|Gposition.wk;
      /*end add.*/
		a.U = Locbv[WHITE];
		b.C[3] = ReverseByte[a.C[0]];
		b.C[2] = ReverseByte[a.C[1]];
		b.C[1] = ReverseByte[a.C[2]];
		b.C[0] = ReverseByte[a.C[3]];
		Blackbv = b.U;
		a.U = Locbv[BLACK];
		b.C[3] = ReverseByte[a.C[0]];
		b.C[2] = ReverseByte[a.C[1]];
		b.C[1] = ReverseByte[a.C[2]];
		b.C[0] = ReverseByte[a.C[3]];
		Whitebv = b.U;
		a.U = Locbv[KINGS];
		b.C[3] = ReverseByte[a.C[0]];
		b.C[2] = ReverseByte[a.C[1]];
		b.C[1] = ReverseByte[a.C[2]];
		b.C[0] = ReverseByte[a.C[3]];
		Kingbv = b.U;
      /* more additional code:*/
      Gposition.bm=Blackbv&(~Kingbv);
      Gposition.bk=Blackbv&(Kingbv);
      Gposition.wm=Whitebv&(~Kingbv);
      Gposition.wk=Whitebv&(Kingbv);
      /* end add. code */
	}

	/* Extract and sort the black checkers */
	vec=Gposition.bm;
	EXTRACT_PIECES(vec, nbp, bppos);
	SquishBP(nbp, bppos);

	/* Extract and sort the white checkers */
   vec=Gposition.wm;
	EXTRACT_PIECES(vec, nwp, wppos);
	SquishWP(nwp, nbp, wppos, bppos);

	/* Extract and sort the black kings */
   vec=Gposition.bk;
	EXTRACT_PIECES(vec, nbk, bkpos);
	SquishBK(nbk, nwp, nbp, bkpos, wppos, bppos);

	/* Extract and sort the white kings */
	vec=Gposition.wk;
	EXTRACT_PIECES(vec, nwk, wkpos);
	SquishWK(nwk, nbk, nwp, nbp, wkpos, bkpos, wppos, bppos);

	/* Only consider positions where black dominates white */
	*dbentry = DBTable[DBINDEX(nbk, nwk, nbp, nwp)];
	if (*dbentry == 0)
   	{
		if(logging&1)
      	{
         fprintf(fp,"ERROR: cannot happen!\n");
         fflush(fp);
         }
		if(logging&2)
      	{
         printf("ERROR: cannot happen!\n");
         }
		exit(-1);
		}
	if (nwp == 0)
		*dbentry += rankbp;
	else    *dbentry += (rankbp * 7 + rankwp);
	if (*dbentry == NULL)
		return((long) (DB_UNKNOWN - 1));

	if ((*dbentry)->value != UNKNOWN)
		return(-((*dbentry)->value + 1));

	/* Determine wpidx, then combine bp & wp indices into bpidx. */
	db    = (*dbentry)->db;
	if (db == NULL)
		return((long)(DB_UNKNOWN - 1));

	ptr = db->dbptr;
	bpidx -= db->firstBPIdx;	/* get matching index number */

	/*
	 * Compute the position index by:
	 *
	 * Index = (BASE * rangeBK * rangeWK) +                 (1)
	 *         ((wpidx - FIRST) * rangeBK * rangeWK) +      (2)
	 *         (bkidx * rangeWK) +                          (3)
	 *          wkidx                                       (4)
	 *
	 * where:
	 *       (1) is the number of positions with the black checker index
	 *	     less than the desired black checker index
	 *       (2) is the number of positions with the desired black checker
	 *	     index but having a white checker index less than the
	 *	     desired white checker index
	 *       (3) is the number of positions with the desired black and
	 *	     white checker index but having a black king index less
	 *	     than the desired black king index
	 *	 (4) is the number of positions with the desired black checker,
	 *	     white checker, and black king index but having a white
	 *	     king index less than the desired white king index
	 */
	firstidx = Choose((ptr[bpidx].sidxindex >> 3), (ptr[bpidx].sidxindex & 07));
	bpidx = ptr[bpidx].sidxbase + wpidx - firstidx;
	Index = (((bpidx * db->rangeBK) + bkidx) * db->rangeWK) + wkidx;
	return(Index);

} /* dbLocbvToSubIdx() */


/*
 * A database slice is determined by the leading rank of the black and white
 * checkers.  A position index within the slice is computed by the position
 * of the black checkers, then adding the white checkers, followed by the
 * black kings, and finally the white kings.
 *
 * The following "|" notation refers to "choose";
 *  i.e. | n | = n!/((n-k)!k!).
 *       | k |
 *
 * The number of positions with nbp black checkers with leading rank rbp
 * (where 0 <= rbp <= 7) is MaxBP = | 4 x (rbp+1) | - | 4 x rbp |.
 *                                  |     nbp     |   |  nbp    |
 *
 * Next the white checkers are added.  This is discussed in more detail below.
 *
 * The number of positions created by adding nbk black kings is
 *      MaxBK = | 32 - nbp - nwp |
 *              |      nbk       |
 *
 * and the number of positions created by adding nwk white kings is
 *      MaxWK = | 32 - nbp - nwp - nbk |.
 *              |         nwk          |
 *
 *
 * Computing the number of positions with nbp black checkers having leading
 * rank rbp and nwp white checkers having leading rank rwp is difficult because
 * some of the black checker configurations may have one or more checkers
 * within the first rwp checker squares.  Thus the simple formula applied to
 * computing MaxBP cannot be applied to MaxWP because different black checker
 * configurations may have a different number of white checker configurations.
 * MaxWP is computed by creating a set of subindexes for every black checker
 * configuration.  The number of white checker positions for each black checker
 * configuration is computed and stored, as well as a cumulative total.  The
 * amount of storage required can be quite large for a large number of black
 * checkers.
 *
 * See Appendix B in the Technical Report TR93-13 for more details.
 */

void sidxCreate(DB_REC_PTR_T p,long nBP,long nWP,long rankBPL,long rankWPL,
                long *numPos,long *firstBPLIdx,long *nextBPLIdx) {
	unsigned long firstWPLIdx, nextWPLIdx, Base;
	sidxindexptr ptr;
	unsigned long Maximum;
	long nsqr1, nsqr2;
	long fsq, fpc;

	if (nBP == 0) {
		*firstBPLIdx = 0;
		*nextBPLIdx = 1;
	}
	else {
		/*
		 * Determine the number of black positions which have one
		 * or more pawns on rankBPL.  This number is obtained by
		 * (4*(rank+1) choose nBP) - (4*rank choose nBP).
		 */
		*firstBPLIdx = Choose(rankBPL << 2, nBP);
		*nextBPLIdx  = Choose((rankBPL + 1) << 2, nBP);

#ifdef UNUSED
		/*
		 * Don't fill the entire rank with black pawns; leave room
		 * for one white pawn.
		 */
		if (nBP == 4 && (rankBPL + rankWPL) == 7)
			(*nextBPLIdx)--;
#endif
	}

	/*
	 * Allocate the secondary index array.  This array holds the
	 * cumulative number of white checker positions for each black
	 * checker configuration.
	 */
	p->dbptr = ptr = (sidxindexptr) malloc (
		(unsigned long) (1 + *nextBPLIdx - *firstBPLIdx) *
		(unsigned long) sizeof(unsigned long));

	/*
	 * This array holds the starting index for each white checker
	 * configuration.  It is used later in dbLocbvToSubIdx to compute
	 * a position index.
	 */

	/* initialize the array with FIRST & BASE */
	Base = 0;
	if (nWP == 0) {
		firstWPLIdx = 0;
		nextWPLIdx  = 1;
		fsq = 0;
		fpc = BICOEF;
	}
	if (nBP) {
		SaveIndex(BPBOARD);
		InitIndex(BPBOARD, nBP, MAX(rankBPL << 2, nBP-1));
	}
	/* Do, for each black checker configuation... */
	for (Maximum = *nextBPLIdx - *firstBPLIdx; Maximum; Maximum--) {
		if (nWP) {
			/* Compute available squares for white checkers */
			if (nBP) {
				nsqr1 = Nsq(rankWPL-1, BoardPos[BPBOARD], nBP);
				nsqr2 = Nsq(rankWPL,   BoardPos[BPBOARD], nBP);
			}
			else {
				nsqr1 = (rankWPL)   << 2;
				nsqr2 = (rankWPL+1) << 2;
			}
			/*
			 * nextWPLIdx - firstWPLidx represents the number of
			 * white checker positions for this black checker
			 * configuration
			 */
			firstWPLIdx = Choose(nsqr1, nWP);
			nextWPLIdx  = Choose(nsqr2, nWP);

			/*
			 * Save the starting index for this white checker
			 * configuration.  This is used later to compute
			 * the index for a position.
			 */
			fsq = nsqr1;
			fpc = nWP;
		}

		/* store these values for future reference */
		ptr->sidxbase = Base;
		ptr->sidxindex = ( fsq << 3 ) | fpc;
		ptr++;

		/*
		 * Add the number of white checker positions to the cumulative
		 * amount
		 */
		Base += (nextWPLIdx - firstWPLIdx);

		/* Get the next black checker configuration */
		if (nBP)
			NextIndex(BPBOARD);
	}
	/* Restore the original black checker configuration */
	if (nBP)
		LoadIndex(BPBOARD);
	*numPos = Base;
} /* sidxCreate() */

/*
 * Nsq - return the number of squares available for placement of the white
 * checkers.
 *
 * Paramters:
 *	rank	leading white checker rank
 *	Ppos	an array describing the square number of the black checkers
 *	nP	is the number of black checkers.
 *
 * Although this routine is generalized, it is used only for determining white
 * checker squares.
 */
long Nsq(rank, Ppos, nP)
	register long rank;
	register long nP;
	WHERE_T *Ppos;
{
	register long pawnhit  = ((7 - rank) << 2);
	register long nsquares = ((rank + 1) << 2);

	while (nP--) {
		if (*Ppos++ >= pawnhit)
			nsquares--;
	}
	return(nsquares);
}

/*
 * The next set of routines initializes and increments the piece counters.
 * BoardPos is a 4x12 array used to store the square number for each of the
 * potential piece types in the following manner:
 *
 *		BoardPos[0] => Black checkers (BPBOARD)
 *		BoardPos[1] => White checkers (WPBOARD)
 *		BoardPos[2] => Black kings (BKBOARD)
 *		BoardPos[3] => White kings (WKBOARD)
 *
 * All square numbers for the Kings and Black checkers are stored in increasing
 * numerical order, from left to right.  The white checker square numbers are
 * stored in decreasing numerical order, left to right.
 *
 * BPK[n] is a pointer to the last piece in BoardPos[n].
 */

/*
 * InitIndex - initialize a piece counter
 *
 * Parameters:
 *	off	piece type (BPBOARD, WPBOARD, BKBOARD, or WKBOARD)
 *	k	number of pieces
 *	n	starting square number of highest ranked piece
 */
void InitIndex(long off,long k,long n) {
	register WHERE_T *BPP, *BPE;

	/* Initialize all the pieces in the starting position. */
	BPK[off] = &BoardPos[off][k];
	for (BPP = BoardPos[off], BPE = &BPK[off][-1]; BPP < BPE; BPP++)
		*BPP = (long)(BPP - BoardPos[off]);
	*BPE = n;
}

/*
 * NextIndex - get the next position for a subset of pieces
 *
 * Parameters:
 *
 *	off	piece type (BPBOARD, WPBOARD, BKBOARD, or WKBOARD)
 *
 * This routine sets the next position for a subset of pieces.  For example,
 * suppose three black checkers occupy database squares 0,1,2 (which correspond
 * to squares 1,2, and 3 in checkers literature).  Successive calls to this
 * routine sets the following checker configurations:
 * (0,1,2) (0,1,3) (0,2,3) (1,2,3) (0,1,4) etc.
 *
 * The number of times NextIndex is called is known in advance so no overflow
 * checking is necessary.
 */
	
void NextIndex(long off) {
	register WHERE_T *BPP, *BPE = &BPK[off][-1];

	for (BPP = BoardPos[off]; BPP < BPE && *BPP == BPP[1] - 1; BPP++)
		*BPP = (long)(BPP - BoardPos[off]);
	*BPP += 1;
}

/*
 * SaveIndex - saves the current position index for future reference
 *
 * Parameters:
 *
 *	off	piece type (BPBOARD, WPBOARD, BKBOARD, or WKBOARD)
 */
void SaveIndex(long off) {
 	register WHERE_T *BPP = BoardPos[off];
	register WHERE_T *pos = BPsave;
	register long k = 12;

	BPKsave = BPK[off];
	while (k--)
		*pos++ = *BPP++;
}

/*
 * LoadIndex - load the position index obtained from SaveIndex
 *
 * Parameters:
 *
 *	off	piece type (BPBOARD, WPBOARD, BKBOARD, or WKBOARD)
 */
void LoadIndex(long off) {
 	register WHERE_T *BPP = BoardPos[off];
	register WHERE_T *pos = BPsave;
	register long k = 12;

	BPK[off] = BPKsave;
	while (k--)
		*BPP++ = *pos++;
}

/*
 * DBrevindex - compute a reverse index for a set of pieces
 *
 * Parameters:
 *
 *	pos	array giving position numbers (in descending order)
 *	k	number of pieces
 *
 * Example:
 *
 *	Suppose black checkers are on squares 1, 3 and 4.  The enumeration
 *	order proceeds as follows: (0,1,2) (0,1,3) (0,2,3) (1,2,3) (0,1,4)
 *	(0,2,4) (1,2,4) (0,3,4) (1,3,4).  Thus the reverse index for the black
 *	checker configuration is 8.
 *
 *	DBrevindex computes this reverse index as follows:
 *
 *		pos	4 3 1
 *		k	3
 *
 *		| 4 | = 4   | 3 | = 3   | 1 | = 1    4 + 3 + 1 = 8.
 *		| 3 |       | 2 | 	| 1 |
 */
long DBrevindex(long *pos,long k) {
	register long offset = 0;

	while (k > 0)
		offset += Choose(*pos++, k--);
	return(offset);
}

/*
 * DBindex - compute an index for a set of pieces 
 *
 * Parameters:
 *
 *	pos	array giving position numbers
 *	k	number of pieces
 *
 * This function is similar to DBrevindex except it is used for computing
 * the index of the white checkers.
 */
long DBindex(long *pos,long k) {
	register long offset = 0;

	pos = &pos[k - 1];
	while (k > 0)
		offset += Choose(*pos--, k--);
	return(offset);
}
