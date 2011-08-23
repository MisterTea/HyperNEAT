#include "NEAT_Defines.h"

#include "NEAT_Random.h"

#include "NEAT_STL.h"

#define DEBUG_RANDOM (1)

namespace NEAT
{
    Random::Random(unsigned int _seed)
            :
            seed(_seed?_seed:static_cast<unsigned int>( ( (std::time(0)&0x0FFF)<<16) + (std::time(0)%0xFFFF) )),
            generator(seed),
            intDist(0,INT_MAX/2),
            intGen(generator,intDist),
            realDist(0,1.0 - 1e-6),
            realGen(generator,realDist),
            normalDist(0,1.0 - 1e-6),
            normalGen(generator,normalDist)
    {}

    int Random::getRandomInt(int limit)
    {
        int randNum = intGen()%limit;

#if DEBUG_RANDOM
        if(randNum<0 || randNum>=limit)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Random number out of range!");
        }
#endif

        return randNum;
    }

    int Random::getRandomWithinRange(int min,int max)
    {
        int randNum = min + (intGen()%( (max-min) + 1));

#if DEBUG_RANDOM
        if(randNum<min || randNum>max)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Random number out of range!");
        }
#endif

        return randNum;
    }

    double Random::getRandomDouble()
    {
        double randNum = realGen();

#if DEBUG_RANDOM
        if(randNum<0.0 || randNum>=1.0)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Random number out of range!");
        }
#endif

        return realGen();
    }

    double Random::getRandomDouble(double low,double high)
    {
        double randNum = realGen()*(high-low) + low;

#if DEBUG_RANDOM
        if(randNum<low || randNum>=high)
        {
            throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Random number out of range!");
        }
#endif

        return randNum;
    }
}
