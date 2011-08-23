#include "NEAT_Defines.h"

#include "NEAT_FractalNetwork.h"

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

	FractalNetwork::FractalNetwork(const vector<NetworkNode *> &_nodes,const vector<NetworkLink *> &_links)
		:
	Network<double>(),
		numNodes(int(_nodes.size())),
		numLinks(int(_links.size())),
		numConstantNodes(0),
		numForwardLinks(0)
	{
		data = (char*)malloc(
			sizeof(double)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<double>)*numLinks
			);

		memset(
			data,
			0,
			sizeof(double)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<double>)*numLinks
			);

		nodeValues = (double*)&data[0];
		nodeNewValues = (double*)&data[sizeof(double)*numNodes];
		activationFunctions = (ActivationFunction*)&data[sizeof(double)*2*numNodes];
		links = (NetworkIndexedLink<double>*)&data[
			sizeof(double)*2*numNodes +
				sizeof(ActivationFunction)*numNodes];

			map<const NetworkNode*,int> nodePointerToIndex;

#if DEBUG_NETWORK
			cout << "Pass #1\n";
#endif
			int currentNode = 0;

			for (int a=0;a<(int)_nodes.size();a++)
			{
#if DEBUG_NETWORK
				cout << "On Node " << a << endl;
#endif
				if (!_nodes[a]->getUpdate())
				{
					nodeNameToIndex[_nodes[a]->getName()] = currentNode;
					activationFunctions[currentNode] = _nodes[a]->getActivationFunction();
					nodePointerToIndex[_nodes[a]] = currentNode;
					currentNode++;
				}
			}

			numConstantNodes = currentNode;

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
			int linkCount=0;

			for (int a=0;a<(int)_links.size();a++)
			{
				if (_links[a]->isForward())
				{
					links[linkCount].fromNode = nodePointerToIndex[_links[a]->getFromNode()];
					links[linkCount].toNode = nodePointerToIndex[_links[a]->getToNode()];
					links[linkCount].weight = _links[a]->getWeight();
					linkCount++;
				}
			}

			numForwardLinks = linkCount;

			for (int a=0;a<(int)_links.size();a++)
			{
				if (!_links[a]->isForward())
				{
					links[linkCount].fromNode = nodePointerToIndex[_links[a]->getFromNode()];
					links[linkCount].toNode = nodePointerToIndex[_links[a]->getToNode()];
					links[linkCount].weight = _links[a]->getWeight();
					linkCount++;
				}
			}
	}

	FractalNetwork::FractalNetwork(const NetworkNode* _nodes,int _numNodes,const NetworkLink* _links,int _numLinks)
		:
	Network<double>(),
		numNodes(_numNodes),
		numLinks(_numLinks),
		numConstantNodes(0),
		numForwardLinks(0)
	{
		data = (char*)malloc(
			sizeof(double)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<double>)*numLinks
			);

		memset(
			data,
			0,
			sizeof(double)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<double>)*numLinks
			);

		nodeValues = (double*)&data[0];
		nodeNewValues = (double*)&data[sizeof(double)*numNodes];
		activationFunctions = (ActivationFunction*)&data[sizeof(double)*2*numNodes];
		links = (NetworkIndexedLink<double>*)&data[
			sizeof(double)*2*numNodes +
				sizeof(ActivationFunction)*numNodes];

			map<const NetworkNode*,int> nodePointerToIndex;

			int currentNode = 0;

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
					nodeNameToIndex[_nodes[a].getName()] = currentNode;
					activationFunctions[currentNode] = _nodes[a].getActivationFunction();
					nodePointerToIndex[&_nodes[a]] = currentNode;
					currentNode++;
				}
			}

			numConstantNodes = currentNode;

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
			int linkCount=0;

			for (int a=0;a<numLinks;a++)
			{
				if (_links[a].isForward())
				{
					links[linkCount].fromNode = nodePointerToIndex[_links[a].getFromNode()];
					links[linkCount].toNode = nodePointerToIndex[_links[a].getToNode()];
					links[linkCount].weight = _links[a].getWeight();
					linkCount++;
				}
			}

			numForwardLinks = linkCount;

			for (int a=0;a<numLinks;a++)
			{
				if (!_links[a].isForward())
				{
					links[linkCount].fromNode = nodePointerToIndex[_links[a].getFromNode()];
					links[linkCount].toNode = nodePointerToIndex[_links[a].getToNode()];
					links[linkCount].weight = _links[a].getWeight();
					linkCount++;
				}
			}
	}

	FractalNetwork::FractalNetwork(
		const vector<GeneticNodeGene> &_nodes,
		const vector<GeneticLinkGene> &_links
		)
		:
	Network<double>(),
		numNodes(int(_nodes.size())),
		numLinks(int(_links.size())),
		numConstantNodes(0),
		numForwardLinks(0)
	{
		data = (char*)malloc(
			sizeof(double)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<double>)*numLinks
			);

		memset(
			data,
			0,
			sizeof(double)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<double>)*numLinks
			);

		nodeValues = (double*)&data[0];
		nodeNewValues = (double*)&data[sizeof(double)*numNodes];
		activationFunctions = (ActivationFunction*)&data[sizeof(double)*2*numNodes];
		links = (NetworkIndexedLink<double>*)&data[
			sizeof(double)*2*numNodes +
				sizeof(ActivationFunction)*numNodes];

			map<const GeneticNodeGene*,int> nodePointerToIndex;
			map<int,int> nodeIDToIndex;
			map<int,int> nodeIDToOriginalIndex;

			int currentNode = 0;

