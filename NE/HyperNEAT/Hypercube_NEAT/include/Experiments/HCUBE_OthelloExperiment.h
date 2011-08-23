#ifndef HCUBE_OTHELLOEXPERIMENT_H_INCLUDED
#define HCUBE_OTHELLOEXPERIMENT_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"
#include "Experiments/HCUBE_OthelloCommon.h"

#define MAX_CACHED_BOARDS (8192)

#define OTHELLO_EXPERIMENT_ENABLE_BIASES (1)

#define OTHELLO_EXPERIMENT_LOG_EVALUATIONS (1)

#define OTHELLO_MAX_ROUNDS (100)

#define OTHELLO_DEPTH_ITERATIONS (1)

#define OTHELLO_OPPONENT_TYPES (1)

typedef float OthelloNEATDatatype;

namespace HCUBE
{
    class OthelloBoardState
    {
    public:
        ushort board[8][8];
        uchar playerToMove;
        int depth;

        OthelloBoardState()
        {}

        OthelloBoardState(ushort _board[8][8],uchar _playerToMove,int _depth)
                :
                playerToMove(_playerToMove),
                depth(_depth)
        {
            memcpy(board,_board,sizeof(ushort)*8*8);
        }

        bool operator <(const OthelloBoardState &bs2) const
        {
            if (playerToMove<bs2.playerToMove)
            {
                return true;
            }
            else if (playerToMove==bs2.playerToMove)
            {
                if (depth < bs2.depth)
                {
                    return true;
                }
                else if (depth == bs2.depth)
                {
                    return (memcmp(board,bs2.board,8*8*sizeof(ushort))==-1);
                }
            }

            return false;
        }

        bool operator ==(const OthelloBoardState &bs2) const
        {
            return memcmp(this,&bs2,sizeof(OthelloBoardState))==0;
        }
    };

    class OthelloCachedBoard
    {
    public:
        ushort b[8][8];

        OthelloCachedBoard()
        {}

        OthelloCachedBoard(ushort _b[8][8])
        {
            memcpy(b,_b,sizeof(ushort)*8*8);
        }

        void loadBoard(ushort _b[8][8])
        {
            memcpy(b,_b,sizeof(ushort)*8*8);
        }

        bool operator <(const OthelloCachedBoard &bs2) const
        {
            return (memcmp(b,bs2.b,8*8*sizeof(ushort))==-1);
        }

        bool operator ==(const OthelloCachedBoard &bs2) const
        {
            return (memcmp(b,bs2.b,8*8*sizeof(ushort))==0);
        }
    };

    typedef map<OthelloCachedBoard,OthelloNEATDatatype> OthelloBoardCacheMap;
    typedef map<OthelloBoardState,OthelloNEATDatatype> OthelloBoardStateMap;

    class OthelloExperiment : public Experiment, public OthelloCommon
    {
    public:
    protected:
#if OTHELLO_EXPERIMENT_ENABLE_BIASES
        NEAT::FastBiasNetwork<OthelloNEATDatatype> substrates[2];
#else
        NEAT::FastNetwork<OthelloNEATDatatype> substrates[2];
#endif
        int currentSubstrateIndex;
        shared_ptr<const NEAT::GeneticIndividual> substrateIndividuals[2];

        int numNodesX[3];
        int numNodesY[3];
        //int numGames;
        //OthelloTreeSearch searchTree;

        NodeMap nameLookup;
        map<Node,NEAT::NetworkNode*> nodeLookup;

        OthelloMove moveToMake;

        ushort userEvaluationBoard[8][8];
        int userEvaluationRound;

        OthelloBoardCacheMap boardEvaluationCache;

        OthelloBoardStateMap boardStateCache;

        int handCodedType;
        int handCodedDepth;

        int DEBUG_USE_HANDCODED_EVALUATION;
        int DEBUG_USE_HYPERNEAT_EVALUATION;
        OthelloCachedBoard tmpboard;

        int randomMoveChance;

#if OTHELLO_EXPERIMENT_LOG_EVALUATIONS
        ushort gameLog[1024][8][8];
#endif

    public:
        OthelloExperiment(string _experimentName,int _threadID);

        virtual ~OthelloExperiment()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        inline string getNameFromNode(Node n)
        {
            return nameLookup[n];
        }

        void generateSubstrate(int substrateNum=0);

        virtual void populateSubstrate(
            shared_ptr<const NEAT::GeneticIndividual> individual,
            int substrateNum=0
        );

        void resetBoard(ushort b[8][8]);

        virtual OthelloNEATDatatype evaluateLeafHyperNEAT(ushort b[8][8]);

        virtual OthelloNEATDatatype evaluateLeafHandCoded(ushort b[8][8]);

        virtual OthelloNEATDatatype evaluateLeafWhite(ushort b[8][8]);

        virtual OthelloNEATDatatype evaluateLeafBlack(ushort b[8][8]);

        OthelloNEATDatatype evaluatemax(ushort b[8][8],  OthelloNEATDatatype parentBeta, int depth,int maxDepth);

        OthelloNEATDatatype evaluatemin(ushort b[8][8],  OthelloNEATDatatype parentAlpha, int depth,int maxDepth);

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);

        virtual void processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual);

#ifndef HCUBE_NOGUI
        virtual void createIndividualImage(wxDC &drawContext,shared_ptr<NEAT::GeneticIndividual> individual);

        virtual bool handleMousePress(wxMouseEvent& event,wxSize &bitmapSize);
#endif

        virtual inline bool performUserEvaluations()
        {
            return false;
        }

        virtual inline bool isDisplayGenerationResult()
        {
            return false;
        }

        virtual Experiment* clone();

        virtual void resetGenerationData(shared_ptr<NEAT::GeneticGeneration> generation);

        virtual void addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,shared_ptr<NEAT::GeneticIndividual> individual);
    };

}

#endif // HCUBE_OTHELLOEXPERIMENT_H_INCLUDED
