/*
 * a part of the cake++ package - a checkers engine Copyright (C) 2000-2005 by
 * Martin Fierz
 *
 * command line interface part (C) 2000 by Peter Chiocchetti
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * contact: nospam1@fierz.ch
 */

/* Changes:

  1.19	(2004-12-17):
	the caller of cake_get_move has to set playnow to 0;
	the engine sets it to signal, it took longer than the
	allocated time to find a move

*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"		/* data structures  */
#include "cakepp.h"		/* function prototypes  */
#include "consts.h"		/* constants  */
#include "movegen.h"		/* makecapturelist ...  */
#include "switches.h"		/* SYS_UNIX  */



/*
 * islegal() is not part of the cake++ checkers engine, but which can be
 * helpful when programming an interface to it.
 */

/*
 * islegal receives the current position, the side to move in color, and the
 * squares from & to (standard checkers notation), which represent the move
 * which the user would like to make.
 *
 * if the move is legal, islegal returns 1 and writes the new position in *p. if
 * not, islegal returns 0.
 */

/*
 * checkers notation:
 *       WHITE
 *   28  29  30  31
 * 24  25  26  27
 *   20  21  22  23
 * 16  17  18  19
 *   12  13  14  15
 *  8   9  10  11
 *    4   5   6   7
 *  0   1   2   3
 *      BLACK
 */

int
islegal(struct pos * position, int color, int from, int to, int jump)
{
	struct move     movelist[MAXMOVES];
	int             i, n, Lfrom, Lto;
	char            c, Lstr[256];
	extern struct pos p;	/* from cakepp.c  */


	/* get a movelist  */
	p = *position;
	n = makecapturelist(movelist, color, 0);
	if (!n) {
		if (jump)
			return 0;
		else
			n = makemovelist(movelist, color, 0, 0);
	}
	/* for all moves: convert move to notation and compare  */
	for (i = 0; i < n; i++) {
		movetonotation(*position, movelist[i], Lstr, color);
		/* Lstr contains something like "11-15" or "15x24"  */
		sscanf(Lstr, "%2i%c%2i", &Lfrom, &c, &Lto);
		//printf("\n%s ... %i %i", Lstr, Lfrom, Lto);
		if (Lto < 0)
			Lto = -Lto;
		if ((Lfrom == from) && (Lto == to)) {
			(*position).bm ^= movelist[i].bm;
			(*position).bk ^= movelist[i].bk;
			(*position).wm ^= movelist[i].wm;
			(*position).wk ^= movelist[i].wk;
			return 1;
		}
	}
	return 0;
}


/* initialize an 8x8 board to the starting position  */

static void
newboard(int b[8][8])
{
	int             i, j;

	for (i = 0; i <= 7; i++) {
		for (j = 0; j <= 7; j++) {
			if ((i + j) % 2 == 0)
				b[i][j] = FREE;
			else
				b[i][j] = 0;
		}
	}
	b[0][0] = BLACK | MAN;
	b[2][0] = BLACK | MAN;
	b[4][0] = BLACK | MAN;
	b[6][0] = BLACK | MAN;
	b[1][1] = BLACK | MAN;
	b[3][1] = BLACK | MAN;
	b[5][1] = BLACK | MAN;
	b[7][1] = BLACK | MAN;
	b[0][2] = BLACK | MAN;
	b[2][2] = BLACK | MAN;
	b[4][2] = BLACK | MAN;
	b[6][2] = BLACK | MAN;

	b[1][7] = WHITE | MAN;
	b[3][7] = WHITE | MAN;
	b[5][7] = WHITE | MAN;
	b[7][7] = WHITE | MAN;
	b[0][6] = WHITE | MAN;
	b[2][6] = WHITE | MAN;
	b[4][6] = WHITE | MAN;
	b[6][6] = WHITE | MAN;
	b[1][5] = WHITE | MAN;
	b[3][5] = WHITE | MAN;
	b[5][5] = WHITE | MAN;
	b[7][5] = WHITE | MAN;
}


/* return an 8x8 square from a bitboard  */

