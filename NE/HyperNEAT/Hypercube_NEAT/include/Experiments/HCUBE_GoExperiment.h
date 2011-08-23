#ifndef HCUBE_GOEXPERIMENT_H_INCLUDED
#define HCUBE_GOEXPERIMENT_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"
//#include "Experiments/HCUBE_GoCommon.h"

//MUST BE INCLUDED FIRST
#include "SgSystem.h"

#include "GoBoard.h"
#include "GoPlayer.h"
#include "SpMoveGenerator.h"
#include "SpSimplePlayer.h"

#include "GoUctPlayoutPolicy.h"
#include "SgUctSearch.h"

#include "GoUctDefaultPriorKnowledge.h"
#include "GoBook.h"

#define MAX_CACHED_BOARDS (8192)

#define GO_EXPERIMENT_ENABLE_BIASES (0)

#define GO_MAX_ROUNDS (100)

namespace HCUBE
{
	typedef float GoNEATDatatype;

	class GoExperiment;

    class GoStats
    {
    public:
        int wins,losses;
		
		
        GoStats()
		:
		wins(0),
		losses(0)
        {
        }

		GoStats(const string &str)
		{
			istringstream istr(str);

			istr >> wins >> losses;
		}
		
        virtual ~GoStats()
        {}
		
        virtual string toString() const
        {
            std::ostringstream oss;
			
            oss << wins << ' ';
            oss << losses;
			
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
			
            return oss.str();
        }
		
        virtual string summaryToString() const
        {
            return toString();
        }
    };

	/** HyperNEAT move generator goes for the jugular, and some influence */
	class HyperNEATMoveGenerator
	: 
	public SpStaticMoveGenerator
	{
	protected:
		GoExperiment *experiment;

	public:
		explicit HyperNEATMoveGenerator(GoBoard& board,GoExperiment *_experiment)
		: 
		SpStaticMoveGenerator(board),
		experiment(_experiment)
		{
		}

		virtual int Score(SgPoint p);

		void GenerateMoves(
		SgEvaluatedMoves& eval,
		SgBlackWhite toPlay
		);
	};

	/** HyperNEAT player */
	class HyperNEATPlayer
		: public SpSimplePlayer
	{
	public:
		HyperNEATPlayer(GoBoard& board,GoExperiment *_experiment)
			: SpSimplePlayer(board, new HyperNEATMoveGenerator(board,_experiment))
		{ }

		std::string Name() const
		{
			return "HyperNEAT";
		}

		virtual bool useRandomGenerator()
		{
			return false;
		}
	};

    class MoveRating
    {
    public:
        float rating;
        SgPoint move;

        MoveRating(float _rating,SgPoint _move)
            :
            rating(_rating),
            move(_move)
        {
        }

        bool operator <(const MoveRating &other) const
        {
            return rating < other.rating;
        }
    };

    float globalGetMoveHeuristic(const GoBoard *board, SgPoint p);

	enum GoScalingType
	{
		GO_SCALING_NONE,
		GO_SCALING_BASIC,
		GO_SCALING_DELAYED_ABSOLUTE,
		GO_SCALING_DELAYED_ABSOLUTE_NO_RANGE,
		GO_SCALING_DELTA_ONLY,
		GO_SCALING_DELTA_ONLY_NO_RANGE,
		GO_SCALING_NO_RANGE,
		GO_SCALING_END
	};

    class GoExperiment : public Experiment
    {
    public:
    protected:
#if GO_EXPERIMENT_ENABLE_BIASES
        NEAT::LayeredSubstrate<GoNEATDatatype> substrates[1];
#else
        NEAT::LayeredSubstrate<GoNEATDatatype> substrates[1];
#endif
        int currentSubstrateIndex;
        const static int handCodedAISubstrateIndex=2;

        shared_ptr<const NEAT::GeneticIndividual> substrateIndividuals[1];

        //HyperNEATPlayer *self;
        GoPlayer* self;
        vector<GoPlayer*> opponents;
		GoBoard* board;
		vector<MoveRating> moves;

        int goBoardSize;
        bool uctEnabled;
        int numGamesPerOpponent;

        GoBook goBook;

    public:
        GoExperiment(string _experimentName,int _threadID);

        virtual ~GoExperiment();
        
        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        virtual void populateSubstrate(
            shared_ptr<NEAT::GeneticIndividual> individual,
            int substrateNum=0
        );

        GoNEATDatatype processEvaluation(
            wxDC *drawContext
        );

        void printBoard(const GoBoard *boardToPrint);

		virtual void preprocessIndividual(
			shared_ptr<NEAT::GeneticGeneration> generation,
			shared_ptr<NEAT::GeneticIndividual> individual
			);

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

		void generateMoves(SgEvaluatedMoves& eval);

        float getMoveHeuristic(const GoBoard *board, SgPoint p);

        inline int getGoBoardSize()
        {
            return goBoardSize;
        }

        void setGoBoardSize(int newSize,bool enableUCT);

        void createGoPlayers(bool enableUCT);

    private:
        GoExperiment(const GoExperiment &other);

        const GoExperiment &operator =(const GoExperiment &other);
    };

}

#endif // HCUBE_TICTACTOEGAMEEXPERIMENT_H_INCLUDED

