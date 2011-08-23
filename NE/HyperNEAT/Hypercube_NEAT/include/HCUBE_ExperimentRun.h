#ifndef HCUBE_EXPERIMENTRUN_H_INCLUDED
#define HCUBE_EXPERIMENTRUN_H_INCLUDED

#include "HCUBE_Defines.h"

namespace HCUBE
{
    /**
    * ExperimentRun contains a population, an array of identical experiments (for multithreading), 
    * and functions to pull information from a run of an experiment and control the experiment while
    * it is running.
    */
    class ExperimentRun
    {
    public:
    protected:
        bool running;
        bool started;
        bool cleanup;
        int experimentType;

        shared_ptr<NEAT::GeneticPopulation> population;

        vector<shared_ptr<Experiment> > experiments;

        mutex* populationMutex;

        MainFrame *frame;

        string outputFileName;

    public:
        ExperimentRun();

        virtual ~ExperimentRun();

        /**
        * Setting Cleanup will cause older individuals that are not generation
        * champions to be destroyed when a new generation is created.
        */
        inline void setCleanup(bool value)
        {
            cleanup = value;
        }

        inline shared_ptr<NEAT::GeneticPopulation> getPopulation()
        {
            return population;
        }

        inline shared_ptr<Experiment> getExperiment()
        {
            return experiments[0];
        }

        inline const void setFrame(MainFrame *_frame)
        {
            frame=_frame;
        }

        inline const bool &isRunning()
        {
            return running;
        }

        inline void setRunning(bool newRunning)
        {
            if (!population)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("Tried to start an experiment with no population!");
            }

            running = newRunning;
        }

        inline const bool &isStarted()
        {
            return started;
        }

        inline shared_ptr<NEAT::GeneticGeneration> getGeneration(int generation)
        {
            return population->getGeneration(generation);
        }

        inline shared_ptr<NEAT::GeneticIndividual> getIndividual(int generation,int individual)
        {
            return population->getIndividual(individual,generation);
        }

        inline shared_ptr<NEAT::GeneticIndividual> getIndividual(int individual)
        {
            return population->getIndividual(individual);
        }

        /**
        * This function initializes an experiment from an XML file with a partial run.
        * This is mainly for continuing an experiment from an XML file or analyzing 
        * individuals from a run that has completed.
        */
        void setupExperimentInProgress(
            string populationFileName,
            string _outputFileName
        );

        /**
         * This function executes the experiment.  Note that this function blocks
         * until the experiment has completed, so ideally it should be run from
         * a thread.
         */
        void start();

        /**
        * This function initializes an experiment given the experiment type ID.
        * This is for beginning a new run from generation 0.
        */
        void setupExperiment(
            int experimentType,
            string _outputFileName
        );

        /**
        * This function creates a population.  This is typically called after setupExperiment
        * to create the initial population for the run.
        */
        void createPopulation(string populationString="");

        /**
        * This function calls the preprocess function for the experiment on every individual
        * in the population.  This is currently used in scaling the individuals in between 
        * generations in the scaling experiments.
        */
        void preprocessPopulation();

        /**
        * This function evaluates all individuals in the population
        */
        virtual void evaluatePopulation();

        /**
        * This function performs speciation and sorts the invidiuals by fitness
        */
        void finishEvaluations();

        /**
        * This function produces the next generation of individuals using 
        * standard NEAT operations (selection, mutation, crossover)
        */
        void produceNextGeneration();

        inline mutex* getPopulationMutex()
        {
            return populationMutex;
        }

    protected:
        /**
        * This class cannot be copied
        */
        ExperimentRun(const ExperimentRun &other)
        {}

        /**
        * This class cannot be copied
        */
        const ExperimentRun &operator=(const ExperimentRun &other)
        {
            return *this;
        }

    };
}

#endif // HCUBE_EXPERIMENTRUN_H_INCLUDED
