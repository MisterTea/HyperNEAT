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

#include "Experiments/HCUBE_CheckersExperiment.h"

#include "Experiments/HCUBE_cliche.h"

/*
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>


// DLL handling
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
#define tocoor(x, y) (coor) {x, y}


// types
typedef struct
{
  int x;   // file
  int y;   // row
}
coor;

typedef struct
{
  int jumps;   // how many jumps are there in this move?
  int newpiece;   // what type of piece appears on to
  int oldpiece;   // what disappears on from
  coor from, to;  // coordinates of the squares in 8x8 notation!
  coor path[12];  // intermediate path coordinates
  coor del[12];   // squares whose pieces are deleted
  int delpiece[12];  // what is on these squares
}
CBmove;

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

typedef struct
{
  char *player;   // player name
  char *opponent;  // engine name
  int view;   // player side
  int gametype;   // gametype code
  int nummoves;   // number of cbmoves
  CBmove cbmoves[500];  // game moves, 1...
  int result;   // result code
}
gameinfo;
*/

/*
 * P R O C E D U R E
 *
 * -> main :
 *    read opponent
 *    opponentsloop
 *    -> gamesloop :
 *       read colour
 *       read strength
 *       first move if engine
 *       -> movesloop :
 *          read move
 *          check move
 *          engine move
 *       : new/part/quit/save/command
 *    : part/quit
 * : opponents name/quit
 *
 */

#define CLICHE_DEBUG (0)

namespace HCUBE
{
    extern int getmove(int b[8][8],int color, double maxtime, char str[255], int *playnow, int info, int unused, CBmove *move);
    extern int islegal(int b[8][8], int color, int from, int to,  CBmove *move);
    extern int enginecommand(char str[256], char reply[256]);

// read the colour, the player wants, ie. her viewpoint
    int
    readview ()
    {
        return BLACK;
    }


// read the time, the engine may take, to compute its moves
    double readmaxtime (char *opponent)
    {
        return 1000.0;
    }


// read player's moves/commands
    /* JGMATH: Let checkersExperiment handle this
        int readmove (int *from, int *to, char *command)
        {
            char buf[64];

            printf ("\nEnter your move ? ");
            fgets (buf, sizeof (buf), stdin);

            if (sscanf (buf, "%d-%d", from, to) == 2)
                return MOVE;
            else if (sscanf (buf, "%dx%d", from, to) == 2)
                return JUMP;
            else if (strstr (buf, "save"))
                return SAVE;
            else if (strstr (buf, "new"))
                return NEW;
            else if (strstr (buf, "part"))
                return PART;
            else if (strstr (buf, "quit") || strstr (buf, "exit"))
                return QUIT;
            strcpy (command, buf);
            return BAD;
        }
        */


// read filename, suggest last entry
    void
    readfn (char *name)
    {
        static char buf[32] = "cliche";

        strcpy (name, buf);
        printf ("\nEnter filename [%s] ? ", buf);
        fgets (buf, sizeof (buf), stdin);
        sscanf (buf, "%s", name);
        strcpy (buf, name);
    }


