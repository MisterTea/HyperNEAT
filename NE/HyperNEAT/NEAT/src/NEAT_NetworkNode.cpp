#include "NEAT_Defines.h"

#include "NEAT_NetworkNode.h"
#include "NEAT_Globals.h"

#define DEBUG_ACTIVATION_CALCULATION (0)

#define DEBUG_DO_NOT_USE_ALL_ZERO

namespace NEAT
{
    extern double signedSigmoidTable[6001];
    extern double unsignedSigmoidTable[6001];

    NetworkNode::NetworkNode(string _name,bool _update,ActivationFunction _activationFunction)
            :
            value(0),
            name(_name),
            update(_update),
            activationFunction(_activationFunction)
    {}

    NetworkNode::~NetworkNode()
    {}

    void NetworkNode::addFromLink(NetworkLink *link)
    {
        fromLinks.push_back(link);
    }

//This function computes the new value based on the children nodes.
    void NetworkNode::computeNewValue()
    {
        if (!update)
        {
            newValue = value;
            return;
        }

        double tmpVal=0;

        for (int a=0;a<(int)fromLinks.size();a++)
        {
            tmpVal+=fromLinks[a]->getFromNode()->getValue() * fromLinks[a]->getWeight();
        }

#ifdef DEBUG_DO_NOT_USE_ALL_ZERO
        if (fabs(tmpVal)<1e-2)
        {
            newValue=0.0;
            return;
        }
#endif

        bool signedActivation=false;

        if (Globals::getSingleton()->hasSignedActivation())
        {
            signedActivation=true;
        }

#if DEBUG_ACTIVATION_CALCULATION
        cout << "Before: " << tmpVal;
#endif

        switch (activationFunction)
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
                if (tmpVal<-2.9||tmpVal>2.9)
                {
                    newValue = ((1 / (1+exp(-tmpVal))) - 0.5)*2.0;
                }
                else
                {
                    newValue = signedSigmoidTable[int(tmpVal*1000.0)+3000];
                }
            }
            else
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Unsigned Sigmoid> ";
#endif
                //unsigned sigmoid
                if (tmpVal<-2.9||tmpVal>2.9)
                {
                    newValue = 1 / (1+exp(-tmpVal));
                }
                else
                {
                    newValue = unsignedSigmoidTable[int(tmpVal*1000.0)+3000];
                }
            }
            break;
        case ACTIVATION_FUNCTION_SIN:
            if (signedActivation)
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Signed Sin> ";
#endif
                newValue = sin(tmpVal);
            }
            else
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Unsigned Sin> ";
#endif
                newValue = (sin(tmpVal)+1)/2;
            }
            break;
        case ACTIVATION_FUNCTION_COS:
            if (signedActivation)
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Signed Cos> ";
#endif
                newValue = cos(tmpVal);
            }
            else
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Unsigned Cos> ";
#endif
                newValue = (cos(tmpVal)+1)/2;
            }
            break;
        case ACTIVATION_FUNCTION_GAUSSIAN:
            if (signedActivation)
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Unsigned Gaussian> ";
#endif
                newValue = exp(-pow(tmpVal,2));
            }
            else
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Unsigned Gaussian> ";
#endif
                newValue = exp(-pow(tmpVal,2));
            }
            break;
        case ACTIVATION_FUNCTION_SQUARE:
#if DEBUG_ACTIVATION_CALCULATION
            cout << " <Square> ";
#endif
            newValue = tmpVal*tmpVal;
            break;
        case ACTIVATION_FUNCTION_ABS_ROOT:
            if (signedActivation)
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Signed Root> ";
#endif
                if (tmpVal<0.0)
                {
                    newValue = -sqrt(fabs(tmpVal));
                }
                else
                {
                    newValue = sqrt(fabs(tmpVal));
                }
            }
            else
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Absolute Root> ";
#endif
                newValue = sqrt(fabs(tmpVal));
            }
            break;
        case ACTIVATION_FUNCTION_LINEAR:
            if (signedActivation)
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Signed Linear> ";
#endif
                newValue = min(max(tmpVal,-3.0),3.0) / 3.0;
            }
            else
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Absolute Linear> ";
#endif
                newValue = ((min(max(tmpVal,-3.0),3.0) / 3.0) + 1) / 2.0;
            }
            break;
        case ACTIVATION_FUNCTION_ONES_COMPLIMENT:
            if (signedActivation)
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Signed Ones Compliment> ";
#endif
                newValue = min(1.0,max(-1.0,tmpVal/3.0));
                if (newValue>-0.1)
                {
                    newValue = max(0.0,newValue);
                    newValue = 1.0-newValue;
                }
                else
                {
                    newValue = (-1.0) - tmpVal;
                }
            }
            else
            {
#if DEBUG_ACTIVATION_CALCULATION
                cout << " <Absolute Ones Compliment> ";
#endif
                newValue = min(1.0,max(0.0,tmpVal/3.0));
                newValue = 1.0-newValue;
            }
            break;
        default:
            throw CREATE_LOCATEDEXCEPTION_INFO("Unknown activation function!!!");
            break;
        }

#if DEBUG_ACTIVATION_CALCULATION
        cout << " After: " << newValue << endl;
#endif

    }
}

