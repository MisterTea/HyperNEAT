#include "NEAT_Defines.h"

#include "NEAT_NetworkLink.h"

#include "NEAT_NetworkNode.h"
#include "NEAT_Globals.h"
#include "NEAT_GeneticLinkGene.h"

namespace NEAT
{

    NetworkLink::NetworkLink(NetworkNode *_fromNode,NetworkNode *_toNode,bool _forward,double _weight)
            :
            fromNode(_fromNode),
            toNode(_toNode),
            forward(_forward),
            weight(_weight)
    {
        toNode->addFromLink(this);
    }

    NetworkLink::NetworkLink(NetworkNode *_fromNode,NetworkNode *_toNode,double _weight)
            :
            fromNode(_fromNode),
            toNode(_toNode),
            forward(true),
            weight(_weight)
    {
        toNode->addFromLink(this);
    }

}
