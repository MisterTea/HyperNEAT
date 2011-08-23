//
//		Cake - a checkers engine			
//															
//		Copyright (C) 1999 - 2007 by Martin Fierz		
//															
//		contact: checkers@fierz.ch				
//

// switches.h: definitions which influence the way cake++ searches 

#ifdef _MSC_VER
#pragma warning( disable : 4996 )  // once instead of disable is maybe better
#endif

//#define _CRT_SECURE_NO_DEPRECATE



#ifdef _WIN64 
#define VERSION "1.8 fixeddepth (x64)"
#else
#define VERSION "1.8 fixeddepth (x32)"
#endif



#include "consts.h"

// compile options for cake
// add a bookgen function?
//#define BOOKGEN

// enable position learning?
#undef LEARNSAVE	// save learned positions
#undef LEARNUSE	// use learned positions

//#define HEURISTICDBSAVE // save positions that were evaluated with 9-12 pieces
#undef HEURISTICDBSAVE

#define COARSEGRAINING      		// use evaluation coarse graining 
//#define IMMEDIATERETURNONFORCED		// if defined, cake will not think about forced moves
#define BOOK						// use opening book
#define CHECKTIME					// check if time is > than breaktime and abort if yes
#define USEDB						// use the endgame database 


#define STOREEXACTDEPTH
#undef  EVALOFF						// return 0 in eval 
#undef EVALMATERIALONLY				// turn off all positional evaluation 
#define REPCHECK					// check for repetitions 
#define MOVEORDERING 				// turns on all move ordering 
									// if not set: overrules the three switches below ! 
#define MOHASH						// use a move from hashtable or killer move 
#define MOKILLER 					// use killer if no move from hashtable 
#define MOHISTORY					// use history table 
#define MOSTATIC					// use static move ordering
#define MOTESTCAPT					// order moves to back of list which lead to a capture for opponent
#define MOTESTCAPT2					// order moves to front of list which lead to threat of capture for sidetomove
#define GRAINSIZE 2					// 2 with this grain size 

#define QLEVEL 200	// 200			// qlevel gives the value an eval must be out of the window to be used
#define QLEVEL2 60	// 60			// if the side not to move has a capture
						


#define MAXDEPTH 99					// maximal number of plies cake++ can search 
#define LAZYEVALWINDOW 1000			// if materialeval is more than this outside of alpha-beta window, 
#define FINEEVALWINDOW 1000			// do only materialeval; LAZY for only material, FINE for material+selftrapeval

#define HISTORYOFFSET 10		
#define ASPIRATIONWINDOW 60			// aspiration window size for windowed search
#define CLDEPTH 5					// if depth < CLDEPTH // 5

//#define HASHSIZE 0x00800000			// 64 MB default hashtable size
#define HASHSIZE 0x00100000			// 8 MB hashtable size

#define HASHITER 2					// 2 probes in the hashtable

#define ALWAYSSTORE					// overwrite hashentries even if the new one has less depth? 
									// if yes, define this

									// book settings, also for book builder 
#define TMPBOOKSIZE (1048576L<<1)	// size of the temporary book 
#define HASHSIZEBOOK 400000			// size of the final book 
#define BUCKETSIZEBOOK 64			// bucket size of book hashtable 
#define BOOKMINDEPTH 0				// minimal depth which a book move must have for cake++ to use it  

#define MTD

#undef FULLLOG						// produce a very explicit logfile
#ifdef FULLLOG
#define MAXPV 40
#else
#define MAXPV 8						// max length of PV printout in ply
#endif

#define MARKPV						// mark pv in hashtable and don't prune



#define PLAYNOW

