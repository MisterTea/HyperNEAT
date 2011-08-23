#ifndef HCUBE_CHECKERSEXPERIMENTPRUNING_H_INCLUDED
#define HCUBE_CHECKERSEXPERIMENTPRUNING_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"
#include "Experiments/HCUBE_CheckersCommon.h"

#define MAX_CACHED_BOARDS (8192)

#define CHECKERS_EXPERIMENT_ENABLE_BIASES (0)

#define CHECKERS_MAX_ROUNDS (170)

namespace HCUBE
{
    class CheckersExperimentPruning : public Experiment, public CheckersCommon, public CheckersAdvisor
    {
    public:
    protected:
        NEAT::LayeredSubstrate<CheckersNEATDatatype> substrate;

        shared_ptr<const NEAT::GeneticIndividual> substrateIndividual;

        NodeMap nameLookup;

        CheckersMove moveToMake;

        double chanceToMakeSecondBestMove;

        CheckersNEATDatatype childAlphaForSecondBestMove;
        CheckersNEATDatatype childBetaForSecondBestMove;
        CheckersMove secondBestMoveToMake;

        vector<CheckersMove> totalMoveList;

        uchar userEvaluationBoard[8][8];
        int userEvaluationRound;

	uchar boardHistory[CHECKERS_MAX_ROUNDS*2+2][8][8];

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

		int maxCakeNodes;

    public:
        CheckersExperimentPruning(string _experimentName,int _threadID);

        virtual ~CheckersExperimentPruning();

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        inline string getNameFromNode(Node n)
        {
			return nameLookup[n];
        }

        virtual void populateSubstrate(
            shared_ptr<NEAT::GeneticIndividual> individual
        );

		virtual void setBoardPosition(unsigned char b[8][8]);

		virtual float getBoardValue(int xpos,int ypos);

        CheckersNEATDatatype processEvaluation(
            wxDC *drawContext
        );

		void makeMoveCake(uchar b[8][8],int colorToMove,int* retval,bool useAdvisor,bool firstMove);

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

