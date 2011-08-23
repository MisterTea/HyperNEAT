#ifndef HCUBE_CHECKERSEXPERIMENTNOGEOM_H_INCLUDED
#define HCUBE_CHECKERSEXPERIMENTNOGEOM_H_INCLUDED

#include "Experiments/HCUBE_Experiment.h"
#include "Experiments/HCUBE_CheckersCommon.h"

#include "Experiments/HCUBE_CheckersExperiment.h"

namespace HCUBE
{
    class CheckersExperimentNoGeom : public CheckersExperiment
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
        CheckersExperimentNoGeom(string _experimentName,int _threadID);

        virtual ~CheckersExperimentNoGeom()
        {}

        virtual NEAT::GeneticPopulation* createInitialPopulation(int populationSize);

        virtual void generateSubstrate(int substrateNum=0);

        virtual void populateSubstrate(
            shared_ptr<NEAT::GeneticIndividual> individual,
            int substrateNum=0
        );

		virtual pair<CheckersNEATDatatype,int> evaluateLeafHyperNEAT(
			uchar b[8][8]
		);

#if 0
        virtual double evaluateLeafHyperNEAT(uchar b[8][8]);
#endif

        virtual Experiment* clone();
    };

}

#endif // HCUBE_CHECKERSEXPERIMENTNOGEOM_H_INCLUDED
