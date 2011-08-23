//----------------------------------------------------------------------------
/** @file SgRandom.h
    Random numbers.
*/
//----------------------------------------------------------------------------

#ifndef SG_RANDOM_H
#define SG_RANDOM_H

#include <algorithm>
#include <list>
#include <boost/random/mersenne_twister.hpp>
#include "SgArray.h"

//----------------------------------------------------------------------------

/** Random number generator.
    Uses a Mersenne Twister, because this is faster than std::rand() and
    game playing programs usually need faster random numbers more than
    high quality ones. All random generators are internally registered to
    make it possible to change the random seed for all of them.

    SgRandom is thread-safe (w.r.t. different instances) after construction
    (the constructor is not thread-safe, because it uses a global variable
    for registration).
*/
class SgRandom
{
public:
    SgRandom();

    ~SgRandom();

    /** Return the global random number generator.
        The global generator is stored as a static variable in this function
        to ensure that it is initialized at first call if SgRandom is used
        in global variables of other compilation units.
        @note The global random number generator is not thread-safe.
    */
    static SgRandom& Global();

    /** Set random seed for all existing and future instances of SgRandom.
        @param seed The seed. If negative, no seed will be set. If zero, a
        non-deterministic random seed will be used (e.g. derived from the
        current time).
        Also calls std::srand()
        @note This function is not thread-safe.
    */
    static void SetSeed(int seed);

    /** Get random seed.
        See SetSeed(int) for the special meaning of zero and negative values.
    */
    static int Seed();

    /** Get a random integer.
        Uses a fast random generator (the Mersenne Twister boost::mt19937),
        because in games and Monte Carlo simulations, speed is more important
        than quality.
    */
    unsigned int Int();

    /** Get a random integer in [0..range - 1]. */
    int Int(int range);

    /** Get a random integer in [min, max - 1] */
    int Range(int min, int max);

    /** Maximum value. */
    unsigned int Max();

    /** convert percentage between 0 and 99 to a threshold for RandomEvent.
        Use as in following example:
        const unsigned int percent80 = PercentageThreshold(80);
    */
    unsigned int PercentageThreshold(int percentage);

    /** return true if random number SgRandom() <= threshold */
    bool RandomEvent(unsigned int threshold);

private:
    struct GlobalData
    {
        /** The random seed.
            Zero means not to set a random seed.
        */
        boost::mt19937::result_type m_seed;

        std::list<SgRandom*> m_allGenerators;

        GlobalData();
    };

    /** Return global data.
        Global data is stored as a static variable in this function to ensure
        that it is initialized at first call if SgRandom is used in global
        variables of other compilation units.
    */
    static GlobalData& GetGlobalData();

    boost::mt19937 m_generator;

    void SetSeed();
};

inline unsigned int SgRandom::Int()
{
    return m_generator();
}

inline int SgRandom::Int(int range)
{
    SG_ASSERT(range > 0);
    SG_ASSERT(static_cast<unsigned int>(range) <= SgRandom::Max());
    int i = Int() % range;
    SG_ASSERTRANGE(i, 0, range - 1);
    return i;
}

inline unsigned int SgRandom::Max()
{
    return m_generator.max();
}

inline unsigned int SgRandom::PercentageThreshold(int percentage)
{
    return (m_generator.max() / 100) * percentage;
}

inline bool SgRandom::RandomEvent(unsigned int threshold)
{
    return Int() <= threshold;
}

inline int SgRandom::Range(int min, int max)
{
    return min + Int(max - min);
}

//----------------------------------------------------------------------------

/** Get a random float in [min, max].
    Used std::rand()
*/
float SgRandomFloat(float min, float max);

//----------------------------------------------------------------------------

#endif // SG_RANDOM_H
