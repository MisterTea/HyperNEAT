#include "NEAT_Defines.h"

#include "NEAT_GeneticGene.h"

#define DEBUG_GENETIC_GENE (0)

namespace NEAT
{

    GeneticGene::GeneticGene()
            :
            enabled(true),
            age(0)
    {}

    GeneticGene::GeneticGene(TiXmlElement *elementPtr)
    {
        elementPtr->Attribute("ID",&ID);
        enabled = (atoi(elementPtr->Attribute("Enabled"))==1);
    }

    GeneticGene::GeneticGene(istream &istr)
    {
        istr >> ID >> enabled;

#if DEBUG_GENETIC_GENE
        cout << "ID: " << ID << " Enabled: " << enabled << endl;
#endif
    }

    GeneticGene::~GeneticGene()
    {}

    bool GeneticGene::operator==(const GeneticGene &other) const
    {
        return
            (
                ID==other.ID &&
                enabled==other.enabled
            );
    }

    void GeneticGene::dump(TiXmlElement *XMLnode)
    {
        XMLnode->SetAttribute("ID",ID);
        XMLnode->SetAttribute("Enabled",enabled);
    }

    void GeneticGene::dump(ostream &ostr)
    {
        ostr << ID << ' ' << enabled << ' ';
    }
}
