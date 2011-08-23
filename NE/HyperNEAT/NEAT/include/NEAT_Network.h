#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "NEAT_NetworkNode.h"
#include "NEAT_NetworkLink.h"

namespace NEAT
{
    /**
    * Network: This class is responsible for creating a neural network phenotype.
    * While this isn't as fast as FastNetwork, it allows you to have more control
    * over the network topology without creating a new network.
    */
    template<class Type>
    class Network
    {
    public:
    protected:
        bool activated;

    public:
        NEAT_DLL_EXPORT Network()
                :
                activated(false)
        {}

        /**
        *  (Assignment) Copies the network
        */
        NEAT_DLL_EXPORT Network& operator=(const Network &other)
        {
            copyFrom(other);

            return *this;
        }

        /**
        *  (Copy Constructor) Copies the network
        */
        NEAT_DLL_EXPORT Network(const Network &other)
        {
            copyFrom(other);
        }

        NEAT_DLL_EXPORT virtual void copyFrom(const Network &other)
        {
            if (this!=&other)
            {
                activated = other.activated;

            }
        }

        NEAT_DLL_EXPORT virtual ~Network()
        {}

        /*This resets the state of the network to its initial state*/
        NEAT_DLL_EXPORT virtual void reinitialize() = 0;

        NEAT_DLL_EXPORT inline void setActivated(bool value)
        {
            activated = value;
        }

        NEAT_DLL_EXPORT inline bool getActivated() const
        {
            return activated;
        }

        /**
        * dummyActivation: pretends to activate the network so that
        * the extra activation updates are not performed.
        */
        inline void dummyActivation()
        {
            activated=true;
        }
    };
}

#endif
