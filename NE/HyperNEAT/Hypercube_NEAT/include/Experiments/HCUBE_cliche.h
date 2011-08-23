#ifndef CLICHE_H_INCLUDED
#define CLICHE_H_INCLUDED

#include "Experiments/HCUBE_chcommon.h"

/*
 * A B O U T
 *
 * Command Line Interface to a checkers library
 * conforming to Martin Fierz' CheckerBoard API
 * (c) 2003-2005 Peter Chiocchetti <pch(a)myzel.net>
 *
 * unix compile flags:
 * gcc -Wall -g -rdynamic -o cliche.o -c cliche.c
 * gcc -g -ldl -o cliche cliche.o
 */


/*
 * C O P Y I N G
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>


// DLL handling
/*
JGMATH: DLL support is not enabled for this hacked version
*/
#ifdef WIN32
#include <windows.h>
#define DLSYM GetProcAddress
#define DLCLOSE FreeLibrary
#define DL_CALL_FCT(fctp, args) (fctp) args
#else
#include <dlfcn.h>
#define DLSYM dlsym
#define DLCLOSE dlclose
#define HINSTANCE void *
#define CALLBACK  // empty on *nix
#endif

// search path for engines
#ifndef LIB_DIR
#define LIB_DIR "/usr/local/share/cliche/"
#endif

// board values
#define EMPTY    0  // un_occupied dark square
#define WHITE    1
#define BLACK    2
#define MAN      4
#define KING     8
#define FREE    16  // the light squares, always _free
#define CC       3  // change colour

// getmove return values
#define DRAW    0
#define WIN     1
#define LOSS    2
#define UNKNOWN 3

// interface values
#define MOVE  1
#define JUMP  2
#define BAD   3
#define SAVE -1
#define NEW  -2
#define PART -3
#define QUIT -4

// game types
#define INTERNL 20
#define ENGLISH 21
#define ITALIAN 22
#define RUSSIAN 25
// played by Martin Fierz dama; italian rules on a russian board
#define MAFIERZ 32

// macros
#define tocoor(x, y) (coor(x, y))

namespace HCUBE
{
    typedef int (CALLBACK * E_COMMAND) (char *, char *);
    typedef int (CALLBACK * E_ISLEGAL) (int[8][8], int, int, int, CBmove *);
    typedef int (CALLBACK * E_GETMOVE) (int[8][8], int, double, char *, int *, int,
                                        int, CBmove *);

    typedef struct
    {
        HINSTANCE handle;
        E_COMMAND command;
        E_ISLEGAL islegal;
        E_GETMOVE getmove;
    }
    CBengine;

    class CheckersExperiment;

    typedef struct
    {
        CheckersExperiment *experiment;
        char *player;   // player name
        char *opponent;  // engine name
        int view;   // player side
        int gametype;   // gametype code
        int nummoves;   // number of cbmoves
        CBmove cbmoves[500];  // game moves, 1...
        int result;   // result code
        int board[8][8];
    }
    gameinfo;

// play games with selected opponent
    int gamesloop (CBengine * engine, gameinfo * gi);

    extern int loadlib (gameinfo * gi, CBengine * engine);

// play selected opponent
    int opponentsloop (gameinfo * gi);

}

#endif // CLICHE_H_INCLUDED
