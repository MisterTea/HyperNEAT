//----------------------------------------------------------------------------
/** @file SgSearch.cpp
    See SgSearch.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgSearchControl.h"

//----------------------------------------------------------------------------

SgSearchControl::~SgSearchControl()
{
}

bool SgSearchControl::StartNextIteration(int depth, double elapsedTime,
                                         int numNodes)
{
    SG_UNUSED(depth);
    SG_UNUSED(elapsedTime);
    SG_UNUSED(numNodes);
    return true;
}

//----------------------------------------------------------------------------

SgTimeSearchControl::SgTimeSearchControl(double maxTime)
    : m_maxTime(maxTime)
{
}

SgTimeSearchControl::~SgTimeSearchControl()
{
}

bool SgTimeSearchControl::Abort(double elapsedTime, int ignoreNumNodes)
{
    SG_UNUSED(ignoreNumNodes);
    return elapsedTime >= m_maxTime;
}

//----------------------------------------------------------------------------

SgNodeSearchControl::SgNodeSearchControl(int maxNumNodes)
    : m_maxNumNodes(maxNumNodes)
{ }

SgNodeSearchControl::~SgNodeSearchControl()
{
}

bool SgNodeSearchControl::Abort(double ignoreElapsedTime, int numNodes)
{
    SG_UNUSED(ignoreElapsedTime);
    return numNodes >= m_maxNumNodes;
}

//----------------------------------------------------------------------------

SgCombinedSearchControl::~SgCombinedSearchControl()
{
}

bool SgCombinedSearchControl::Abort(double elapsedTime, int numNodes)
{
    return (numNodes >= m_maxNumNodes || elapsedTime >= m_maxTime);
}

//----------------------------------------------------------------------------

SgRelaxedSearchControl::~SgRelaxedSearchControl()
{
}

bool SgRelaxedSearchControl::Abort(double elapsedTime, int numNodes)
{
    return (elapsedTime >= m_maxTime
            && numNodes >= MIN_NODES_PER_SECOND * m_maxTime);
}

//----------------------------------------------------------------------------

