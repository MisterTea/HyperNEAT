#ifndef NEAT_FASTBIASNETWORK_H_INCLUDED
#define NEAT_FASTBIASNETWORK_H_INCLUDED

#include "NEAT_Network.h"

#include "NEAT_NetworkNode.h"
#include "NEAT_NetworkLink.h"
#include "NEAT_NetworkIndexedLink.h"

namespace NEAT
{

    /**
     *  The FastBiasNetwork class is designed to be faster at the cost
     *  of being less dynamic.  Adding/Removing links and nodes
     *  is not supported with this network.
     */
    template<class Type>
    class FastBiasNetwork : public Network<Type>
    {
        int numNodes;
        int numLinks;
        map<string,int> nodeNameToIndex;
        char *data;
        Type *nodeValues;
        Type *nodeNewValues;
        ActivationFunction *activationFunctions;
        NetworkIndexedLink<Type> *links;
        Type *nodeBiases;
        bool activated;

        /**
         * numConstantNodes holds the index of the first node that is updated.  All nodes before
         * numConstantNodes are constant
         */
        int numConstantNodes;

    public:
        /**
         *  (Constructor) Create a Network with the inputed toplogy
         */
        NEAT_DLL_EXPORT FastBiasNetwork(
            const vector<NetworkNode *> &_nodes,
            const vector<NetworkLink *> &_links,
            const vector<Type> &_nodeBiases
        );

        /**
         *  (Constructor) Create a Network with the inputed toplogy
         */
        NEAT_DLL_EXPORT FastBiasNetwork(
            const NetworkNode* _nodes,
            int _numNodes,
            const NetworkLink* _links,
            int _numLinks,
            const Type* _nodeBiases
        );

        /**
         *  (Constructor) Create a Network with the inputed geneology
         */
        /*
        NEAT_DLL_EXPORT FastBiasNetwork(
            const vector<GeneticNodeGene> &_nodes,
            const vector<GeneticLinkGene> &_links
        );
        */

        /**
         *  (Constructor) Empty Constructor
         */
        NEAT_DLL_EXPORT FastBiasNetwork();

        /**
         *  (Assignment) Copies the network
         */
        NEAT_DLL_EXPORT FastBiasNetwork& operator=(const FastBiasNetwork &other);

        /**
         *  (Copy Constructor) Copies the network
         */
        NEAT_DLL_EXPORT FastBiasNetwork(const FastBiasNetwork &other);

        NEAT_DLL_EXPORT virtual ~FastBiasNetwork();

        //NetworkNode *getNode(const string name);

        /**
         *  hasNode: Returns "true" if the node with the specified name
         *  exists
         */
        NEAT_DLL_EXPORT virtual bool hasNode(const string &nodeName);

        /**
         *  getValue: gets the value for a specified node
         */
        NEAT_DLL_EXPORT Type getValue(const string &nodeName);

        /**
         *  setValue: sets the value for a specified node
         */
        NEAT_DLL_EXPORT void setValue(const string &nodeName,Type newValue);

        /**
         *  setValue: sets the value for a specified node
         */
        NEAT_DLL_EXPORT void setBias(const string &nodeName,Type newBias);

        NEAT_DLL_EXPORT Type getBias(const string &nodeName);

        /**
         *  getLink: gets the link according to its index when created
         */
        NetworkIndexedLink<Type> *getLink(int index)
        {
            return &links[index];
        }

        /**
         *  getLink: gets the link according to the names
         *  of it's nodes
         */
        NEAT_DLL_EXPORT NetworkIndexedLink<Type> *getLink(const string &fromNodeName,const string &toNodeName);

        /**
         *  getLinkCount: gets the number of links
         */
        inline const int getLinkCount()
        {
            return numLinks;
        }

        /**
         * reinitialize: This resets the state of the network
         * to its initial state
         */
        NEAT_DLL_EXPORT void reinitialize();

        /**
         * update: This updates the network.
         * If the network has not been updated since construction or
         * reinitialize(), the network will be activated.  This means
         * it will update (1+ExtraActivationUpdates+iterations) times!
         * Otherwise, it will update (iterations) times.  If you do not
         * want the extra updates, call dummyActivation() before the first
         * update.
         */
        NEAT_DLL_EXPORT void updateFixedIterations(int iterations);

        NEAT_DLL_EXPORT virtual void update()
        {
            updateFixedIterations(1);
        }

    protected:
        void copyFrom(const FastBiasNetwork &other);

        Type runActivationFunction(Type value,ActivationFunction function);
    };

}


#endif // NEAT_FASTBIASNETWORK_H_INCLUDED
