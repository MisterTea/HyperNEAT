#include "NEAT_Defines.h"

#include "NEAT_VectorNetwork.h"

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
	VectorNetwork<Type>::VectorNetwork(const vector<NetworkNode *> &_nodes,const vector<NetworkLink *> &_links)
		:
	Network<Vector2<Type> >(),
		numNodes(int(_nodes.size())),
		numLinks(int(_links.size()))
	{
		data = (char*)malloc(
			sizeof(Vector2<Type>)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<Type>)*numLinks
			);

		memset(
			data,
			0,
			sizeof(double)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<double>)*numLinks
			);

		nodeValues = (Vector2<Type>*)&data[0];
		nodeNewValues = (Vector2<Type>*)&data[sizeof(Vector2<Type>)*numNodes];
		activationFunctions = (ActivationFunction*)&data[sizeof(Vector2<Type>)*2*numNodes];
		links = (NetworkIndexedLink<Type>*)&data[
			sizeof(Vector2<Type>)*2*numNodes +
				sizeof(ActivationFunction)*numNodes];

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
	}

	template<class Type>
	VectorNetwork<Type>::VectorNetwork(const NetworkNode* _nodes,int _numNodes,const NetworkLink* _links,int _numLinks)
		:
	Network<Vector2<Type> >(),
		numNodes(_numNodes),
		numLinks(_numLinks)
	{
		data = (char*)malloc(
			sizeof(Vector2<Type>)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<Type>)*numLinks
			);

		memset(
			data,
			0,
			sizeof(Vector2<Type>)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<Type>)*numLinks
			);

		nodeValues = (Vector2<Type>*)&data[0];
		nodeNewValues = (Vector2<Type>*)&data[sizeof(Vector2<Type>)*numNodes];
		activationFunctions = (ActivationFunction*)&data[sizeof(Vector2<Type>)*2*numNodes];
		links = (NetworkIndexedLink<Type>*)&data[
			sizeof(Vector2<Type>)*2*numNodes +
				sizeof(ActivationFunction)*numNodes];

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
	}

	template<class Type>
	VectorNetwork<Type>::VectorNetwork(
		const vector<GeneticNodeGene> &_nodes,
		const vector<GeneticLinkGene> &_links
		)
		:
	Network<Vector2<Type> >(),
		numNodes(int(_nodes.size())),
		numLinks(int(_links.size()))
	{
		data = (char*)malloc(
			sizeof(Vector2<Type>)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<Type>)*numLinks
			);

		memset(
			data,
			0,
			sizeof(Vector2<Type>)*2*numNodes +
			sizeof(ActivationFunction)*numNodes +
			sizeof(NetworkIndexedLink<Type>)*numLinks
			);

		nodeValues = (Vector2<Type>*)&data[0];
		nodeNewValues = (Vector2<Type>*)&data[sizeof(Vector2<Type>)*numNodes];
		activationFunctions = (ActivationFunction*)&data[sizeof(Vector2<Type>)*2*numNodes];
		links = (NetworkIndexedLink<Type>*)&data[
			sizeof(Vector2<Type>)*2*numNodes +
				sizeof(ActivationFunction)*numNodes];

			numConstantNodes = 0;

			map<const GeneticNodeGene*,int> nodePointerToIndex;
			map<int,int> nodeIDToIndex;

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
			for (int a=0;a<(int)_links.size();a++)
			{
#if DEBUG_NETWORK
				cout << "On Link " << a << endl;
#endif
				links[a].fromNode = nodeIDToIndex[_links[a].getFromNodeID()];
				links[a].toNode = nodeIDToIndex[_links[a].getToNodeID()];
				links[a].weight = (Type)_links[a].getWeight();
			}
	}

	template<class Type>
	VectorNetwork<Type>::VectorNetwork()
		:
	Network<Vector2<Type> >(),
		numNodes(0),
		numLinks(0),
		data(NULL)
	{}

	template<class Type>
	VectorNetwork<Type>& VectorNetwork<Type>::operator=(const VectorNetwork<Type> &other)
	{
#if DEBUG_NETWORK
		cout << "Network assignment called\n";
#endif
		copyFrom(other);

		return *this;
	}

	template<class Type>
	VectorNetwork<Type>::VectorNetwork(const VectorNetwork<Type> &other)
		:
	data(NULL)
	{
#if DEBUG_NETWORK
		cout << "Network copy called!\n";
#endif
		copyFrom(other);
	}

	template<class Type>
	void VectorNetwork<Type>::copyFrom(const VectorNetwork<Type> &other)
	{
		if (this!=&other)
		{
			Network<Vector2<Type> >::copyFrom(other);

			numNodes = other.numNodes;
			numLinks = other.numLinks;
			nodeNameToIndex = other.nodeNameToIndex;
			numConstantNodes = other.numConstantNodes;

			data = (char*)realloc(
				data,
				sizeof(Vector2<Type>)*2*numNodes +
				sizeof(ActivationFunction)*numNodes +
				sizeof(NetworkIndexedLink<Type>)*numLinks
				);

			nodeValues = (Vector2<Type>*)&data[0];
			nodeNewValues = (Vector2<Type>*)&data[sizeof(Vector2<Type>)*numNodes];
			activationFunctions = (ActivationFunction*)&data[sizeof(Vector2<Type>)*2*numNodes];
			links = (NetworkIndexedLink<Type>*)&data[
				sizeof(Vector2<Type>)*2*numNodes +
					sizeof(ActivationFunction)*numNodes];

				if (other.data)
				{
					memcpy(
						data,
						other.data,
						sizeof(Vector2<Type>)*2*numNodes +
						sizeof(ActivationFunction)*numNodes +
						sizeof(NetworkIndexedLink<Type>)*numLinks
						);
				}

#if DEBUG_NETWORK
				cout << "Done!\n";
#endif
		}
	}

	template<class Type>
	VectorNetwork<Type>::~VectorNetwork()
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
	bool VectorNetwork<Type>::hasNode(const string &nodeName)
	{
		if (nodeNameToIndex.count(nodeName)>0)
			return true;
		else
			return false;
	}

	template<class Type>
	Vector2<Type> VectorNetwork<Type>::getValue(const string &nodeName)
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
	void VectorNetwork<Type>::setValue(const string &nodeName,Vector2<Type> newValue)
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
	NetworkIndexedLink<Type> *VectorNetwork<Type>::getLink(const string &fromNodeName,const string &toNodeName)
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

	template<class Type>
	void VectorNetwork<Type>::reinitialize()
	{
		bool dummy=false;
		this->setActivated(dummy);
		memset(nodeValues,0,sizeof(Vector2<Type>)*numNodes);
	}

	template<class Type>
	void VectorNetwork<Type>::updateFixedIterations(int iterations)
	{
		int count=iterations;
		if (!this->getActivated())
		{
			//count += 19; //run 19 extra times.
			//This should hopefully get everything
			//up to date on the first run

			count += Globals::getSingleton()->getExtraActivationUpdates();
			this->setActivated(true);
		}

		for (int a=0;a<count;a++)
		{
			/*for (int a=0;a<nodes.size();a++)
			{
			nodes[a]->computeNewValue();
			}*/
			memset(nodeNewValues,0,sizeof(Vector2<Type>)*numNodes);

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
				<< " Size: " << (sizeof(Vector2<Type>)*(numNodes-numConstantNodes)) << endl;
#endif

			memcpy(
				nodeValues+numConstantNodes,
				nodeNewValues+numConstantNodes,
				sizeof(Vector2<Type>)*(numNodes-numConstantNodes)
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
	}

	template<class Type>
	Vector2<Type> VectorNetwork<Type>::runActivationFunction(Vector2<Type> tmpVal,ActivationFunction function)
	{
		Vector2<Type> retVal;

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
					retVal.x = 2 / (1 + exp(-2 * tmpVal.x)) - 1;
					retVal.y = 2 / (1 + exp(-2 * tmpVal.y)) - 1;
				}
				else
				{
					if (tmpVal.x<-2.9||tmpVal.x>2.9)
					{
						retVal.x = ((1 / (1+exp(-tmpVal.x))) - (Type)0.5)*(Type)2.0;
					}
					else
					{
						retVal.x = (Type)signedSigmoidTable[int(tmpVal.x*(Type)1000.0)+3000];
					}

					if (tmpVal.y<-2.9||tmpVal.y>2.9)
					{
						retVal.y = (Type)( ((1.0 / (1+exp(-tmpVal.y))) - 0.5)*2.0 );
					}
					else
					{
						retVal.y = (Type)signedSigmoidTable[int(tmpVal.y*1000.0)+3000];
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
					retVal.x = (1 + exp(-2 * tmpVal.x));
					retVal.y = (1 + exp(-2 * tmpVal.y));
				}
				else
				{
					if (tmpVal.x<-2.9||tmpVal.x>2.9)
					{
						retVal.x = 1 / (1+exp(-tmpVal.x));
					}
					else
					{
						retVal.x = (Type)unsignedSigmoidTable[int(tmpVal.x*1000.0)+3000];
					}

					if (tmpVal.y<-2.9||tmpVal.y>2.9)
					{
						retVal.y = 1 / (1+exp(-tmpVal.y));
					}
					else
					{
						retVal.y = (Type)unsignedSigmoidTable[int(tmpVal.y*1000.0)+3000];
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
				retVal.x = sin(tmpVal.x);
				retVal.y = sin(tmpVal.y);

				/* Rotation */
				/*
				double tempx = retVal.x;

				retVal.x = retVal.x*cos(90.0) + retVal.y*sin(90.0);
				retVal.y = retVal.y*cos(90.0) - tempx*sin(90.0);
				*/
			}
			else
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Unsigned Sin> ";
#endif
				retVal.x = (sin(tmpVal.x)+1)/2;
				retVal.y = (sin(tmpVal.y)+1)/2;
			}
			break;
		case ACTIVATION_FUNCTION_COS:
			if (signedActivation)
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Signed Cos> ";
#endif
				retVal.x = cos(tmpVal.x);
				retVal.y = cos(tmpVal.y);
			}
			else
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Unsigned Cos> ";
#endif
				retVal.x = (cos(tmpVal.x)+1)/2;
				retVal.y = (cos(tmpVal.y)+1)/2;
			}
			break;
		case ACTIVATION_FUNCTION_GAUSSIAN:
			if (signedActivation)
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Unsigned Gaussian> ";
#endif
				retVal.x = exp(-pow(tmpVal.x,2));
				retVal.y = exp(-pow(tmpVal.y,2));
			}
			else
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Unsigned Gaussian> ";
#endif
				retVal.x = exp(-pow(tmpVal.x,2));
				retVal.y = exp(-pow(tmpVal.y,2));
			}
			break;
		case ACTIVATION_FUNCTION_SQUARE:
