#ifndef __NETWORKNODE_H__
#define __NETWORKNODE_H__
#include "NEAT_STL.h"
#include "NEAT_NetworkLink.h"
#include "NEAT_GeneticNodeGene.h"
#include "tinyxmlplus.h"
namespace NEAT
{
// SIGMOID FUNCTION ********************************
//This is a signmoidal activation function, which is an S-shaped squashing function
//It smoothly limits the amplitude of the output of a neuron to between 0 and 1
//It is a helper to the neural-activation function get_active_out
//It is made inline so it can execute quickly since it is at every non-sensor
// node in a network.
//NOTE:  In order to make node insertion in the middle of a link possible,
// the signmoid can be shifted to the right and more steeply sloped:
// slope=4.924273
// constant= 2.4621365
// These parameters optimize mean squared error between the old output,
// and an output of a node inserted in the middle of a link between
// the old output and some other node.
// When not right-shifted, the steepened slope is closest to a linear
// ascent as possible between -0.5 and 0.5
    inline double fsigmoid(double activesum,double slope,double constant)
    {
        //RIGHT SHIFTED ---------------------------------------------------------
        //return (1/(1+(exp(-(slope*activesum-constant))))); //ave 3213 clean on 40 runs of p2m and 3468 on another 40
        //41394 with 1 failure on 8 runs
        //LEFT SHIFTED ----------------------------------------------------------
        //return (1/(1+(exp(-(slope*activesum+constant))))); //original setting ave 3423 on 40 runs of p2m, 3729 and 1 failure also
        //PLAIN SIGMOID ---------------------------------------------------------
        return (1/(1+(exp(-activesum)))); //3511 and 1 failure
        //LEFT SHIFTED NON-STEEPENED---------------------------------------------
        //return (1/(1+(exp(-activesum-constant)))); //simple left shifted
        //NON-SHIFTED STEEPENED
        //return (1/(1+(exp(-(slope*activesum))))); //Compressed
    }

    static const string network_node_type("NetworkNode");

    class NetworkNode
    {
    public:
    protected:
        double value;
        double newValue;
        vector<NetworkLink *> fromLinks;
        string name;

        bool update;
        //GeneticNodeGene *genotype;

        ActivationFunction activationFunction;
    public:
        NEAT_DLL_EXPORT NetworkNode(
            string _name,
            bool _update=true,
            ActivationFunction _activationFunction=ACTIVATION_FUNCTION_SIGMOID
        );

        NEAT_DLL_EXPORT NetworkNode()
        {}

        NEAT_DLL_EXPORT virtual ~NetworkNode();

        virtual inline const string &getType()
        {
            return network_node_type;
        }

        virtual inline const double &getValue() const
        {
            return value;
        }

        inline const string &getName() const
        {
            return name;
        }

        inline const bool &getUpdate() const
        {
            return update;
        }

        inline const ActivationFunction &getActivationFunction() const
        {
            return activationFunction;
        }

        inline void setUpdate(bool _update)
        {
            update = _update;
        }

        NEAT_DLL_EXPORT void addFromLink(NetworkLink *link);

        //This function computes the new value based on the children nodes.
        NEAT_DLL_EXPORT virtual void computeNewValue();

        //This function updates the value with the value at the next timestep.
        inline void updateValue()
        {
            if (update)
            {
                if (newValue==-999)
                    throw CREATE_LOCATEDEXCEPTION_INFO("Value wasn't computed before update!");
                value = newValue;
                newValue = -999;
            }
        }

        inline void setValue(double _value)
        {
            value = _value;
        }

    protected:
    };
}
#endif
