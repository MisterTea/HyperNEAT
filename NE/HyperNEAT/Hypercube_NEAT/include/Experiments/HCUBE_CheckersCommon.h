#ifndef HCUBE_CHECKERSCOMMON_H_INCLUDED
#define HCUBE_CHECKERSCOMMON_H_INCLUDED

#include "HCUBE_Defines.h"

//#include "Experiments/HCUBE_cliche.h"
//#include "Experiments/HCUBE_simplech.h"

extern "C"
{
    //Cliche
    #include "simplech.h"
}

//Cake
#include "structs.h"
#include "consts.h"
#include "cakepp.h"
#include "move_gen.h"

#define NUM_BLACK_PIECES(ARRAY) (ARRAY[0][1])

#define NUM_WHITE_PIECES(ARRAY) (ARRAY[0][3])

#define MAX_TOTAL_MOVES (262144)

/*
 * The data type to be used in the checkers experiments.
 * -NOTE- This should be a float for speed reasons.  It should only
 * be a double if you are evaluating on a different computer than you evolved on
 *
 * For more information, see the README.txt
 */
typedef float CheckersNEATDatatype;

namespace HCUBE
{
    typedef boost::pool<> CheckersMovePool;

    class CheckersMove
    {
    public:
        Vector2<uchar> from,to;
        uchar pieceCaptured;
        CheckersMove *nextJump;
        bool promoted;
        boost::shared_ptr<CheckersMovePool> checkersMovePoolPtr;

        CheckersMove();

        CheckersMove(boost::shared_ptr<CheckersMovePool> _checkersMovePoolPtr);

        CheckersMove(Vector2<uchar> _from,Vector2<uchar> _to,boost::shared_ptr<CheckersMovePool> _checkersMovePoolPtr);

        CheckersMove(const CheckersMove &other);

        const CheckersMove &operator=(const CheckersMove &other);

        virtual ~CheckersMove();

        Vector2<uchar> getFinalDestination();

        void addJump(Vector2<uchar> from,Vector2<uchar> to);

        void addJump(const CheckersMove &move);

        bool operator==(const CheckersMove &move)
        {
            bool retval =
                (
                    from == move.from &&
                    to == move.to
                );

            if (!retval)
                return false;

            if (
                (nextJump && !move.nextJump) ||
                (!nextJump && move.nextJump)
            )
            {
                return false;
            }

            if (nextJump)
            {
                return (*nextJump) == *(move.nextJump);
            }
            else
            {
                return true;
            }
        }
    };

    typedef vector<CheckersMove>::iterator MoveListIterator;

    class CheckersCommon
    {
	protected:
        uchar gameLog[1024][8][8];
		uchar moveLog[1024][64];
        int moves;
        boost::shared_ptr<CheckersMovePool> checkersMovePoolPtr;

    public:
        CheckersCommon();

        CheckersCommon(const CheckersCommon &other);

        void printBoard(uchar b[8][8]);

        void printBoard(uchar b[8][8],ofstream &stream);

        void printBoard(uchar b[8][8],ofstream &stream,NEAT::FastLayeredNetwork<CheckersNEATDatatype>* network,const NodeMap &nameLookup);

        void checkBoard(uchar b[8][8]);

        void makeMove(CheckersMove &move,uchar b[8][8]);

        void reverseMove(CheckersMove &move,uchar b[8][8]);

        bool hasJump(
            uchar b[8][8],
            int color,
            int x,
            int y,
            int deltax,
            int deltay
        );

        bool tryMoreJumps(
            vector<CheckersMove> &totalMoveList,
            int &numMoves,
            CheckersMove &moveInProgress,
            int color,
            uchar b[8][8]
        );


        void tryMove(
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
        );

        int generateMoveList(
            vector<CheckersMove> &totalMoveList,
            int moveBeginIndex,
            uchar b[8][8],
            int color,
            bool &foundJump
        );

        bool hasMove(
            uchar b[8][8],
            int color,
            int x,
            int y,
            int deltax,
            int deltay
        );

