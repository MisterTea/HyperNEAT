#include "NEAT_Defines.h"

#include "NEAT_ModularNetwork.h"

#include "NEAT_Random.h"

#include "NEAT_GeneticIndividual.h"

#include "NEAT_Globals.h"

namespace NEAT
{

    ModularNetwork::ModularNetwork(vector<NetworkNode *> &_nodes,vector<NetworkLink *> &_links)
            :
            Network<double>()
    {
        for (int a=0;a<(int)_nodes.size();a++)
        {
            nodes.push_back(
                new NetworkNode(
                    _nodes[a]->getName(),
                    _nodes[a]->getUpdate(),
                    _nodes[a]->getActivationFunction()
                )
            );
        }

        for (int a=0;a<(int)_links.size();a++)
        {
            NetworkNode *fromNodeTemp=NULL,*toNodeTemp=NULL;
            for (int b=0;b<(int)_nodes.size();b++)
            {
                if (_nodes[b]==_links[a]->getFromNode())
                {
                    fromNodeTemp = nodes[b];
                }
                if (_nodes[b]==_links[a]->getToNode())
                {
                    toNodeTemp = nodes[b];
                }
            }

            links.push_back(
                new NetworkLink(
                    fromNodeTemp,
                    toNodeTemp,
                    _links[a]->isForward(),
                    _links[a]->getWeight()
                )
            );
        }
    }

    ModularNetwork::ModularNetwork()
            :
            Network<double>()
    {}

    ModularNetwork& ModularNetwork::operator=(const ModularNetwork &other)
    {
        copyFrom(other);

        return *this;
    }

    ModularNetwork::ModularNetwork(const ModularNetwork &other)
    {
        copyFrom(other);
    }

    void ModularNetwork::copyFrom(const ModularNetwork &other)
    {
        Network<double>::copyFrom(other);

        if (this!=&other)
        {
            for (int a=0;a<(int)other.nodes.size();a++)
            {
                nodes.push_back(
                    new NetworkNode(
                        other.nodes[a]->getName(),
                        other.nodes[a]->getUpdate(),
                        other.nodes[a]->getActivationFunction()
                    )
                );
            }

            for (int a=0;a<(int)other.links.size();a++)
            {
                NetworkNode *fromNodeTemp=NULL,*toNodeTemp=NULL;
                for (int b=0;b<(int)other.nodes.size();b++)
                {
                    if (other.nodes[b]==other.links[a]->getFromNode())
                    {
                        fromNodeTemp = nodes[b];
                    }
                    if (other.nodes[b]==other.links[a]->getToNode())
                    {
                        toNodeTemp = nodes[b];
                    }
                }

                links.push_back(
                    new NetworkLink(
                        fromNodeTemp,
                        toNodeTemp,
                        other.links[a]->isForward(),
                        other.links[a]->getWeight()
                    )
                );
            }
        }
    }

    ModularNetwork::~ModularNetwork()
    {
        for (int a=0;a<(int)nodes.size();a++)
        {
            delete nodes[a];
        }

        for (int a=0;a<(int)links.size();a++)
        {
            delete links[a];
        }
    }

    bool ModularNetwork::hasNode(const string &nodeName)
    {
        for (int a=0;a<(int)nodes.size();a++)
        {
            if (!strcmp(nodes[a]->getName().c_str(),nodeName.c_str()))
                return true;
        }

        return false;
    }

    NetworkNode *ModularNetwork::getNode(const string &name)
    {
        for (int a=0;a<(int)nodes.size();a++)
        {
            if (!strcmp(nodes[a]->getName().c_str(),name.c_str()))
                return nodes[a];
        }

        throw CREATE_LOCATEDEXCEPTION_INFO(string("Could not find node by the name: ")+name+string(" !"));
    }

    NetworkLink *ModularNetwork::getLink(const string &fromNodeName,const string &toNodeName)
    {
        NetworkNode* fromNode = getNode(fromNodeName);
        NetworkNode* toNode = getNode(toNodeName);

        for (int a=0;a<(int)links.size();a++)
        {
            if (
                links[a]->getFromNode()==fromNode &&
                links[a]->getToNode() == toNode
            )
            {
                return links[a];
            }

        }

        return NULL;
    }

    void ModularNetwork::reinitialize()
    {
        activated=false;
        for (int a=0;a<(int)nodes.size();a++)
        {
            nodes[a]->setValue(0);
        }
    }

    void ModularNetwork::updateFixedIterations(int iterations)
    {
        int count=iterations;
        if (!activated)
        {
            //count += 19; //run 19 extra times.
            //This should hopefully get everything
            //up to date on the first run

            count += Globals::getSingleton()->getExtraActivationUpdates();
            activated=true;
        }

        for (int a=0;a<count;a++)
        {
            for (int a=0;a<(int)nodes.size();a++)
            {
                nodes[a]->computeNewValue();
            }
            //cout << "Done computing values.  Updating...\n";

            for (int a=0;a<(int)nodes.size();a++)
            {
                nodes[a]->updateValue();
            }
            //cout << "Done updating values.\n";
        }
    }

}