    /*
     * checker board setup
     * shown from the side of the player to make the first move
     * coordinates as in CB-api board paramenter
     *

       int board[x][y]; struct coor (x, y); // x = col, y = row

       ENGLISH:

       row                           row

       7     w   w   w   w           7      32  31  30  29
       6   w   w   w   w             6    28  27  26  25
       5     w   w   w   w           5      24  23  22  21
       4   -   -   -   -             4    20  19  18  17
       3     -   -   -   -           3      16  15  14  13
       2   b   b   b   b             2    12  11  10   9
       1     b   b   b   b           1       8   7   6   5
       0   b   b   b   b             0     4   3   2   1

           0 1 2 3 4 5 6 7  col            0 1 2 3 4 5 6 7  col


       MAFIERZ: (same as ENGLISH but notation, italian rules)

       row                           row

       0  b   b   b   b    0  4   3   2   1
       1   b   b   b   b   1     8   7   6   5
       2  b   b   b   b    2 12  11  10   9
       3   -   -   -   -   3    16  15  14  13
       4  -   -   -   -    4 20  19  18  17
       5   w   w   w   w   5    24  23  22  21
       6  w   w   w   w    6 28  27  26  25
       7   w   w   w   w   7    32  31  30  29

           7 6 5 4 3 2 1 0  col            7 6 5 4 3 2 1 0  col

       ITALIAN:

       row                           row

       7   b   b   b   b             7     1   2   3   4
       6     b   b   b   b           6       5   6   7   8
       5   b   b   b   b             5     9  10  11  12
       4     -   -   -   -           4      13  14  15  16
       3   -   -   -   -             3    17  18  19  20
       2     w   w   w   w           2      21  22  23  24
       1   w   w   w   w             1    25  26  27  28
       0     w   w   w   w           0      29  30  31  32

           0 1 2 3 4 5 6 7  col            0 1 2 3 4 5 6 7  col


       RUSSIAN:                      INTERNATIONAL:

       row                           row


                                     9     b   b   b   b   b
                                     8   b   b   b   b   b
       7     b   b   b   b           7     b   b   b   b   b
       6   b   b   b   b             6   b   b   b   b   b
       5     b   b   b   b           5     -   -   -   -   -
       4   -   -   -   -             4   -   -   -   -   -
       3     -   -   -   -           3     w   w   w   w   w
       2   w   w   w   w             2   w   w   w   w   w
       1     w   w   w   w           1     w   w   w   w   w
       0   w   w   w   w             0   w   w   w   w   w

           0 1 2 3 4 5 6 7  col          0 1 2 3 4 5 6 7 8 9  col

     */


// return board number for coordinates
    int
    coortonumber (gameinfo * gi, coor c)
    {
        // board coordinates are [y][x]!
        // ENGLISH
        const int en[8][8] =
        {
            {
                4, 0, 3, 0, 2, 0, 1, 0
            },
            {0, 8, 0, 7, 0, 6, 0, 5},
            {12, 0, 11, 0, 10, 0, 9, 0},
            {0, 16, 0, 15, 0, 14, 0, 13},
            {20, 0, 19, 0, 18, 0, 17, 0},
            {0, 24, 0, 23, 0, 22, 0, 21},
            {28, 0, 27, 0, 26, 0, 25, 0},
            {0, 32, 0, 31, 0, 30, 0, 29},
        };

        // MAFIERZ
        const int mf[8][8] =
        {
            {
                1, 0, 2, 0, 3, 0, 4, 0
            },
            {0, 5, 0, 6, 0, 7, 0, 8},
            {9, 0, 10, 0, 11, 0, 12, 0},
            {0, 13, 0, 14, 0, 15, 0, 16},
            {17, 0, 18, 0, 19, 0, 20, 0},
            {0, 21, 0, 22, 0, 23, 0, 24},
            {25, 0, 26, 0, 27, 0, 28, 0},
            {0, 29, 0, 30, 0, 31, 0, 32},
        };

        // ITALIAN
        const int it[8][8] =
        {
            {
                0, 29, 0, 30, 0, 31, 0, 32
            },
            {25, 0, 26, 0, 27, 0, 28, 0},
            {0, 21, 0, 22, 0, 23, 0, 24},
            {17, 0, 18, 0, 19, 0, 20, 0},
            {0, 13, 0, 14, 0, 15, 0, 16},
            {9, 0, 10, 0, 11, 0, 12, 0},
            {0, 5, 0, 6, 0, 7, 0, 8},
            {1, 0, 2, 0, 3, 0, 4, 0},
        };

        // RUSSIAN
        const int ru[8][8] =
        {
            {
                32, 0, 31, 0, 30, 0, 29, 0
            },
            {0, 28, 0, 27, 0, 26, 0, 25},
            {24, 0, 23, 0, 22, 0, 21, 0},
            {0, 20, 0, 19, 0, 18, 0, 17},
            {16, 0, 15, 0, 14, 0, 13, 0},
            {0, 12, 0, 11, 0, 10, 0, 9},
            {8, 0, 7, 0, 6, 0, 5, 0},
            {0, 4, 0, 3, 0, 2, 0, 1},
        };

        switch (gi->gametype)
        {
        case MAFIERZ:
            return mf[c.y][c.x];
        case ITALIAN:
            return it[c.y][c.x];
        case RUSSIAN:
            return ru[c.y][c.x];
        case ENGLISH:
            return en[c.y][c.x];
        default:
            printf ("Cannot get number from coordinate, unknown gametype %d.",
                    gi->gametype);
            return -1;
        }
    }

// initialize an 8x8 board to the starting position
    static void
    newboard_8x8 (gameinfo * gi, int b[8][8])
    {
        int x, y, r;   // row, column, rotation
        r = gi->gametype == ITALIAN ? 1 : 0;

        for (y = 0; y < 8; y++)
            for (x = 0; x < 8; x++)
                if ((x + y) % 2 == r)
                    b[x][y] = EMPTY;
                else
                    b[x][y] = FREE;

        switch (gi->gametype)
        {
        case ITALIAN:
            b[1][0] = b[3][0] = b[5][0] = b[7][0] =
                                              b[0][1] = b[2][1] = b[4][1] = b[6][1] =
                                                                                b[1][2] = b[3][2] = b[5][2] = b[7][2] = WHITE | MAN;
            b[0][7] = b[2][7] = b[4][7] = b[6][7] =
                                              b[1][6] = b[3][6] = b[5][6] = b[7][6] =
                                                                                b[0][5] = b[2][5] = b[4][5] = b[6][5] = BLACK | MAN;
            break;
        case RUSSIAN:
            b[0][0] = b[2][0] = b[4][0] = b[6][0] =
                                              b[1][1] = b[3][1] = b[5][1] = b[7][1] =
                                                                                b[0][2] = b[2][2] = b[4][2] = b[6][2] = WHITE | MAN;
            b[1][7] = b[3][7] = b[5][7] = b[7][7] =
                                              b[0][6] = b[2][6] = b[4][6] = b[6][6] =
                                                                                b[1][5] = b[3][5] = b[5][5] = b[7][5] = BLACK | MAN;
            break;
        case MAFIERZ:
        case ENGLISH:
            b[0][0] = b[2][0] = b[4][0] = b[6][0] =
                                              b[1][1] = b[3][1] = b[5][1] = b[7][1] =
                                                                                b[0][2] = b[2][2] = b[4][2] = b[6][2] = BLACK | MAN;
            b[1][7] = b[3][7] = b[5][7] = b[7][7] =
                                              b[0][6] = b[2][6] = b[4][6] = b[6][6] =
                                                                                b[1][5] = b[3][5] = b[5][5] = b[7][5] = WHITE | MAN;
            break;
        default:
            printf ("Cannot initialize board, unknown gametype %d.", gi->gametype);
            return;
        }
    }


// print an 8x8 board
    void printboard_8x8 (gameinfo * gi, int color, int b[8][8])
    {
        int x, y, r;   // row, column, rotation, number
        // figures: none=0, w=5, b=6, W=9, B=10
        // field val:       01234567890123456
        const char *f = "-    wb  WB      ";

        // rotate board, so the player sees it from her point of view
        r = 0;
        if (gi->gametype == ENGLISH && gi->view == BLACK)
            r = 1;
        if (gi->gametype == MAFIERZ && gi->view == BLACK)
            r = 1;
        if (gi->gametype == ITALIAN && gi->view == WHITE)
            r = 1;
        if (gi->gametype == RUSSIAN && gi->view == WHITE)
            r = 1;

        printf ("\n");
        {
            for (y = 0; y < 8; y++)
            {
                // board setup
                for (x = 0; x < 8; x++)
                    if (r)
                        printf (" %c", f[b[x][8 - y - 1]]);
                    else
                        printf (" %c", f[b[8 - x - 1][y]]);
                printf ("\t");

                // board numbers
                for (x = 0; x < 8; x++)
                {
                    if (r)
                    {
                        printf ("%2.0d", coortonumber (gi, tocoor(x, 8 - y - 1)));
                    }
                    else
                    {
                        printf ("%2.0d", coortonumber (gi, tocoor(8 - x - 1, y)));
                    }
                }
                printf ("\n");
            }
        }
    }


