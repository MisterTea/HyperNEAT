#include "NEAT_Defines.h"

#include "NEAT_GeneticLinkGene.h"

#include "NEAT_NetworkLink.h"
#include "NEAT_Random.h"
#include "NEAT_Globals.h"

#include "NEAT_GeneticNodeGene.h"

#include <iomanip>

namespace NEAT
{

    GeneticLinkGene::GeneticLinkGene(int _fromNodeID,int _toNodeID,double _weight)
            :
            GeneticGene(),
            fromNodeID(_fromNodeID),
            toNodeID(_toNodeID),
            weight(_weight),
            fixed(false)
    {
        //legacyID = -1;

        //This means a link was created with the same input and output in the same generation, so it's the same ID.
        Globals::getSingleton()->assignLinkID(this);

        //DON'T WRITE CODE PAST THE ID ASSIGNMENT BECAUSE OF COPY ISSUES
    }

    GeneticLinkGene::GeneticLinkGene(int _fromNodeID,int _toNodeID)
            :
            GeneticGene(),
            fromNodeID(_fromNodeID),
            toNodeID(_toNodeID),
            fixed(false)
    {
        weight = Globals::getSingleton()->getRandom().getRandomDouble(-3,3);
        //legacyID = -1;

        //This means a link was created with the same input and output in the same generation, so it's the same ID.
        Globals::getSingleton()->assignLinkID(this);

        //DON'T WRITE CODE PAST THE ID ASSIGNMENT BECAUSE OF COPY ISSUES
    }

    GeneticLinkGene::GeneticLinkGene(TiXmlElement *linkElementPtr)
            :
            GeneticGene(linkElementPtr)
    {
        fromNodeID = atoi(linkElementPtr->Attribute("fromNode"));
        toNodeID = atoi(linkElementPtr->Attribute("toNode"));
        weight = atof(linkElementPtr->Attribute("weight"));

        if(linkElementPtr->Attribute("fixed"))
        {
            fixed = (atoi(linkElementPtr->Attribute("fixed"))>0);
        }
        else
        {
            fixed = false;
        }
    }

    GeneticLinkGene::GeneticLinkGene(istream &istr)
            :
            GeneticGene(istr)
    {
        istr >> fromNodeID >> toNodeID >> fixed >> setprecision(15) >> weight;
    }

    GeneticLinkGene::~GeneticLinkGene()
    {}


    bool GeneticLinkGene::operator==(const GeneticLinkGene &other) const
    {
        return
            (
                GeneticGene::operator==(other) &&
                fromNodeID == other.fromNodeID &&
                toNodeID == other.toNodeID &&
                weight == other.weight &&
                fixed == other.fixed
            );
    }

    void GeneticLinkGene::mutate()
    {
        if(!fixed)
        {
            int mod=1;
            //mod = max(20-age,2)/2;
            double mutationPower = mod*Globals::getSingleton()->getParameterValue("MutationPower");
            weight += mutationPower*(2.0*(Globals::getSingleton()->getRandom().getRandomDouble()-0.5));

            if (weight>5)
                weight=5;
            else if (weight<-5)
                weight=-5;
        }
    }

    void GeneticLinkGene::dump(TiXmlElement *XMLnode)
    {
        GeneticGene::dump(XMLnode);
        XMLnode->SetAttribute("fromNode",fromNodeID);
        XMLnode->SetAttribute("toNode",toNodeID);
        XMLnode->SetDoubleAttribute("weight",weight);
        XMLnode->SetAttribute("fixed",fixed);
    }

    void GeneticLinkGene::dump(ostream &ostr)
    {
        GeneticGene::dump(ostr);

        ostr << fromNodeID << ' ' << toNodeID << ' ' << fixed << ' ' << setprecision(15) << weight << ' ';
    }
}
