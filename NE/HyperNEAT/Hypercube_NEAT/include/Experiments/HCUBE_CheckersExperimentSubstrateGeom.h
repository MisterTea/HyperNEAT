#ifndef HCUBE_CHECKERSEXPERIMENTSUBSTRATEGEOM_H_INCLUDED
#define HCUBE_CHECKERSEXPERIMENTSUBSTRATEGEOM_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"
#include "Experiments/HCUBE_CheckersCommon.h"

#include "Experiments/HCUBE_CheckersExperiment.h"

namespace HCUBE
{
    class CheckersExperimentSubstrateGeom : public CheckersExperiment
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
        CheckersExperimentSubstrateGeom(string _experimentName,int _threadID);

        virtual ~CheckersExperimentSubstrateGeom()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        virtual void generateSubstrate(int substrateNum=0);

        virtual void populateSubstrate(
            shared_ptr<NEAT::GeneticIndividual> individual,
            int substrateNum=0
        );

        virtual Experiment* clone();
    };

}

#endif // HCUBE_CHECKERSEXPERIMENTSUBSTRATEGEOM_H_INCLUDED
