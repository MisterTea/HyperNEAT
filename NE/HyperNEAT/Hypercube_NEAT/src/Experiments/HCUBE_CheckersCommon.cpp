#include "HCUBE_Defines.h"

#include "Experiments/HCUBE_CheckersCommon.h"

#define CHECKERS_COMMON_DEBUG (0)

namespace HCUBE
{
    CheckersMove::CheckersMove()
        :
    from(255,255),
        to(255,255),
        pieceCaptured(255),
        nextJump(NULL),
        promoted(false)
    {}

    CheckersMove::CheckersMove(boost::shared_ptr<CheckersMovePool> _checkersMovePoolPtr)
        :
    from(255,255),
        to(255,255),
        pieceCaptured(255),
        nextJump(NULL),
        promoted(false),
        checkersMovePoolPtr(_checkersMovePoolPtr)
    {}

    CheckersMove::CheckersMove(Vector2<uchar> _from,Vector2<uchar> _to,boost::shared_ptr<CheckersMovePool> _checkersMovePoolPtr)
        :
    from(_from),
        to(_to),
        pieceCaptured(255),
        nextJump(NULL),
        promoted(false),
        checkersMovePoolPtr(_checkersMovePoolPtr)
    {}

    CheckersMove::CheckersMove(const CheckersMove &other)
        :
    from(other.from),
        to(other.to),
        pieceCaptured(other.pieceCaptured),
        promoted(other.promoted),
        checkersMovePoolPtr(other.checkersMovePoolPtr)
    {
        if (other.nextJump)
        {
            if(checkersMovePoolPtr)
            {
                nextJump = (CheckersMove*)checkersMovePoolPtr->malloc();
                new(nextJump) CheckersMove(*other.nextJump);
            }
            else
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
                nextJump = new CheckersMove(*other.nextJump);
            }
        }
        else
        {
            nextJump = NULL;
        }
    }

    const CheckersMove &CheckersMove::operator=(const CheckersMove &other)
    {
        if (this != &other)
        {
            from = other.from;
            to = other.to;
            pieceCaptured = other.pieceCaptured;
            promoted = other.promoted;

            if (nextJump)
            {
                if(checkersMovePoolPtr)
                {
                    nextJump->~CheckersMove();
                    checkersMovePoolPtr->free( nextJump );
                }
                else
                {
                    throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
                    delete nextJump;
                }
            }

            checkersMovePoolPtr = other.checkersMovePoolPtr;

            if (other.nextJump)
            {
                if(checkersMovePoolPtr)
                {
                    nextJump = (CheckersMove*)checkersMovePoolPtr->malloc();
                    new(nextJump) CheckersMove(*other.nextJump);
                }
                else
                {
                    throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
                    nextJump = new CheckersMove(*other.nextJump);
                }
            }
            else
            {
                nextJump = NULL;
            }

        }

        return *this;
    }

    CheckersMove::~CheckersMove()
    {
        if (nextJump)
        {
            if(checkersMovePoolPtr)
            {
                nextJump->~CheckersMove();
                checkersMovePoolPtr->free( nextJump );
            }
            else
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
                delete nextJump;
            }
        }
    }

    Vector2<uchar> CheckersMove::getFinalDestination()
    {
        if (nextJump)
        {
            return nextJump->getFinalDestination();
        }
        else
        {
            return to;
        }
    }

    void CheckersMove::addJump(Vector2<uchar> from,Vector2<uchar> to)
    {
        if (nextJump)
        {
            nextJump->addJump(from,to);
        }
        else
        {
            if(checkersMovePoolPtr)
            {
                nextJump = (CheckersMove*)checkersMovePoolPtr->malloc();
                new(nextJump) CheckersMove(from,to,checkersMovePoolPtr);
            }
            else
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
                nextJump = new CheckersMove(from,to,checkersMovePoolPtr);
            }
        }
    }

    void CheckersMove::addJump(const CheckersMove &move)
    {
        if (nextJump)
        {
            nextJump->addJump(move);
        }
        else
        {
            if(checkersMovePoolPtr)
            {
                nextJump = (CheckersMove*)checkersMovePoolPtr->malloc();
                new(nextJump) CheckersMove(move);
            }
            else
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
                nextJump = new CheckersMove(move);
            }
        }
    }

    CheckersCommon::CheckersCommon()
    {
        checkersMovePoolPtr = boost::shared_ptr<CheckersMovePool>(new CheckersMovePool(sizeof(CheckersMove)));
    }

    CheckersCommon::CheckersCommon(const CheckersCommon &other)
    {
        checkersMovePoolPtr = boost::shared_ptr<CheckersMovePool>(new CheckersMovePool(sizeof(CheckersMove)));
        memcpy(gameLog,other.gameLog,sizeof(uchar)*1024*8*8);
        memcpy(moveLog,other.moveLog,sizeof(uchar)*1024*64);
        moves = other.moves;
    }

    void CheckersCommon::printBoard(uchar b[8][8])
    {
        /*
        const char *f = "-    wb  WB      ";

        // rotate board, so the player sees it from her point of view
        int r = 1;

	printf("NUM PIECES: %d %d\n",(int)NUM_BLACK_PIECES(b),(int)NUM_WHITE_PIECES(b));
        printf ("\n");
        {
        for (int y = 0; y < 8; y++)
        {
        // board setup
        for (int x = 0; x < 8; x++)
        {
        if (r)
        printf (" %d", b[x][8 - y - 1]);
        else
        printf (" %d", b[8 - x - 1][y]);
        }

        printf("\t");
        for (int x = 0; x < 8; x++)
        {
        int tmpx = x;
        int tmpy = y;

        if (r)
        tmpy = 8 - y - 1;
        else
        tmpx = 8 - x - 1;

        if ( (tmpx+tmpy)%2 == 1)
        {
        printf("  ");
        }
        else
        {
        printf (" %c", f[b[tmpx][tmpy]]);
        }
        }

        printf ("\n");
        }
        }
        */

	cout << "NUM PIECES: " << int(NUM_BLACK_PIECES(b)) << " " << int(NUM_WHITE_PIECES(b)) << endl;
        const char *f = "-    wb  WB      ";
        bool error=false;
        for (int y=0;y<8;y++)
        {
            for (int x=0;x<8;x++)
            {
                if (x==0 && y==1)
                {
                    cout << "  ";
                    continue;
                }
                if (x==0 && y==3)
                {
                    cout << "  ";
                    continue;
                }

                cout << " " << f[b[x][y]];
                if (((y+x) % 2 == 1) && f[b[x][y]]!=' ')
                {
                    error=true;
                }
            }
            cout << endl;
        }
        cout << endl;
        if (error)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("ERROR! INVALID BOARD ^^^^!");
        }
    }

    void CheckersCommon::printBoard(uchar b[8][8],ofstream &stream)
    {
        /*
        const char *f = "-    wb  WB      ";

        // rotate board, so the player sees it from her point of view
        int r = 1;

        printf ("\n");
        {
        for (int y = 0; y < 8; y++)
        {
        // board setup
        for (int x = 0; x < 8; x++)
        {
        if (r)
        printf (" %d", b[x][8 - y - 1]);
        else
        printf (" %d", b[8 - x - 1][y]);
        }

        printf("\t");
        for (int x = 0; x < 8; x++)
        {
        int tmpx = x;
        int tmpy = y;

        if (r)
        tmpy = 8 - y - 1;
        else
        tmpx = 8 - x - 1;

        if ( (tmpx+tmpy)%2 == 1)
        {
        printf("  ");
        }
        else
        {
        printf (" %c", f[b[tmpx][tmpy]]);
        }
        }

        printf ("\n");
        }
        }
        */

        const char *f = "-    wb  WB      ";
        bool error=false;
        for (int y=0;y<8;y++)
        {
            for (int x=0;x<8;x++)
            {
                if (x==0 && y==1)
                {
                    stream << "  ";
                    continue;
                }
                if (x==0 && y==3)
                {
                    stream << "  ";
                    continue;
                }

                stream << " " << f[b[x][y]];
                if (((y+x) % 2 == 1) && f[b[x][y]]!=' ')
                {
                    error=true;
                }
            }
            stream << endl;
        }
        stream << endl;
        if (error)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("ERROR! INVALID BOARD ^^^^!");
        }
    }

    void CheckersCommon::printBoard(uchar b[8][8],ofstream &stream,NEAT::FastLayeredNetwork<CheckersNEATDatatype>* network,const NodeMap &nameLookup)
    {
        const char *f = "-    wb  WB      ";
        bool error=false;
        for (int y=0;y<8;y++)
        {
            for (int x=0;x<8;x++)
            {
                if (x==0 && y==1)
                {
                    stream << "  ";
                    continue;
                }
                if (x==0 && y==3)
                {
                    stream << "  ";
                    continue;
                }

                stream << " " << f[b[x][y]];
                if (((y+x) % 2 == 1) && f[b[x][y]]!=' ')
                {
                    error=true;
                }
            }
            stream << " *** ";
            for (int x=0;x<8;x++)
            {
                stream << setprecision(2) << setw(7) << network->getValue( Node(x,y,0) );
            }
            stream << " *** ";
            for (int x=0;x<8;x++)
            {
                stream << setprecision(2) << setw(7) << network->getValue( Node(x,y,1) );
            }
            if(y==3)
            {
                stream << " *** ";
                stream << setprecision(2) << setw(7) << network->getValue( Node(0,0,2) );
            }

            stream << endl;
        }
        stream << endl;
        if (error)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("ERROR! INVALID BOARD ^^^^!");
        }
    }

    void CheckersCommon::checkBoard(uchar b[8][8])
    {
        const char *f = "-    wb  WB      ";
        bool error=false;
        for (int y=0;y<8;y++)
        {
            for (int x=0;x<8;x++)
            {
                if (x==0 && y==1)
                    continue;
                if (x==0 && y==3)
                    continue;

                if (((y+x) % 2 == 1) && f[b[x][y]]!=' ')
                {
                    error=true;
                }
            }
        }
        if (error)
        {
            printBoard(b);
        }
    }

    void CheckersCommon::makeMove(CheckersMove &move,uchar b[8][8])
    {
#if CHECKERS_COMMON_DEBUG
        cout << "Running makeMove from " << move.from.x << ',' << move.from.y << " to " << move.to.x << ',' << move.to.y << endl;
        printBoard(b);
        CREATE_PAUSE("");
#endif
        checkBoard(b);

        bool isJump = (abs(move.to.x - move.from.x)>1);

        if (
            b[move.from.x][move.from.y] == 0 ||
            b[move.to.x][move.to.y] != 0 ||
            move.to.x >= 8 ||
            move.to.y >= 8 ||
            (move.to.x+move.to.y) %2 == 1
            )
        {
            cout << (int)move.from.x << ',' << (int)move.from.y << '\t' << (int)move.to.x << ',' << (int)move.to.y << endl;

            printBoard(b);
            throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: tried to move to a square which wasn't empty!");
        }

        b[move.to.x][move.to.y] = b[move.from.x][move.from.y];
        b[move.from.x][move.from.y] = 0;

        Vector2<uchar> mdpt;

        if (isJump)
        {
            mdpt = (move.to + move.from)/2;

            move.pieceCaptured = b[mdpt.x][mdpt.y];
            b[mdpt.x][mdpt.y] = 0;

            if ((move.pieceCaptured&BLACK)>0)
            {
                NUM_BLACK_PIECES(b) = NUM_BLACK_PIECES(b) - 1;
            }
            else //white was captured
            {
                NUM_WHITE_PIECES(b) = NUM_WHITE_PIECES(b) - 1;
            }
        }
        else
        {
            move.pieceCaptured = 0;
        }

        if ( b[move.to.x][move.to.y] & BLACK )
        {
            if (move.to.y == 7)
            {
                if ( b[move.to.x][move.to.y] & MAN )
                {
                    move.promoted = true;
                    b[move.to.x][move.to.y] &= (~MAN);
                    b[move.to.x][move.to.y] |= KING;
                }
                else
                {
                    move.promoted = false;
                }
            }
        }
        else if ( b[move.to.x][move.to.y] & WHITE )
        {
            if (move.to.y == 0)
            {
                if ( b[move.to.x][move.to.y] & MAN )
                {
                    move.promoted = true;
                    b[move.to.x][move.to.y] &= (~MAN);
                    b[move.to.x][move.to.y] |= KING;
                }
                else
                {
                    move.promoted = false;
                }
            }
        }

        if (move.nextJump)
        {
            makeMove(*(move.nextJump),b);
        }

        checkBoard(b);
#if CHECKERS_COMMON_DEBUG
        cout << "done!\n";
#endif
    }

    void CheckersCommon::reverseMove(CheckersMove &move,uchar b[8][8])
    {
#if CHECKERS_COMMON_DEBUG
        cout << "Running reverseMove from " << move.from.x << ',' << move.from.y << " to " << move.to.x << ',' << move.to.y << endl;
#endif
        checkBoard(b);

        if (move.nextJump)
        {
            reverseMove(*(move.nextJump),b);
        }

        bool isJump = (abs(move.to.x - move.from.x)>1);

        b[move.from.x][move.from.y] = b[move.to.x][move.to.y];
        b[move.to.x][move.to.y] = 0;

        if (isJump)
        {
            Vector2<uchar> mdpt = (move.to + move.from)/2;

            b[mdpt.x][mdpt.y] = move.pieceCaptured;

            if ((move.pieceCaptured&BLACK)>0)
            {
                NUM_BLACK_PIECES(b) = NUM_BLACK_PIECES(b) + 1;
            }
            else //white was captured
            {
                NUM_WHITE_PIECES(b) = NUM_WHITE_PIECES(b) + 1;
            }
        }

        if (move.promoted)
        {
            b[move.from.x][move.from.y] &= (~KING);
            b[move.from.x][move.from.y] |= MAN;
        }

        checkBoard(b);
#if CHECKERS_COMMON_DEBUG
        cout << "done!\n";
#endif
    }

