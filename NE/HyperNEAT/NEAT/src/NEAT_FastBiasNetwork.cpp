#include "NEAT_Defines.h"

#include "NEAT_FastBiasNetwork.h"

#include "NEAT_Random.h"

#include "NEAT_GeneticIndividual.h"

#include "NEAT_Globals.h"

#include "NEAT_GeneticLinkGene.h"
#include "NEAT_GeneticNodeGene.h"

#define DEBUG_ACTIVATION_CALCULATION (0)

#define DEBUG_NETWORK (0)

namespace NEAT
{
	extern double signedSigmoidTable[6001];
	extern double unsignedSigmoidTable[6001];

	template<class Type>
	FastBiasNetwork<Type>::FastBiasNetwork(
		const vector<NetworkNode *> &_nodes,
		const vector<NetworkLink *> &_links,
		const vector<Type> &_nodeBiases
		)
		:
	Network<Type>(),
		numNodes(int(_nodes.size())),
		numLinks(int(_links.size()))
	{
		data = (char*)malloc(
			sizeof(Type)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<Type>)*numLinks +
			sizeof(Type)*numNodes
			);

		memset(
			data,
			0,
			sizeof(Type)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<Type>)*numLinks +
			sizeof(Type)*numNodes
			);

		nodeValues = (Type*)&data[0];
		nodeNewValues = (Type*)&data[sizeof(Type)*numNodes];
		activationFunctions = (ActivationFunction*)&data[sizeof(Type)*2*numNodes];
		links = (NetworkIndexedLink<Type>*)&data[
			sizeof(Type)*2*numNodes +
				sizeof(ActivationFunction)*numNodes];
			nodeBiases = (Type*)&data[
				sizeof(Type)*2*numNodes +
					sizeof(ActivationFunction)*numNodes +
					sizeof(NetworkIndexedLink<Type>)*numLinks
			];

			numConstantNodes = 0;

			map<const NetworkNode*,int> nodePointerToIndex;

#if DEBUG_NETWORK
			cout << "Pass #1\n";
#endif
			for (int a=0;a<(int)_nodes.size();a++)
			{
#if DEBUG_NETWORK
				cout << "On Node " << a << endl;
#endif
				if (!_nodes[a]->getUpdate())
				{
					nodeNameToIndex[_nodes[a]->getName()] = numConstantNodes;
					activationFunctions[numConstantNodes] = _nodes[a]->getActivationFunction();
					nodePointerToIndex[_nodes[a]] = numConstantNodes;
					numConstantNodes++;
				}
			}

			int currentNode = numConstantNodes;

#if DEBUG_NETWORK
			cout << "Pass #2\n";
#endif
			for (int a=0;a<(int)_nodes.size();a++)
			{
#if DEBUG_NETWORK
				cout << "On Node " << a << endl;
#endif
				if (_nodes[a]->getUpdate())
				{
					nodeNameToIndex[_nodes[a]->getName()] = currentNode;
					activationFunctions[currentNode] = _nodes[a]->getActivationFunction();
					nodePointerToIndex[_nodes[a]] = currentNode;
					currentNode++;
				}
			}

#if DEBUG_NETWORK
			cout << "Link Pass\n";
