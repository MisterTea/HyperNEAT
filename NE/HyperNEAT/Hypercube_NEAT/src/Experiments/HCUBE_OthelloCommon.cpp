#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_OthelloCommon.h"

#define OTHELLO_COMMON_DEBUG (0)

namespace HCUBE
{
    OthelloMove::OthelloMove()
            :
            position(255,255)
    {
        memset(piecesFlipped,255,sizeof(Vector2<uchar>)*OTHELLO_MAX_FLIPS);
    }

    OthelloMove::OthelloMove(const Vector2<uchar> &_position,uchar _color)
            :
            position(_position),
            color(_color)
    {
        memset(piecesFlipped,255,sizeof(Vector2<uchar>)*OTHELLO_MAX_FLIPS);
    }

    OthelloMove::OthelloMove(const OthelloMove &other)
            :
            position(other.position),
            color(other.color)
    {
        memcpy(piecesFlipped,other.piecesFlipped,sizeof(Vector2<uchar>)*OTHELLO_MAX_FLIPS);
    }

    const OthelloMove &OthelloMove::operator=(const OthelloMove &other)
    {
        if (this != &other)
        {
            position = other.position;
            color = other.color;
            memcpy(piecesFlipped,other.piecesFlipped,sizeof(Vector2<uchar>)*OTHELLO_MAX_FLIPS);
        }

        return *this;
    }

    void OthelloMove::reset(const Vector2<uchar> &_position,uchar _color)
    {
        color = _color;
        position = _position;
        memset(piecesFlipped,255,sizeof(Vector2<uchar>)*OTHELLO_MAX_FLIPS);
    }

    void OthelloMove::appendPieceFlipped(const Vector2<uchar> &location)
    {
        for (int a=0;a<OTHELLO_MAX_FLIPS;a++)
        {
            if (piecesFlipped[a].x==255)
            {
                piecesFlipped[a] = location;
                return;
            }
        }

        throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: TRIED TO ADD TOO MANY PIECES TO FLIPPED LIST!");
    }

    int OthelloMove::getNumPiecesFlipped()
    {
        for (int a=0;a<OTHELLO_MAX_FLIPS;a++)
        {
            if (piecesFlipped[a].x==255)
            {
                return a;
            }
        }

        throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: TRIED TO ADD TOO MANY PIECES TO FLIPPED LIST!");
    }

    OthelloMove::~OthelloMove()
    {}

    void OthelloCommon::printBoard(ushort b[8][8])
    {
        printf(
            "Piece count: White: %-2d Black %-2d\n",
            OTHELLO_GET_NUM_WHITE_PIECES(b),
            OTHELLO_GET_NUM_BLACK_PIECES(b)
        );

        const char *f = " BW ";
        printf ("\n");
        {
            printf("  ");
            for (int x = 0; x < 8; x++)
            {
                printf (" %d", x);
            }
            printf("\n");

            for (int y = 0; y < 8; y++)
            {
                printf (" %d", y);
                for (int x = 0; x < 8; x++)
                {
                    printf (" %c", f[OTHELLO_GET_PIECE(b[x][y])]);
                }
                printf ("\n");
            }
        }

        printf("{");
        for (int a=0;a<64;a++)
        {
            if (a) printf(", ");

            if ( OTHELLO_GET_PIECE(b[a%8][a/8]) == OTHELLO_WHITE )
            {
                printf("-1");
            }
            else if ( OTHELLO_GET_PIECE(b[a%8][a/8]) == OTHELLO_BLACK )
            {
                printf("1");
            }
            else
            {
                printf("0");
            }
        }
        printf("}\n");
    }

