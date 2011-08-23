#ifndef HCUBE_CHECKERSEXPERIMENTORIGINALFOGEL_H_INCLUDED
#define HCUBE_CHECKERSEXPERIMENTORIGINALFOGEL_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"
#include "Experiments/HCUBE_CheckersCommon.h"

#include "Experiments/HCUBE_CheckersExperiment.h"

namespace HCUBE
{
    class CheckersExperimentOriginalFogel : public CheckersExperiment
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
        CheckersExperimentOriginalFogel(string _experimentName,int _threadID);

        virtual ~CheckersExperimentOriginalFogel()
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

#endif // HCUBE_CHECKERSEXPERIMENTORIGINALFOGEL_H_INCLUDED
