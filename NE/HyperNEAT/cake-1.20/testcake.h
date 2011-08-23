#ifndef __CAKE_TESTCAKE_H__
#define __CAKE_TESTCAKE_H__

int InitBoard(int b[8][8]);
void boardtobitboard(int b[8][8], struct pos *position);
void ucharboardtobitboard(unsigned char b[8][8], struct pos *position);

void bitboardtoboard(struct pos position,int b[8][8]);
void ucharbitboardtoboard(struct pos position,unsigned char b[8][8]);

#endif
