// dblookup.h
// all you need.

// include 
#include "switches.h"


// choose which db you want to use

#define PRELOAD // preload (parts) of db in cache? 

#define AUTOLOADSIZE 0

#define DB_BLACK 0
#define DB_WHITE 1







#define DB_UNKNOWN 0
#define DB_WIN 1
#define DB_LOSS 2
#define DB_DRAW 3
#define DB_NOT_LOOKED_UP 4


#define SPLITSIZE 8
#define BLOCKNUM4 250
#define BLOCKNUM5 2000
#define BLOCKNUM6 42000
#define BLOCKNUM7 400000
#define BLOCKNUM8 4600000

#define MAXIDX4 100
#define MAXIDX5 500
#define MAXIDX6 1000
#define MAXIDX7 31000
#define MAXIDX8 44000
#define SPLITSIZE 8
#define MAXPIECE 4


#define MINCACHESIZE 65536

//int initdblookup(char str[256]);
int db_init(int suggestedMB, char str[256]);
int db_exit(void);
int dblookup(POSITION *p,int cl);
int revert(int32 n);
int db_getcachesize(void);
void db_infostring(char *str);
int LSB(int32 x);
int MSB(int32 x);
