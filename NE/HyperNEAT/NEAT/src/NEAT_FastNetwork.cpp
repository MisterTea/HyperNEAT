#include "NEAT_Defines.h"

#include "NEAT_FastNetwork.h"

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
    FastNetwork<Type>::FastNetwork(const vector<NetworkNode *> &_nodes,const vector<NetworkLink *> &_links)
        :
    Network<Type>(),
        numNodes(int(_nodes.size())),
        numLinks(int(_links.size()))
    {
        data = (char*)malloc(
            sizeof(Type)*2*numNodes +
            sizeof(ActivationFunction)*numNodes +
            sizeof(NetworkIndexedLink<Type>)*numLinks
            );

        memset(
            data,
            0,
            sizeof(Type)*2*numNodes +
            sizeof(ActivationFunction)*numNodes +
            sizeof(NetworkIndexedLink<Type>)*numLinks
            );

        nodeValues = (Type*)&data[0];
        nodeNewValues = (Type*)&data[sizeof(Type)*numNodes];
        activationFunctions = (ActivationFunction*)&data[sizeof(Type)*2*numNodes];
        links = (NetworkIndexedLink<Type>*)&data[
            sizeof(Type)*2*numNodes +
                sizeof(ActivationFunction)*numNodes];

            numConstantNodes = 0;

            map<const NetworkNode*,int> nodePointerToIndex;

#if DEBUG_NETWORK_CREATION
            cout << "Pass #1\n";
#endif
            for (int a=0;a<(int)_nodes.size();a++)
            {
#if DEBUG_NETWORK_CREATION
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

#if DEBUG_NETWORK_CREATION
            cout << "Pass #2\n";
#endif
            for (int a=0;a<(int)_nodes.size();a++)
            {
#if DEBUG_NETWORK_CREATION
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

#if DEBUG_NETWORK_CREATION
            cout << "Link Pass\n";
#endif
            for (int a=0;a<(int)_links.size();a++)
            {
#if DEBUG_NETWORK_CREATION
                cout << "On Link " << a << endl;
#endif
                links[a].fromNode = nodePointerToIndex[_links[a]->getFromNode()];
                links[a].toNode = nodePointerToIndex[_links[a]->getToNode()];
                links[a].weight = (Type)_links[a]->getWeight();

                nodeLinkMap[pair<int,int>(links[a].fromNode,links[a].toNode)] = a;
            }
    }

    template<class Type>
    FastNetwork<Type>::FastNetwork(const NetworkNode* _nodes,int _numNodes,const NetworkLink* _links,int _numLinks)
        :
    Network<Type>(),
        numNodes(_numNodes),
        numLinks(_numLinks)
    {
        data = (char*)malloc(
            sizeof(Type)*2*numNodes +
            sizeof(ActivationFunction)*numNodes +
            sizeof(NetworkIndexedLink<Type>)*numLinks
            );

        memset(
            data,
            0,
            sizeof(Type)*2*numNodes +
            sizeof(ActivationFunction)*numNodes +
            sizeof(NetworkIndexedLink<Type>)*numLinks
            );

        nodeValues = (Type*)&data[0];
        nodeNewValues = (Type*)&data[sizeof(Type)*numNodes];
        activationFunctions = (ActivationFunction*)&data[sizeof(Type)*2*numNodes];
        links = (NetworkIndexedLink<Type>*)&data[
            sizeof(Type)*2*numNodes +
                sizeof(ActivationFunction)*numNodes];

            numConstantNodes = 0;

            map<const NetworkNode*,int> nodePointerToIndex;

#if DEBUG_NETWORK_CREATION
            cout << "Pass #1\n";
#endif
            for (int a=0;a<numNodes;a++)
            {
#if DEBUG_NETWORK_CREATION
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

#if DEBUG_NETWORK_CREATION
            cout << "Pass #2\n";
#endif
            for (int a=0;a<numNodes;a++)
            {
#if DEBUG_NETWORK_CREATION
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

#if DEBUG_NETWORK_CREATION
            cout << "Link Pass\n";
#endif
            for (int a=0;a<numLinks;a++)
            {
#if DEBUG_NETWORK_CREATION
                cout << "On Link " << a << endl;
#endif
                links[a].fromNode = nodePointerToIndex[_links[a].getFromNode()];
                links[a].toNode = nodePointerToIndex[_links[a].getToNode()];
                links[a].weight = (Type)_links[a].getWeight();

                nodeLinkMap[pair<int,int>(links[a].fromNode,links[a].toNode)] = a;
            }
    }

    template<class Type>
    FastNetwork<Type>::FastNetwork(
        const vector<GeneticNodeGene> &_nodes,
        const vector<GeneticLinkGene> &_links
        )
        :
    Network<Type>(),
        numNodes(int(_nodes.size())),
        numLinks(int(_links.size()))
    {
        data = (char*)malloc(
            sizeof(Type)*2*numNodes +
            sizeof(ActivationFunction)*numNodes +
            sizeof(NetworkIndexedLink<Type>)*numLinks
            );

        memset(
            data,
            0,
            sizeof(Type)*2*numNodes +
            sizeof(ActivationFunction)*numNodes +
            sizeof(NetworkIndexedLink<Type>)*numLinks
            );

        nodeValues = (Type*)&data[0];
        nodeNewValues = (Type*)&data[sizeof(Type)*numNodes];
        activationFunctions = (ActivationFunction*)&data[sizeof(Type)*2*numNodes];
        links = (NetworkIndexedLink<Type>*)&data[
            sizeof(Type)*2*numNodes +
                sizeof(ActivationFunction)*numNodes];

            numConstantNodes = 0;

            map<const GeneticNodeGene*,int> nodePointerToIndex;
            map<int,int> nodeIDToIndex;

#if DEBUG_NETWORK_CREATION
            cout << "Pass #1\n";
#endif
            for (int a=0;a<(int)_nodes.size();a++)
            {
#if DEBUG_NETWORK_CREATION
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

#if DEBUG_NETWORK_CREATION
            cout << "Pass #2\n";
#endif
            for (int a=0;a<(int)_nodes.size();a++)
            {
#if DEBUG_NETWORK_CREATION
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

#if DEBUG_NETWORK_CREATION
            cout << "Link Pass\n";
#endif
            for (int a=0;a<(int)_links.size();a++)
            {
#if DEBUG_NETWORK_CREATION
                cout << "On Link " << a << endl;
#endif
                links[a].fromNode = nodeIDToIndex[_links[a].getFromNodeID()];
                links[a].toNode = nodeIDToIndex[_links[a].getToNodeID()];
                links[a].weight = (Type)_links[a].getWeight();

                nodeLinkMap[pair<int,int>(links[a].fromNode,links[a].toNode)] = a;
            }

    }

    template<class Type>
    FastNetwork<Type>::FastNetwork()
        :
    Network<Type>(),
        numNodes(0),
        numLinks(0),
        data(NULL)
    {
	}

    template<class Type>
    FastNetwork<Type>& FastNetwork<Type>::operator=(const FastNetwork<Type> &other)
    {
#if DEBUG_NETWORK_CREATION
        cout << "Network assignment called\n";
#endif
        copyFrom(other);

        return *this;
    }

    template<class Type>
    FastNetwork<Type>::FastNetwork(const FastNetwork<Type> &other)
        :
    data(NULL)
    {
#if DEBUG_NETWORK_CREATION
        cout << "Network copy called!\n";
#endif
        copyFrom(other);
    }

    template<class Type>
    void FastNetwork<Type>::copyFrom(const FastNetwork<Type> &other)
    {
        if (this!=(&other))
        {
            Network<Type>::copyFrom(other);

            numNodes = other.numNodes;
            numLinks = other.numLinks;
            nodeNameToIndex = other.nodeNameToIndex;
            numConstantNodes = other.numConstantNodes;
            nodeLinkMap = other.nodeLinkMap;

            data = (char*)realloc(
                data,
                sizeof(Type)*2*numNodes +
                sizeof(ActivationFunction)*numNodes +
                sizeof(NetworkIndexedLink<Type>)*numLinks
                );

            nodeValues = (Type*)&data[0];
            nodeNewValues = (Type*)&data[sizeof(Type)*numNodes];
            activationFunctions = (ActivationFunction*)&data[sizeof(Type)*2*numNodes];
            links = (NetworkIndexedLink<Type>*)&data[
                sizeof(Type)*2*numNodes +
                    sizeof(ActivationFunction)*numNodes];

                if (other.data)
                {
                    memcpy(
                        data,
                        other.data,
                        sizeof(Type)*2*numNodes +
                        sizeof(ActivationFunction)*numNodes +
                        sizeof(NetworkIndexedLink<Type>)*numLinks
                        );
                }

#if DEBUG_NETWORK_CREATION
                cout << "Done!\n";
#endif
        }
    }

    template<class Type>
    FastNetwork<Type>::~FastNetwork()
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
    bool FastNetwork<Type>::hasNode(const string &nodeName)
    {
        if (nodeNameToIndex.count(nodeName)>0)
            return true;
        else
            return false;
    }

    template<class Type>
    Type FastNetwork<Type>::getValue(const string &nodeName)
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
    void FastNetwork<Type>::setValue(const string &nodeName,Type newValue)
    {
		map<string,int>::iterator it = nodeNameToIndex.find(nodeName);
        if(it==nodeNameToIndex.end())
        {
            throw CREATE_LOCATEDEXCEPTION_INFO( (string("ERROR: Could not find node named ") + string(nodeName) + string("\n")) );
        }
        else
        {
#if DEBUG_NETWORK_UPDATE
            cout << nodeName << " is at index " << nodeNameToIndex[nodeName] << endl;
#endif
            nodeValues[it->second] = newValue;
#if DEBUG_NETWORK_UPDATE
            cout << nodeNameToIndex[nodeName] << " set to " << newValue << endl;
#endif
        }
    }

    template<class Type>
    NetworkIndexedLink<Type> *FastNetwork<Type>::getLink(const string &fromNodeName,const string &toNodeName)
    {
        int fromNodeIndex = nodeNameToIndex[fromNodeName];
        int toNodeIndex = nodeNameToIndex[toNodeName];

        if (!nodeNameToIndex.count(fromNodeName)||!nodeNameToIndex.count(toNodeName))
        {
            cout << "ERROR: Could not find node!" << endl;
            CREATE_PAUSE("PAUSE");
        }

        map<pair<int,int>,int>::iterator it = 
            nodeLinkMap.find(pair<int,int>(fromNodeIndex,toNodeIndex));

        if(it == nodeLinkMap.end())
        {
            return NULL;
        }
        else
        {
            return &links[it->second];
        }
    }

    template<class Type>
    bool FastNetwork<Type>::hasLink(const string &fromNodeName,const string &toNodeName)
    {
        if(getLink(fromNodeName,toNodeName))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    template<class Type>
    Type FastNetwork<Type>::getLinkWeight(const string &fromNodeName,const string &toNodeName)
    {
        if(getLink(fromNodeName,toNodeName))
        {
            return getLink(fromNodeName,toNodeName)->weight;
        }
        else
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
        }
    }

    template<class Type>
    void FastNetwork<Type>::reinitialize()
    {
        this->activated=false;
        memset(nodeValues,0,sizeof(Type)*numNodes);
    }

    template<class Type>
    void FastNetwork<Type>::updateFixedIterations(int iterations)
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
        else
        {
            //throw CREATE_LOCATEDEXCEPTION_INFO("THE NETWORK HAS BEEN UPDATED WHILE ALREADY ACTIVE!");
        }

        for (int a=0;a<count;a++)
        {
            /*for (int a=0;a<nodes.size();a++)
            {
            nodes[a]->computeNewValue();
            }*/
            memset(nodeNewValues,0,sizeof(Type)*numNodes);

#if DEBUG_NETWORK_UPDATE
            cout << "NumLinks: " << numLinks << endl;
#endif
            for (int a=0;a<numLinks;a++)
            {
#if DEBUG_NETWORK_UPDATE
                cout << links[a].fromNode << "->" << links[a].toNode << " : " << links[a].weight << endl;
                cout << "From value: " << nodeValues[links[a].fromNode] << endl;
                cout << "Node value changed from " << nodeNewValues[links[a].toNode];
#endif
                nodeNewValues[links[a].toNode] += nodeValues[links[a].fromNode]*links[a].weight;
#if DEBUG_NETWORK_UPDATE
                cout << " to " << nodeNewValues[links[a].toNode] << endl;
#endif
            }
            //cout << "Done computing values.  Updating...\n";

            /*for (int a=0;a<nodes.size();a++)
            {
            nodes[a]->updateValue();
            }*/

#if DEBUG_NETWORK_UPDATE
            cout << "Before Activation: " << endl;
            for (int a=0;a<numNodes;a++)
            {
                cout << a << ": " << nodeValues[a] << '/' << nodeNewValues[a] << ' ' << activationFunctions[a] << endl;
            }
#endif

			bool signedActivation=false;

			if (Globals::getSingleton()->hasSignedActivation())
			{
				signedActivation=true;
			}

			bool usingTanhSigmoid = Globals::getSingleton()->isUsingTanhSigmoid();

            for (int a=numConstantNodes;a<numNodes;a++)
            {
                nodeNewValues[a] = runActivationFunction(nodeNewValues[a],activationFunctions[a],signedActivation,usingTanhSigmoid);
            }

#if DEBUG_NETWORK_UPDATE
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

#if DEBUG_NETWORK_UPDATE
            cout << "After Copy: " << endl;
            for (int a=0;a<numNodes;a++)
            {
                cout << a << ": " << nodeValues[a] << '/' << nodeNewValues[a] << endl;
            }

            //system("PAUSE");
#endif

            //cout << "Done updating values.\n";
        }
    }

    template<class Type>
    void FastNetwork<Type>::print()
    {
        cout << "FASTNETWORK CHECKSUM:\n";
        double sum=0.0;

        for (int a=0;a<numLinks;a++)
        {
            sum += links[a].weight;
        }
        cout << sum << endl;
        cout << "DONE WITH FASTNETWORK SCREEN DUMP\n";
    }

    template<class Type>
    void FastNetwork<Type>::clearAllLinkWeights()
    {
        for (int a=0;a<numLinks;a++)
        {
            links[a].weight = (Type)0.0;
        }
    }

    const float LEARNING_RATE = (0.5f);//(0.5f);

    template<class Type>
    void FastNetwork<Type>::backProp(const vector<string> &nodeNames,const vector<Type> &correctedValues,bool perceptron)
    {
        set<int> fromNodes;

        map<int,double> linkErrorTerms;

        for(int a=0;a<(int)nodeNames.size();a++)
        {
            int outputNodeIndex = nodeNameToIndex[nodeNames[a]];

            Type outputError = (Type)fabs(nodeValues[outputNodeIndex]-correctedValues[a]);

            if(fabs(outputError) < 1e-6)
            {
                //The error is sufficiently small, bail
                continue;
            }

            //We begin by computing net2p.  We also store the from nodes so we can do backwards chaining
            Type net2p=0.0;
            for(int b=0;b<numLinks;b++)
            {
                if(links[b].toNode == outputNodeIndex)
                {
                    net2p += links[b].weight*nodeValues[links[b].fromNode];
                    fromNodes.insert(links[b].fromNode);
                }
            }

            Type diff = 
                (
                correctedValues[a] - 
                nodeValues[outputNodeIndex]
            );

            Type errorTerm = 
                diff * activationFunctionDerivative(net2p,activationFunctions[outputNodeIndex]);

            //Now we can use net2p to compute the error function
            for(int b=0;b<numLinks;b++)
            {
                if(links[b].toNode == outputNodeIndex)
                {
                    //We need to change the weight of this connection to correct any error.
                    linkErrorTerms[b] = (Type)errorTerm;
                }
            }
        }

        if(!perceptron)
        {
            for(set<int>::iterator fromNodeIterator = fromNodes.begin();fromNodeIterator != fromNodes.end();fromNodeIterator++)
            {
                int outputNodeIndex = *fromNodeIterator;

                //We begin by computing net1p.
                Type net1p=0.0f;
                for(int a=0;a<numLinks;a++)
                {
                    if(links[a].toNode == outputNodeIndex)
                    {
                        net1p += (Type)(links[a].weight*nodeValues[links[a].fromNode]);
                    }
                }

                //Compute the error term
                Type errorTerm = 0.0f;

                for(int b=0;b<numLinks;b++)
                {
                    if(links[b].fromNode == outputNodeIndex)
                    {
                        errorTerm += (Type)(linkErrorTerms[b]*links[b].weight);
                    }
                }

                errorTerm *= activationFunctionDerivative(net1p,activationFunctions[outputNodeIndex]);

                for(int a=0;a<numLinks;a++)
                {
                    if(links[a].toNode == outputNodeIndex)
                    {
                        //We need to change the weight of this connection to correct any error.
                        Type deltaWeight = (Type)
                            LEARNING_RATE * ( errorTerm * nodeValues[links[a].fromNode]);

                        links[a].weight += deltaWeight;
                    }
                }
            }
        }

        //Now, we need to apply the link weight changes for the output layer links
        for(
            map< int,double >::iterator linkErrorIterator = linkErrorTerms.begin();
            linkErrorIterator != linkErrorTerms.end();
        linkErrorIterator++
            )
        {
            Type deltaWeight = (Type)
                (LEARNING_RATE * ( linkErrorIterator->second * nodeValues[links[linkErrorIterator->first].fromNode]));

            links[linkErrorIterator->first].weight += deltaWeight;
        }
    }

    template<class Type>
    Type FastNetwork<Type>::runActivationFunction(Type tmpVal,ActivationFunction function,bool signedActivation,bool usingTanhSigmoid)
    {
        Type retVal;


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
                if(usingTanhSigmoid)
                {
                    retVal = 2 / (1 + exp(-2 * tmpVal)) - 1;
                }
                else
                {
                    //signed sigmoid
                    if (tmpVal<-2.9||tmpVal>2.9)
                    {
                        retVal = (Type)( ((1.0 / (1+exp(-tmpVal))) - 0.5)*2.0 );
                    }
                    else
                    {
						int sigmoidIndex = int(tmpVal*1000.0)+3000;
						if(sigmoidIndex<0 || sigmoidIndex>6000)
						{
							throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
						}
                        retVal = (Type)signedSigmoidTable[sigmoidIndex];
                    }
                }
            }
            else
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Unsigned Sigmoid> ";
#endif
                //unsigned sigmoid
                if(usingTanhSigmoid)
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
                Type tmpVal2 = (Type)min(1.0,max(-1.0,tmpVal/3.0));
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

    template<class Type>
    Type FastNetwork<Type>::activationFunctionDerivative(Type tmpVal,ActivationFunction function)
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
                if(Globals::getSingleton()->isUsingTanhSigmoid())
                {
                    //retVal = 2 / (1 + exp(-2 * tmpVal)) - 1;
                    throw CREATE_LOCATEDEXCEPTION_INFO("TODO");
                }
                else
                {
                    //signed sigmoid
                    {
                        //retVal = (Type) ( ((2 / (1 + exp(-tmpVal))) * (1 - (1 / (1 + exp(-tmpVal))))) - 1);
                        Type tmpExp = exp(-tmpVal);
                        retVal = (Type)( 2.0 * ( (1.0/(((1+tmpExp))*(1+tmpExp))) * tmpExp) );
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
                    //retVal = (1 + exp(-2 * tmpVal));
                    throw CREATE_LOCATEDEXCEPTION_INFO("TODO");
                }
                else
                {
                    {
                        retVal = (Type) ( ((1 / (1 + exp(-tmpVal))) * (1 - (1 / (1 + exp(-tmpVal))))));
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
                retVal = cos(tmpVal);
            }
            else
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Unsigned Sin> ";
#endif
                retVal = (cos(tmpVal))/2;
            }
            break;
        case ACTIVATION_FUNCTION_COS:
            if (signedActivation)
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Signed Cos> ";
#endif
                retVal = -sin(tmpVal);
            }
            else
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Unsigned Cos> ";
#endif
                retVal = (-sin(tmpVal))/2;
            }
            break;
        case ACTIVATION_FUNCTION_GAUSSIAN:
            if (signedActivation)
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Unsigned Gaussian> ";
#endif
                retVal = -2*tmpVal*exp(-tmpVal*tmpVal);
            }
            else
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Unsigned Gaussian> ";
#endif
                retVal = -2*tmpVal*exp(-tmpVal*tmpVal);
            }
            break;
        case ACTIVATION_FUNCTION_SQUARE:
#if DEBUG_ACTIVATION_CALCULATION
            cout << " <Square> ";
#endif
            throw CREATE_LOCATEDEXCEPTION_INFO("TODO");
            retVal = tmpVal*tmpVal;
            break;
        case ACTIVATION_FUNCTION_ABS_ROOT:
            throw CREATE_LOCATEDEXCEPTION_INFO("TODO");
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
            throw CREATE_LOCATEDEXCEPTION_INFO("TODO");
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
            throw CREATE_LOCATEDEXCEPTION_INFO("TODO");
            if (signedActivation)
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Signed Ones Compliment> ";
#endif
                Type tmpVal2 = (Type)min(1.0,max(-1.0,tmpVal/3.0));
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

    template class FastNetwork<float>; // explicit instantiation
    template class FastNetwork<double>; // explicit instantiation
}