void
bitboardtoboard(struct pos position, int b[8][8])
{
	int             i, board[32];

	for (i = 0; i < 32; i++) {
		if (position.bm & (1 << i))
			board[i] = (BLACK | MAN);
		if (position.bk & (1 << i))
			board[i] = (BLACK | KING);
		if (position.wm & (1 << i))
			board[i] = (WHITE | MAN);
		if (position.wk & (1 << i))
			board[i] = (WHITE | KING);
		if ((~(position.bm | position.bk | position.wm | position.wk))
		    & (1 << i))
			board[i] = FREE;
	}
	/* return the board  */
	b[0][0] = board[0];
	b[2][0] = board[1];
	b[4][0] = board[2];
	b[6][0] = board[3];
	b[1][1] = board[4];
	b[3][1] = board[5];
	b[5][1] = board[6];
	b[7][1] = board[7];
	b[0][2] = board[8];
	b[2][2] = board[9];
	b[4][2] = board[10];
	b[6][2] = board[11];
	b[1][3] = board[12];
	b[3][3] = board[13];
	b[5][3] = board[14];
	b[7][3] = board[15];
	b[0][4] = board[16];
	b[2][4] = board[17];
	b[4][4] = board[18];
	b[6][4] = board[19];
	b[1][5] = board[20];
	b[3][5] = board[21];
	b[5][5] = board[22];
	b[7][5] = board[23];
	b[0][6] = board[24];
	b[2][6] = board[25];
	b[4][6] = board[26];
	b[6][6] = board[27];
	b[1][7] = board[28];
	b[3][7] = board[29];
	b[5][7] = board[30];
	b[7][7] = board[31];
}


/* initialize bitboard from an 8x8 square  */

static void
boardtobitboard(int b[8][8], struct pos * position)
{
	int             i, board[32];

	board[0] = b[0][0];
	board[1] = b[2][0];
	board[2] = b[4][0];
	board[3] = b[6][0];
	board[4] = b[1][1];
	board[5] = b[3][1];
	board[6] = b[5][1];
	board[7] = b[7][1];
	board[8] = b[0][2];
	board[9] = b[2][2];
	board[10] = b[4][2];
	board[11] = b[6][2];
	board[12] = b[1][3];
	board[13] = b[3][3];
	board[14] = b[5][3];
	board[15] = b[7][3];
	board[16] = b[0][4];
	board[17] = b[2][4];
	board[18] = b[4][4];
	board[19] = b[6][4];
	board[20] = b[1][5];
	board[21] = b[3][5];
	board[22] = b[5][5];
	board[23] = b[7][5];
	board[24] = b[0][6];
	board[25] = b[2][6];
	board[26] = b[4][6];
	board[27] = b[6][6];
	board[28] = b[1][7];
	board[29] = b[3][7];
	board[30] = b[5][7];
	board[31] = b[7][7];

	(*position).bm = 0;
	(*position).bk = 0;
	(*position).wm = 0;
	(*position).wk = 0;

	for (i = 0; i < 32; i++) {
		switch (board[i]) {
		case BLACK | MAN:
			(*position).bm = (*position).bm | (1 << i);
			break;
		case BLACK | KING:
			(*position).bk = (*position).bk | (1 << i);
			break;
		case WHITE | MAN:
			(*position).wm = (*position).wm | (1 << i);
			break;
		case WHITE | KING:
			(*position).wk = (*position).wk | (1 << i);
			break;
		}
	}
}


/*
 * command line interface,
 * ask for colour, ask for maxtime, loop some games, loop some moves
 * print nice boards, that xcheckers can read.
 * lets "quit" at any prompt
 */


/* ask for new game or load game  */

static int
getgame()
{
	int             n;
	int             game = 0;

	while (!game) {
		printf("\nload board or play new game l/p ? ");
		n = tolower(getchar());
		if (n == 'p')
			game = 1;
		else if (n == 'l')
			game = 2;
		else if (n == 'q')
			break;
		/* clear buffer  */
		while (n != '\n' && n != EOF)
			n = getchar();
	}
	return game;
}


/* read in the players color, i.e. his viewpoint  */

static int
getview()
{
	int             n;
	int             color = 0;

	while (!color) {
		printf("\ntype b/w to select your color: ");
		n = tolower(getchar());
		if (n == 'b')
			color = BLACK;
		else if (n == 'w')
			color = WHITE;
		else if (n == 'q')
			break;
		/* clear buffer  */
		while (n != '\n' && n != EOF)
			n = getchar();
	}
	return color;
}


/* read in the time cake has to compute its moves  */

