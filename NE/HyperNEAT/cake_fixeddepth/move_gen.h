/* movegen.h: function prototypes of movegen.c */

int makemovelist(SEARCHINFO *si, POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES], int bestindex, int32 killer);
void blackorderevaluation(SEARCHINFO *si, POSITION *p,CAKE_MOVE ml[MAXMOVES],int values[MAXMOVES],int n);
void whiteorderevaluation(SEARCHINFO *si, POSITION *p,CAKE_MOVE ml[MAXMOVES],int values[MAXMOVES],int n);
int getorderedmovelist(POSITION *p, CAKE_MOVE movelist[MAXMOVES]);
int numberofmoves(CAKE_MOVE m[MAXMOVES]);

// qs
#ifdef QSEARCH
int makeQSmovelist(POSITION *p, MOVE movelist[MAXMOVES]);
#endif

/* captgen.h: function prototypes for captgen.c */

int makecapturelist(POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES], int32 bestindex);

static void blackmancapture1( POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES], int *n, CAKE_MOVE *partial, int32 square);
static void blackkingcapture1(POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES], int *n, CAKE_MOVE *partial, int32 square);
static void whitemancapture1( POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES], int *n, CAKE_MOVE *partial, int32 square);
static void whitekingcapture1(POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES], int *n, CAKE_MOVE *partial, int32 square);
static void blackmancapture2( POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES], int *n, CAKE_MOVE *partial, int32 square);
static void blackkingcapture2(POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES], int *n, CAKE_MOVE *partial, int32 square);
static void whitemancapture2( POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES], int *n, CAKE_MOVE *partial, int32 square);
static void whitekingcapture2(POSITION *p,CAKE_MOVE movelist[MAXMOVES],int values[MAXMOVES], int *n, CAKE_MOVE *partial, int32 square);

class CheckersAdvisor
{
public:
    virtual void setBoardPosition(unsigned char board[8][8]) = 0;

    virtual float getBoardValue(int xpos,int ypos) = 0;
};

