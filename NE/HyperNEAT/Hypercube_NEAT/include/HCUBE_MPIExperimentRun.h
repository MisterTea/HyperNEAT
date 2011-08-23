#ifndef HCUBE_MPIEXPERIMENTRUN_H_INCLUDED
#define HCUBE_MPIEXPERIMENTRUN_H_INCLUDED

#include "HCUBE_Defines.h"

#include "HCUBE_ExperimentRun.h"

namespace HCUBE
{
    class MPIExperimentRun : public ExperimentRun
    {
    public:
    protected:

    public:
        MPIExperimentRun();

        virtual ~MPIExperimentRun();

        virtual void evaluatePopulation();

    protected:

    };
}


#endif // HCUBE_MPIEXPERIMENTRUN_H_INCLUDED
