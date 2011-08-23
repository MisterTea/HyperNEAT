#ifndef HCUBE_OTHELLOCOEXPERIMENT_H_INCLUDED
#define HCUBE_OTHELLOCOEXPERIMENT_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"

#include "Experiments/HCUBE_OthelloExperiment.h"
#include "Experiments/HCUBE_OthelloCommon.h"

#ifdef EPLEX_INTERNAL

namespace HCUBE
{
    class OthelloCoExperiment : public OthelloExperiment, public NEAT::CoEvoExperiment
    {
    public:
    protected:

    public:
        OthelloCoExperiment(string _experimentName,int _threadID);

        virtual ~OthelloCoExperiment()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        virtual pair<double,double> playGame(
            shared_ptr<NEAT::GeneticIndividual> ind1,
            shared_ptr<NEAT::GeneticIndividual> ind2
        );

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);

        virtual Experiment* clone();
    };

}

#endif

#endif // HCUBE_OTHELLOCOEXPERIMENT_H_INCLUDED