#define IS_IN_BOUNDS(X,Y) ((X)>=0&&(Y)>=0&&(X)<8&&(Y)<8)

    //Check if we haven't found a jump yet, erase the moves and add the jump
#define CHECK_FIRST_JUMP if (!foundJump) { totalMoveList.erase(totalMoveList.begin()+moveBeginIndex,totalMoveList.end()); numMoves=0; foundJump=true; }

#define MAX_MOVES (128)

    bool CheckersCommon::hasJump(
        uchar b[8][8],
        int color,
        int x,
        int y,
        int deltax,
        int deltay
        )
    {
#if CHECKERS_COMMON_DEBUG
        cout << "Running hasJump\n";
#endif
        if (IS_IN_BOUNDS(x+deltax,y+deltay))
        {
            if (b[x+deltax][y+deltay]&(MAN|KING))
            {
                //A piece exists in the square we could move to, can we jump it?

                if (
                    IS_IN_BOUNDS(x+deltax*2,y+deltay*2) && //must be in bounds
                    (!(b[x+deltax][y+deltay]&color)) && //can't jump your own color
                    (!(b[x+deltax*2][y+deltay*2]&(MAN|KING))) //must have an empty square behind
                    )
                {
                    return true;
                }
            }
        }
#if CHECKERS_COMMON_DEBUG
        cout << "Done Running hasJump\n";
#endif

        return false;
    }

    /* tryMoveJumps:
    returns true if more jumps exist and false otherwise.
    If returns true, that means that you don't need to worry about adding the current jump
    */
    bool CheckersCommon::tryMoreJumps(
        vector<CheckersMove> &totalMoveList,
        int &numMoves,
        CheckersMove &moveInProgress,
        int color,
        uchar b[8][8]
    )
    {
        int x = moveInProgress.getFinalDestination().x;
        int y = moveInProgress.getFinalDestination().y;
#if CHECKERS_COMMON_DEBUG
        cout << "Running tryMoreJumps on location " << x << ',' << y << endl;
#endif

        bool hasMoreJumps=false;

        if ( (b[x][y]&color) )
        {
            if ( (b[x][y]&(BLACK|KING)) )
            {
                //Black moves in the +y direction

                for (int deltax=-1;deltax<=1;deltax+=2)
                {
                    if (
                        hasJump(b,color,x,y,deltax,1)
                        &&
                        (
                        moveInProgress.from.x != x+deltax*2 ||
                        moveInProgress.from.y != y+2
                        //This conditional logic is there to prevent making jumps which
                        //contain the starting position.  This is because it tends to bomb on
                        //the current checkers implementation
                        )
                        )
                    {
                        hasMoreJumps=true;

                        //A new jump exists.  Try the jump
                        CheckersMove jump = CheckersMove(Vector2<uchar>(x,y),Vector2<uchar>(x+deltax*2,y+2),checkersMovePoolPtr);

                        CheckersMove fullMove = moveInProgress;
                        fullMove.addJump(jump);

                        makeMove(jump,b);
                        bool moreJumps=false;
                        if (!jump.promoted)
                        {
                            //"A piece that has just kinged, cannot continue jumping pieces, until the next move."
                            moreJumps = tryMoreJumps(totalMoveList,numMoves,fullMove,color,b);
                        }
                        reverseMove(jump,b);

                        if (!moreJumps)
                        {
                            //Add the current move if there's no more jumps
                            //(otherwise you shuold have added the
                            //jumps and this move is not legal)
                            //CheckersMove tmpMove = moveInProgress;
                            //tmpMove.addJump(Vector2<uchar>(x,y),Vector2<uchar>(x+deltax*2,y+2));
                            totalMoveList.push_back(fullMove);
                            numMoves++;
                        }
                    }
                }
            }

            if ( (b[x][y]&(WHITE|KING)) )
            {
                //White moves in the -y direction

                for (int deltax=-1;deltax<=1;deltax+=2)
                {
                    if (
                        hasJump(b,color,x,y,deltax,-1)
                        &&
                        (
                        moveInProgress.from.x != x+deltax*2 ||
                        moveInProgress.from.y != y-2
                        //This conditional logic is there to prevent making jumps which
                        //contain the starting position.  This is because it tends to bomb on
                        //the current checkers implementation
                        )
                        )
                    {
                        hasMoreJumps=true;

                        //A new jump exists.  Try the jump
                        CheckersMove jump = CheckersMove(Vector2<uchar>(x,y),Vector2<uchar>(x+deltax*2,y-2),checkersMovePoolPtr);

                        CheckersMove fullMove = moveInProgress;
                        fullMove.addJump(jump);

                        makeMove(jump,b);
                        bool moreJumps=false;
                        if (!jump.promoted)
                        {
                            //"A piece that has just kinged, cannot continue jumping pieces, until the next move."
                            moreJumps = tryMoreJumps(totalMoveList,numMoves,fullMove,color,b);
                        }
                        reverseMove(jump,b);

                        if (!moreJumps)
                        {
                            //Add the current move if there's no more jumps
                            //(otherwise you shuold have added the
                            //jumps and this move is not legal)
                            //CheckersMove tmpMove = moveInProgress;
                            //tmpMove.addJump(Vector2<uchar>(x,y),Vector2<uchar>(x+deltax*2,y-2));

                            totalMoveList.push_back(fullMove);
                            numMoves++;
                        }
                    }
                }
            }
        }
        else
        {
            CREATE_PAUSE("ERROR!");
            throw CREATE_LOCATEDEXCEPTION_INFO("Tried to add more jumps but there wasn't a piece!");
        }

#if CHECKERS_COMMON_DEBUG
        cout << "Done Running tryMoreJumps\n";
#endif
        return hasMoreJumps;
    }

    void CheckersCommon::tryMove(
        vector<CheckersMove> &totalMoveList,
        int moveBeginIndex,
        uchar b[8][8],
        int &numMoves,
        bool &foundJump,
        int color,
        int x,
        int y,
        int deltax,
        int deltay
        )
    {
#if CHECKERS_COMMON_DEBUG
        cout << "Running tryMove\n";
#endif
        if (IS_IN_BOUNDS(x+deltax,y+deltay))
        {
            if (b[x+deltax][y+deltay]&(MAN|KING))
            {
                //A piece exists in the square we could move to, can we jump it?

                if (
                    IS_IN_BOUNDS(x+deltax*2,y+deltay*2) && //must be in bounds
                    (!(b[x+deltax][y+deltay]&color)) && //can't jump your own color
                    (!(b[x+deltax*2][y+deltay*2]&(MAN|KING))) //must have an empty square behind
                    )
                {
                    CHECK_FIRST_JUMP
                        CheckersMove jump = CheckersMove(Vector2<uchar>(x,y),Vector2<uchar>(x+deltax*2,y+deltay*2),checkersMovePoolPtr);

                    bool foundMoreJumps=false;
                    //At this point, we have found a jump, try to find more jumps
                    makeMove(jump,b);
                    if (!jump.promoted)
                    {
                        //"A piece that has just kinged, cannot continue jumping pieces, until the next move."
                        foundMoreJumps = tryMoreJumps(totalMoveList,numMoves,jump,color,b);
                    }
                    reverseMove(jump,b);

                    if (!foundMoreJumps)
                    {
                        //No multi-jumps found, just add the one jump
                        totalMoveList.push_back(jump);
                        numMoves++;
                    }
                }
            }
            else
            {
                //No piece exists, a move is possible
                if (!foundJump)
                {
                    //Only try to move if you haven't yet found a possible capture
                    totalMoveList.push_back(CheckersMove(Vector2<uchar>(x,y),Vector2<uchar>(x+deltax,y+deltay),checkersMovePoolPtr) );
                    numMoves++;
                }
            }
        }

        if (numMoves==MAX_MOVES)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("Too many possible moves for a given board state! Oh shiz!");
        }
