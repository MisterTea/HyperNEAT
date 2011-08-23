#ifndef __GENETICLINKGENE_H__
#define __GENETICLINKGENE_H__

#include "NEAT_GeneticGene.h"

#include "tinyxmlplus.h"

namespace NEAT
{
    /**
     * GeneticLinkGene: This gene contains a link between two GeneticNodeGenes
     */
    class NEAT_DLL_EXPORT GeneticLinkGene : public GeneticGene
    {
    protected:
        int fromNodeID,toNodeID;

        double weight;

        bool fixed;

    public:
        /**
         * Constructor: This creates a new GeneticLinkGene
         * \param _fromNodeID is the node to which the link is coming from
         * \param _toNodeID is the node to which the link is pointing to
         */
        GeneticLinkGene(int _fromNodeID,int _toNodeID,double _weight);

        /**
         * Constructor: This creates a new GeneticLinkGene with a random weight
         * \param _fromNodeID is the node to which the link is coming from
         * \param _toNodeID is the node to which the link is pointing to
         */
        GeneticLinkGene(int _fromNodeID,int _toNodeID);

        /**
         * Constructor: This creates a new GeneticLinkGene from it's serialized XML format
         * \param linkElementPtr is the root of the XML format
         */
        GeneticLinkGene(TiXmlElement *linkElementPtr);

        GeneticLinkGene(istream &istr);

        virtual ~GeneticLinkGene();

        virtual bool operator==(const GeneticLinkGene &other) const;

        inline double getWeight() const
        {
            return weight;
        }

        inline void setWeight(double _weight)
        {
            weight = _weight;
        }

        inline int getFromNodeID() const
        {
            return fromNodeID;
        }

        inline int getToNodeID() const
        {
            return toNodeID;
        }

        //This function is necessary when a link points to IDs that have changed
        //because of an XML load
        inline void updateLegacy(int newFromNodeID,int newToNodeID)
        {
            fromNodeID = newFromNodeID;
            toNodeID = newToNodeID;
        }

        virtual void mutate();

        virtual inline int getID() const
        {
            return ID;
        }

        virtual void dump(TiXmlElement *XMLnode);

        virtual void dump(ostream &ostr);

        void setFixed(bool _fixed)
        {
            fixed = _fixed;
        }

        inline bool isFixed() const
        {
            return fixed;
        }
    };

}

#endif