#if DEBUG_ACTIVATION_CALCULATION
			cout << " <Square> ";
#endif
			//Not used
			break;
		case ACTIVATION_FUNCTION_ABS_ROOT:
			if (signedActivation)
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Signed Root> ";
#endif
				if (tmpVal.x<0.0)
				{
					retVal.x = -sqrt(fabs(tmpVal.x));
				}
				else
				{
					retVal.x = sqrt(fabs(tmpVal.x));
				}
				if (tmpVal.y<0.0)
				{
					retVal.y = -sqrt(fabs(tmpVal.y));
				}
				else
				{
					retVal.y = sqrt(fabs(tmpVal.y));
				}
			}
			else
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Absolute Root> ";
#endif
				retVal.x = sqrt(fabs(tmpVal.x));
				retVal.y = sqrt(fabs(tmpVal.y));
			}
			break;
		case ACTIVATION_FUNCTION_LINEAR:
			if (signedActivation)
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Signed Linear> ";
#endif
				retVal.x = min(max(tmpVal.x,(Type)-3.0),(Type)3.0) / (Type)3.0;
				retVal.y = min(max(tmpVal.y,(Type)-3.0),(Type)3.0) / (Type)3.0;
			}
			else
			{
#if DEBUG_ACTIVATION_CALCULATION
				cout << " <Absolute Linear> ";
#endif
				retVal.x = ((min(max(tmpVal.x,(Type)-3.0),(Type)3.0) / (Type)3.0) + 1) / (Type)2.0;
				retVal.y = ((min(max(tmpVal.y,(Type)-3.0),(Type)3.0) / (Type)3.0) + 1) / (Type)2.0;
			}
			break;
		case ACTIVATION_FUNCTION_ONES_COMPLIMENT:
			//not set up

			/*
			if (signedActivation)
			{
			#if DEBUG_ACTIVATION_CALCULATION
			cout << " <Signed Ones Compliment> ";
			#endif
			Vector2<Type> tmpVal2 = min(1.0,max(-1.0,tmpVal/3.0));
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
			Vector2<Type> tmpVal2 = min(1.0,max(0.0,tmpVal/3.0));
			retVal = 1.0-tmpVal2;
			}
			*/
			retVal = tmpVal;
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

	template class VectorNetwork<float>; // explicit instantiation
	template class VectorNetwork<double>; // explicit instantiation
}
