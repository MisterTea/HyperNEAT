#ifndef HCUBE_CHECKERSEXPERIMENT_H_INCLUDED
#define HCUBE_CHECKERSEXPERIMENT_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"
#include "Experiments/HCUBE_CheckersCommon.h"

#define MAX_CACHED_BOARDS (8192)

#define CHECKERS_EXPERIMENT_ENABLE_BIASES (0)

#define CHECKERS_MAX_ROUNDS (170)

namespace HCUBE
{
    class CheckersExperiment : public Experiment, public CheckersCommon
    {
    public:
    protected:
        NEAT::LayeredSubstrate<CheckersNEATDatatype> substrates[2];
        int currentSubstrateIndex;
        const static int handCodedAISubstrateIndex=2;

        shared_ptr<const NEAT::GeneticIndividual> substrateIndividuals[2];

        int numNodesX[3];
        int numNodesY[3];
        //int numGames;
        //CheckersTreeSearch searchTree;

        NodeMap nameLookup;

        CheckersMove moveToMake;

        double chanceToMakeSecondBestMove;

        CheckersNEATDatatype childAlphaForSecondBestMove;
        CheckersNEATDatatype childBetaForSecondBestMove;
        CheckersMove secondBestMoveToMake;

        vector<CheckersMove> totalMoveList;

        uchar userEvaluationBoard[8][8];
        int userEvaluationRound;

        BoardCacheList boardEvaluationCaches[2][65336];

        BoardStateList boardStateLists[3][3][65536];

        Vector2<uchar> from;

        int handCodedType;

        int DEBUG_USE_HANDCODED_EVALUATION;
        int DEBUG_USE_HYPERNEAT_EVALUATION;
        CheckersCachedBoard tmpboard;

		bool dumpEvaluationImages;

		int numHandCodedStreams;
		int numHandCodedEvaluations;
		int numHyperNEATStreams;
		int numHyperNEATEvaluations;

		int cakeRandomSeed;
        SEARCHINFO searchInfo;

        int currentRound;

    public:
        CheckersExperiment(string _experimentName,int _threadID);

        virtual ~CheckersExperiment();

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        inline string getNameFromNode(Node n)
        {
			return nameLookup[n];
        }

        virtual void populateSubstrate(
            shared_ptr<NEAT::GeneticIndividual> individual,
            int substrateNum=0
        );

        CheckersNEATDatatype processEvaluation(
            wxDC *drawContext
        );

        virtual pair<CheckersNEATDatatype,int> evaluateLeafWhite(uchar b[8][8]);

        virtual pair<CheckersNEATDatatype,int> evaluateLeafHyperNEAT(uchar b[8][8]);

        virtual pair<CheckersNEATDatatype,int> evaluatemax(
            uchar b[8][8],
            CheckersNEATDatatype parentBeta,
            int depth,
            int maxDepth
        );

        virtual pair<CheckersNEATDatatype,int> evaluatemin(
            uchar b[8][8],
            CheckersNEATDatatype parentAlpha,
            int depth,
            int maxDepth
        );

        virtual pair<CheckersNEATDatatype,int> firstevaluatemax(
            uchar b[8][8],
            int maxDepth,
            double timeLimit = double(INT_MAX)
        );

        virtual CheckersNEATDatatype firstevaluatemin(
            uchar b[8][8],
            int maxDepth,
            double timeLimit = double(INT_MAX)
        );

		void makeMoveCliche(uchar b[8][8],int colorToMove,int* retval);

		void makeMoveCake(uchar b[8][8],int colorToMove,int* retval);

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

        void setChanceToMakeSecondBestMove(double newChance)
        {
            chanceToMakeSecondBestMove = newChance;
        }
    };

}

#endif // HCUBE_TICTACTOEGAMEEXPERIMENT_H_INCLUDED

