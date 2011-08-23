#ifndef FASTLAYEREDNETWORK_H_INCLUDED
#define FASTLAYEREDNETWORK_H_INCLUDED

#include "NEAT_Network.h"

#include "NEAT_NetworkNode.h"
#include "NEAT_NetworkLink.h"
#include "NEAT_NetworkIndexedLink.h"

namespace NEAT
{
    template<class Type>
    class NetworkLayer
    {
    public:
        string name;
        vector<int> fromLayers;
        vector< vector< Type > > fromWeights;
        vector<Type> nodeValues;

        //The node stride is the number of nodes in a single row of a 2-D sheet
        int nodeStride;

        NetworkLayer()
        {
        }

        NetworkLayer(
            const string &_name,
            int numNodes,
            int _nodeStride,
            const vector<int> &_fromLayers,
            const vector<JGTL::Vector2<int> > &layerSizes
            )
            :
            name(_name),
            nodeStride(_nodeStride),
            fromLayers(_fromLayers)
        {
            nodeValues.resize(numNodes,0.0f);
            for(int a=0;a<int(fromLayers.size());a++)
            {
                fromWeights.push_back(
                    vector< Type >(
                        nodeValues.size()*layerSizes[a].x*layerSizes[a].y,0.0f
                        )
                    );
            }
        }

        inline void initialize()
        {
            memset(&nodeValues[0],0,sizeof(Type)*nodeValues.size());
        }
    };

    /**
     *  The FastLayeredNetwork class is designed to be faster at the cost
     *  of being less dynamic.  Adding/Removing links and nodes
     *  is not supported with this network.
     */
    template<class Type>
    class FastLayeredNetwork : public Network<Type>
    {
    protected:
        vector<NetworkLayer<Type> > layers;

    public:
        /**
         *  (Constructor) Create a Network with the inputed toplogy
         */
        NEAT_DLL_EXPORT FastLayeredNetwork(
            const vector<NetworkLayer<Type> > &_layers
        );

        /**
         *  (Constructor) Empty Constructor
         */
        NEAT_DLL_EXPORT FastLayeredNetwork();

        NEAT_DLL_EXPORT virtual ~FastLayeredNetwork();

        //NetworkNode *getNode(const string name);

        inline int getLayerIndex(const string &layerName)
        {
            for(int a=0;a<(int)layers.size();a++)
            {
                if(layers[a].name==layerName)
                {
                    return a;
                }
            }

            throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
        }

        /**
         *  getValue: gets the value for a specified node
         */
        NEAT_DLL_EXPORT bool hasNode(const Node &nodeIndex);

        /**
         *  getValue: gets the value for a specified node
         */
        NEAT_DLL_EXPORT Type getValue(const Node &nodeIndex);

        /**
         *  setValue: sets the value for a specified node
         */
        NEAT_DLL_EXPORT void setValue(const Node &nodeIndex,Type newValue);

        /**
         *  getLink: gets the link weight between two specified nodes
         */
        NEAT_DLL_EXPORT Type getLink(const Node &fromNodeIndex,const Node &toNodeIndex);

        /**
         *  setLink: gets the link weight between two specified nodes
         */
        NEAT_DLL_EXPORT void setLink(const Node &fromNodeIndex,const Node &toNodeIndex,Type weight);

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
        NEAT_DLL_EXPORT virtual void update();

    protected:
    };

}


#endif // FASTNETWORK_H_INCLUDED
