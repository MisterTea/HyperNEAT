#ifndef HCUBE_OTHELLOCOMMON_H_INCLUDED
#define HCUBE_OTHELLOCOMMON_H_INCLUDED

#include "HCUBE_Defines.h"

//Lowest 2 bits hold the piece
#define OTHELLO_EMPTY (0)
#define OTHELLO_BLACK (1)
#define OTHELLO_WHITE (2)
#define OTHELLO_INVALID (3)

#define OTHELLO_END_TIE (-1)
#define OTHELLO_END_UNKNOWN (-2)

//Position 0,0 is also used for the number of black pieces
#define OTHELLO_GET_NUM_BLACK_PIECES(ARRAY) ((ARRAY[0][0])>>2)

//Position 0,1 is also used for the number of white pieces
#define OTHELLO_GET_NUM_WHITE_PIECES(ARRAY) ((ARRAY[0][1])>>2)

//The number of black pieces is moved up two bits, then merged with the piece type in 0,0
#define OTHELLO_SET_NUM_BLACK_PIECES(ARRAY,VALUE) ( ARRAY[0][0] = ( (ARRAY[0][0]&0x3)|((VALUE)<<2) ) )

//The number of white pieces is moved up two bits, then merged with the piece type in 0,1
#define OTHELLO_SET_NUM_WHITE_PIECES(ARRAY,VALUE) ( ARRAY[0][1] = ( (ARRAY[0][1]&0x3)|((VALUE)<<2) ) )

#define OTHELLO_INC_NUM_BLACK_PIECES(ARRAY) \
    ( OTHELLO_SET_NUM_BLACK_PIECES( ARRAY, (OTHELLO_GET_NUM_BLACK_PIECES(ARRAY)+1) ) )

#define OTHELLO_DEC_NUM_BLACK_PIECES(ARRAY) \
    ( OTHELLO_SET_NUM_BLACK_PIECES( ARRAY, (OTHELLO_GET_NUM_BLACK_PIECES(ARRAY)-1) ) )

#define OTHELLO_INC_NUM_WHITE_PIECES(ARRAY) \
    ( OTHELLO_SET_NUM_WHITE_PIECES( ARRAY, (OTHELLO_GET_NUM_WHITE_PIECES(ARRAY)+1) ) )

#define OTHELLO_DEC_NUM_WHITE_PIECES(ARRAY) \
    ( OTHELLO_SET_NUM_WHITE_PIECES( ARRAY, (OTHELLO_GET_NUM_WHITE_PIECES(ARRAY)-1) ) )

//Lowest 2 bits hold the piece
#define OTHELLO_GET_PIECE(DATA) ((DATA) & 0x3)

//Clear the lowest 2 bits, then set them with an or.
#define OTHELLO_SET_PIECE(DATA,COLOR) ((DATA) = (((DATA)&(~0x3))|(COLOR)))

//To reverse a piece, flip the last two bits while leaving the rest of the bits intact
#define OTHELLO_REVERSE_PIECE(DATA) ( (DATA) = ( (DATA & (~0x3)) | ( (~(DATA)) & 0x3) ) )

#define OTHELLO_IS_IN_BOUNDS(X,Y) ((X)>=0&&(Y)>=0&&(X)<8&&(Y)<8)

#define MAX_TOTAL_MOVES (262144)

#define OTHELLO_USE_BOOST_POOL (0)

#define OTHELLO_MAX_FLIPS (64)

namespace HCUBE
{
    class OthelloMove
    {
    public:
        Vector2<uchar> position;
        Vector2<uchar> piecesFlipped[OTHELLO_MAX_FLIPS];
        uchar color;

        OthelloMove();

        OthelloMove(const Vector2<uchar> &_position,uchar _color);

        OthelloMove(const OthelloMove &other);

        const OthelloMove &operator=(const OthelloMove &other);

        void reset(const Vector2<uchar> &_position,uchar _color);

        //This is expensive so I try to avoid using it, but it gives an idea on
        //how this is supposed to work.
        void appendPieceFlipped(const Vector2<uchar> &location);

        int getNumPiecesFlipped();

        ~OthelloMove();
    };

#if OTHELLO_USE_BOOST_POOL
    struct OthelloMovePoolTag
        { };

    typedef boost::singleton_pool<OthelloMovePoolTag, sizeof(OthelloMove)> checkersMovePool;
#endif

    class OthelloCommon
    {
    protected:

        OthelloMove totalMoveList[MAX_TOTAL_MOVES];
        int totalNumMoves;
    public:
        void printBoard(ushort b[8][8]);

        void makeMove(OthelloMove &move,ushort b[8][8]);

        void reverseMove(OthelloMove &move,ushort b[8][8]);

        bool hasMove(
            ushort b[8][8],
            int color,
            int x,
            int y
        );

        bool hasAnyMove(
            ushort b[8][8]
        );

        int generateMoveList(
            ushort b[8][8],
            OthelloMove *moveList,
            int color
        );

        int getWinner(ushort b[8][8]);
    };

}

#endif // HCUBE_OTHELLOCOMMON_H_INCLUDED