#endif
			for (int a=0;a<(int)_links.size();a++)
			{
#if DEBUG_NETWORK
				cout << "On Link " << a << endl;
#endif
				links[a].fromNode = nodePointerToIndex[_links[a]->getFromNode()];
				links[a].toNode = nodePointerToIndex[_links[a]->getToNode()];
				links[a].weight = (Type)_links[a]->getWeight();
			}

			for (int a=0;a<(int)_nodeBiases.size();a++)
			{
				nodeBiases[a] = _nodeBiases[a];
			}
	}

	template<class Type>
	FastBiasNetwork<Type>::FastBiasNetwork(
		const NetworkNode* _nodes,
		int _numNodes,
		const NetworkLink* _links,
		int _numLinks,
		const Type* _nodeBiases
		)
		:
	Network<Type>(),
		numNodes(_numNodes),
		numLinks(_numLinks)
	{
		data = (char*)malloc(
			sizeof(Type)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<Type>)*numLinks +
			sizeof(Type)*numNodes
			);

		memset(
			data,
			0,
			sizeof(Type)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<Type>)*numLinks +
			sizeof(Type)*numNodes
			);

		nodeValues = (Type*)&data[0];
		nodeNewValues = (Type*)&data[sizeof(Type)*numNodes];
		activationFunctions = (ActivationFunction*)&data[sizeof(Type)*2*numNodes];
		links = (NetworkIndexedLink<Type>*)&data[
			sizeof(Type)*2*numNodes +
				sizeof(ActivationFunction)*numNodes];
			nodeBiases = (Type*)&data[
				sizeof(Type)*2*numNodes +
					sizeof(ActivationFunction)*numNodes +
					sizeof(NetworkIndexedLink<Type>)*numLinks
			];

			numConstantNodes = 0;

			map<const NetworkNode*,int> nodePointerToIndex;

#if DEBUG_NETWORK
			cout << "Pass #1\n";
#endif
			for (int a=0;a<numNodes;a++)
			{
#if DEBUG_NETWORK
				cout << "On Node " << a << endl;
#endif
				if (!_nodes[a].getUpdate())
				{
					nodeNameToIndex[_nodes[a].getName()] = numConstantNodes;
					activationFunctions[numConstantNodes] = _nodes[a].getActivationFunction();
					nodePointerToIndex[&_nodes[a]] = numConstantNodes;
					numConstantNodes++;
				}
			}

			int currentNode = numConstantNodes;

#if DEBUG_NETWORK
			cout << "Pass #2\n";
#endif
			for (int a=0;a<numNodes;a++)
			{
#if DEBUG_NETWORK
				cout << "On Node " << a << endl;
#endif
				if (_nodes[a].getUpdate())
				{
					nodeNameToIndex[_nodes[a].getName()] = currentNode;
					activationFunctions[currentNode] = _nodes[a].getActivationFunction();
					nodePointerToIndex[&_nodes[a]] = currentNode;
					currentNode++;
				}
			}

#if DEBUG_NETWORK
			cout << "Link Pass\n";
