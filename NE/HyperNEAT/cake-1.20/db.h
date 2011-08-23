/* cake_db.h: function prototypes for cake_db.c */

long CheckPos(void);
long DBLookup(struct pos p, int turn);
void Setup(void);
void SkipLine(void);
int DBInit(void); /* modified MF returns the number of stones it still has */
//void exit(int x);
long Nsq();
int32 dbLocbvToSubIdx();
