#ifndef HCUBE_TICTACTOEGAMENOGEOMEXPERIMENT_H_INCLUDED
#define HCUBE_TICTACTOEGAMENOGEOMEXPERIMENT_H_INCLUDED

#ifndef DOXYGEN_PROCESSOR

#include "Experiments/HCUBE_Experiment.h"

#include "HCUBE_TicTacToeGameTreeSearch.h"

namespace HCUBE
{

    class TicTacToeGameNoGeomExperiment : public Experiment
    {
    public:
    protected:
        NEAT::FastNetwork<double> substrate;
        int numNodesX,numNodesY;
        //int numGames;
        TicTacToeGameTreeSearch searchTree;

        map<Node,string> nameLookup;

    public:
        TicTacToeGameNoGeomExperiment(string _experimentName,int _threadID);

        virtual ~TicTacToeGameNoGeomExperiment()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        double processEvaluation(
            wxDC *drawContext,
            int_gen_type &move_gen
        );

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);

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

#endif

#endif // HCUBE_TICTACTOEGAMENOGEOMEXPERIMENT_H_INCLUDED