    /*
     * ICDS style<5> 8x8 board string, 114 chars, 10x10 ->xxx chars
     *

      0         1         2         3         4         5
      012345678901234567890123456789012345678901234567890123456789
      <5>001eWWBlack Player NameWhite Player Nameb b b b  b b b bb
      6         7         8         9        10        11
      0123456789012345678901234567890123456789012345678901234
       b b b  - - - -- - - w  w w - ww w w w  w w w w00000000

     */


// print an 8x8 icds style 5 board
    static void
    printboard_icds (gameinfo * gi, int color, int b[8][8])
    {
        int x, y;   // row, column
        // figures: w=5, b=6, W=9, B=10
        // field val:       01234567890123456
        const char *f = "-    wb  WB      ";

        printf ("\n");
        printf ("<5>");  // board display type
        printf ("001");  // game number
        switch (gi->gametype)
        {
        case ITALIAN:
            printf ("i");
            break;
        case MAFIERZ:
            printf ("m");
            break;
        case RUSSIAN:
            printf ("r");
            break;
        case ENGLISH:
            printf ("e");
            break;
        default:
            printf ("Cannot print board, unknown gametype %d.", gi->gametype);
            return;
        }
        gi->view == BLACK ? printf ("B") : printf ("W");
        color == BLACK ? printf ("B") : printf ("W");
        gi->view == BLACK
        ? printf ("%17s", gi->player) : printf ("%17s", gi->opponent);
        gi->view == WHITE
        ? printf ("%17s", gi->player) : printf ("%17s", gi->opponent);

        for (y = 0; y < 8; y++)
            for (x = 0; x < 8; x++)
                printf ("%c", f[b[x][y]]);

        printf ("1000");  // black time
        printf ("2000");  // white time
        printf ("\n");
    }


// animate user move
    static void
    docbmove (gameinfo * gi, int b[8][8], CBmove cbmove)
    {
        int i;

        // just from/to, no path now
#if CLICHE_DEBUG
        printf("Moving from %d,%d to %d,%d\n",
               cbmove.from.x,
               cbmove.from.y,
               cbmove.to.x,
               cbmove.to.y
              );
#endif

        b[cbmove.from.x][cbmove.from.y] = EMPTY;
        b[cbmove.to.x][cbmove.to.y] = cbmove.newpiece;

        // take thrown men
        for (i = 0; i < cbmove.jumps; i++)
        {
            //printf("JUMP!\n");
            b[cbmove.del[i].x][cbmove.del[i].y] = EMPTY;
        }

        // save move
        gi->nummoves++;
        memcpy (&gi->cbmoves[gi->nummoves], &cbmove, sizeof (cbmove));

        // if its a jump, say so
        //printf("jump=%d\n", cbmove.jumps);
    }


// return position as a FEN string
    static char *
    fenpos (gameinfo * gi, int b[8][8], int color)
    {
        char *fen = (char *) malloc (sizeof (char) * 128);
        int x, y;

        // setup
        sprintf (fen, "[SetUp \"%d\"]\n[FEN \"", gi->nummoves);

        // color to move
        if (color == BLACK)
            strcat (fen, "B");
        else
            strcat (fen, "W");

        // white stones
        strcat (fen, ":W");
        for (y = 0; y < 8; y++)
            for (x = 0; x < 8; x++)
                if (b[x][y] & (WHITE))
                {
                    if (b[x][y] & (KING))
                        strcat (fen, "K");
                    sprintf (fen, "%s%d,", fen, coortonumber (gi, tocoor (x, y)));
                }
        fen[strlen (fen) - 1] = '\0';

        // black stones
        strcat (fen, ":B");
        for (y = 0; y < 8; y++)
            for (x = 0; x < 8; x++)
                if (b[x][y] & (BLACK))
                {
                    if (b[x][y] & (KING))
                        strcat (fen, "K");
                    sprintf (fen, "%s%d,", fen, coortonumber (gi, tocoor (x, y)));
                }
        fen[strlen (fen) - 1] = '\0';

        // close setup
        strcat (fen, ".\"]");
        return fen;
    }


// save game as a pdn file
    static void
    savegame (gameinfo * gi)
    {
        FILE *fp;
        char fn[64];
        char date[32];
        time_t nowp;
        struct tm *nows;
        CBmove cbmove;
        int i, n;

        // date
        nowp = time (NULL);
        nows = localtime (&nowp);
        strftime (date, sizeof (date), "%Y.%m.%d", nows);

        // check gametype
        if (coortonumber (gi, tocoor(0, 0)) < 0)
        {
            perror ("Unknown game type, cannot save.\n");
            return;
        }

        // read filename
        readfn (fn);
        strcat (fn, ".pdn");

        // create file
        fp = fopen (fn, "w");
        if (!fp)
        {
            perror ("Game not saved\n");
            perror (fn);
            return;
        }
        // header
        fprintf (fp, "[Event \"Cliche SaveGame\"]\n");
        fprintf (fp, "[Date \"%s\"]\n", date);
        fprintf (fp, "[Black \"%s\"]\n",
                 gi->view == BLACK ? gi->player : gi->opponent);
        fprintf (fp, "[White \"%s\"]\n",
                 gi->view == WHITE ? gi->player : gi->opponent);
        fprintf (fp, "[GameType \"%d\"]\n", gi->gametype);

        // moves
        for (n = 1; n <= gi->nummoves; n++)
        {
            cbmove = gi->cbmoves[n];
            if (n % 2)
                fprintf (fp, "%d. ", n / 2 + 1);
            if (cbmove.jumps)
            {
                fprintf (fp, "%d", coortonumber (gi, cbmove.from));
                for (i = 1; i <= cbmove.jumps; i++)
                    fprintf (fp, "x%d", coortonumber (gi, cbmove.path[i]));
                fprintf (fp, " ");
            }
            else
                fprintf (fp, "%d-%d ",
                         coortonumber (gi, cbmove.from),
                         coortonumber (gi, cbmove.to));
        }
        // result
        switch (gi->result)
        {
        case DRAW:
        case BLACK:
        case WHITE:
        case UNKNOWN:
            fprintf (fp, "*\n");
            break;
        }
        fclose (fp);
        printf ("Wrote game to file %s\n", fn);
    }

