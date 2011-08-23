#ifndef HCUBE_COCHECKERSEXPERIMENT_H_INCLUDED
#define HCUBE_COCHECKERSEXPERIMENT_H_INCLUDED

#include "Experiments/HCUBE_CheckersExperiment.h"

#ifdef EPLEX_INTERNAL

namespace HCUBE
{

    class CoCheckersExperiment : public CheckersExperiment, public NEAT::CoEvoExperiment
    {
    public:
    protected:
        //This is kind of a misnomer because all players are board evaluators for black-moving boards.
        //The second guy gets 1 higher ply depth so he can still see black boards at the gametree leafs.
        shared_ptr<NEAT::GeneticIndividual> individualBlack,individualWhite;

        int testCases;

        //The current generation.  Important for getting coevolution test cases.

    public:

        CoCheckersExperiment(string _experimentName,int _threadID);

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        //virtual CheckersNEATDatatype evaluateLeafWhite(uchar b[8][8]);

        virtual pair<double,double> playGame(
            shared_ptr<NEAT::GeneticIndividual> ind1,
            shared_ptr<NEAT::GeneticIndividual> ind2
        );

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);

        virtual void processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual);

        virtual Experiment* clone();

        virtual int getGroupCapacity()
        {
            return 1;
        }

        virtual void addGenerationData(shared_ptr<NEAT::GeneticGeneration> generation,shared_ptr<NEAT::GeneticIndividual> individual);

    protected:
    };
}

#endif

#endif // HCUBE_COCHECKERSEXPERIMENT_H_INCLUDED
