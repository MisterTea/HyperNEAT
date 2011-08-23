 /* a part of the cake++ package - a checkers engine
    Copyright (C) 2000 by Martin Fierz

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    contact: checkers@fierz.ch
    */


/* interface.c provides the function islegal() which is not part
	of the cake++ checkers engine, but which can be helpful when
   programming an interface to it. */


#include <stdio.h>
#include "structs.h"
#include "consts.h"
#include "movegen.h"
#include "cakepp.h"
#include "switches.h"


int islegal(struct pos *position,int color, int from, int to)
	{
   /*
   islegal receives the current position, the side to move in color, and
   the squares from & to (standard checkers notation),
   which represent the move which the user would like
   to make.

   if the move is legal, islegal returns 1 and writes the new position
   in *p. if not, islegal returns 0.
   */
   /* checkers notation:
       32  31  30  29
     28  27  26  25
       24  23  22  21
     20  19  18  17
       16  15  14  13
     12  11  10  9
       8   7   6   5
     4   3   2   1
   */
   struct move movelist[MAXMOVES];
   int i,n,Lfrom,Lto;
   char c,Lstr[256];
   extern struct pos p; /* from cakepp.c */


   /*printboard(*position);*/
   /* get a movelist */
   p=*position;
   n=makecapturelist(movelist,color,0);
   if(!n)
   	n=makemovelist(movelist,color,0,0);

   /* for all moves: convert move to notation and compare */
   for(i=0;i<n;i++)
   	{
      movetonotation(*position,movelist[i], Lstr, color);
      /*Lstr contains something like "11-15" or "15x24" */
      sscanf(Lstr,"%2i%c%2i",&Lfrom,&c,&Lto);
      printf("\n%s ... %i %i",Lstr,Lfrom,Lto);
      if(Lto<0) Lto=-Lto;  
      if((Lfrom==from) && (Lto==to))
      	{
         (*position).bm^=movelist[i].bm;
         (*position).bk^=movelist[i].bk;
         (*position).wm^=movelist[i].wm;
         (*position).wk^=movelist[i].wk;
         return 1;
         }
      }
   return 0;
   }
