#ifndef __GENETICPOPULATION_H__
#define __GENETICPOPULATION_H__

#include "tinyxmlplus.h"
#include "NEAT_STL.h"

#include "NEAT_Globals.h"
#include "NEAT_GeneticSpecies.h"

#ifdef EPLEX_INTERNAL
#include "NEAT_CoEvoExperiment.h"
#endif

namespace NEAT
{

    /**
     * The Genetic Population class is responsible for holding and managing a population of individuals
     * over multiple generations.
     */
    class GeneticPopulation
    {
        vector<shared_ptr<GeneticGeneration> > generations;

        vector<shared_ptr<GeneticSpecies> > species;

        //we use a separate vector for extinct species to save CPU.
        vector<shared_ptr<GeneticSpecies> > extinctSpecies;

        int onGeneration;
    public:
        NEAT_DLL_EXPORT GeneticPopulation();

        NEAT_DLL_EXPORT GeneticPopulation(string fileName);

#ifdef EPLEX_INTERNAL
        NEAT_DLL_EXPORT GeneticPopulation(shared_ptr<CoEvoExperiment> experiment);

        NEAT_DLL_EXPORT GeneticPopulation(string fileName,shared_ptr<CoEvoExperiment> experiment);
#endif

        NEAT_DLL_EXPORT virtual ~GeneticPopulation();

        inline shared_ptr<GeneticGeneration> getGeneration(int generationIndex=-1)
        {
            if (generationIndex==-1)
                generationIndex=int(onGeneration);

            return generations[generationIndex];
        }

        NEAT_DLL_EXPORT void addIndividual(shared_ptr<GeneticIndividual> individual);

        NEAT_DLL_EXPORT int getIndividualCount(int generation=-1);

        NEAT_DLL_EXPORT shared_ptr<GeneticIndividual> getIndividual(int individualIndex,int generation=-1);

        NEAT_DLL_EXPORT vector<shared_ptr<GeneticIndividual> >::iterator getIndividualIterator(int a,int generation=-1);

        NEAT_DLL_EXPORT shared_ptr<GeneticIndividual> getBestAllTimeIndividual();

        NEAT_DLL_EXPORT shared_ptr<GeneticIndividual> getBestIndividualOfGeneration(int generation=LAST_GENERATION);

        inline shared_ptr<GeneticSpecies> getSpecies(int id)
        {
            for (int a=0;a<(int)species.size();a++)
            {
                if (species[a]->getID()==id)
                    return species[a];
            }

            throw CREATE_LOCATEDEXCEPTION_INFO("Tried to get a species which doesn't exist (Maybe it went extinct?)");
        }

        NEAT_DLL_EXPORT void speciate();

        NEAT_DLL_EXPORT void setSpeciesMultipliers();

        NEAT_DLL_EXPORT void adjustFitness();

        NEAT_DLL_EXPORT void produceNextGeneration();

        NEAT_DLL_EXPORT void dump(string filename,bool includeGenes,bool doGZ);

        NEAT_DLL_EXPORT void dumpBest(string filename,bool includeGenes,bool doGZ);

        NEAT_DLL_EXPORT void cleanupOld(int generationSkip);

        inline int getGenerationCount()
        {
            return (int)generations.size();
        }
    };

}

#endif