        bool hasMove(
            uchar fromB[8][8],
            int color,
            uchar toB[8][8]
        );

        bool hasAnyMove(uchar b[8][8],int color);

        int getWinner(uchar b[8][8]);

        int getWinner(uchar b[8][8],int colorToMove);

        void countPieces(uchar b[8][8],int &whiteMen,int &blackMen,int &whiteKings,int &blackKings);

        void resetBoard(uchar b[8][8]);

        CheckersNEATDatatype convertOutputToWeight(CheckersNEATDatatype output);

        int gridToIndex(const JGTL::Vector2<uchar> &pos)
		{
			return gridToIndex(pos.x,pos.y);
		}

		int gridToIndex(int x,int y);

		string gameLogToPDN();
	};

    class CheckersBoardState
    {
    public:
        uchar b[8][8];
        uchar playerToMove;

        CheckersBoardState()
                :
                playerToMove(255)
        {
            memset(b,0,sizeof(uchar)*8*8);
        }

        CheckersBoardState(uchar _b[8][8],uchar _playerToMove)
                :
                playerToMove(_playerToMove)
        {
            memcpy(b,_b,sizeof(uchar)*8*8);
        }

        void set(uchar _b[8][8],uchar _playerToMove)
        {
            playerToMove = _playerToMove;
        }

        bool operator <(const CheckersBoardState &bs2) const
        {
            if (playerToMove<bs2.playerToMove)
            {
                return true;
            }
            else if (playerToMove==bs2.playerToMove)
            {
                return (memcmp(b,bs2.b,8*8*sizeof(uchar))==-1);
            }

            return false;
        }

        bool operator ==(const CheckersBoardState &bs2) const
        {
            if (playerToMove != bs2.playerToMove)
            {
                return false;
            }

            return memcmp(b,bs2.b,sizeof(uchar)*8*8)==0;
        }

        size_t operator()() const
        {
            size_t retval=0;

            for (int y=2;y<6;y++)
            {
                for (int x=(y%2);x<8;x+=2)
                {
                    retval <<= 1;
                    if (b[x][y]&WHITE)
                    {
                        retval++;
                    }

                    retval <<= 1;
                    if (b[x][y]&BLACK)
                    {
                        retval++;
                    }
                }
            }
            return retval;
        }

        ushort getShortHash() const
        {
            int retval=0;

            for (int y=2;y<6;y++)
            {
                for (int x=(y%2+2);x<6;x+=2)
                {
                    retval <<= 1;
                    if (b[x][y]&WHITE)
                    {
                        retval++;
                    }

                    retval <<= 1;
                    if (b[x][y]&BLACK)
                    {
                        retval++;
                    }
                }
            }

            if(retval<0 || retval>=65536)
            {
                CREATE_PAUSE("ERROR CALCULATING HASH FUNCTION!!!");
            }

            return (ushort)retval;
        }
    };

    class CheckersBoardStateData
    {
    public:
        int depth;
        int maxDepth;
        bool foundJump;
    protected:
        int numMoves;
        CheckersNEATDatatype scores[32];
        CheckersMove moves[32];

    public:
        CheckersBoardStateData()
                :
                numMoves(0)
        {
        }

        CheckersNEATDatatype getHighestScore()
        {
            if (!numMoves)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: NO MOVES!");
            }

            return scores[numMoves-1];
        }

        CheckersNEATDatatype getLowestScore()
        {
            if (!numMoves)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: NO MOVES!");
            }