static int
getmaxtime()
{
	int             maxtime = 0;
	char            str[6];

	while (!maxtime) {
		printf("\nenter cake's time in seconds: ");
		scanf("%5s", &str[0]);
		if (strstr(str, "quit"))
			return 0;
		maxtime = (int) strtol(str, (char **) NULL, 10);
	}
	return maxtime;
}


/* read in the players move  */

static int
getmove(int *start, int *end)
{
	char            str[20];

	printf("\nenter your move ? ");
	scanf("%20s", &str[0]);
	if (strstr(str, "part"))
		return 0;
	else if (sscanf(str, "%d-%d", start, end) == 2)
{
	printf("MOVE: %d - %d\n",*start,*end);
		return 1;
}
	else if (sscanf(str, "%dx%d", start, end) == 2)
		return 2;
	else if (strstr(str, "quit"))
		return 3;
	else if (strstr(str, "load"))
		return 4;
	else if (strstr(str, "save"))
		return 5;
	else if (strstr(str, "time"))
		return 6;
	return 7;
}


/* print an 8x8 square  */

void
ansiprintboard(int b[8][8], int view, int color, FILE * out)
{
	int             i, j;

	if (view == BLACK) {
		if (color == BLACK)
			fprintf(out, "\n   ~~~~~~~~~~~~~~~ ");
		else
			fprintf(out, "\n.  ~~~~~~~~~~~~~~~ ");
		for (j = 7; j >= 0; j--) {
			fprintf(out, "\n   ");
			for (i = 0; i <= 7; i++) {
				switch (b[i][j]) {
				case BLACK | MAN:
					fprintf(out, "b ");
					break;
				case BLACK | KING:
					fprintf(out, "B ");
					break;
				case WHITE | MAN:
					fprintf(out, "w ");
					break;
				case WHITE | KING:
					fprintf(out, "W ");
					break;
				case FREE:
					fprintf(out, "- ");
					break;
				default:
					fprintf(out, "  ");
				}
			}
		}
	} else {
		if (color == BLACK)
			fprintf(out, "\n-  ~~~~~~~~~~~~~~~ ");
		else
			fprintf(out, "\n*  ~~~~~~~~~~~~~~~ ");
		for (j = 0; j <= 7; j++) {
			fprintf(out, "\n   ");
			for (i = 7; i >= 0; i--) {
				switch (b[i][j]) {
				case BLACK | MAN:
					fprintf(out, "b ");
					break;
				case BLACK | KING:
					fprintf(out, "B ");
					break;
				case WHITE | MAN:
					fprintf(out, "w ");
					break;
				case WHITE | KING:
					fprintf(out, "W ");
					break;
				case FREE:
					fprintf(out, "- ");
					break;
				default:
					fprintf(out, "  ");
				}
			}
		}
	}
	fprintf(out, "\n\n");
}


/*
 * quick save
 *
 * write/read nice human readable board to/from file
 * first nonempty line codes view and color
 * board lines end with a blank
 * last line is cake maxtime in seconds
 * file may change while cake is running (cheat :)
 * can only save, when engine made its move
 * does a little checking on sanity of file (improve this!)
 */

#define SAVEGAME "cakegame"
#define strneq(a,b,n) (strncmp ((a),(b),(n)) == 0)

/* save current game state to a file  */

static void
saveboard(int b[8][8], int view, int color, int maxtime, char *path)
{
	FILE           *cakegame;

	cakegame = fopen(path, "w");
	if (!cakegame) {
		perror(path);
		return;
	}
	ansiprintboard(b, view, color, cakegame);
	fprintf(cakegame, "%d\n", maxtime);
	fclose(cakegame);
	printf("\nwrote to file %s.\n", path);
}


/* read a line from a saved game  */

