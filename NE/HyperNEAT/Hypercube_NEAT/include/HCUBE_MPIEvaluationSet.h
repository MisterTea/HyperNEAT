#ifndef HCUBE_MPIEVALUATIONSET_H_INCLUDED
#define HCUBE_MPIEVALUATIONSET_H_INCLUDED

#include "HCUBE_Defines.h"

#include "HCUBE_EvaluationSet.h"

#include "Experiments/HCUBE_Experiment.h"

#define INDIVIDUAL_GENOMES_TAG 1000
//#define NEW_FITNESSES_TAG 1001
#define DIE_TAG 1002
#define NEW_INDIVIDUALS_TAG 1003

namespace HCUBE
{
    class MPIEvaluationSet : public EvaluationSet
    {
    public:
    protected:
        int targetProcessor;

    public:
        MPIEvaluationSet(
            shared_ptr<Experiment> _experiment,
            shared_ptr<NEAT::GeneticGeneration> _generation,
            vector<shared_ptr<NEAT::GeneticIndividual> >::iterator _individualIterator,
            int _individualCount,
            int _targetProcessor
        )
                :
                EvaluationSet(
                    _experiment,
                    _generation,
                    _individualIterator,
                    _individualCount
                ),
                targetProcessor(_targetProcessor)
        {}

        virtual ~MPIEvaluationSet()
        {}

        virtual void run();

        void collectData();

	protected:
    };
}

#endif // HCUBE_MPIEVALUATIONSET_H_INCLUDED