#if DEBUG_NETWORK
			cout << "Pass #1\n";
#endif
			for (int a=0;a<(int)_nodes.size();a++)
			{
#if DEBUG_NETWORK
				cout << "On Node " << a << endl;
#endif
				if (_nodes[a].getType()==string("NetworkSensor"))
				{
					nodeNameToIndex[_nodes[a].getName()] = currentNode;
					activationFunctions[currentNode] = _nodes[a].getActivationFunction();
					nodePointerToIndex[&_nodes[a]] = currentNode;
					nodeIDToIndex[_nodes[a].getID()] = currentNode;
					currentNode++;
				}

				nodeIDToOriginalIndex[_nodes[a].getID()] = a;
			}

			numConstantNodes = currentNode;

#if DEBUG_NETWORK
			cout << "Pass #2\n";
#endif
			for (int a=0;a<(int)_nodes.size();a++)
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
			int linkCount=0;

			for (int a=0;a<(int)_links.size();a++)
			{
				if (
					_nodes[nodeIDToOriginalIndex[_links[a].getFromNodeID()]].getDrawingPosition() <
					_nodes[nodeIDToOriginalIndex[_links[a].getToNodeID()]].getDrawingPosition()
					)
				{
					links[linkCount].fromNode = nodeIDToIndex[_links[a].getFromNodeID()];
					links[linkCount].toNode = nodeIDToIndex[_links[a].getToNodeID()];
					links[linkCount].weight = _links[a].getWeight();
					linkCount++;
				}
			}

			numForwardLinks = linkCount;

			for (int a=0;a<(int)_links.size();a++)
			{
				if (
					_nodes[nodeIDToOriginalIndex[_links[a].getFromNodeID()]].getDrawingPosition() >=
					_nodes[nodeIDToOriginalIndex[_links[a].getToNodeID()]].getDrawingPosition()
					)
				{
					links[linkCount].fromNode = nodeIDToIndex[_links[a].getFromNodeID()];
					links[linkCount].toNode = nodeIDToIndex[_links[a].getToNodeID()];
					links[linkCount].weight = _links[a].getWeight();
					linkCount++;
				}
			}
	}

	FractalNetwork::FractalNetwork()
		:
	Network<double>(),
		numNodes(0),
		numLinks(0),
		data(NULL),
		numConstantNodes(0),
		numForwardLinks(0)
	{}

	FractalNetwork& FractalNetwork::operator=(const FractalNetwork &other)
	{
#if DEBUG_NETWORK
		cout << "Network assignment called\n";
#endif
		copyFrom(other);

		return *this;
	}

	FractalNetwork::FractalNetwork(const FractalNetwork &other)
		:
	data(NULL)
	{
#if DEBUG_NETWORK
		cout << "Network copy called!\n";
#endif
		copyFrom(other);
	}

	void FractalNetwork::copyFrom(const FractalNetwork &other)
	{
		if (this!=&other)
		{
			Network<double>::copyFrom(other);

			numNodes = other.numNodes;
			numLinks = other.numLinks;
			nodeNameToIndex = other.nodeNameToIndex;
			numConstantNodes = other.numConstantNodes;
			numForwardLinks = other.numForwardLinks;

			data = (char*)realloc(
				data,
				sizeof(double)*2*numNodes +
				sizeof(ActivationFunction)*numNodes +
				sizeof(NetworkIndexedLink<double>)*numLinks
				);

			nodeValues = (double*)&data[0];
			nodeNewValues = (double*)&data[sizeof(double)*numNodes];
			activationFunctions = (ActivationFunction*)&data[sizeof(double)*2*numNodes];
			links = (NetworkIndexedLink<double>*)&data[
				sizeof(double)*2*numNodes +
					sizeof(ActivationFunction)*numNodes];

				if (other.data)
				{
					memcpy(
						data,
						other.data,
						sizeof(double)*2*numNodes +
						sizeof(ActivationFunction)*numNodes +
						sizeof(NetworkIndexedLink<double>)*numLinks
						);
				}

#if DEBUG_NETWORK
				cout << "Done!\n";
#endif
		}
	}

	FractalNetwork::~FractalNetwork()
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

	bool FractalNetwork::hasNode(const string &nodeName)
	{
		if (nodeNameToIndex.count(nodeName)>0)
			return true;
		else
			return false;
	}

	double FractalNetwork::getValue(const string &nodeName)
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

	void FractalNetwork::setValue(const string &nodeName,double newValue)
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

	NetworkIndexedLink<double> *FractalNetwork::getLink(const string &fromNodeName,const string &toNodeName)
	{
		int fromNodeIndex = nodeNameToIndex[fromNodeName];
		int toNodeIndex = nodeNameToIndex[toNodeName];

		if (!nodeNameToIndex.count(fromNodeName)||!nodeNameToIndex.count(toNodeName))
		{
			cout << "ERROR: Could not find node!" << endl;
			CREATE_PAUSE("PAUSE");
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

	void FractalNetwork::reinitialize()
	{
		activated=false;
		memset(nodeValues,0,sizeof(double)*numNodes);
	}

	void FractalNetwork::updateFixedIterations(int iterations,int fractalIterations)
	{
		int count=iterations;
		int fractalCount=fractalIterations;
		if (!activated)
		{
			//count += 19; //run 19 extra times.
			//This should hopefully get everything
			//up to date on the first run

			//count += int(Globals::getSingleton()->hasExtraActivationUpdates());
			//fractalCount += 1;
			fractalCount += Globals::getSingleton()->getExtraActivationUpdates();
			activated=true;
		}

		for (int b=0;b<fractalCount;b++)
		{

			for (int a=0;a<count;a++)
			{
				/*for (int a=0;a<nodes.size();a++)
				{
				nodes[a]->computeNewValue();
				}*/
				memset(nodeNewValues,0,sizeof(double)*numNodes);

#if DEBUG_NETWORK
				cout << "NumLinks: " << numLinks << endl;
#endif
				for (int a=0;a<numForwardLinks;a++)
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
					<< " Size: " << (sizeof(double)*(numNodes-numConstantNodes)) << endl;
#endif

				memcpy(
					nodeValues+numConstantNodes,
					nodeNewValues+numConstantNodes,
					sizeof(double)*(numNodes-numConstantNodes)
					);

#if DEBUG_NETWORK
				cout << "After Copy: " << endl;
				for (int a=0;a<numNodes;a++)
				{
					cout << a << ": " << nodeValues[a] << '/' << nodeNewValues[a] << endl;
				}

				system("PAUSE");
#endif

				//cout << "Done updating values.\n";
			}

			///BEGIN FRACTAL UPDATE

			if (b+1!=fractalCount)
			{

				/*for (int a=0;a<nodes.size();a++)
				{
				nodes[a]->computeNewValue();
				}*/
				memcpy(nodeNewValues,nodeValues,sizeof(double)*numNodes);

#if DEBUG_NETWORK
				cout << "NumLinks: " << numLinks << endl;
#endif
				for (int a=numForwardLinks;a<numLinks;a++)
				{
					nodeNewValues[links[a].toNode] = 0;
				}

				for (int a=numForwardLinks;a<numLinks;a++)
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
					<< " Size: " << (sizeof(double)*(numNodes-numConstantNodes)) << endl;
#endif

				memcpy(
					nodeValues+numConstantNodes,
					nodeNewValues+numConstantNodes,
					sizeof(double)*(numNodes-numConstantNodes)
					);

#if DEBUG_NETWORK
				cout << "After Copy: " << endl;
				for (int a=0;a<numNodes;a++)
				{
					cout << a << ": " << nodeValues[a] << '/' << nodeNewValues[a] << endl;
				}

				system("PAUSE");
#endif
			}

			//cout << "Done updating values.\n";

		}
	}

	double FractalNetwork::runActivationFunction(double tmpVal,ActivationFunction function)
	{
		double retVal;

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
						retVal = ((1 / (1+exp(-tmpVal))) - 0.5)*2.0;
					}
					else
					{
						retVal = signedSigmoidTable[int(tmpVal*1000.0)+3000];
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
						retVal = unsignedSigmoidTable[int(tmpVal*1000.0)+3000];
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
				retVal = min(max(tmpVal,-3.0),3.0) / 3.0;
			}
			else
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Absolute Linear> ";
#endif
				retVal = ((min(max(tmpVal,-3.0),3.0) / 3.0) + 1) / 2.0;
			}
			break;
		case ACTIVATION_FUNCTION_ONES_COMPLIMENT:
			if (signedActivation)
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Signed Ones Compliment> ";
#endif
				double tmpVal2 = min(1.0,max(-1.0,tmpVal/3.0));
				if (tmpVal>-0.1)
				{
					retVal = max(0.0,tmpVal2);
					retVal = 1.0-tmpVal2;
				}
				else
				{
					retVal = (-1.0) - tmpVal;
				}
			}
			else
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Absolute Ones Compliment> ";
#endif
				double tmpVal2 = min(1.0,max(0.0,tmpVal/3.0));
				retVal = 1.0-tmpVal2;
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
}
