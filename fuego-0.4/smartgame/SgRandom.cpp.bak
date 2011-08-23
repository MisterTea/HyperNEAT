//----------------------------------------------------------------------------
/** @file SgRandom.cpp
    See SgRandom.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgRandom.h"

#include <cstdlib>
#include <functional>
#include "SgDebug.h"

using namespace std;

//----------------------------------------------------------------------------

SgRandom::GlobalData::GlobalData()
{
    m_seed = 0;
}

//----------------------------------------------------------------------------

SgRandom::SgRandom()
{
    SetSeed();
    GetGlobalData().m_allGenerators.push_back(this);
}

SgRandom::~SgRandom()
{
    GetGlobalData().m_allGenerators.remove(this);
}

SgRandom& SgRandom::Global()
{
    static SgRandom s_globalGenerator;
    return s_globalGenerator;
}

SgRandom::GlobalData& SgRandom::GetGlobalData()
{
    static GlobalData s_data;
    return s_data;
}

int SgRandom::Seed()
{
    return GetGlobalData().m_seed;
}

void SgRandom::SetSeed()
{
    boost::mt19937::result_type seed = GetGlobalData().m_seed;
    if (seed == 0)
        return;
    m_generator.seed(seed);
}

void SgRandom::SetSeed(int seed)
{
    if (seed < 0)
    {
        GetGlobalData().m_seed = 0;
        return;
    }
    if (seed == 0)
        GetGlobalData().m_seed = time(0);
    else
        GetGlobalData().m_seed = seed;
    SgDebug() << "SgRandom::SetSeed: " << GetGlobalData().m_seed << '\n';
    for_each(GetGlobalData().m_allGenerators.begin(),
             GetGlobalData().m_allGenerators.end(),
             mem_fun(&SgRandom::SetSeed));
    srand(GetGlobalData().m_seed);
}

//----------------------------------------------------------------------------

float SgRandomFloat(float min, float max)
{
    return (max - min) * static_cast<float>(std::rand())
        / static_cast<float>(RAND_MAX) + min;
}

//----------------------------------------------------------------------------
