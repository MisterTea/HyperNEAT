#include "mpi.h"

#include "HCUBE_MPIExperimentRun.h"

#include "Experiments/HCUBE_Experiment.h"

#ifndef HCUBE_NOGUI
#include "HCUBE_MainFrame.h"
#include "HCUBE_UserEvaluationFrame.h"

#include "HCUBE_ExperimentPanel.h"
#endif

#include "HCUBE_MPIEvaluationSet.h"

namespace HCUBE
{
    MPIExperimentRun::MPIExperimentRun()
            :
            ExperimentRun()
    {}

    MPIExperimentRun::~MPIExperimentRun()
    {}

    void MPIExperimentRun::evaluatePopulation()
    {
        try
        {
            shared_ptr<NEAT::GeneticGeneration> generation = population->getGeneration();
            //Randomize population order for evaluation
            generation->randomizeIndividualOrder();

            int populationSize = population->getIndividualCount();

            int numThreads;

            MPI_Comm_size(MPI_COMM_WORLD,&numThreads);

            cout << "Number of threads: " << numThreads << endl;

            //NOTE: YOU SHOULD NEVER HAVE MORE THREADS THAN INDIVIDUALS!
            int populationPerProcess = populationSize/numThreads;

            boost::thread** threads = new boost::thread*[numThreads];
            MPIEvaluationSet** MPIEvaluationSets = new MPIEvaluationSet*[numThreads];

            for (int i = 0; i < numThreads; ++i)
            {
                if (i+1==numThreads)
                {
                    //Fix for uneven distribution
                    int populationIteratorSize =
                        populationSize
                        - populationPerProcess*(numThreads-1);
                    MPIEvaluationSets[i] =
                        new MPIEvaluationSet(
                        experiments[0],
                        generation,
                        population->getIndividualIterator(populationPerProcess*i),
                        populationIteratorSize,
                        i
                    );

                    //MPIEvaluationSets[i]->run();
                }
                else
                {

                    MPIEvaluationSets[i] =
                        new MPIEvaluationSet(
                        experiments[0],
                        generation,
                        population->getIndividualIterator(populationPerProcess*i),
                        populationPerProcess,
                        i
                    );

                    //MPIEvaluationSets[i]->run();
                }

                threads[i] =
                    new boost::thread(
                    boost::bind(
                        &MPIEvaluationSet::run,
                        MPIEvaluationSets[i]
                    )
                );
            }

            //loop through each thread, making sure it is finished before we move on
            for (int i=0;i<numThreads;++i)
            {
                threads[i]->join();
                delete threads[i];
                threads[i] =
                    new boost::thread(
                    boost::bind(
                        &MPIEvaluationSet::collectData,
                        MPIEvaluationSets[i]
                    )
                );
            }

            for (int i = 0; i < numThreads; ++i)
            {
                threads[i]->join();
                delete threads[i];
                delete MPIEvaluationSets[i];
            }

            for (int a=0;a<populationSize;a++)
            {
                double fitness = population->getIndividual(a)->getFitness();

                if (fabs(fitness) < 1e-3)
                {
                    throw CREATE_LOCATEDEXCEPTION_INFO(string("ERROR: 0 fitness for individual: ")+toString(a));
                }
            }

            delete threads;
            delete MPIEvaluationSets;
        }
        catch (const std::exception &ex)
        {
            CREATE_PAUSE(ex.what());
        }
    }
}
