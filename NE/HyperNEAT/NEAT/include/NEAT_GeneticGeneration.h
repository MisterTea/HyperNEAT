#ifndef __GENETICGENERATION_H__
#define __GENETICGENERATION_H__

#include "NEAT_Defines.h"

#include "NEAT_GeneticIndividual.h"

#ifdef EPLEX_INTERNAL
#include "NEAT_CoEvoExperiment.h"
#endif

namespace NEAT
{

    /**
     * The GeneticGeneration class is responsible for containing and managing a single generation of indiviudals
     */
    class GeneticGeneration
    {
    protected:
        vector<shared_ptr<GeneticIndividual> > individuals;

        int generationNumber;

        bool sortedByFitness;

        string userData;

        //This is true when a generation is loaded from an xml file.
        //In this case, do not recompute things like average fitness
        //because some of the population might be missing
        bool isCached;
        double cachedAverageFitness;

    public:

        /** produceNextGeneration:
         *  Creates the next generation.  Use this function instead of a constructor
         */
        virtual shared_ptr<GeneticGeneration> produceNextGeneration(
            const vector<shared_ptr<GeneticIndividual> > &newIndividuals,
            int _generationNumber
        );

        /**
         * Constructor: Creates an empty generation
         * \param _generationNumber Is the number of this generation
         */
        NEAT_DLL_EXPORT GeneticGeneration(int _generationNumber);

        NEAT_DLL_EXPORT GeneticGeneration(const GeneticGeneration &other);

        NEAT_DLL_EXPORT GeneticGeneration &operator=(const GeneticGeneration &other);

        NEAT_DLL_EXPORT virtual ~GeneticGeneration();

        /**
         * Constructor: Creates a generation from it's serialized XML format
         * \param generationElement Is the root of the XML format
         */
        NEAT_DLL_EXPORT GeneticGeneration(TiXmlElement *generationElement);

        virtual inline const char *getTypeName()
        {
            return "GeneticGeneration";
        }

        inline int getGenerationNumber()
        {
            return generationNumber;
        }

        inline void addIndividual(shared_ptr<GeneticIndividual> i)
        {
            individuals.push_back(i);
        }

        inline int getIndividualCount()
        {
            return (int)individuals.size();
        }

        inline shared_ptr<GeneticIndividual> getIndividual(int a)
        {
            if (a>=(int)individuals.size())
            {
                cout << string("GENETICGENERATION::GETINDIVIDUAL: Individual index out of range!\n");
                throw CREATE_LOCATEDEXCEPTION_INFO("GENETICGENERATION::GETINDIVIDUAL: Individual index out of range!\n");
            }

            return individuals[a];
        }

        inline vector<shared_ptr<GeneticIndividual> >::iterator getIndividualIterator(int a)
        {
            return (individuals.begin()+a);
        }

        inline void setUserData(string _userData)
        {
            userData = _userData;
        }

        inline string getUserData()
        {
            return userData;
        }

        inline string getUserData() const
        {
            return userData;
        }

        NEAT_DLL_EXPORT virtual void dump(TiXmlElement *generationElement,bool includeGenes=true);

        NEAT_DLL_EXPORT virtual void dumpBest(TiXmlElement *generationElement,bool includeGenes=true);

        /**
         * mateIndividuals: mates two individuals
         */
        NEAT_DLL_EXPORT shared_ptr<GeneticIndividual> mateIndividuals(int i1,int i2);

        /**
         * getCompatibility: Returns the compatiblity between two individuals
         */
        NEAT_DLL_EXPORT double getCompatibility(int i1,int i2);

        NEAT_DLL_EXPORT void sortByFitness();

        NEAT_DLL_EXPORT virtual shared_ptr<GeneticIndividual> getGenerationChampion();

        /**
         * cleanup: Removes all individuals from this generation except the generation champion
         */
        NEAT_DLL_EXPORT virtual void cleanup();

        /**
         * randomizeIndividualOrder: After computing fitness, individuals are typically sorted
         * by their fitness.  This randomizes their relative order
         */
        NEAT_DLL_EXPORT void randomizeIndividualOrder();

    protected:
        /**
         * Constructor: Creates a new generation from a previous one
         * \param previousGeneration is a poitner to the previous gen.
         * \param _individuals Is a list of individuals for this generation
         * \param _generationNumber Is the number of this generation
         */
        GeneticGeneration(
            GeneticGeneration *previousGeneration,
            const vector<shared_ptr<GeneticIndividual> > &newIndividuals,
            int _generationNumber);

        void setAttributes(TiXmlElement *generationElement);
    };

}

#endif
