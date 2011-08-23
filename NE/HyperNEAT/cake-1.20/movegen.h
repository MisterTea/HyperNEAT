/* movegen.h: function prototypes of movegen.c */

int makemovelist(struct move movelist[MAXMOVES],int color, int32 hashmove, int32 killer);
void blackorderevaluation(struct move ml[MAXMOVES],int n);
void whiteorderevaluation(struct move ml[MAXMOVES],int n);

/* captgen.h: function prototypes for captgen.c */

int makecapturelist(struct move movelist[MAXMOVES],int color, int32 best);

void blackmancapture1(struct move movelist[MAXMOVES], int *n, struct move partial, int32 square);
void blackkingcapture1(struct move movelist[MAXMOVES], int *n, struct move partial, int32 square);
void whitemancapture1(struct move movelist[MAXMOVES], int *n, struct move partial, int32 square);
void whitekingcapture1(struct move movelist[MAXMOVES], int *n, struct move partial, int32 square);
void blackmancapture2(struct move movelist[MAXMOVES], int *n, struct move partial, int32 square);
void blackkingcapture2(struct move movelist[MAXMOVES], int *n, struct move partial, int32 square);
void whitemancapture2(struct move movelist[MAXMOVES], int *n, struct move partial, int32 square);
void whitekingcapture2(struct move movelist[MAXMOVES], int *n, struct move partial, int32 square);