#endif
			for (int a=0;a<numLinks;a++)
			{
#if DEBUG_NETWORK
				cout << "On Link " << a << endl;
#endif
				links[a].fromNode = nodePointerToIndex[_links[a].getFromNode()];
				links[a].toNode = nodePointerToIndex[_links[a].getToNode()];
				links[a].weight = (Type)_links[a].getWeight();
			}

			memcpy(nodeBiases,_nodeBiases,sizeof(Type)*numNodes);
	}

	/*
	FastBiasNetwork<Type>::FastBiasNetwork(
	const vector<GeneticNodeGene> &_nodes,
	const vector<GeneticLinkGene> &_links
	)
	:
	numNodes(_nodes.size()),
	numLinks(_links.size()),
	this->activated(false)
	{
	data = (char*)malloc(
	sizeof(Type)*2*numNodes +
	sizeof(ActivationFunction)*numNodes +
	sizeof(NetworkIndexedLink<Type>)*numLinks +
	sizeof(Type)*numNodes
	);
	memset(data,0,sizeof(data));

	nodeValues = (Type*)&data[0];
	nodeNewValues = (Type*)&data[sizeof(Type)*numNodes];
	activationFunctions = (ActivationFunction*)&data[sizeof(Type)*2*numNodes];
	links = (NetworkIndexedLink<Type>*)&data[
	sizeof(Type)*2*numNodes +
	sizeof(ActivationFunction)*numNodes];
	nodeBiases = (Type*)&data[
	sizeof(Type)*2*numNodes +
	sizeof(ActivationFunction)*numNodes +
	sizeof(NetworkIndexedLink<Type>)*numLinks
	];

	numConstantNodes = 0;

	map<const GeneticNodeGene*,int> nodePointerToIndex;
	map<int,int> nodeIDToIndex;

	#if DEBUG_NETWORK
	cout << "Pass #1\n";
	#endif
	for (int a=0;a<_nodes.size();a++)
	{
	#if DEBUG_NETWORK
	cout << "On Node " << a << endl;
	#endif
	if (_nodes[a].getType()==string("NetworkSensor"))
	{
	nodeNameToIndex[_nodes[a].getName()] = numConstantNodes;
	activationFunctions[numConstantNodes] = _nodes[a].getActivationFunction();
	nodePointerToIndex[&_nodes[a]] = numConstantNodes;
	nodeIDToIndex[_nodes[a].getID()] = numConstantNodes;
	numConstantNodes++;
	}
	}

	int currentNode = numConstantNodes;

	#if DEBUG_NETWORK
	cout << "Pass #2\n";
	#endif
	for (int a=0;a<_nodes.size();a++)
	{
	#if DEBUG_NETWORK
	cout << "On Node " << a << endl;
	#endif
	if (_nodes[a].getType()!=string("NetworkSensor"))
	{
	nodeNameToIndex[_nodes[a].getName()] = currentNode;
	activationFunctions[currentNode] = _nodes[a].getActivationFunction();
	nodePointerToIndex[&_nodes[a]] = currentNode;
	nodeIDToIndex[_nodes[a].getID()] = currentNode;
	currentNode++;
	}
	}

	#if DEBUG_NETWORK
	cout << "Link Pass\n";
	#endif
	for (int a=0;a<_links.size();a++)
	{
	#if DEBUG_NETWORK
	cout << "On Link " << a << endl;
	#endif
	links[a].fromNode = nodeIDToIndex[_links[a].getFromNodeID()];
	links[a].toNode = nodeIDToIndex[_links[a].getToNodeID()];
	links[a].weight = _links[a].getWeight();
	}
	}
	*/

	template<class Type>
	FastBiasNetwork<Type>::FastBiasNetwork()
		:
	Network<Type>(),
		numNodes(0),
		numLinks(0),
		data(NULL)
	{}

	template<class Type>
	FastBiasNetwork<Type>& FastBiasNetwork<Type>::operator=(const FastBiasNetwork<Type> &other)
	{
#if DEBUG_NETWORK
		cout << "Network assignment called\n";
#endif
		copyFrom(other);

		return *this;
	}

	template<class Type>
	FastBiasNetwork<Type>::FastBiasNetwork(const FastBiasNetwork<Type> &other)
		:
	data(NULL)
	{
#if DEBUG_NETWORK
		cout << "Network copy called!\n";
#endif
		copyFrom(other);
	}

	template<class Type>
	void FastBiasNetwork<Type>::copyFrom(const FastBiasNetwork &other)
	{
		if (this!=&other)
		{
			Network<Type>::copyFrom(other);

			numNodes = other.numNodes;
			numLinks = other.numLinks;
			nodeNameToIndex = other.nodeNameToIndex;
			numConstantNodes = other.numConstantNodes;

			data = (char*)realloc(
				data,
				sizeof(Type)*2*numNodes +
				sizeof(ActivationFunction)*numNodes +
				sizeof(NetworkIndexedLink<Type>)*numLinks +
				sizeof(Type)*numNodes
				);

			nodeValues = (Type*)&data[0];
			nodeNewValues = (Type*)&data[sizeof(Type)*numNodes];
			activationFunctions = (ActivationFunction*)&data[sizeof(Type)*2*numNodes];
			links = (NetworkIndexedLink<Type>*)&data[
				sizeof(Type)*2*numNodes +
					sizeof(ActivationFunction)*numNodes];
				nodeBiases = (Type*)&data[
					sizeof(Type)*2*numNodes +
						sizeof(ActivationFunction)*numNodes +
						sizeof(NetworkIndexedLink<Type>)*numLinks
				];


				if (other.data)
				{
					memcpy(
						data,
						other.data,
						sizeof(Type)*2*numNodes +
						sizeof(ActivationFunction)*numNodes +
						sizeof(NetworkIndexedLink<Type>)*numLinks +
						sizeof(Type)*numNodes
						);
				}

#if DEBUG_NETWORK
				cout << "Done!\n";
#endif
		}
	}

	template<class Type>
	FastBiasNetwork<Type>::~FastBiasNetwork()
	{
		if (data)
			free(data);
	}

	/*NetworkNode *Network::getNode(const string name)
	{
	for (int a=0;a<nodes.size();a++)
	{
	if (nodes[a]->getName()==name)
	return nodes[a];
	}

	throw CREATE_LOCATEDEXCEPTION_INFO("Could not find node by the name: ")+name+string(" !");
	}*/

	template<class Type>
	bool FastBiasNetwork<Type>::hasNode(const string &nodeName)
	{
		if (nodeNameToIndex.count(nodeName)>0)
			return true;
		else
			return false;
	}

	template<class Type>
	Type FastBiasNetwork<Type>::getValue(const string &nodeName)
	{
		if (!nodeNameToIndex.count(nodeName))
		{
			cout << "ERROR: Could not find node named " << nodeName << endl;
			throw (string("ERROR: Could not find node named ") + string(nodeName) + string("\n"));
		}
		else
		{
			return nodeValues[nodeNameToIndex[nodeName]];
		}
	}

	template<class Type>
	void FastBiasNetwork<Type>::setValue(const string &nodeName,Type newValue)
	{
		if (!nodeNameToIndex.count(nodeName))
		{
			cout << "ERROR: Could not find node named " << nodeName << endl;
			throw (string("ERROR: Could not find node named ") + string(nodeName) + string("\n"));
		}
		else
		{
			nodeValues[nodeNameToIndex[nodeName]] = newValue;
		}
	}

	template<class Type>
	void FastBiasNetwork<Type>::setBias(const string &nodeName,Type newBias)
	{
		if (!nodeNameToIndex.count(nodeName))
		{
			cout << "ERROR: Could not find node named " << nodeName << endl;
			throw (string("ERROR: Could not find node named ") + string(nodeName) + string("\n"));
		}
		else
		{
			nodeBiases[nodeNameToIndex[nodeName]] = newBias;
		}
	}

	template<class Type>
	Type FastBiasNetwork<Type>::getBias(const string &nodeName)
	{
		if (!nodeNameToIndex.count(nodeName))
		{
			cout << "ERROR: Could not find node named " << nodeName << endl;
			throw (string("ERROR: Could not find node named ") + string(nodeName) + string("\n"));
		}
		else
		{
			return nodeBiases[nodeNameToIndex[nodeName]];
		}
	}

	template<class Type>
	NetworkIndexedLink<Type> *FastBiasNetwork<Type>::getLink(const string &fromNodeName,const string &toNodeName)
	{
		int fromNodeIndex = nodeNameToIndex[fromNodeName];
		int toNodeIndex = nodeNameToIndex[toNodeName];

		if (!nodeNameToIndex.count(fromNodeName)||!nodeNameToIndex.count(toNodeName))
		{
			cout << "ERROR: Could not find node!" << endl;
			CREATE_PAUSE("PAUSE");
			throw CREATE_LOCATEDEXCEPTION_INFO(string("Could not find link from ")+fromNodeName+string(" to ")+toNodeName);
		}

		for (int a=0;a<numLinks;a++)
		{
			if (
				links[a].fromNode == fromNodeIndex &&
				links[a].toNode == toNodeIndex
				)
			{
				return &links[a];
			}
		}

		cout << string("Could not find link from ")+fromNodeName+string(" to ")+toNodeName+string("\n");
		CREATE_PAUSE("PAUSE");
		throw CREATE_LOCATEDEXCEPTION_INFO(string("Could not find link from ")+fromNodeName+string(" to ")+toNodeName);
	}

	template<class Type>
	void FastBiasNetwork<Type>::reinitialize()
	{
		this->activated=false;
		memset(nodeValues,0,sizeof(Type)*numNodes);
	}

	template<class Type>
	void FastBiasNetwork<Type>::updateFixedIterations(int iterations)
	{
		int count=iterations;
		if (!this->activated)
		{
			//count += 19; //run 19 extra times.
			//This should hopefully get everything
			//up to date on the first run

			count += Globals::getSingleton()->getExtraActivationUpdates();
			this->activated=true;
		}

		for (int a=0;a<count;a++)
		{
			/*for (int a=0;a<nodes.size();a++)
			{
			nodes[a]->computeNewValue();
			}*/
			memcpy(nodeNewValues,nodeBiases,sizeof(Type)*numNodes);

#if DEBUG_NETWORK
			cout << "NumLinks: " << numLinks << endl;
#endif
			for (int a=0;a<numLinks;a++)
			{
#if DEBUG_NETWORK
				cout << links[a].fromNode << "->" << links[a].toNode << " : " << links[a].weight << endl;
				cout << "Old value: " << nodeValues[links[a].fromNode] << endl;
				cout << "Node value changed from " << nodeNewValues[links[a].toNode];
#endif
				nodeNewValues[links[a].toNode] += nodeValues[links[a].fromNode]*links[a].weight;
#if DEBUG_NETWORK
				cout << " to " << nodeNewValues[links[a].toNode] << endl;
#endif
			}
			//cout << "Done computing values.  Updating...\n";

			/*for (int a=0;a<nodes.size();a++)
			{
			nodes[a]->updateValue();
			}*/

#if DEBUG_NETWORK
			cout << "Before Activation: " << endl;
			for (int a=0;a<numNodes;a++)
			{
				cout << a << ": " << nodeValues[a] << '/' << nodeNewValues[a] << ' ' << activationFunctions[a] << endl;
			}
#endif

			for (int a=numConstantNodes;a<numNodes;a++)
			{
				nodeNewValues[a] = runActivationFunction(nodeNewValues[a],activationFunctions[a]);
			}

#if DEBUG_NETWORK
			cout << "Before Copy: " << endl;
			for (int a=0;a<numNodes;a++)
			{
				cout << a << ": " << nodeValues[a] << '/' << nodeNewValues[a] << ' ' << activationFunctions[a] << endl;
			}

			cout << "NumNodes: " << numNodes << ". NumConstantNodes: " << numConstantNodes << endl;
			cout << "Copying " << (nodeValues+numConstantNodes) << " from " << (nodeNewValues+numConstantNodes)
				<< " Size: " << (sizeof(Type)*(numNodes-numConstantNodes)) << endl;
#endif

			memcpy(
				nodeValues+numConstantNodes,
				nodeNewValues+numConstantNodes,
				sizeof(Type)*(numNodes-numConstantNodes)
				);

#if DEBUG_NETWORK
			cout << "After Copy: " << endl;
			for (int a=0;a<numNodes;a++)
			{
				cout << a << ": " << nodeValues[a] << '/' << nodeNewValues[a] << endl;
			}

			system("PAUSE");
#endif

			/*
			for (int a=numConstantNodes;a<numNodes;a++)
			{
			nodeValues[a] += nodeBiases[a];
			}
			*/

			//cout << "Done updating values.\n";
		}
	}

	template<class Type>
	Type FastBiasNetwork<Type>::runActivationFunction(Type tmpVal,ActivationFunction function)
	{
		Type retVal;

		bool signedActivation=false;

		if (Globals::getSingleton()->hasSignedActivation())
		{
			signedActivation=true;
		}

#if DEBUG_ACTIVATION_CALCULATION
		cout << "Before: " << tmpVal;
#endif

		switch (function)
		{
		case ACTIVATION_FUNCTION_SIGMOID:
			//newValue = fsigmoid(tmpVal,4.924273,2.4621365);

			//try my own sigmoid

			if (signedActivation)
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Signed Sigmoid> ";
#endif
				//signed sigmoid
				if(Globals::getSingleton()->isUsingTanhSigmoid())
				{
					retVal = 2 / (1 + exp(-2 * tmpVal)) - 1;
				}
				else
				{
					if (tmpVal<-2.9||tmpVal>2.9)
					{
						retVal = (Type)( ((1.0 / (1+exp(-tmpVal))) - 0.5)*2.0 );
					}
					else
					{
						retVal = (Type)signedSigmoidTable[int(tmpVal*1000.0)+3000];
					}
				}
			}
			else
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Unsigned Sigmoid> ";
#endif
				//unsigned sigmoid
				if(Globals::getSingleton()->isUsingTanhSigmoid())
				{
					retVal = (1 + exp(-2 * tmpVal));
				}
				else
				{
					if (tmpVal<-2.9||tmpVal>2.9)
					{
						retVal = 1 / (1+exp(-tmpVal));
					}
					else
					{
						retVal = (Type)unsignedSigmoidTable[int(tmpVal*1000.0)+3000];
					}
				}
			}
			break;
		case ACTIVATION_FUNCTION_SIN:
			if (signedActivation)
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Signed Sin> ";
#endif
				retVal = sin(tmpVal);
			}
			else
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Unsigned Sin> ";
#endif
				retVal = (sin(tmpVal)+1)/2;
			}
			break;
		case ACTIVATION_FUNCTION_COS:
			if (signedActivation)
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Signed Cos> ";
#endif
				retVal = cos(tmpVal);
			}
			else
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Unsigned Cos> ";
#endif
				retVal = (cos(tmpVal)+1)/2;
			}
			break;
		case ACTIVATION_FUNCTION_GAUSSIAN:
			if (signedActivation)
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Unsigned Gaussian> ";
#endif
				retVal = exp(-pow(tmpVal,2));
			}
			else
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Unsigned Gaussian> ";
#endif
				retVal = exp(-pow(tmpVal,2));
			}
			break;
		case ACTIVATION_FUNCTION_SQUARE:
