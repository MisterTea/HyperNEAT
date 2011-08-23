#ifndef NEAT_COEVOGENETICGENERATION_H_INCLUDED
#define NEAT_COEVOGENETICGENERATION_H_INCLUDED

#include "NEAT_GeneticGeneration.h"

#ifdef EPLEX_INTERNAL

namespace NEAT
{

    /**
     * The GeneticGeneration class is responsible for containing and managing a single generation of indiviudals
     */
    class CoEvoGeneticGeneration : public GeneticGeneration
    {
        vector<shared_ptr<GeneticIndividual> > tests;

        vector< vector<bool> > testResults;

        //gives the fitness of <index1> from playing <index2>
        vector< vector<double> > testFitnesses;

        shared_ptr<CoEvoExperiment> experiment;

    public:

        /** produceNextGeneration:
         *  Creates the next generation.  Use this function instead of a constructor
         * *NOTE* Do not pass a regular GeneticGeneration into a CoEvoGeneticGeneration
         * produce function!
         */
        virtual shared_ptr<GeneticGeneration> produceNextGeneration(
            const vector<shared_ptr<GeneticIndividual> > &newIndividuals,
            int _generationNumber
        );

        /**
         * Constructor: Creates an empty generation
         * \param _generationNumber Is the number of this generation
         */
        NEAT_DLL_EXPORT CoEvoGeneticGeneration(int _generationNumber,shared_ptr<CoEvoExperiment> _experiment);

        NEAT_DLL_EXPORT CoEvoGeneticGeneration(const CoEvoGeneticGeneration &other);

        NEAT_DLL_EXPORT CoEvoGeneticGeneration &operator=(const CoEvoGeneticGeneration &other);

        NEAT_DLL_EXPORT virtual ~CoEvoGeneticGeneration();

        /**
         * Constructor: Creates a generation from it's serialized XML format
         * \param generationElement Is the root of the XML format
         */
        NEAT_DLL_EXPORT CoEvoGeneticGeneration(
            TiXmlElement *generationElement,
            shared_ptr<CoEvoExperiment> _experiment
        );

        virtual inline const char *getTypeName()
        {
            return "CoEvoGeneticGeneration";
        }

        NEAT_DLL_EXPORT double getAverageTestFitness();

        NEAT_DLL_EXPORT double getMaxTestFitness();

        NEAT_DLL_EXPORT void replaceLowestTest(shared_ptr<GeneticIndividual> indToReplace);

        NEAT_DLL_EXPORT shared_ptr<GeneticIndividual> addTest(shared_ptr<GeneticIndividual> test);

        //TODO: Replace this hack for MPI with a better solution (maybe stream generations?)
        NEAT_DLL_EXPORT void addTestHack(shared_ptr<GeneticIndividual> test)
        {
            tests.push_back(test);
        }

        NEAT_DLL_EXPORT void removeTest(int index);

        inline int getTestCount()
        {
            return (int)tests.size();
        }

        inline shared_ptr<GeneticIndividual> getTest(int a)
        {
            if (a>=(int)tests.size())
            {
                cout << string("GENETICGENERATION::GETINDIVIDUAL: Individual index out of range!\n");
                throw CREATE_LOCATEDEXCEPTION_INFO("GENETICGENERATION::GETINDIVIDUAL: Individual index out of range!\n");
            }

            return tests[a];
        }

        inline vector<shared_ptr<GeneticIndividual> >::iterator getTestIterator(int a)
        {
            return (tests.begin()+a);
        }

        NEAT_DLL_EXPORT virtual void dump(TiXmlElement *generationElement,bool includeGenes=true);

        NEAT_DLL_EXPORT virtual void dumpBest(TiXmlElement *generationElement,bool includeGenes=true);

        /**
         * cleanup: Removes all individuals from this generation except the generation champion
         */
        NEAT_DLL_EXPORT virtual void cleanup();

        NEAT_DLL_EXPORT void bootstrap();

        NEAT_DLL_EXPORT void updateTests();

    protected:
        /**
         * Constructor: Creates a generation
         * \param _individuals Is a list of individuals for this generation
         * \param _generationNumber Is the number of this generation
         */
        CoEvoGeneticGeneration(
            CoEvoGeneticGeneration *previousGeneration,
            const vector<shared_ptr<GeneticIndividual> > &newIndividuals,
            int _generationNumber,
            const vector<shared_ptr<GeneticIndividual> > &newTests,
            const vector< vector<bool> > &newTestResults,
            const vector< vector<double> > &newTestFitnesses,
            shared_ptr<CoEvoExperiment> _experiment
        );

        bool getTestResult(int t1,int t2);
    };
}

#endif

#endif // NEAT_COEVOGENETICGENERATION_H_INCLUDED
