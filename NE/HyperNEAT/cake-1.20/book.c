/*
 * book.c
 *
 * a part of the cake++ package - a checkers engine Copyright (C) 2000-2005 by
 * Martin Fierz
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


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "structs.h"
#include "consts.h"
#include "book.h"

static int book_entries;
static BOOK_ENTRY *book;


int32 booklookup(struct pos *p, int color)
	{
	/* look up the best move for color in position p */
	int32 black, white, kings;
	int i;

	/* the seven first moves in a game, pch */
	int f[] = {0x1100, 0x1200, 0x2200, 0x2400, 0x4400, 0x4800, 0x8800};

	black = p->bm|p->bk;
	white = p->wm|p->wk;
	kings = p->bk|p->wk;

	/* "randomly" select first move in game, pch */
	if (color == BLACK && !kings && black == 0xFFF && white == 0xFFF00000)
		return f[time(0) % (sizeof(f)/sizeof(f[0]))];

	for(i = 0; i < book_entries; i++)
		{
		if(book[i].black == black && book[i].white == white && book[i].kings == kings && book[i].color == color)
			{
			if(book[i].move == 0)
				printf("\nbook move is zero - this should never happen!");
			return book[i].move;
			}
		}
	
	return 0;
	}

int   initbook(void)
	{
	/* load book file and set the number of book entries*/
	FILE *fp;
	BOOK_ENTRY be;
	/*struct pos p;
	int board[8][8];*/

	book_entries = 0;

	/* open the book file */
	fp = fopen("../cake-1.20/xbook.bin", "rb");
	if(fp == NULL)
    {
        printf("Error: Missing book file!\n");
        exit(1);
		return 0;
    }

	/* find out how many entries n there are in the book */
	while(!feof(fp))
		{
		fread(&be, sizeof(BOOK_ENTRY), 1, fp);
		/*if(bitcount(be.move)>4)
			{
			p.bm = be.black;
			p.wm = be.white;
			p.wk = 0;
			p.bk = 0;
			bitboardtoboard(p, board);
			ansiprintboard(board, 2, be.color, stdout);
			printf("\nbest move is %x for book pos #%i",be.move,book_entries);
			if(be.color == 2)
				printf("\nblack to move");
			else
				printf("\nwhite to move");
			p.bm = be.move;
			p.wm = 0;
			bitboardtoboard(p,board);
			ansiprintboard(board, 2, be.color, stdout);
			
			getch();
			}*/
		book_entries++;
		}

	/* allocate enough memory for the book */
	book = malloc(book_entries*sizeof(BOOK_ENTRY));
	if(book == NULL)
		{
		fclose(fp);
		return 0;
		}

	/* read the book file into memory */
	rewind(fp);
	fread(book, sizeof(BOOK_ENTRY), book_entries, fp);

	/* close file and return */
	fclose(fp);

	return book_entries;
	}
