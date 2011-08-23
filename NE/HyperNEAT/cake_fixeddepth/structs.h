#ifndef __CAKE_STRUCTS_H__
#define __CAKE_STRUCTS_H__

/*		cake++ - a checkers engine
 *
 *		Copyright (C) 2001 by Martin Fierz
 *
 *		contact: checkers@fierz.ch
 */
/* structs.h: data structures for cake++ */

/*definitions for platform-independence*/
#define int32 unsigned int
#define int16 unsigned short
#define int8  unsigned char
#define sint32 signed int
#define sint16 signed short
#define sint8  signed char

#ifndef __SIMPLECH_H__
struct coor             /* coordinate structure for board coordinates */
	{
	int x;
	int y;
	};

struct CBmove				/* all the information you need about a move */
	{
	int ismove; 		 /* kind of superfluous: is 0 if the move is not a valid move */
	int newpiece;		/* what type of piece appears on to */
	int oldpiece;		/* what disappears on from */
	struct coor from,to; /* coordinates of the piece - in 8x8 notation!*/
	struct coor path[12]; /* intermediate path coordinates of the moving piece */
	struct coor del[12]; /* squares whose pieces are deleted after the move */
	int delpiece[12];	/* what is on these squares */
	};
#endif

typedef struct
	{
	int bm;
	int bk;
	int wm;
	int wk;
	} MATERIALCOUNT;

typedef struct
	{
	int32 key;
	int32 lock;
	} HASH;

typedef struct
	{
	int32 hash;
	int irreversible;
	} REPETITION;

typedef struct
	{
	int32 negamax;
	int32 iidnegamax;
	int32 qsearch;
	int32 qsearchfail;
	int32 qsearchsuccess;
	int32 leaf;
	int32 leafdepth;
	int32 cutoffs;
	int32 cutoffsatfirst;
	int32 dblookup;
	int32 dblookupsuccess;
	int32 dblookupfail;
	int32 hashlookup;
	int32 hashlookupsuccess;
	int32 hashstores;
	int32 spalookups;
	int32 spasuccess;
	int maxdepth;
	int realdepth;
	int bm;
	int bk;
	int wm;
	int wk;
	int Gbestindex;
	int searchmode;
	int allscores;
	int *play;				// is set to 1 by the interface if the search should be stopped
	char *out;				// is the address to write output to
	HASH hash;
	MATERIALCOUNT matcount;
	REPETITION *repcheck;
	double start;
	double maxtime;
	double aborttime;
	} SEARCHINFO;



typedef struct
	{
	int32 hashkey;
	int value:14;
	unsigned int color:2;
	} BOOKHT_ENTRY;

typedef struct
	{
	int32 hashkey;
	sint16 value;
	} SPA_ENTRY;

typedef struct
	{
	int32 black;
	int32 white;
	int32 kings;
	sint16 value;
	sint16 staticeval;
	} SPA_POSITION;

typedef struct 
	{
	int32 bm;
	int32 bk;
	int32 wm;
	int32 wk;
	int color;
	} POSITION;


typedef struct 
	{
	int32 bm;
	int32 bk;
	int32 wm;
	int32 wk;
	} OLDPOSITION;


typedef struct 
	{
	int32 bm;
	int32 wm;
	} MANPOSITION;

typedef struct
	{
	int32 black;
	int32 white;
	int32 kings;
	unsigned int best:6;
	unsigned int color:2;
	unsigned int depth:10;
	int value:14;
	} LEARNENTRY;

typedef struct 
	{
	int32 bm;
	int32 bk;
	int32 wm;
	int32 wk;
	} CAKE_MOVE;

typedef struct
	{
	int freebk;				// number of untrapped black kings
	int freewk;				// number of untrapped white kings
	int32 untrappedbk;		// bitmap of untrapped black kings
	int32 untrappedwk;		// bitmap of untrapped white kings
	} KINGINFO;


typedef struct
// struct hashentry needs 8 bytes 
	{
	int32  lock;
	unsigned int best:6;
	int value:12;
	unsigned int color:1;
	unsigned int ispvnode:1;
	unsigned int depth:10;
	unsigned int valuetype:2;
	} HASHENTRY;



struct bookhashentry
	// bookhashentry is the struct used for the book hashtable
	// we can store 3 moves per position.
	{
	int32  lock;
	unsigned int color:2;
	unsigned int best:6;
	int value:14;
	unsigned int depth:10;
	};

struct dbhashentry
	{
	unsigned int lock: 28;
	unsigned int color: 2;
	unsigned int result: 2;
	};

typedef struct 
	{
	struct hashentry *pointer;
	int32 size;
	int bucketsize;
	} HASHTABLE;


typedef struct
	{
	int material;
	int selftrap;
	int men;
	int backrank;
	int cramp;
	int hold;
	int runaway;
	int king;
	int king_man;
	int compensation;
	} EVALUATION;

#endif
