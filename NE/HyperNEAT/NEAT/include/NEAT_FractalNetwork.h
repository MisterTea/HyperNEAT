#ifndef NEAT_FRACTALNETWORK_H_INCLUDED
#define NEAT_FRACTALNETWORK_H_INCLUDED

#ifndef DOXYGEN_PROCESSOR

#include "NEAT_Network.h"

#include "NEAT_NetworkNode.h"
#include "NEAT_NetworkLink.h"

#include "NEAT_FastNetwork.h"

namespace NEAT
{

    class FractalNetwork : public Network<double>
    {
        int numNodes;
        int numLinks;
        map<string,int> nodeNameToIndex;
        char *data;
        double *nodeValues;
        double *nodeNewValues;
        ActivationFunction *activationFunctions;
        NetworkIndexedLink<double> *links;

        //vector<NetworkNode*> oldNodes;
        //vector<NetworkLink*> oldLinks;

        /**
         * numConstantNodes holds the index of the first node that is updated.  All nodes before
         * numConstantNodes are constant
         */
        int numConstantNodes;

        /**
         * numForwardLinks holds the index of the first link that is recurrent/loop.  All links before
         * numForwardLinks are non-recurrent non-loop
         */
        int numForwardLinks;

    public:
        /* Create a Network with the inputed toplogy */
        NEAT_DLL_EXPORT FractalNetwork(const vector<NetworkNode *> &_nodes,const vector<NetworkLink *> &_links);

        NEAT_DLL_EXPORT FractalNetwork(const NetworkNode* _nodes,int _numNodes,const NetworkLink* _links,int _numLinks);

        NEAT_DLL_EXPORT FractalNetwork(
            const vector<GeneticNodeGene> &_nodes,
            const vector<GeneticLinkGene> &_links
        );

        NEAT_DLL_EXPORT FractalNetwork();

        NEAT_DLL_EXPORT FractalNetwork& operator=(const FractalNetwork &other);

        NEAT_DLL_EXPORT FractalNetwork(const FractalNetwork &other);

        NEAT_DLL_EXPORT virtual ~FractalNetwork();

        //NetworkNode *getNode(const string name);

        NEAT_DLL_EXPORT virtual bool hasNode(const string &nodeName);

        NEAT_DLL_EXPORT double getValue(const string &nodeName);

        NEAT_DLL_EXPORT void setValue(const string &nodeName,double newValue);

        NetworkIndexedLink<double> *getLink(int index)
        {
            return &links[index];
        }

        NEAT_DLL_EXPORT NetworkIndexedLink<double> *getLink(const string &fromNodeName,const string &toNodeName);

        inline const int getLinkCount()
        {
            return numLinks;
        }

        /*This resets the state of the network to its initial state*/
        NEAT_DLL_EXPORT void reinitialize();

        NEAT_DLL_EXPORT virtual inline void updateFixedIterations(int iterations)
        {
            updateFixedIterations(iterations,1);
        }

        NEAT_DLL_EXPORT void updateFixedIterations(int iterations,int fractalIterations);

        NEAT_DLL_EXPORT int getLongestPathLength();

    protected:
        void copyFrom(const FractalNetwork &other);

        double runActivationFunction(double value,ActivationFunction function);
    };

}

#endif

#endif // NEAT_FRACTALNETWORK_H_INCLUDED
