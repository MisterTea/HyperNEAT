#ifndef __CAKE_MISC_H__
#define __CAKE_MISC_H__

#include <stdio.h>
#include <stdlib.h>

#include "structs.h"

void searchinfotostring(char *out, int depth, double time, char *valuestring, char *pvstring, SEARCHINFO *counter);
void movetonotation(POSITION *p, CAKE_MOVE *m, char *str);
void printint32(int32 x);
void printboard(POSITION *p);
void printboardtofile(POSITION *p, FILE *fp);
int isforced(POSITION *p);
int SquareToBit(int square);
int logtofile(char *str);

#endif
