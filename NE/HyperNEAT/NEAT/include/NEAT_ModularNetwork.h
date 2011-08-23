#ifndef NEAT_MODULARNETWORK_H_INCLUDED
#define NEAT_MODULARNETWORK_H_INCLUDED

#include "NEAT_Network.h"

#include "NEAT_NetworkNode.h"
#include "NEAT_NetworkLink.h"

namespace NEAT
{
    /**
     * ModularNetwork: This class is responsible for creating a neural network phenotype.
     * While this isn't as fast as FastNetwork, it allows you to have more control
     * over the network topology without creating a new network.
     */
    class ModularNetwork : public Network<double>
    {
        vector<NetworkNode *> nodes;
        vector<NetworkLink *> links;

    public:
        /**
         *  (Constructor) Create a ModularNetwork with the inputed toplogy
         */
        NEAT_DLL_EXPORT ModularNetwork(vector<NetworkNode *> &_nodes,vector<NetworkLink *> &_links);

        /**
         *  (Constructor) Empty Constructor
         */
        NEAT_DLL_EXPORT ModularNetwork();

        /**
         *  (Assignment) Copies the network
         */
        NEAT_DLL_EXPORT ModularNetwork& operator=(const ModularNetwork &other);

        /**
         *  (Copy Constructor) Copies the network
         */
        NEAT_DLL_EXPORT ModularNetwork(const ModularNetwork &other);

        NEAT_DLL_EXPORT void copyFrom(const ModularNetwork &other);

        NEAT_DLL_EXPORT virtual ~ModularNetwork();

        NEAT_DLL_EXPORT virtual bool hasNode(const string &nodeName);

        NEAT_DLL_EXPORT NetworkNode *getNode(const string &name);

        NEAT_DLL_EXPORT NetworkLink *getLink(const string &fromNodeName,const string &toNodeName);

        NetworkLink *getLink(int index)
        {
            return links[index];
        }

        inline int getLinkCount()
        {
            return (int)links.size();
        }

        void setValue(const string &nodeName,double value)
        {
            getNode(nodeName)->setValue(value);
        }

        double getValue(const string &nodeName)
        {
            return getNode(nodeName)->getValue();
        }

        /*This resets the state of the network to its initial state*/
        NEAT_DLL_EXPORT void reinitialize();

        NEAT_DLL_EXPORT void updateFixedIterations(int iterations);

        NEAT_DLL_EXPORT virtual void update()
        {
            updateFixedIterations(1);
        }

        NEAT_DLL_EXPORT int getLongestPathLength();
    };

}


#endif // NEAT_MODULARNETWORK_H_INCLUDED