    extern int getWinner(uchar b[8][8],int colorToMove);

// play games with selected opponent
    int gamesloop (CBengine * engine, gameinfo * gi)
    {
        char command[64] = "";
        char reply[1024] = "";
        int board[8][8];  // XXX how about other sizes ?
        double maxtime;   // engines time for a move
        int move, from, to;
        int value, color;
        int playnow = 0, info = 0, moreinfo = 0;
        //char str[1024];
        CBmove cbmove;

        // user colour
        gi->view = readview ();

        // engine maxtime
        maxtime = readmaxtime (gi->opponent);

        // initialize/reset board
        newboard_8x8 (gi, board);
        gi->nummoves = 0;
        gi->result = UNKNOWN;

        // engine moves first ?
        /* JGMATH: No, player always moves first
        if ((gi->view == WHITE && gi->gametype == ENGLISH)
                || (gi->view == BLACK
                    && (gi->gametype == ITALIAN || gi->gametype == MAFIERZ
                        || gi->gametype == RUSSIAN)))
        {
            color = gi->view ^ CC;
            printboard_icds (gi, color, board);
            printboard_8x8 (gi, color, board);
            value =
                DL_CALL_FCT (engine->getmove,
                             (board, color, (double) maxtime, str, &playnow, info,
                              moreinfo, &cbmove));
            docbmove (gi, board, cbmove);
            printf ("\n%s\n", str);
        }
        */

        color = gi->view;

#if CLICHE_DEBUG
        printboard_icds (gi, color, board);
        printboard_8x8 (gi, color, board);
#endif

        // start movesloop
        do
        {
            // player moves
            {
                unsigned char charBoard[8][8];
                for (int h=0;h<8;h++)
                {
                    for (int q=0;q<8;q++)
                    {
                        charBoard[h][q] = board[h][q];
                    }
                }
                move = gi->experiment->readmove (charBoard,&from, &to, command);
                for (int h=0;h<8;h++)
                {
                    for (int q=0;q<8;q++)
                    {
                        board[h][q] = charBoard[h][q];
                    }
                }
            }

            switch (move)
            {
            case JUMP:
            case MOVE:
                // is it valid ?
                value = islegal(board, color, from, to, &cbmove);
                if (!value)
                {
                    printf ("%d -> %d\n",from,to);
                    printf ("\nInvalid move, try again.\n");
                    printboard_icds (gi, color, board);
                    printboard_8x8 (gi, color, board);
                    continue;
                }
#if CLICHE_DEBUG
                printf("Moving from %d to %d\n",from,to);
#endif
                docbmove (gi, board, cbmove);
                color = color ^ CC;
#if CLICHE_DEBUG
                printboard_icds (gi, color, board);
                printboard_8x8 (gi, color, board);
#endif
                break;
            case BAD:
                value = enginecommand(command, reply);
                if (value)
                    printf ("%s\n", reply);
                else
                    printf ("\nMalformed move, try again.\n");
                continue;
            case SAVE:
                printf ("%s\n", fenpos (gi, board, color));
                savegame (gi);
                continue;
            case NEW:
                strcpy (command, "new");
                continue;
            case PART:
                //Not implemented in an AI
                continue;
            case QUIT:
                //Not implemented in an AI
                continue;
            default:
                printf ("\nOops, this shouldn't happen.\n");
                continue;
            }
            // engine moves
            /*
            value = getmove(board, color, maxtime,
                            str, &playnow, info, moreinfo,
                            &cbmove);
            */
            docbmove (gi, board, cbmove);
#if CLICHE_DEBUG
            printf ("\n%s\n", str);
#endif
            color = color ^ CC;
#if CLICHE_DEBUG
            printboard_icds (gi, color, board);
            printboard_8x8 (gi, color, board);
#endif

            memcpy(gi->board,board,sizeof(int)*8*8);

            int winner;
            /* Commented out because of unresolved externals
            {
                unsigned char charBoard[8][8];
                for (int h=0;h<8;h++)
                {
                    for (int q=0;q<8;q++)
                    {
                        charBoard[h][q] = board[h][q];
                    }
                }
                winner = getWinner(charBoard,BLACK);
                for (int h=0;h<8;h++)
                {
                    for (int q=0;q<8;q++)
                    {
                        board[h][q] = charBoard[h][q];
                    }
                }
            }
            */

            if (gi->nummoves >= 200)
            {
                //Black (the AI) loses if it goes too long.
                gi->nummoves = 200;
                winner = WHITE;
            }

            if (winner==BLACK)
            {
                value = LOSS;
            }
            else if (winner==WHITE)
            {
                value = WIN;
            }

            if (value == DRAW)
            {
#if CLICHE_DEBUG
                printf ("Game ends with a draw.\n");
                printboard_icds (gi, color, board);
                printboard_8x8 (gi, color, board);
#endif
                return DRAW;
            }
            if (value == WIN)
            {
#if CLICHE_DEBUG
                printf ("Game ends, you lose.\n");
                printboard_icds (gi, color, board);
                printboard_8x8 (gi, color, board);
#endif
                return LOSS;
            }
            if (value == LOSS)
            {
#if CLICHE_DEBUG
                printf ("Game ends, you win.\n");
                printboard_icds (gi, color, board);
                printboard_8x8 (gi, color, board);
#endif
                return WIN;
            }
        }
        while (strcmp (command, "new"));

        return -1;
    }

