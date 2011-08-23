/* Dummy Engine
 * skin and bones, just whats needed to compile
 * compile flags:
 * gcc -g -Wall -fPIC -o dummy.o -c dummy.c
 * gcc -g -Wall -shared -Wl,-soname,dummy.so -o dummy.so dummy.o
 */


#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#endif

#ifndef WIN32
#define WINAPI			// empty on *nix
#endif

/* board values */
#define OCCUPIED 0		// un_occupied dark squares
#define WHITE 1
#define BLACK 2
#define MAN 4
#define KING 8
#define FREE 16			// the light squares, always _free
#define CHANGECOLOR 3

/* getmove return values */
#define DRAW 0
#define WIN 1
#define LOSS 2
#define UNKNOWN 3


/* types */

typedef struct			// coordinate structure for board coordinates
{
  int x;
  int y;
} coor;

typedef struct			// all the information there is about a move
{
  int jumps;			// how many jumps are there in this move?
  int newpiece;			// what type of piece appears on to
  int oldpiece;			// what disappears on from
  coor from, to;		// coordinates of the piece in 8x8 notation!
  coor path[12];		// intermediate path coordinates
  coor del[12];			// squares whose pieces are deleted
  int delpiece[12];		// what is on these squares
} CBmove;


/* function prototypes */

int WINAPI getmove (int b[8][8], int color, double maxtime, char str[255],
		    int *playnow, int info, int unused, CBmove * move);
int WINAPI enginecommand (char command[256], char reply[256]);
int WINAPI islegal (int b[8][8], int color, int from, int to, CBmove * move);


/* dll/dso stuff */

#ifdef WIN32
BOOL WINAPI
DllEntryPoint (HANDLE hDLL, DWORD dwReason, LPVOID lpReserved)
{
  /* in a dll you used to have LibMain instead of WinMain in
     windows programs, or main in normal C programs win32
     replaces LibMain with DllEntryPoint. */

  switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
      /* dll loaded. put initializations here! */
      break;
    case DLL_PROCESS_DETACH:
      /* program is unloading dll. put clean up here! */
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    default:
      break;
    }
  return TRUE;
}
#endif // WIN32


/* CheckerBoard API: enginecommand(), islegal(), getmove() */

int WINAPI
enginecommand (char str[256], char reply[256])
{
  /* answer to commands sent by CheckerBoard.  This does not
   * answer to some of the commands, eg it has no engine
   * options. */

  char command[256], param1[256], param2[256];

  sscanf (str, "%s %s %s", command, param1, param2);

  // check for command keywords 

  // by default, return "i don't understand this"
  sprintf (reply, "?");

  if (strcmp (command, "name") == 0)
    {
      sprintf (reply, "dummy engine 1.0");
      return 1;
    }

  if (strcmp (command, "about") == 0)
    {
      sprintf (reply, "dummy Engine\n\n2003 Public Domain");
      return 1;
    }

  if (strcmp (command, "help") == 0)
    {
      sprintf (reply, "missing.htm");
      return 1;
    }

  if (strcmp (command, "set") == 0)
    {
      if (strcmp (param1, "hashsize") == 0)
	{
	  return 0;
	}
      if (strcmp (param1, "book") == 0)
	{
	  return 0;
	}
    }

  if (strcmp (command, "get") == 0)
    {
      if (strcmp (param1, "hashsize") == 0)
	{
	  return 0;
	}
      if (strcmp (param1, "book") == 0)
	{
	  return 0;
	}
      if (strcmp (param1, "protocolversion") == 0)
	{
	  sprintf (reply, "2");
	  return 1;
	}
      if (strcmp (param1, "gametype") == 0)
	{
	  sprintf (reply, "21");
	  return 1;
	}
    }
  return 0;
}


int WINAPI
islegal (int b[8][8], int color, int from, int to, CBmove * move)
{
  /* islegal tells CheckerBoard if a move the user wants to
   * make is legal or not. to check this, we generate a
   * movelist and compare the moves in the movelist to the
   * move the user wants to make with from&to */

  return 0;
}


int WINAPI
getmove (int b[8][8], int color, double maxtime, char str[255], int *playnow,
	 int info, int unused, CBmove * move)
{
  /* getmove is what checkerboard calls. you get the parameters:

     - b[8][8]
     is the current position. the values in the array are
     determined by the #defined values of BLACK, WHITE, KING,
     MAN. a black king for instance is represented by BLACK|KING.

     - color
     is the side to make a move. BLACK or WHITE.

     - maxtime
     is the time your program should use to make a move. this
     is what you specify as level in checkerboard. so if you
     exceed this time it's not too bad - just don't exceed it
     too much...

     - str
     is a pointer to the output string of the checkerboard status bar.
     you can use sprintf(str,"information"); to print any information you
     want into the status bar.

     - *playnow
     is a pointer to the playnow variable of checkerboard. if
     the user would like your engine to play immediately, this
     value is nonzero, else zero. you should respond to a
     nonzero value of *playnow by interrupting your search
     IMMEDIATELY.

     - CBmove
     tells checkerboard what your move is, see above.
   */

  strcpy(str, "This is just a dummy");
  return 9;
}