            return scores[0];
        }

        CheckersNEATDatatype getScore(int index)
        {
            return scores[index];
        }

        const CheckersMove &getMove(int index)
        {
            if (index>=32)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("ERROR!");
            }

            return moves[index];
        }

        int getNumMoves()
        {
            return numMoves;
        }

        void insertMove(CheckersNEATDatatype score,const CheckersMove &move)
        {
            if (numMoves==32)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: NO ROOM FOR MOVE!");
            }

            for (int a=0;a<32;a++)
            {
                if (a==numMoves || scores[a]>score)
                {
                    for (int b=numMoves-1;b>=a;b--)
                    {
                        scores[b+1] = scores[b];
                        moves[b+1] = moves[b];
                    }

                    scores[a] = score;
                    moves[a] = move;
                    numMoves++;
                    return;
                }
            }
        }
    };

    class CheckersCachedBoard
    {
    public:
        uchar b[8][8];

        CheckersCachedBoard()
        {}

        CheckersCachedBoard(uchar _b[8][8])
        {
            memcpy(b,_b,sizeof(uchar)*8*8);
        }

        void loadBoard(uchar _b[8][8])
        {
            memcpy(b,_b,sizeof(uchar)*8*8);
        }

        bool operator <(const CheckersCachedBoard &bs2) const
        {
            return (memcmp(b,bs2.b,8*8*sizeof(uchar))==-1);
        }

        bool operator ==(const CheckersCachedBoard &bs2) const
        {
            return (memcmp(b,bs2.b,8*8*sizeof(uchar))==0);
        }

        size_t operator()() const
        {
            size_t retval=0;

            for (int y=2;y<6;y++)
            {
                for (int x=(y%2);x<8;x+=2)
                {
                    retval <<= 1;
                    if (b[x][y]&WHITE)
                    {
                        retval++;
                    }

                    retval <<= 1;
                    if (b[x][y]&BLACK)
                    {
                        retval++;
                    }
                }
            }

            return retval;
        }

        ushort getShortHash() const
        {
            ushort retval=0;

            for (int y=2;y<6;y++)
            {
                for (int x=(y%2+2);x<6;x+=2)
                {
                    retval <<= 1;
                    if (b[x][y]&WHITE)
                    {
                        retval++;
                    }

                    retval <<= 1;
                    if (b[x][y]&BLACK)
                    {
                        retval++;
                    }
                }
            }

            return retval;
        }

    };

    /*
    //namespace std
    //{
    template<CheckersCachedBoard>
    struct hash : unary_function<CheckersCachedBoard, size_t>
    {
    // Define the hash function. We'll just stub it out here.
    size_t operator()( const CheckersCachedBoard &board ) const
    {
    size_t retval=0;

    for(int y=2;y<6;y++)
    {
    for(int x=(y%2);x<8;x+=2)
    {
    retval <<= 1;
    if(board.b[x][y]&WHITE)
    {
    retval++;
    }

    retval <<= 1;
    if(board.b[x][y]&BLACK)
    {
    retval++;
    }
    }
    }
    return retval;
    }
    };
    //}
    */

//namespace HCUBE
//{
    typedef pair<CheckersCachedBoard,CheckersNEATDatatype> BoardCachePair;
    typedef vector<BoardCachePair> BoardCacheList;

    typedef pair<CheckersBoardState,CheckersBoardStateData> BoardStatePair;
    typedef vector<BoardStatePair> BoardStateList;

    class CheckersStats
    {
    public:
        int wins,losses,ties;


        CheckersStats()
                :
                wins(0),
                losses(0),
                ties(0)
        {
        }

        virtual ~CheckersStats()
        {}

		CheckersStats(const string &str)
		{
			istringstream istr(str);

			istr >> wins >> losses >> ties;
		}

        virtual string toString() const
        {
            std::ostringstream oss;

            oss << wins << ' ';
            oss << losses << ' ';
            oss << ties;

            return oss.str();
        }

        virtual string toMultiLineString() const
        {
            return toString();
        }

        virtual string summaryHeaderToString() const
        {
            std::ostringstream oss;

            oss << "Wins: ";
            oss << " Losses: ";
            oss << " Ties: ";

            return oss.str();
        }

        virtual string summaryToString() const
        {
            return toString();
        }

		string gameLogToPDN();
    };

    void boardtobitboard(int b[8][8], POSITION *position);
    void ucharboardtobitboard(unsigned char b[8][8], POSITION *position);

    void bitboardtoboard(POSITION position,int b[8][8]);
    void ucharbitboardtoboard(POSITION position,unsigned char b[8][8]);
}

#endif // HCUBE_CHECKERSCOMMON_H_INCLUDED