    int
    loadlib (gameinfo * gi, CBengine * engine)
    {
        return 1;
        /*
        char enginepath[512];

        #ifdef WIN32

        strcpy (enginepath, gi->opponent);
        strcat (enginepath, ".dll");

        engine->handle = LoadLibrary (enginepath);
        if (!engine->handle)
        {
            printf ("\nNo such opponent.\n");
            fprintf (stderr, "DLL not found: %s.\n", enginepath);
            strcpy (gi->opponent, "part");
            return 0;
        }

        #else // not WIN32

        const char *error;

        // try system location
        strcpy (enginepath, LIB_DIR);
        strcat (enginepath, gi->opponent);
        strcat (enginepath, ".so");
        engine->handle = dlopen (enginepath, RTLD_LAZY);

        // try cwd
        if (!engine->handle)
        {
            strcpy (enginepath, "./");
            strcat (enginepath, gi->opponent);
            strcat (enginepath, ".so");
            engine->handle = dlopen (enginepath, RTLD_LAZY);
        }

        if (!engine->handle)
        {
            printf ("\nNo such opponent.\n");
            fprintf (stderr, "%s", dlerror ());
            printf ("\nAdditional search path: %s\n", LIB_DIR);
            strcpy (gi->opponent, "part");
            return 0;
        }

        #endif // WIN32

        // mandatory library functions
        engine->command = (E_COMMAND) DLSYM (engine->handle, "enginecommand");
        engine->islegal = (E_ISLEGAL) DLSYM (engine->handle, "islegal");
        engine->getmove = (E_GETMOVE) DLSYM (engine->handle, "getmove");

        #ifdef WIN32
        if (engine->command == NULL
                || engine->islegal == NULL || engine->getmove == NULL)
        {
            fprintf (stderr, "Invalid engine.\n");
            strcpy (gi->opponent, "part");
            DLCLOSE (engine->handle);
            return 0;
        }
        #else // not WIN32
        if (engine->command == NULL
                || engine->islegal == NULL || engine->getmove == NULL)
        {
            fprintf (stderr, "Invalid engine: ");
            if ((error = dlerror ()) != NULL)
                fprintf (stderr, "%s\n", error);
            strcpy (gi->opponent, "part");
            DLCLOSE (engine->handle);
            return 0;
        }
        #endif

        return 1;
        */
    }

// play selected opponent
    int opponentsloop (gameinfo *gi)
    {
        char command[64];
        char reply[1024];
        CBengine engine;

        if (!loadlib (gi, &engine))
            return -1;

        // use DL_CALL_FCT macro
        // to enable profiling of shared object with sprof

        // engine info
        //strcpy (command, "about");
        //enginecommand(command, reply);
        //printf ("\n%s\n\n", reply);

        // gametype
        strcpy (command, "get gametype");
        enginecommand(command, reply);
        gi->gametype = strtol (reply, (char **) NULL, 10);

        // start gamesloop
        switch (gi->gametype)
        {
        case ENGLISH:
            //printf ("Playing english rules.\n");
            return gamesloop (&engine, gi);
            break;
        case ITALIAN:
            //printf ("Playing italian rules.\n");
            return gamesloop (&engine, gi);
            break;
        case MAFIERZ:
            //printf ("Playing italian rules on english board.\n");
            return gamesloop (&engine, gi);
            break;
        case RUSSIAN:
            //printf ("Playing russian rules.\n");
            return gamesloop (&engine, gi);
            break;
        default:
            //printf ("Unknown rules.\n");
            strcpy (gi->opponent, "part");
        }
        //printf ("\nOpponent left.\n");
        return -1;
    }