static int
readboard(int b[8][8], int *view, int *color, char *line)
{
	static int      i = 0;
	int             j;
	static int      state = 0;


	if (state == 0) {	/* got no board  */
		if (strneq(&line[0], "   ~~~~~~~~~~~~~~~ ", 19)) {
			i = 0;
			state = 1;
			*view = BLACK;
			*color = BLACK;
			return 0;
		} else if (strneq(&line[0], "*  ~~~~~~~~~~~~~~~ ", 19)) {
			i = 0;
			state = 1;
			*view = WHITE;
			*color = WHITE;
			return 0;
		}
	}
	if (state == 1 && i == 8) {	/* board complete  */
		state = 0;
		i = 0;
		return 9;
	}
	if (state == 1 && *view == BLACK) {	/* next line  */
		for (j = 3; j < 18; j += 2) {
			if (strneq(&line[j], "  ", 2))
				b[(j - 3) / 2][7 - i] = 0;
			else if (strneq(&line[j], "- ", 2))
				b[(j - 3) / 2][7 - i] = FREE;
			else if (strneq(&line[j], "b ", 2))
				b[(j - 3) / 2][7 - i] = BLACK | MAN;
			else if (strneq(&line[j], "w ", 2))
				b[(j - 3) / 2][7 - i] = WHITE | MAN;
			else if (strneq(&line[j], "B ", 2))
				b[(j - 3) / 2][7 - i] = BLACK | KING;
			else if (strneq(&line[j], "W ", 2))
				b[(j - 3) / 2][7 - i] = WHITE | KING;
			else
				state = 0;
		}
		i++;
	} else if (state == 1 && *view == WHITE) {	/* next line  */
		for (j = 3; j < 18; j += 2) {
			if (strneq(&line[j], "  ", 2))
				b[7 - (j - 3) / 2][i] = 0;
			else if (strneq(&line[j], "- ", 2))
				b[7 - (j - 3) / 2][i] = FREE;
			else if (strneq(&line[j], "b ", 2))
				b[7 - (j - 3) / 2][i] = BLACK | MAN;
			else if (strneq(&line[j], "w ", 2))
				b[7 - (j - 3) / 2][i] = WHITE | MAN;
			else if (strneq(&line[j], "B ", 2))
				b[7 - (j - 3) / 2][i] = BLACK | KING;
			else if (strneq(&line[j], "W ", 2))
				b[7 - (j - 3) / 2][i] = WHITE | KING;
			else
				state = 0;
		}
		i++;
	}
	return i;
}


/* load a suspended game from a file  */

static int
loadboard(int b[8][8], int *view, int *color, int *maxtime, char *path)
{
	int             i, n;
	int             t = 0;
	char            buf[31];
	FILE           *cakegame;

	cakegame = fopen(path, "r");
	if (!cakegame) {
		perror(path);
		return 0;
	}
	/* fill buffer  */
	while ((n = fgetc(cakegame)) != EOF) {
		ungetc(n, cakegame);
		/* search for a newline  */
		for (i = 0; i < 30; i++) {
			if ((n = fgetc(cakegame)) == '\n') {
				buf[i] = '\0';
				break;
			}
			buf[i] = n;
		}
		t += readboard(b, view, color, buf);
	}
	if (t != 45)
		printf("\ngame file %s corrupt ?\n", path);
	*maxtime = (int) strtol(buf, (char **) NULL, 10);
	fclose(cakegame);
	printf("\nread from file %s.\n", path);
	printf("cake maxtime set to %d.\n", *maxtime);
	return 1;
}


/*
 * cake_getmove is the entry point to cake++
 * - give a  pointer to a position and you get the new
 *   position in this structure after cake++ has calculated.  -
 *   color is BLACK or WHITE and is the side to move.
 * - how is 0 for time-based search and 1 for depth-based
 *   search and 2 for node-based search
 * - maxtime and depthtosearch and maxnodes are used for these
 *   two search modes.
 * - cake++ prints information in str
 * - if playnow is set to a value != 0 cake++ aborts the
 *   search.
 * - if (logging&1) cake++ will write information into
 *   "log.txt"
 *   if(logging&2) cake++ will also print the information to
 *   stdout.
 * - if reset!=0 cake++ will reset hashtables and repetition
 *   checklist
 * int cake_getmove(struct pos *position, int color,
 *                  int how, double maxtime,
 *                  int depthtosearch, int32 maxnodes,
 *                  char str[255], int *playnow,
 *                  int log, int reset)
 */

