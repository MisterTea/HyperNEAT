#ifndef HCUBE_CHECKERSSCALINGEXPERIMENT_H_INCLUDED
#define HCUBE_CHECKERSSCALINGEXPERIMENT_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"
#include "Experiments/HCUBE_CheckersCommon.h"

#include "Experiments/HCUBE_CheckersExperiment.h"

#define MAX_CACHED_BOARDS (8192)

namespace HCUBE
{
    class CheckersScalingExperiment : public CheckersExperiment
    {
    public:
    protected:
#if CHECKERS_EXPERIMENT_ENABLE_BIASES
        NEAT::FastBiasNetwork<CheckersNEATDatatype> minisubstrates[2];
#else
        NEAT::FastNetwork<CheckersNEATDatatype> minisubstrates[2];
#endif
        int currentSubstrateIndex;
        shared_ptr<const NEAT::GeneticIndividual> minisubstrateIndividuals[2];

        int mininumNodesX[2];
        int mininumNodesY[2];

        NodeMap mininameLookup;

        CheckersNEATDatatype substrateBaseX,substrateBaseY;

    public:
        CheckersScalingExperiment(string _experimentName,int _threadID);

        virtual ~CheckersScalingExperiment()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        inline string getMiniNameFromNode(Node n)
        {
            return mininameLookup[n];
        }

        void generateMiniSubstrate(int substrateNum=0);

        virtual void populateMiniSubstrate(
            shared_ptr<const NEAT::GeneticIndividual> individual,
            int substrateNum=0
        );

        virtual void processGroup(shared_ptr<NEAT::GeneticGeneration> generation);

        virtual void processIndividualPostHoc(shared_ptr<NEAT::GeneticIndividual> individual);

        virtual inline bool performUserEvaluations()
        {
            return false;
        }

        virtual inline bool isDisplayGenerationResult()
        {
            return false;
        }

        virtual Experiment* clone();
    };

}

#endif // HCUBE_CHECKERSSCALINGEXPERIMENT_H_INCLUDED
