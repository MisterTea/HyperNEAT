#ifndef __GENETICINDIVIDUAL_H__
#define __GENETICINDIVIDUAL_H__

#include "NEAT_Defines.h"
#include "NEAT_STL.h"
#include "tinyxmlplus.h"

#include "NEAT_Network.h"
#include "NEAT_ModularNetwork.h"
#include "NEAT_FastNetwork.h"
#ifdef EPLEX_INTERNAL
#include "NEAT_VectorNetwork.h"
#include "NEAT_FractalNetwork.h"
#endif

namespace NEAT
{

    class GeneticIndividual
    {
    protected:
        vector<GeneticNodeGene> nodes;
        vector<GeneticLinkGene> links;

        double fitness;

        int speciesID;

        bool canReproduce;

        string userData;

    public:
        /**
         * Constructor: Creates an individual with the inputed nodes.
         * \param createTopology Creates a set of links for the new individual
         * \param edgeDensity The probability that a link will exist
        */
        NEAT_DLL_EXPORT GeneticIndividual(
            const vector<GeneticNodeGene> &_nodes,
            bool createTopology=true,
            double edgeDensity=0.75
        );

        /**
         * Create an individual with the inputed nodes.
         * \param randomizeLinkWeights Randomizes all input link weights
         * \param createTopology Creates more links for the new individual
         * \param edgeDensity The probability that a link will exist
        */
        NEAT_DLL_EXPORT GeneticIndividual(
            const vector<GeneticNodeGene> &_nodes,
            const vector<GeneticLinkGene> &_links,
            bool randomizeLinkWeights=true,
            bool createTopology=false,
            double edgeDensity=0.75);

        /**
         * Create an individual from the XML description.
        */
        NEAT_DLL_EXPORT GeneticIndividual(TiXmlElement *individualElement);

        /**
         * Create an individual from the stream description.
         */
        NEAT_DLL_EXPORT GeneticIndividual(istream& stream);

        /**
         * Create a baby individual from two parents
        */
        NEAT_DLL_EXPORT GeneticIndividual(shared_ptr<GeneticIndividual> parent1,shared_ptr<GeneticIndividual> parent2,bool mate_multipoint_avg=false);

        /**
         * Create a baby individual from one parent
        */
        NEAT_DLL_EXPORT GeneticIndividual(shared_ptr<GeneticIndividual> parent,bool tryMutation);

        /**
         * Copy an individual. THIS COPIES FITNESS TOO!  DO NOT USE THIS TO MAKE OFFSPRING!
        */
        NEAT_DLL_EXPORT GeneticIndividual(GeneticIndividual &copy);

        NEAT_DLL_EXPORT virtual ~GeneticIndividual();

        NEAT_DLL_EXPORT virtual bool operator==(const GeneticIndividual &other) const;

        /**
         * testMutate: Attempts to mutate an individual.
        */
        NEAT_DLL_EXPORT void testMutate();

        NEAT_DLL_EXPORT int getNodesCount() const;

        NEAT_DLL_EXPORT GeneticNodeGene *getNode(int index);

        NEAT_DLL_EXPORT const GeneticNodeGene *getNode(int index) const;

        NEAT_DLL_EXPORT GeneticNodeGene *getNode(const string &name);

        NEAT_DLL_EXPORT int getMaxNodePositionOccurance() const;

        NEAT_DLL_EXPORT int getLinksCount() const;

        NEAT_DLL_EXPORT GeneticLinkGene *getLink(int index);

        NEAT_DLL_EXPORT const GeneticLinkGene *getLink(int index) const;

        NEAT_DLL_EXPORT GeneticLinkGene *getLink(int fromNodeID,int toNodeID);

        NEAT_DLL_EXPORT const GeneticLinkGene *getLink(int fromNodeID,int toNodeID) const;

        NEAT_DLL_EXPORT bool linkExists(int fromNode,int toNode) const;

        NEAT_DLL_EXPORT void dump(TiXmlElement *root,bool dumpGenes=true);

        NEAT_DLL_EXPORT void dump(ostream &ostr);

        NEAT_DLL_EXPORT void print() const;

        inline void setFitness(double _fitness)
        {
            fitness = _fitness;
        }

        inline void reward(double _fitness)
        {
            fitness += _fitness;
        }

        inline double getFitness() const
        {
            return fitness;
        }

        inline int getSpeciesID() const
        {
            return speciesID;
        }

        inline void setSpeciesID(int _speciesID)
        {
            speciesID = _speciesID;
        }

        inline string getUserData()
        {
            return userData;
        }

        inline void setUserData(string data)
        {
            userData = data;
        }

        /**
         * getCompatibility: returns the compatibility between this individual and another
         */
        NEAT_DLL_EXPORT double getCompatibility(shared_ptr<GeneticIndividual> other);

        inline void setCanReproduce(bool _canReproduce)
        {
            canReproduce = _canReproduce;
        }

        inline bool getCanReproduce()
        {
            return canReproduce;
        }

        NEAT_DLL_EXPORT bool mutateAddLink();

        NEAT_DLL_EXPORT bool mutateAddNode(int fromNodeID=-1,int toNodeID=-1);

        NEAT_DLL_EXPORT void incrementAge();

        NEAT_DLL_EXPORT ModularNetwork *spawnPhenotype() const;

        NEAT_DLL_EXPORT ModularNetwork spawnPhenotypeStack() const;

        template<class Type>
        inline FastNetwork<Type> spawnFastPhenotypeStack() const
        {
            return FastNetwork<Type>(nodes,links);
        }

#ifdef EPLEX_INTERNAL
        inline FractalNetwork spawnFractalPhenotypeStack() const
        {
            return FractalNetwork(nodes,links);
        }

        template<class Type>
        inline VectorNetwork<Type> spawnVectorPhenotypeStack() const
        {
            return VectorNetwork<Type>(nodes,links);
        }
#endif

        NEAT_DLL_EXPORT void addNode(GeneticNodeGene node);

        NEAT_DLL_EXPORT void addLink(GeneticLinkGene link);

        NEAT_DLL_EXPORT bool isValid();
	protected:
    };
}

#endif
