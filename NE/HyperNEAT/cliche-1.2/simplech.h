#ifndef __SIMPLECH_H__
#define __SIMPLECH_H__

/*----------> includes */
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef WIN32
#include <windows.h>
#else
#define WINAPI // empty on *nix
#endif

/*----------> platform stuff */
#ifdef WIN32
#define TICKS CLK_TCK
#else
#define TICKS CLOCKS_PER_SEC
#endif

/*----------> definitions */
#define OCCUPIED 0
#define WHITE 1
#define BLACK 2
#define MAN 4
#define KING 8
#define FREE 16
#define CHANGECOLOR 3
extern int CLICHE_MAXDEPTH;
#define MAXMOVES 28

/*----------> compile options  */
#undef MUTE
#undef VERBOSE
#define STATISTICS

/* return values */
#define DRAW 0
#define WIN 1
#define LOSS 2
#define UNKNOWN 3

// board values
#define EMPTY    0		// un_occupied dark square
#define WHITE    1
#define BLACK    2
#define MAN      4
#define KING     8
#define FREE    16		// the light squares, always _free
#define CC       3		// change colour

// getmove return values
#define DRAW    0
#define WIN     1
#define LOSS    2
#define UNKNOWN 3

// interface values
#define CLICHE_MOVE  1
#define JUMP  2
#define BAD   3
#define SAVE -1
#define NEW  -2
#define PART -3
#define QUIT -4

/*----------> structure definitions  */
struct move2
   {
   short n;
   int m[12];
   };

struct coor              /* coordinate structure for board coordinates */
   {
   int x;
   int y;
   };

static struct CBmove     /* GLOBAL all the information you need about a move */
   {
   int ismove;           /* kind of superfluous: is 0 if the move is not a valid move */
   int newpiece;         /* what type of piece appears on to */
   int oldpiece;         /* what disappears on from */
   struct coor from,to;  /* coordinates of the piece - in 8x8 notation!*/
   struct coor path[12]; /* intermediate path coordinates of the moving piece */
   struct coor del[12];  /* squares whose pieces are deleted after the move */
   int delpiece[12];     /* what is on these squares */
   } GCBmove;

/*----------> function prototypes  */
/*----------> part I: interface to CheckerBoard: CheckerBoard requires that
                      at getmove and enginename are present in the dll. the
                      functions help, options and about are optional. if you
                      do not provide them, CheckerBoard will display a
                      MessageBox stating that this option is in fact not an option*/

/* required functions */
int getmove(int b[8][8],int color, double maxtime, char str[255], int *playnow, int info, int unused, struct CBmove *move);
int islegal(int b[8][8], int color, int from, int to, struct CBmove *move);
int enginecommand(char str[256], char reply[256]);

#endif

