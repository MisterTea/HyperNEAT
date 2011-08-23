int evaluation(POSITION *p, MATERIALCOUNT *mc, int alpha, int *delta, int capture, int maxNdb);
int fineevaluation(EVALUATION *e, POSITION *p, MATERIALCOUNT *mc, KINGINFO *ki, int *noprune, int*likelydraw);
int selftrapeval(POSITION *p, MATERIALCOUNT *mc, KINGINFO *ki, int *delta);
static int32 attack_backwardjump(int32 x, int32 free);
static int32 attack_forwardjump(int32 x, int32 free);
static int32 backwardjump(int32 x, int32 free,int32 other);
static int32 forwardjump(int32 x, int32 free,int32 other);
int initeval(void);
int materialevaluation(MATERIALCOUNT *m);

#ifdef USEDB
int dbwineval(POSITION *p, MATERIALCOUNT *mc);
#endif