int
main()
{
	int             view;
	int             move = 0;
	int             start, end;
	char           *path;	/* pathname for cakegame  */
	char           *temp;	/* myname and myhome  */

	int             board[8][8];
	int             logging = 0;	/* cake_getmove()  */
	int             maxtime = 2;	/* cake_getmove()  */
	int             playnow = 0;	/* cake_getmove()  */
	int             value;
	char            str[255];
	struct pos      p;	/* struct pos defined in cake_structs.h  */
    char            moveNotation[256];
	int             color;	/* BLACK & WHITE defined in cake_consts.h  */
	static char     banner[] =
	"\nCake - a checkers engine,\n"
	"Copyright (C) 2000 Martin Fierz.\n"
	"Cake comes with ABSOLUTELY NO WARRANTY; for details\n"
	"see the file \"COPYING\" in the distribution directory.\n";

	printf(banner);

#ifdef SYS_UNIX
	/* make stdout unbuffered  */
	setvbuf(stdout, NULL, _IONBF, 0);

	if ((temp = getenv("LOGNAME")) == NULL)
		temp = strdup("unnamed");
	printf("\nWelcome %s!\n\n", temp);

	if ((temp = getenv("HOME")) == NULL)
		temp = strdup(".");
	path = (char *) malloc(strlen(temp) + strlen(SAVEGAME) + 2);
	strcpy(path, temp);
	strcat(path, "/");
	strcat(path, SAVEGAME);
#endif
#ifdef SYS_WINDOWS
	path = strdup(SAVEGAME);
#endif

	/* initialize cake++: allocate memory for hashtables etc  */
	initcake(logging);

	do {			/* games loop  */
		color = BLACK;
		value = getgame();
		playnow = 0; // important!
		if (value == 0)	/* quit */
			break;
		else if (value == 1) {	/* new game */
			view = getview();
			if (view != BLACK && view != WHITE)
				break;	/* quit */
			maxtime = 1;//getmaxtime(); //Don't use maxtime
			if (maxtime <= 0)
				break;	/* quit */
			/* initialize board */
			newboard(board);
		} else if (value == 2) {	/* load game */
			if (!loadboard(board, &view, &color, &maxtime, path))
				continue;
		}
		boardtobitboard(board, &p);
		ansiprintboard(board, view, color, stdout);

		if (view == WHITE && value != 2) {	/* make a move */
			value = cake_getmove(&p, moveNotation, color, 0, maxtime, 9, 10000,
					     str, &playnow, logging, 1);
			printf("EV: %d\n", value);
			bitboardtoboard(p, board);
			/* change color to move */
			color = color ^ CC;
			ansiprintboard(board, view, color, stdout);
		}
		do {		/* moves loop  */
			value = 1;	/* assume legal move  */
			move = getmove(&start, &end);
			switch (move) {
			case 7:		/* malformed move  */
				printf("malformed move, try again.\n");
				ansiprintboard(board, view, color, stdout);
				continue;
			case 6:		/* time  */
				maxtime = getmaxtime();
					if (maxtime <= 0) { /* quit */
						color = 0;
						move = 0;
					}
				continue;
			case 5:		/* save  */
				saveboard(board, view, color, maxtime, path);
				continue;
			case 4:		/* load  */
				loadboard(board, &view, &color, &maxtime, path);
				ansiprintboard(board, view, color, stdout);
				boardtobitboard(board, &p);
				continue;
			case 3:		/* quit  */
				color = 0;
				move = 0;
				continue;
			case 2:		/* jump  */
				value = islegal(&p, color, start, end, 1);
				break;
			case 1:		/* move  */
				value = islegal(&p, color, start, end, 0);
				break;
			case 0:		/* part  */
				continue;
			default:
				printf("internal error\n");
				continue;
			}
			/* was it a legal move ?  */
			if (value == 0) {	/* bad, restore board  */
				printf("illegal move, try again.\n");
				boardtobitboard(board, &p);
				ansiprintboard(board, view,
					       color, stdout);
				continue;
			}
			/* change color to move  */
			color = color ^ CC;
			bitboardtoboard(p, board);
			ansiprintboard(board, view, color, stdout);
			/* make a move  */
			playnow = 0; // important!
			value = cake_getmove(&p, moveNotation, color, 0, maxtime, 9, 10000,
					     str, &playnow, logging, 1);
			if (playnow)
				printf("Used 4times allocated time.\n");
			if (value >= 4999) {
				printf("program wins !\n");
				move = 0;
			} else if (value <= -4999) {
				printf("you win !\n");
				move = 0;
				break;
			}
			printf("EV: %d\n", value);
			bitboardtoboard(p, board);
			/* change color to move  */
			color = color ^ CC;
			ansiprintboard(board, view, color, stdout);
		} while (move);
	} while (color);
	exitcake();
	return 0;
}
