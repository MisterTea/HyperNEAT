#ifndef HCUBE_CHECKERSEXPERIMENTFOGEL_H_INCLUDED
#define HCUBE_CHECKERSEXPERIMENTFOGEL_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"
#include "Experiments/HCUBE_CheckersCommon.h"

#include "Experiments/HCUBE_CheckersExperiment.h"

#ifdef EPLEX_INTERNAL

namespace HCUBE
{
    class CheckersExperimentFogel : public CheckersExperiment
    {
    public:
    protected:
#if CHECKERS_EXPERIMENT_ENABLE_BIASES
        NEAT::FastBiasNetwork<CheckersNEATDatatype> networks[2];

        throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Can't run NoGeom version of CheckersExperiment with biases turned on!");
#else
        NEAT::FastNetwork<CheckersNEATDatatype> networks[2];
#endif

    public:
        CheckersExperimentFogel(string _experimentName,int _threadID);

        virtual ~CheckersExperimentFogel()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        virtual void generateSubstrate(int substrateNum=0);

        virtual void populateSubstrate(
            shared_ptr<NEAT::GeneticIndividual> individual,
            int substrateNum=0
        );

        virtual pair<CheckersNEATDatatype,int> evaluateLeafHyperNEAT(uchar b[8][8]);

        virtual CheckersNEATDatatype getSpatialInput(uchar b[8][8],int x,int y,int sizex,int sizey);

        virtual Experiment* clone();
    };

}

#endif

#endif // HCUBE_CHECKERSEXPERIMENTFOGEL_H_INCLUDED
