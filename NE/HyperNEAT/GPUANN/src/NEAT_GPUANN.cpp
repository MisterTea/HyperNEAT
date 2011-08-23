#include "NEAT_Defines.h"

#include "NEAT_GPUANN.h"

#include "NEAT_Random.h"

#include "NEAT_GeneticIndividual.h"

#include "NEAT_Globals.h"

#include "NEAT_GeneticLinkGene.h"
#include "NEAT_GeneticNodeGene.h"

#define DEBUG_ACTIVATION_CALCULATION (0)

#define DEBUG_NETWORK_CREATION (0)

#define DEBUG_NETWORK_UPDATE (0)

namespace NEAT
{
    extern double signedSigmoidTable[6001];
    extern double unsignedSigmoidTable[6001];

    template<class Type>
    GPUANN<Type>::GPUANN(const vector<NetworkLayer<Type> > &_layers)
        :
        Network<Type>(),
        layers(_layers)
    {
        //Perform a sanity check on the layers
        for(size_t toLayer=0;toLayer<layers.size();toLayer++)
        {
            for(int a=0;a<(int)layers[toLayer].fromLayers.size();a++)
            {
                size_t fromLayer = layers[toLayer].fromLayers[a];

                if(fromLayer>=toLayer)
                {
                    throw CREATE_LOCATEDEXCEPTION_INFO("Network is not feed-forward!");
                }
            }
        }
    }

    template<class Type>
    GPUANN<Type>::GPUANN()
    {
    }

    template<class Type>
    GPUANN<Type>::~GPUANN()
    {
    }

    template<class Type>
    bool GPUANN<Type>::hasNode(const Node &nodeIndex)
    {
        if(nodeIndex.z>=(int)layers.size())
        {
            return false;
        }

        NetworkLayer<Type> &layer = layers[nodeIndex.z];

        int nodeArrayIndex = nodeIndex.y*layer.nodeStride + nodeIndex.x;
        if(nodeArrayIndex>(int)layer.nodeValues.size())
        {
            return false;
        }

        return true;
    }

    template<class Type>
    Type GPUANN<Type>::getValue(const Node &nodeIndex)
    {
        if(nodeIndex.z>=(int)layers.size())
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
        }

        NetworkLayer<Type> &layer = layers[nodeIndex.z];

        int nodeArrayIndex = nodeIndex.y*layer.nodeStride + nodeIndex.x;
        if(nodeArrayIndex>(int)layer.nodeValues.size())
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
        }

        return layer.nodeValues[nodeArrayIndex];
    }

    template<class Type>
    void GPUANN<Type>::setValue(const Node &nodeIndex,Type newValue)
    {
        if(nodeIndex.z>=(int)layers.size())
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
        }

        NetworkLayer<Type> &layer = layers[nodeIndex.z];

        int nodeArrayIndex = nodeIndex.y*layer.nodeStride + nodeIndex.x;
        if(nodeArrayIndex>(int)layer.nodeValues.size())
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
        }

        layer.nodeValues[nodeArrayIndex] = newValue;
    }

    template<class Type>
    Type GPUANN<Type>::getLink(const Node &fromNodeIndex,const Node &toNodeIndex)
    {
        if(toNodeIndex.z>=(int)layers.size() || fromNodeIndex.z>=(int)layers.size())
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
        }

        NetworkLayer<Type> &toLayer = layers[toNodeIndex.z];

        for(int a=0;a<(int)toLayer.fromLayers.size();a++)
        {
            if(toLayer.fromLayers[a] != fromNodeIndex.z)
            {
                //Not the source layer we are looking for
                continue;
            }

            NetworkLayer<Type> &fromLayer = layers[toLayer.fromLayers[a]];

            int fromNodeArrayIndex = fromNodeIndex.y*fromLayer.nodeStride + fromNodeIndex.x;
            int toNodeArrayIndex = toNodeIndex.y*toLayer.nodeStride + toNodeIndex.x;
            if(fromNodeArrayIndex>(int)fromLayer.nodeValues.size() || toNodeArrayIndex>(int)toLayer.nodeValues.size())
            {
                return 0;
            }

            return toLayer.fromWeights[a][toNodeArrayIndex][fromNodeArrayIndex];
        }

        return 0;
    }

    template<class Type>
    void GPUANN<Type>::setLink(const Node &fromNodeIndex,const Node &toNodeIndex,Type weight)
    {
        if(toNodeIndex.z>=(int)layers.size() || fromNodeIndex.z>=(int)layers.size())
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
        }

        NetworkLayer<Type> &toLayer = layers[toNodeIndex.z];

        for(int a=0;a<(int)toLayer.fromLayers.size();a++)
        {
            if(toLayer.fromLayers[a] != fromNodeIndex.z)
            {
                //Not the source layer we are looking for
                continue;
            }

            NetworkLayer<Type> &fromLayer = layers[toLayer.fromLayers[a]];

            int fromNodeArrayIndex = fromNodeIndex.y*fromLayer.nodeStride + fromNodeIndex.x;
            int toNodeArrayIndex = toNodeIndex.y*toLayer.nodeStride + toNodeIndex.x;
            if(fromNodeArrayIndex>(int)fromLayer.nodeValues.size() || toNodeArrayIndex>(int)toLayer.nodeValues.size())
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
            }

            toLayer.fromWeights[a][toNodeArrayIndex][fromNodeArrayIndex] = weight;
            return;
        }

        throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
    }

    template<class Type>
    void GPUANN<Type>::reinitialize()
    {
        for(size_t a=0;a<layers.size();a++)
        {
            layers[a].initialize();
        }
    }

    template<class Type>
    void GPUANN<Type>::update()
    {
        for(typename vector<NetworkLayer<Type> >::iterator layer = layers.begin();layer != layers.end();layer++)
        {
            for(size_t a=0;a<layer->fromLayers.size();a++)
            {
                const NetworkLayer<Type> &fromLayer = layers[layer->fromLayers[a]];

                const vector<Type> &fromNodes = fromLayer.nodeValues;
                const Type* fromNodesPtr = &(fromLayer.nodeValues[0]);
                int numFromNodes = (int)fromNodes.size();

                vector<Type> &toNodes = layer->nodeValues;
                int numToNodes = (int)toNodes.size();

                Type nodeValue;
                int fromNode;
                int toNode;
                Type* weightsPtr;
                for(toNode=0;toNode<numToNodes;toNode++)
                {
                    nodeValue=0;
                    weightsPtr = &(layer->fromWeights[a][toNode][0]);
                    for(fromNode=0;fromNode<numFromNodes;fromNode++)
                    {
                        nodeValue += fromNodesPtr[fromNode] * weightsPtr[fromNode];
                    }

                    //Signed sigmoid activation function
                    toNodes[toNode] = (2.0f / (1.0f + exp(-nodeValue))) - 1.0f;
                }
            }
        }
    }

    template class GPUANN<float>; // explicit instantiation
    template class GPUANN<double>; // explicit instantiation
}