#if DEBUG_ACTIVATION_CALCULATION
			cout << " <Square> ";
#endif
			retVal = tmpVal*tmpVal;
			break;
		case ACTIVATION_FUNCTION_ABS_ROOT:
			if (signedActivation)
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Signed Root> ";
#endif
				if (tmpVal<0.0)
				{
					retVal = -sqrt(fabs(tmpVal));
				}
				else
				{
					retVal = sqrt(fabs(tmpVal));
				}
			}
			else
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Absolute Root> ";
#endif
				retVal = sqrt(fabs(tmpVal));
			}
			break;
		case ACTIVATION_FUNCTION_LINEAR:
			if (signedActivation)
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Signed Linear> ";
#endif
				retVal = min(max(tmpVal,(Type)-3.0),(Type)3.0) / (Type)3.0;
			}
			else
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Absolute Linear> ";
#endif
				retVal = ((min(max(tmpVal,(Type)-3.0),(Type)3.0) / (Type)3.0) + 1) / (Type)2.0;
			}
			break;
		case ACTIVATION_FUNCTION_ONES_COMPLIMENT:
			if (signedActivation)
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Signed Ones Compliment> ";
#endif
				Type tmpVal2 = min((Type)1.0,max((Type)-1.0,tmpVal/(Type)3.0));
				if (tmpVal>-0.1)
				{
					retVal = max((Type)0.0,tmpVal2);
					retVal = (Type)1.0-tmpVal2;
				}
				else
				{
					retVal = ((Type)-1.0) - tmpVal;
				}
			}
			else
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Absolute Ones Compliment> ";
#endif
				Type tmpVal2 = (Type)min(1.0,max(0.0,tmpVal/3.0));
				retVal = ((Type)1.0)-tmpVal2;
			}
			break;
		default:
			throw CREATE_LOCATEDEXCEPTION_INFO("Unknown activation function!!!");
			break;
		}

#if DEBUG_ACTIVATION_CALCULATION
		cout << " After: " << retVal << endl;
#endif

		return retVal;

	}

	template class FastBiasNetwork<float>; // explicit instantiation
	template class FastBiasNetwork<double>; // explicit instantiation
}
