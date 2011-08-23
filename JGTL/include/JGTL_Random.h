#ifndef __JGTL_RANDOM_H__
#define __JGTL_RANDOM_H__

#include <ctime>

#include "JGTL_LocatedException.h"
#include "JGTL_StringConverter.h"

#include <string>
#include <utility>

#include <boost/random/linear_congruential.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_smallint.hpp>
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

namespace JGTL
{
	class Random
	{
		public:

		protected:
			unsigned int seed;

			base_generator_type generator;

			int_distribution_type intDist;
			boost::variate_generator<base_generator_type,int_distribution_type> intGen;

			real_distribution_type realDist;
			boost::variate_generator<base_generator_type,real_distribution_type> realGen;

			normal_distribution_type normalDist;
			boost::variate_generator<base_generator_type,normal_distribution_type> normalGen;

		public:
			/**
			 * Constructor:
			 * Passing 0 for 'seed' sets the seed to the current time value
			 */
			Random(unsigned int _seed=0)
				:
					seed(_seed?_seed:static_cast<unsigned int>( ( (std::time(0)&0x0FFF)<<16) + (std::time(0)%0xFFFF) )),
					generator(seed),
					intDist(0,INT_MAX/2),
					intGen(generator,intDist),
					realDist(0,1.0 - 1e-6),
					realGen(generator,realDist),
					normalDist(0,1.0 - 1e-6),
					normalGen(generator,normalDist)
			{
			}

			///Gets a random int 0 <= x < limit;
			int getInt(int limit)
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

			int rollD2()
			{
				return getInt(1,2);
			}

			int rollD4()
			{
				return getInt(1,4);
			}

			int rollD6()
			{
				return getInt(1,6);
			}

			int rollD8()
			{
				return getInt(1,8);
			}

			int rollD10()
			{
				return getInt(1,10);
			}

			int rollD20()
			{
				return getInt(1,20);
			}

			int rollD100()
			{
				return getInt(1,100);
			}

			int rollDice(int numDice,int dieSize)
			{
				return rollDice(numDice,dieSize,0);
			}

			int rollDice(int numDice,int dieSize,int modifier)
			{
				if(dieSize<=0)
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
				}

				if (numDice==0)
					return modifier;

				int total = modifier;

				for (int a=0;a<numDice;a++)
				{
					total += getInt(1,dieSize);
				}

				int smallestValue = modifier + numDice;
				int largestValue = modifier + numDice*dieSize;
				if(total < smallestValue || total > largestValue)
				{
					throw CREATE_LOCATEDEXCEPTION_INFO(std::string("ERROR:  ROLL DICE RETURNED A VALUE OUTSIDE THE POSSIBLE RANGE OF VALUES:  ") + toString(total) + std::string(" IS OUTSIDE [") + toString(smallestValue) + std::string(" - ") + toString(largestValue) + std::string ("]") );
				}

				return total;
			}

			///Gets a random int min <= x <= max;
			int getInt(int min,int max)
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

			///Gets a random int min <= x <= max;
			///Gets a random floating point number low <= x <= high
			double getDouble()
			{
				double randNum = realGen();

#if DEBUG_RANDOM
				if(randNum<0.0 || randNum>=1.0)
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Random number out of range!");
				}
#endif

				return randNum;
			}

			//Gets a double in the range [low,high)
			double getDouble(double low,double high)
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

			//A normally-distributed number has a (very low)
			//chance of being a very large/small number, 
			//hence the min and max
			double getNormalDistributionDouble(
					double scale,
					double minValue,
					double maxValue
					)
			{
				double retval = normalGen()*scale;
				if(retval<minValue)
				{
					return minValue;
				}
				else if(retval>maxValue)
				{
					return maxValue;
				}
				else
				{
					return retval;
				}
			}
			
			inline unsigned int getSeed()
			{
				return seed;
			}

		protected:
	};
}

#endif

