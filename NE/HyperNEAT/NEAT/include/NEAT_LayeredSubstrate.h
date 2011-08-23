#ifndef __NEAT_LAYERED_SUBSTRATE_H__
#define __NEAT_LAYERED_SUBSTRATE_H__

#include "NEAT_FastNetwork.h"
#include "NEAT_FastLayeredNetwork.h"
#include "NEAT_FastBiasNetwork.h"
#ifdef USE_GPU
#include "NEAT_GPUANN.h"
#endif

#define LAYERED_SUBSTRATE_ENABLE_BIASES (0)

namespace NEAT
{
    class LayeredSubstrateInfo
    {
    public:
		vector< string > layerNames;
		vector< JGTL::Vector2<int> > layerSizes;
		vector< JGTL::Vector2<int> > layerValidSizes;
		vector< std::pair<string,string> > layerAdjacencyList;
        vector< bool > layerIsInput;
		vector< JGTL::Vector3<float> > layerLocations;
		bool normalize;
        bool useOldOutputNames;
        int maxDeltaLength;
        int maxConnectionLength;

        LayeredSubstrateInfo()
            :
        normalize(true),
        useOldOutputNames(false),
        maxDeltaLength(1000000),
        maxConnectionLength(1000000)
        {
        }
    };

	template< class NetworkDataType >
	class LayeredSubstrate
	{
	public:
	protected:
#if LAYERED_SUBSTRATE_ENABLE_BIASES
        NEAT::FastBiasNetwork<NetworkDataType> network;
#else
#ifdef USE_GPU
        NEAT::GPUANN gpuNetwork;
#endif
        NEAT::FastLayeredNetwork<NetworkDataType> network;
#if 0
        NEAT::FastNetwork<NetworkDataType> originalNetwork;
#endif
#endif

        NodeMap nameLookup;

        vector< JGTL::Vector2<int> > layerSizes;
        vector< JGTL::Vector2<int> > layerValidSizes;
        vector< string > layerNames;
        vector< JGTL::Vector2<int> > layerAdjacencyList;
        vector< bool > layerIsInput;
		bool normalize;
        int maxDeltaLength;
        int maxConnectionLength;

        bool useOldOutputNames;

		//Location of the layer is only used for drawing purposes
		vector< JGTL::Vector3<float> > layerLocations;

	public:
		NEAT_DLL_EXPORT LayeredSubstrate();

		NEAT_DLL_EXPORT void setLayerInfo(LayeredSubstrateInfo layerInfo);

		NEAT_DLL_EXPORT void populateSubstrate(
			shared_ptr<NEAT::GeneticIndividual> individual
			);

		inline NetworkDataType convertOutputToWeight(
			NetworkDataType output
			)
		{
			if (fabs(output)>0.2f)
			{
				if (output>0.0f)
					output = ( ((output-0.2f)/0.8f)*3.0f );
				else
					output = ( ((output+0.2f)/0.8f)*3.0f );
			}
			else
			{
				output = (0.0f);
			}
			return output;
		}

		inline bool hasNode(const Node &node)
		{
			return network.hasNode(node);
		}

		NEAT_DLL_EXPORT NetworkDataType getValue(const Node &node);

		NEAT_DLL_EXPORT void setValue(const Node &node,NetworkDataType _value);

		inline int getNumLayers()
		{
			return (int)layerSizes.size();
		}

		inline const JGTL::Vector2<int> &getLayerSize(int z)
        {
            return layerSizes[z];
        }

		inline const JGTL::Vector3<float> &getLayerLocation(int z)
        {
            return layerLocations[z];
        }

#ifdef USE_GPU
        NEAT::GPUANN* getNetwork()
		{
			return &gpuNetwork;
		}
#else
#if LAYERED_SUBSTRATE_ENABLE_BIASES
        NEAT::FastBiasNetwork<NetworkDataType>* getNetwork()
		{
			return &network;
		}
#else
        NEAT::FastLayeredNetwork<NetworkDataType>* getNetwork()
		{
			return &network;
		}
#endif
#endif

		NEAT_DLL_EXPORT void getWeightRGB(float &r,float &g,float &b,const Node &currentNode,const Node &sourceNode);
		
		NEAT_DLL_EXPORT void getActivationRGB(float &r,float &g,float &b,const Node &currentNode);
		
		NEAT_DLL_EXPORT void dumpWeightsFrom(string directoryname,Node sourceNode);
		
		NEAT_DLL_EXPORT void dumpActivationLevels(string directoryname);

        inline int getLayerIndex(const string &name)
        {
            for(int a=0;a<int(layerNames.size());a++)
            {
                if(name==layerNames[a])
                {
                    return a;
                }
            }
            return -1;
        }
		
	protected:
	};
}

#endif
