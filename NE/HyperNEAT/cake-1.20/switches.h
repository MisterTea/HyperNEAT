/* switches.h: definitions which influence the way cake++ searches */

#undef OPLIB
/* compile options for cake++*/


/* undef the two #defines below for OPLIB */
#ifndef OPLIB
	#define COARSEGRAINING      		/* use evaluation coarse graining */
	#define IMMEDIATERETURNONFORCED  /* if defined, cake++ will not think about forced moves*/
#endif

//#define USEDB					/* use the endgame database */

#undef EVALOFF              /* return 0 in eval */
#undef EVALMATERIALONLY     /* turn off all positional evaluation */
#define REPCHECK            /* check for repetitions */
#define MOVEORDERING 		 /* turns on all move ordering */
									 /* if not set: overrules the three switches below ! */
#define MOSTATIC  				/* use static move ordering */
#define MOTESTCAPT         /* a very expensive part of static move ordering */
#define MOHASH          /* use a move from hashtable or killer move */
#define MOKILLER 				/* use killer if no move from hashtable */
#define MOHISTORY          /* use history table */
#define DOORDERING			/* do the bubble sort: just to test how slow this really is */

#define GRAINSIZE 2         /* with this grain size */
#define EXTENDALL				 /* if extendall is defined cake++ will extend if a capture
                               for the side not to move is possible */
                            /* extension for capture for the side to move is fix! */
#define QLEVEL 130
#define DOEXTENSIONS        /* use the truncation */
#define TRUNCATIONDEPTH 15  /* how much do i truncate... */
                            /* this will be AUTOMATICALLY overruled in endgames */
                            /* where truncationdepth is set to 0! */
#define TRUNCATEVALUE 110   /* 110...if value is more than this out of window */
#undef  TABLE8              /* use bitcount with bitsinbyte[256] */
#define TABLE16             /* use bitcount with bitsinword[65536] */
#define ETC						/* use enhanced transposition cutoffs */
#define ETCDEPTH 20			/* if depth>etcdepth do ETC */
/* some stuff for search */
#define CAKE_MAXDEPTH 99
#define FINEEVALWINDOW 150
#define HISTORYOFFSET 10
#define ASPIRATIONWINDOW 10

#define SINGLEEXTEND 5 /* 5 looks good here */

/* hashtable settings */
/*0x00010000=65535*/   /*1.5MB for the deep hashtable*/
/*=0x00100000 = 12MB*/   /*6MB for the shallow hashtable*/
#define HASHSIZEDEEP    0x00020000
#define HASHSIZESHALLOW 0x00080000 //0x00080000
#define HASHMASKDEEP    0x0001FFFF
#define HASHMASKSHALLOW 0x0007FFFF
#define DEEPLEVEL 10
/* positions with realdepth < deeplevel
													  are stored in deep */
#define HASHITER 2

#undef ANALYSISMODULE /* compiles a version of cake++ which analyses all moves */
