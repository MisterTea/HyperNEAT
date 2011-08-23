#ifndef NEAT_COEVOEXPERIMENT_H_INCLUDED
#define NEAT_COEVOEXPERIMENT_H_INCLUDED

#ifdef EPLEX_INTERNAL

#include "NEAT_Defines.h"

namespace NEAT
{
    class NEAT_DLL_EXPORT CoEvoExperiment
    {
    protected:

    public:
        virtual pair<double,double> playGame(
            shared_ptr<GeneticIndividual> ind1,
            shared_ptr<GeneticIndividual> ind2) = 0;

        virtual ~CoEvoExperiment() {}
    };
}

#endif

#endif // NEAT_COEVOEXPERIMENT_H_INCLUDED
