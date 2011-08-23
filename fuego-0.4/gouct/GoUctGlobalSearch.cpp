//----------------------------------------------------------------------------
/** @file GoUctGlobalSearch.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctGlobalSearch.h"

using namespace std;

//----------------------------------------------------------------------------

GoUctGlobalSearchStateParam::GoUctGlobalSearchStateParam()
    : m_mercyRule(true),
      m_territoryStatistics(false),
      m_lengthModification(0),
      m_scoreModification(0.02)
{
}

//----------------------------------------------------------------------------
