#ifndef GPUANN_H_INCLUDED
#define GPUANN_H_INCLUDED

#include "NEAT_Network.h"

#include "NEAT_NetworkNode.h"
#include "NEAT_NetworkLink.h"
#include "NEAT_NetworkIndexedLink.h"

#include "NEAT_FastLayeredNetwork.h"

namespace NEAT
{
    /**
     *  The GPUANN class is designed to be faster at the cost
     *  of being less dynamic.  Adding/Removing links and nodes
     *  is not supported with this network.
     */
    class GPUANN : public FastLayeredNetwork<float>
    {

    public:
        /**
         *  (Constructor) Create a Network with the inputed toplogy
         */
        NEAT_DLL_EXPORT GPUANN(
            const vector<NetworkLayer<float> > &_layers
        );

        /**
         *  (Constructor) Empty Constructor
         */
        NEAT_DLL_EXPORT GPUANN();

        NEAT_DLL_EXPORT virtual ~GPUANN();

        /**
         * update: This updates the network.
         * If the network has not been updated since construction or
         * reinitialize(), the network will be activated.  This means
         * it will update (1+ExtraActivationUpdates+iterations) times!
         * Otherwise, it will update (iterations) times.  If you do not
         * want the extra updates, call dummyActivation() before the first
         * update.
         */
        NEAT_DLL_EXPORT virtual void update();

    protected:
    };

}


#endif // FASTNETWORK_H_INCLUDED