#if CHECKERS_COMMON_DEBUG
        cout << "Done Running tryMove\n";
#endif
    }

    int CheckersCommon::generateMoveList(
        vector<CheckersMove> &totalMoveList,
        int moveBeginIndex,
        uchar b[8][8],
        int color,
        bool &foundJump
        )
    {
#if CHECKERS_COMMON_DEBUG
        cout << "Running generateMoveList\n";
#endif
        int numMoves=0;

        foundJump=false;

        for (int x=0;x<8;x++)
        {
            for (int y=(x%2);y<8;y+=2)
            {
                if ( (b[x][y]&color) )
                {
                    if ( (b[x][y]&(BLACK|KING)) )
                    {
                        //Black moves in the +y direction
                        tryMove(totalMoveList,moveBeginIndex,b,numMoves,foundJump,color,x,y,1,1);
                        tryMove(totalMoveList,moveBeginIndex,b,numMoves,foundJump,color,x,y,-1,1);
                    }

                    if ( (b[x][y]&(WHITE|KING)) )
                    {
                        //White moves in the -y direction
                        tryMove(totalMoveList,moveBeginIndex,b,numMoves,foundJump,color,x,y,1,-1);
                        tryMove(totalMoveList,moveBeginIndex,b,numMoves,foundJump,color,x,y,-1,-1);
                    }
                }
            }
        }

#if CHECKERS_COMMON_DEBUG
        cout << "Done Running generateMoveList\n";
#endif
        return numMoves;
    }

    bool CheckersCommon::hasMove(
        uchar b[8][8],
        int color,
        int x,
        int y,
        int deltax,
        int deltay
        )
    {
        if (IS_IN_BOUNDS(x+deltax,y+deltay))
        {
            if (b[x+deltax][y+deltay]&(MAN|KING))
            {
                //A piece exists in the square we could move to, can we jump it?

                if (
                    IS_IN_BOUNDS(x+deltax*2,y+deltay*2) && //must be in bounds
                    (!(b[x+deltax][y+deltay]&color)) && //can't jump your own color
                    (!(b[x+deltax*2][y+deltay*2]&(MAN|KING))) //must have an empty square behind
                    )
                {
                    return true;
                }
            }
            else
            {
                //No piece exists, a move is possible
                return true;
            }
        }

        return false;
    }

    bool CheckersCommon::hasMove(
        uchar fromB[8][8],
        int color,
        uchar toB[8][8]
        )
    {
		JGTL::Vector2<int> moveFrom,moveTo;

        for (int x=0;x<8;x++)
        {
            for (int y=(x%2);y<8;y+=2)
            {
                if ( (fromB[x][y]&color)>0 && (toB[x][y]&color)==0 )
                {
					moveFrom.x = x;
					moveFrom.y = y;
				}
                else if ( (fromB[x][y]&color)==0 && (toB[x][y]&color)>0 )
                {
					moveTo.x = x;
					moveTo.y = y;
				}
			}
		}

		JGTL::Vector2<int> deltaMove(moveTo.x-moveFrom.x,moveTo.y-moveFrom.y);

		return hasMove(fromB,color,moveFrom.x,moveFrom.y,deltaMove.x,deltaMove.y);
    }

	bool CheckersCommon::hasAnyMove(uchar b[8][8],int color)
    {
        for (int x=0;x<8;x++)
        {
            for (int y=(x%2);y<8;y+=2)
            {
                if ( (b[x][y]&color) )
                {
                    if ( (b[x][y]&(BLACK|KING)) )
                    {
                        //Black regular pieces move in the +y direction
                        if (hasMove(b,color,x,y,1,1)) return true;
                        if (hasMove(b,color,x,y,-1,1)) return true;
                    }

                    if ( (b[x][y]&(WHITE|KING)) )
                    {
                        //White regular pieces move in the -y direction
                        if (hasMove(b,color,x,y,1,-1)) return true;
                        if (hasMove(b,color,x,y,-1,-1)) return true;
                    }
                }
            }
        }
        return false;
    }

    int CheckersCommon::getWinner(uchar b[8][8])
    {
        if (!(NUM_WHITE_PIECES(b)))
        {
            return BLACK;
        }
        else if (!(NUM_BLACK_PIECES(b)))
        {
            return WHITE;
        }

        return -1;
    }

    int CheckersCommon::getWinner(uchar b[8][8],int colorToMove)
    {
        int pieceWin = getWinner(b);

        if (pieceWin!=-1)
            return pieceWin;

        if (colorToMove == BLACK)
        {
            if (!hasAnyMove(b,BLACK))
            {
                return WHITE;
            }
        }
        else
        {
            if (!hasAnyMove(b,WHITE))
            {
                return BLACK;
            }
        }

        return -1;
    }

    void CheckersCommon::countPieces(uchar b[8][8],int &whiteMen,int &blackMen,int &whiteKings,int &blackKings)
    {
        whiteMen = blackMen = whiteKings = blackKings = 0;

        for (int x=0;x<8;x++)
        {
            for (int y=(x%2);y<8;y+=2)
            {
                if ( b[x][y]&BLACK )
                {
                    if (b[x][y]&KING)
                    {
                        blackKings++;
                    }
                    else
                    {
                        blackMen++;
                    }
                }

                if ( b[x][y]&WHITE )
                {
                    if (b[x][y]&KING)
                    {
                        whiteKings++;
                    }
                    else
                    {
                        whiteMen++;
                    }
                }
            }
        }
    }

    void CheckersCommon::resetBoard(uchar b[8][8])
    {
        for (int y = 0; y < 8; y++)
            for (int x = 0; x < 8; x++)
                if ((x + y) % 2 == 0)
                    b[x][y] = 0;
                else
                    b[x][y] = FREE;

        for (int x=0;x<8;x+=2)
        {
            for (int y=0;y<3;y++)
            {
                b[x + (y%2)][y] = BLACK|MAN;
            }
        }

        for (int x=0;x<8;x+=2)
        {
            for (int y=7;y>4;y--)
            {
                b[x + (y%2)][y] = WHITE|MAN;
            }
        }

        NUM_BLACK_PIECES(b) = 12;
        NUM_WHITE_PIECES(b) = 12;
    }

    CheckersNEATDatatype CheckersCommon::convertOutputToWeight(CheckersNEATDatatype output)
    {
        if (fabs(output)>0.2)
        {
            if (output>0.0)
                output = ( ((output-0.2)/0.8)*3.0 );
            else
                output = ( ((output+0.2)/0.8)*3.0 );
        }
        else
        {
            output = (0.0);
        }
        return output;
    }

    int CheckersCommon::gridToIndex(int x,int y)
    {
#if 0 //old way
        /*
        BLACK
        0   1   2   3
        4   5   6   7
        8   9  10  11
        12  13  14  15
        16  17  18  19
        20  21  22  23
        24  25  26  27
        28  29  30  31
        WHITE
        */

        int index = (y*4)+(x/2);
        return index;
#endif

        //new way
        /*
        BLACK
        4   3   2   1
        8   7   6   5
        12  11  10  9
        16  15  14  13
        20  19  18  17
        24  23  22  21
        28  27  26  25
        32  31  30  29
        WHITE
        */

        int index = (y*4)+((7-x)/2) + 1;
        return index;
    }

    string CheckersCommon::gameLogToPDN()
    {
        ostringstream output;

        output << "[Event \"HyperNEAT (Black) vs. Cake (White)\"] " << endl;

        for(int a=0;a<moves;a++)
        {
            if(a%2==0)
            {
                output << ((a/2)+1) << ". ";
            }

            output << moveLog[a] << " ";
        }
        output << "*";

        return output.str();
    }

    void bitboardtoboard(POSITION position,int b[8][8])
    {
        /* return a board from a bitboard */
        int i,board[32];

        for(i=0;i<32;i++)
        {
            if (position.bm & (1<<i))
                board[i]=(BLACK|MAN);
            if (position.bk & (1<<i))
                board[i]=(BLACK|KING);
            if (position.wm & (1<<i))
                board[i]=(WHITE|MAN);
            if (position.wk & (1<<i))
                board[i]=(WHITE|KING);
            if ( (~(position.bm|position.bk|position.wm|position.wk)) & (1<<i))
                board[i]=0;
        }
        /* return the board */
        b[0][0]=board[0];b[2][0]=board[1];b[4][0]=board[2];b[6][0]=board[3];
        b[1][1]=board[4];b[3][1]=board[5];b[5][1]=board[6];b[7][1]=board[7];
        b[0][2]=board[8];b[2][2]=board[9];b[4][2]=board[10];b[6][2]=board[11];
        b[1][3]=board[12];b[3][3]=board[13];b[5][3]=board[14];b[7][3]=board[15];
        b[0][4]=board[16];b[2][4]=board[17];b[4][4]=board[18];b[6][4]=board[19];
        b[1][5]=board[20];b[3][5]=board[21];b[5][5]=board[22];b[7][5]=board[23];
        b[0][6]=board[24];b[2][6]=board[25];b[4][6]=board[26];b[6][6]=board[27];
        b[1][7]=board[28];b[3][7]=board[29];b[5][7]=board[30];b[7][7]=board[31];
    }

    void ucharbitboardtoboard(POSITION position,unsigned char b[8][8])
    {
        /* return a board from a bitboard */
        int i,board[32];

        int countWhite=0;
        int countBlack=0;

        for(i=0;i<32;i++)
        {
            if (position.bm & (1<<i))
            {
                board[i]=(BLACK|MAN);
                countBlack++;
            }
            if (position.bk & (1<<i))
            {
                board[i]=(BLACK|KING);
                countBlack++;
            }
            if (position.wm & (1<<i))
            {
                board[i]=(WHITE|MAN);
                countWhite++;
            }
            if (position.wk & (1<<i))
            {
                board[i]=(WHITE|KING);
                countWhite++;
            }
            if ( (~(position.bm|position.bk|position.wm|position.wk)) & (1<<i))
                board[i]=0;
        }
        /* return the board */
        b[0][0]=board[0];b[2][0]=board[1];b[4][0]=board[2];b[6][0]=board[3];
        b[1][1]=board[4];b[3][1]=board[5];b[5][1]=board[6];b[7][1]=board[7];
        b[0][2]=board[8];b[2][2]=board[9];b[4][2]=board[10];b[6][2]=board[11];
        b[1][3]=board[12];b[3][3]=board[13];b[5][3]=board[14];b[7][3]=board[15];
        b[0][4]=board[16];b[2][4]=board[17];b[4][4]=board[18];b[6][4]=board[19];
        b[1][5]=board[20];b[3][5]=board[21];b[5][5]=board[22];b[7][5]=board[23];
        b[0][6]=board[24];b[2][6]=board[25];b[4][6]=board[26];b[6][6]=board[27];
        b[1][7]=board[28];b[3][7]=board[29];b[5][7]=board[30];b[7][7]=board[31];

        NUM_BLACK_PIECES(b) = countBlack;
        NUM_WHITE_PIECES(b) = countWhite;
    }

    void boardtobitboard(int b[8][8], POSITION *position)
    {
        /* initialize bitboard */
        int i,board[32];
        /*
        WHITE
        28  29  30  31
        24  25  26  27
        20  21  22  23
        16  17  18  19
        12  13  14  15
        8   9  10  11
        4   5   6   7
        0   1   2   3
        BLACK
        */
        board[0]=b[0][0];board[1]=b[2][0];board[2]=b[4][0];board[3]=b[6][0];
        board[4]=b[1][1];board[5]=b[3][1];board[6]=b[5][1];board[7]=b[7][1];
        board[8]=b[0][2];board[9]=b[2][2];board[10]=b[4][2];board[11]=b[6][2];
        board[12]=b[1][3];board[13]=b[3][3];board[14]=b[5][3];board[15]=b[7][3];
        board[16]=b[0][4];board[17]=b[2][4];board[18]=b[4][4];board[19]=b[6][4];
        board[20]=b[1][5];board[21]=b[3][5];board[22]=b[5][5];board[23]=b[7][5];
        board[24]=b[0][6];board[25]=b[2][6];board[26]=b[4][6];board[27]=b[6][6];
        board[28]=b[1][7];board[29]=b[3][7];board[30]=b[5][7];board[31]=b[7][7];

        (*position).bm=0;
        (*position).bk=0;
        (*position).wm=0;
        (*position).wk=0;

        for(i=0;i<32;i++)
        {
            switch (board[i])
            {
            case BLACK|MAN:
                (*position).bm=(*position).bm|(1<<i);
                break;
            case BLACK|KING:
                (*position).bk=(*position).bk|(1<<i);
                break;
            case WHITE|MAN:
                (*position).wm=(*position).wm|(1<<i);
                break;
            case WHITE|KING:
                (*position).wk=(*position).wk|(1<<i);
                break;
            }
        }
    }

    void ucharboardtobitboard(unsigned char b[8][8], POSITION *position)
    {
        /* initialize bitboard */
        int i,board[32];
        /*
        WHITE
        28  29  30  31
        24  25  26  27
        20  21  22  23
        16  17  18  19
        12  13  14  15
        8   9  10  11
        4   5   6   7
        0   1   2   3
        BLACK
        */
        board[0]=b[0][0];board[1]=b[2][0];board[2]=b[4][0];board[3]=b[6][0];
        board[4]=b[1][1];board[5]=b[3][1];board[6]=b[5][1];board[7]=b[7][1];
        board[8]=b[0][2];board[9]=b[2][2];board[10]=b[4][2];board[11]=b[6][2];
        board[12]=b[1][3];board[13]=b[3][3];board[14]=b[5][3];board[15]=b[7][3];
        board[16]=b[0][4];board[17]=b[2][4];board[18]=b[4][4];board[19]=b[6][4];
        board[20]=b[1][5];board[21]=b[3][5];board[22]=b[5][5];board[23]=b[7][5];
        board[24]=b[0][6];board[25]=b[2][6];board[26]=b[4][6];board[27]=b[6][6];
        board[28]=b[1][7];board[29]=b[3][7];board[30]=b[5][7];board[31]=b[7][7];

        (*position).bm=0;
        (*position).bk=0;
        (*position).wm=0;
        (*position).wk=0;

        for(i=0;i<32;i++)
        {
            switch (board[i])
            {
            case BLACK|MAN:
                (*position).bm=(*position).bm|(1<<i);
                break;
            case BLACK|KING:
                (*position).bk=(*position).bk|(1<<i);
                break;
            case WHITE|MAN:
                (*position).wm=(*position).wm|(1<<i);
                break;
            case WHITE|KING:
                (*position).wk=(*position).wk|(1<<i);
                break;
            }
        }
    }
}