    /*
    // main
    int
    main (int argc, char *argv[])
    {
      char *progname;
      char buf[32];
      gameinfo gi;

      // make stdout unbuffered
      // xcheckers needs that
      setvbuf (stdout, NULL, _IONBF, 0);

      // program name
      if ((progname = strrchr (argv[0], '/')) == NULL)
        progname = argv[0];
      else
        progname++;

      // init gameinfo
      memset (&gi, 0, sizeof (gi));
      gi.result = UNKNOWN;

      // player name
      if ((gi.player = getenv ("LOGNAME")) == NULL)
        gi.player = strdup ("player");

      // check opponent
      gi.opponent = malloc (32);
      if (argc >= 2)
        strncpy (gi.opponent, argv[1], 32);
      else
        strcpy (gi.opponent, "part");

      // print greeting
      printf ("\n#\n# %s\n", progname);
      printf ("# Command Line Interface for CHeckers Engines,\n");
      printf ("# that implement the CheckerBoard API\n");
      printf ("#\n");
      printf ("# Commands: quit, part, new, save\n");
      printf ("# Moves: from-to\n");
      printf ("\nWelcome %s !\n", gi.player);

      // start opponentsloop
      while (strcmp (gi.opponent, "quit") && strcmp (gi.opponent, "exit"))
        {
          if (strcmp (gi.opponent, "part"))
     {
       opponentsloop (&gi);
     }
          else
     {
       printf ("\nName your opponent : ");
       fgets (buf, sizeof (buf), stdin);
       sscanf (buf, "%32s", gi.opponent);
     }
        }
      printf ("\nGood bye.\n");
      free (gi.opponent);
      return 0;
    }
    */

}
