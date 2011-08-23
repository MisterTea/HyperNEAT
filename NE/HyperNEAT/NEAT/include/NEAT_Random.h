#ifndef __NEAT_RANDOM_H__
#define __NEAT_RANDOM_H__

#include "NEAT_Defines.h"

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/random/normal_distribution.hpp>

typedef boost::minstd_rand base_generator_type;
// Define a uniform random number distribution of integer values between
// 1 and 6 inclusive.
typedef boost::uniform_int<> int_distribution_type;

typedef boost::uniform_real<> real_distribution_type;

typedef boost::normal_distribution<> normal_distribution_type;

namespace NEAT
{
    class Random
    {
    public:

    protected:
        unsigned int seed;

        base_generator_type generator;

        int_distribution_type intDist;
        variate_generator<base_generator_type,int_distribution_type> intGen;

        real_distribution_type realDist;
        variate_generator<base_generator_type,real_distribution_type> realGen;

        normal_distribution_type normalDist;
        variate_generator<base_generator_type,normal_distribution_type> normalGen;

    public:
        /**
         * Constructor:
         * Passing 0 for 'seed' sets the seed to the current time value
         */
        NEAT_DLL_EXPORT Random(unsigned int _seed=0);

        ///Gets a random int 0 <= x < limit;
        NEAT_DLL_EXPORT int getRandomInt(int limit);

        ///Gets a random int min <= x <= max;
        NEAT_DLL_EXPORT int getRandomWithinRange(int min,int max);

        ///Gets a random int min <= x <= max;
        ///Gets a random floating point number low <= x <= high
        NEAT_DLL_EXPORT double getRandomDouble();

        NEAT_DLL_EXPORT double getRandomDouble(double low,double high);

        inline unsigned int getSeed()
        {
            return seed;
        }
    protected:
    };
}

#endif