    void OthelloCommon::makeMove(OthelloMove &move,ushort b[8][8])
    {
        OTHELLO_SET_PIECE(b[move.position.x][move.position.y],move.color);

        if (move.position.x==0)
        {
            if (move.position.y==0)
            {
                //cout << "SETTING PIECE IN DANGER ZONE1!\n";
            }
            else if (move.position.y==1)
            {
                //cout << "SETTING PIECE IN DANGER ZONE2!\n";
            }
        }

        if (move.color==OTHELLO_BLACK)
        {
            int before = OTHELLO_GET_NUM_BLACK_PIECES(b);

            OTHELLO_INC_NUM_BLACK_PIECES(b);

            int after = OTHELLO_GET_NUM_BLACK_PIECES(b);

            if ((before+1)!=after)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("ERROR ON INC");
            }

        }
        else // (move.color==OTHELLO_WHITE)
        {
            int before = OTHELLO_GET_NUM_WHITE_PIECES(b);

            OTHELLO_INC_NUM_WHITE_PIECES(b);

            int after = OTHELLO_GET_NUM_WHITE_PIECES(b);

            if ((before+1)!=after)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("ERROR ON INC");
            }
        }

        //We use short so we can check out of bounds
        Vector2<short> lookLoc;

        for (int deltax=-1;deltax<=1;deltax++)
        {
            for (int deltay=-1;deltay<=1;deltay++)
            {
                if (!deltax && !deltay)
                    continue; //no movement

                lookLoc.x = move.position.x + deltax;
                lookLoc.y = move.position.y + deltay;

                for (;;)
                {
                    if ( !(OTHELLO_IS_IN_BOUNDS(lookLoc.x,lookLoc.y)) )
                    {
                        break; //off the board
                    }

                    int pieceOnSquare = OTHELLO_GET_PIECE(b[lookLoc.x][lookLoc.y]);

                    if (pieceOnSquare==OTHELLO_EMPTY)
                    {
                        //No possible captures on this axis
                        break;
                    }
                    else if (pieceOnSquare==move.color)
                    {
                        //You see your own color, time to capture some pieces (0 or more)
                        lookLoc.x -= deltax;
                        lookLoc.y -= deltay;

                        int pieceFlipIndex=move.getNumPiecesFlipped();

                        for (;lookLoc != move.position;)
                        {
                            move.piecesFlipped[pieceFlipIndex++] = lookLoc;

                            if (pieceFlipIndex==OTHELLO_MAX_FLIPS)
                            {
                                throw CREATE_LOCATEDEXCEPTION_INFO("TRIED TO FLIP TOO MANY PIECES IN ONE MOVE!");
                            }

                            OTHELLO_REVERSE_PIECE(b[lookLoc.x][lookLoc.y]);

                            if (move.color==OTHELLO_BLACK)
                            {
                                OTHELLO_INC_NUM_BLACK_PIECES(b);
                                OTHELLO_DEC_NUM_WHITE_PIECES(b);
                            }
                            else // (move.color==OTHELLO_WHITE)
                            {
                                OTHELLO_INC_NUM_WHITE_PIECES(b);
                                OTHELLO_DEC_NUM_BLACK_PIECES(b);
                            }

                            lookLoc.x -= deltax;
                            lookLoc.y -= deltay;
                        }

                        break;
                    }
                    else //pieceOnSquare == other color
                    {
                        //Keep going, maybe these are supposed to be captured.
                        lookLoc.x += deltax;
                        lookLoc.y += deltay;
                    }
                }
            }
        }
    }

    void OthelloCommon::reverseMove(OthelloMove &move,ushort b[8][8])
    {
        {
            int beforeWhite,beforeBlack,afterWhite,afterBlack;

            beforeBlack = OTHELLO_GET_NUM_BLACK_PIECES(b);
            beforeWhite = OTHELLO_GET_NUM_WHITE_PIECES(b);

            OTHELLO_SET_PIECE(b[move.position.x][move.position.y],0);

            afterBlack = OTHELLO_GET_NUM_BLACK_PIECES(b);
            afterWhite = OTHELLO_GET_NUM_WHITE_PIECES(b);

            if (move.color==OTHELLO_BLACK)
            {
                if (afterBlack!=(beforeBlack))
                {
                    throw CREATE_LOCATEDEXCEPTION_INFO("INVALID NUM");
                }
            }
            else
            {
                if (afterWhite!=(beforeWhite))
                {
                    throw CREATE_LOCATEDEXCEPTION_INFO("INVALID NUM");
                }
            }
        }

        if (move.position.x==0)
        {
            if (move.position.y==0)
            {
                //cout << "SETTING PIECE IN DANGER ZONE1!\n";
            }
            else if (move.position.y==1)
            {
                //cout << "SETTING PIECE IN DANGER ZONE2!\n";
            }
        }

        if (move.color==OTHELLO_BLACK)
        {
            int before = OTHELLO_GET_NUM_BLACK_PIECES(b);

            OTHELLO_DEC_NUM_BLACK_PIECES(b);

            int after = OTHELLO_GET_NUM_BLACK_PIECES(b);

            if ((before-1)!=after)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("ERROR ON INC");
            }
        }
        else // (move.color==OTHELLO_WHITE)
        {
            int before = OTHELLO_GET_NUM_WHITE_PIECES(b);

            OTHELLO_DEC_NUM_WHITE_PIECES(b);

            int after = OTHELLO_GET_NUM_WHITE_PIECES(b);

            if ((before-1)!=after)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("ERROR ON INC");
            }
        }

        int numFlips = move.getNumPiecesFlipped();

        for (int a=0;a<numFlips;a++)
        {
            OTHELLO_REVERSE_PIECE(b[move.piecesFlipped[a].x][move.piecesFlipped[a].y]);

            if (move.color==OTHELLO_BLACK)
            {
                {
                    int before = OTHELLO_GET_NUM_WHITE_PIECES(b);

                    OTHELLO_INC_NUM_WHITE_PIECES(b);

                    int after = OTHELLO_GET_NUM_WHITE_PIECES(b);

                    if ((before+1)!=after)
                    {
                        throw CREATE_LOCATEDEXCEPTION_INFO("ERROR ON INC");
                    }
                }

                OTHELLO_DEC_NUM_BLACK_PIECES(b);
            }
            else // (move.color==OTHELLO_WHITE)
            {
                OTHELLO_INC_NUM_BLACK_PIECES(b);
                OTHELLO_DEC_NUM_WHITE_PIECES(b);
            }
        }
    }

    bool OthelloCommon::hasMove(
        ushort b[8][8],
        int color,
        int x,
        int y
    )
    {
        if (OTHELLO_GET_PIECE(b[x][y])!=OTHELLO_EMPTY)
        {
            //place is taken
            return false;
        }

        //We use short so we can check out of bounds
        Vector2<short> lookLoc;

        for (int deltax=-1;deltax<=1;deltax++)
        {
            for (int deltay=-1;deltay<=1;deltay++)
            {
                if (!deltax && !deltay)
                    continue; //no movement

                lookLoc.x = x+deltax;
                lookLoc.y = y+deltay;

                bool jumpedOnce=false;

                for (;;)
                {
                    if ( !OTHELLO_IS_IN_BOUNDS(lookLoc.x,lookLoc.y) )
                    {
                        break; //off the board
                    }

                    int pieceOnSquare = OTHELLO_GET_PIECE(b[lookLoc.x][lookLoc.y]);

                    if (pieceOnSquare==OTHELLO_EMPTY)
                    {
                        //No possible captures on this axis
                        break;
                    }
                    else if (pieceOnSquare==color)
                    {
                        if (!jumpedOnce)
                        {
                            //You haven't jumped any pieces, not valid
                            break;
                        }

                        //You see your own color, you can jump, this is a valid move
                        return true;
                    }
                    else //pieceOnSquare == other color
                    {
                        //Keep going, maybe these are supposed to be captured.
                        jumpedOnce=true;
                        lookLoc.x += deltax;
                        lookLoc.y += deltay;
                    }
                }
            }
        }

        return false;
    }

    bool OthelloCommon::hasAnyMove(
        ushort b[8][8]
    )
    {
        for (int x=0;x<8;x++)
        {
            for (int y=0;y<8;y++)
            {
                if (hasMove(b,OTHELLO_BLACK,x,y))
                {
                    return true;
                }
                if (hasMove(b,OTHELLO_WHITE,x,y))
                {
                    return true;
                }
            }
        }

        return false;
    }

    int OthelloCommon::generateMoveList(ushort b[8][8],OthelloMove *moveList,int color)
    {
#if OTHELLO_COMMON_DEBUG
        cout << "Running generateMoveList\n";
#endif
        int numMoves=0;

        for (int x=0;x<8;x++)
        {
            for (int y=0;y<8;y++)
            {
                if (hasMove(b,color,x,y))
                {
                    moveList[numMoves++].reset( Vector2<uchar>(x,y), color );
                    if ( (moveList+numMoves) >= (totalMoveList+MAX_TOTAL_MOVES) )
                    {
                        throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: MAXED OUT MOVE LIST!");
                    }
                }
            }
        }

#if OTHELLO_COMMON_DEBUG
        cout << "Done Running generateMoveList\n";
#endif
        return numMoves;
    }

    int OthelloCommon::getWinner(ushort b[8][8])
    {
        int blackPieces = OTHELLO_GET_NUM_BLACK_PIECES(b);
        int whitePieces = OTHELLO_GET_NUM_WHITE_PIECES(b);

        if (!blackPieces)
        {
            return OTHELLO_WHITE;
        }
        else if (!whitePieces)
        {
            return OTHELLO_BLACK;
        }
        else if (hasAnyMove(b))
        {
            //Game is still ongoing
            return OTHELLO_END_UNKNOWN;
        }
        else if (blackPieces<whitePieces)
        {
            return OTHELLO_WHITE;
        }
        else if (blackPieces>whitePieces)
        {
            return OTHELLO_BLACK;
        }
        else
        {
            return OTHELLO_END_TIE;
        }
    }
}
