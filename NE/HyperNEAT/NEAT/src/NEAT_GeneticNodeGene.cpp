#include "NEAT_Defines.h"

#include "NEAT_GeneticNodeGene.h"

#include "NEAT_Globals.h"

#define GENETIC_NODE_GENE (0)

namespace NEAT
{

    GeneticNodeGene::GeneticNodeGene(
        const string &_name,
        const string &_type,
        double _drawingPosition,
        bool randomizeActivation,
        ActivationFunction _activationFunction
    )
            :
            name(_name),
            type(_type),
            drawingPosition(_drawingPosition),
            topologyFrozen(false),
            activationFunction(_activationFunction)
    {
        if (randomizeActivation)
        {
            if (Globals::getSingleton()->getParameterValue("OnlyGaussianHiddenNodes")>0.5)
            {
				activationFunction = ACTIVATION_FUNCTION_GAUSSIAN;
            }
            else
            {
                do
                {
                    activationFunction = (ActivationFunction)Globals::getSingleton()->getRandom().getRandomInt(ACTIVATION_FUNCTION_END);
                }
                while (  //These activation functions don't seem to contribute much
                    activationFunction==ACTIVATION_FUNCTION_ABS_ROOT||
                    activationFunction==ACTIVATION_FUNCTION_SQUARE||
                    activationFunction==ACTIVATION_FUNCTION_COS||
                    //activationFunction==ACTIVATION_FUNCTION_SIGMOID||
                    //activationFunction==ACTIVATION_FUNCTION_SIN||
                    //activationFunction==ACTIVATION_FUNCTION_LINEAR||
                    activationFunction==ACTIVATION_FUNCTION_ONES_COMPLIMENT||
                    false);
            }
        }

        Globals::getSingleton()->assignNodeID(this);
        //DON'T WRITE CODE PAST THE ID ASSIGNMENT BECAUSE OF COPY ISSUES
    }

    GeneticNodeGene::GeneticNodeGene(
        const string &_name,
        const string &_type,
        double _drawingPosition,
        bool _topologyFrozen,
        bool randomizeActivation,
        ActivationFunction _activationFunction
    )
            :
            name(_name),
            type(_type),
            drawingPosition(_drawingPosition),
            topologyFrozen(_topologyFrozen),
            activationFunction(_activationFunction)
    {
        if (randomizeActivation)
        {
            if (Globals::getSingleton()->getParameterValue("OnlyGaussianHiddenNodes")>0.5)
            {
                do
                {
                    activationFunction = (ActivationFunction)Globals::getSingleton()->getRandom().getRandomInt(ACTIVATION_FUNCTION_END);
                }
                while (
                    activationFunction==ACTIVATION_FUNCTION_ABS_ROOT||
                    activationFunction==ACTIVATION_FUNCTION_SQUARE||
                    activationFunction==ACTIVATION_FUNCTION_COS||
                    activationFunction==ACTIVATION_FUNCTION_SIGMOID||
                    activationFunction==ACTIVATION_FUNCTION_SIN||
                    activationFunction==ACTIVATION_FUNCTION_LINEAR||
                    activationFunction==ACTIVATION_FUNCTION_ONES_COMPLIMENT||
                    false);
            }
            else
            {
                do
                {
                    activationFunction = (ActivationFunction)Globals::getSingleton()->getRandom().getRandomInt(ACTIVATION_FUNCTION_END);
                }
                while (
                    activationFunction==ACTIVATION_FUNCTION_ABS_ROOT||
                    activationFunction==ACTIVATION_FUNCTION_SQUARE||
                    activationFunction==ACTIVATION_FUNCTION_COS||
                    //activationFunction==ACTIVATION_FUNCTION_SIGMOID||
                    //activationFunction==ACTIVATION_FUNCTION_SIN||
                    //activationFunction==ACTIVATION_FUNCTION_LINEAR||
                    activationFunction==ACTIVATION_FUNCTION_ONES_COMPLIMENT||
                    false);
            }
        }

        Globals::getSingleton()->assignNodeID(this);
        //DON'T WRITE CODE PAST THE ID ASSIGNMENT BECAUSE OF COPY ISSUES
    }

    GeneticNodeGene::GeneticNodeGene(TiXmlElement *nodeElementPtr)
            :
            GeneticGene(nodeElementPtr),
            activationFunction(ACTIVATION_FUNCTION_SIGMOID)
    {
        name = nodeElementPtr->Attribute("Name");
        type = nodeElementPtr->Attribute("Type");

        nodeElementPtr->Attribute("DrawingPosition",&drawingPosition);

        int actVal = (int)activationFunction;

        nodeElementPtr->Attribute("ActivationFunction",&actVal);

        if(nodeElementPtr->Attribute("TopologyFrozen"))
        {
            topologyFrozen = stringTo<bool>(nodeElementPtr->Attribute("TopologyFrozen"));
        }
        else
        {
            topologyFrozen=false;
        }

        activationFunction = (ActivationFunction)actVal;
    }

    GeneticNodeGene::GeneticNodeGene(istream &istr)
            :
            GeneticGene(istr)
    {
        int actVal;

        istr >> name >> type >> drawingPosition >> topologyFrozen >> actVal;

        activationFunction = (ActivationFunction)actVal;

        if (name==string("__NO_NAME__"))
        {
            name = string("");
        }

        if (type==string("__NO_TYPE__"))
        {
            type = string("");
        }

#if DEBUG_GENETIC_NODE_GENE
        cout << "Name: " << name << " Type: " << type << " Draw Position: " << drawingPosition
        << " Activation Function: " << actVal << endl;
#endif
    }

    GeneticNodeGene::~GeneticNodeGene()
    {}

    bool GeneticNodeGene::operator==(const GeneticNodeGene &other) const
    {
        return
            (
                GeneticGene::operator==(other) &&
                activationFunction==other.activationFunction

            );
    }

    void GeneticNodeGene::dump(TiXmlElement *XMLnode)
    {
        GeneticGene::dump(XMLnode);
        XMLnode->SetAttribute("Name",name);
        XMLnode->SetAttribute("Type",type);
        XMLnode->SetDoubleAttribute("DrawingPosition",drawingPosition);
        XMLnode->SetAttribute("TopologyFrozen",(int)topologyFrozen);
        XMLnode->SetAttribute("ActivationFunction",(int)activationFunction);
    }

    void GeneticNodeGene::dump(ostream &ostr)
    {
        GeneticGene::dump(ostr);

        string tmpName = name;

        if (tmpName.length()==0)
        {
            tmpName = string("__NO_NAME__");
        }

        string tmpType = type;

        if (tmpType.length()==0)
        {
            tmpType = string("__NO_TYPE__");
        }

        ostr << tmpName << ' ' << type << ' ' << drawingPosition << ' ' << topologyFrozen << ' '
            << ((int)activationFunction) << ' ';
    }

    void GeneticNodeGene::mutate()
    {
        throw CREATE_LOCATEDEXCEPTION_INFO("Don\'t try to mutate node genes!");
        //activationFunction = Globals::getSingleton()->getRandom().getRandomInt(ACTIVATION_FUNCTION_END);
    }

}
