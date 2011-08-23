#ifndef __GENETICNODEGENE_H__
#define __GENETICNODEGENE_H__

#include "NEAT_GeneticGene.h"

#include "NEAT_Globals.h"

#include "NEAT_Random.h"

namespace NEAT
{
    /**
     * GeneticNodeGene: This gene contains a link between two GeneticNodeGenes
     */
    class NEAT_DLL_EXPORT GeneticNodeGene : public GeneticGene
    {
    protected:
        string name,type;

        /*Greater drawing position means closer to output!*/
        double drawingPosition;

        bool topologyFrozen;

        ActivationFunction activationFunction;
    public:
        GeneticNodeGene(
            const string &_name,
            const string &_type,
            double _drawingPosition,
            bool randomizeActivation,
            ActivationFunction _activationFunction=ACTIVATION_FUNCTION_SIGMOID
        );

        GeneticNodeGene(
            const string &_name,
            const string &_type,
            double _drawingPosition,
            bool _topologyFrozen,
            bool randomizeActivation,
            ActivationFunction _activationFunction=ACTIVATION_FUNCTION_SIGMOID
        );

        virtual ~GeneticNodeGene();

        GeneticNodeGene(TiXmlElement *nodeElementPtr);

        GeneticNodeGene(istream &istr);

        virtual bool operator==(const GeneticNodeGene &other) const;

        inline const string &getName() const
        {
            return name;
        }

        inline const string &getType() const
        {
            return type;
        }

        /*int getLegacyNodeID()
        {
        return legacyID;
        }*/

        inline const double &getDrawingPosition() const
        {
            return drawingPosition;
        }

        virtual void mutate();

        virtual void dump(TiXmlElement *XMLnode);

        virtual void dump(ostream &ostr);

        inline ActivationFunction getActivationFunction() const
        {
            return activationFunction;
        }

        virtual inline void setActivationFunction(ActivationFunction _activationFunction)
        {
            activationFunction = _activationFunction;
        }

        inline bool isTopologyFrozen()
        {
            return topologyFrozen;
        }

        inline void setTopologyFrozen(bool _topologyFrozen)
        {
            topologyFrozen = _topologyFrozen;
        }
    };

}

#endif
