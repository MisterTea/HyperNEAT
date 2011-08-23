//----------------------------------------------------------------------------
/** @file SgMiaiMap.cpp
    See SgMiaiMap.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgMiaiMap.h"

#include "SgWrite.h"

//----------------------------------------------------------------------------

SgMiaiMap::SgMiaiMap() 
    : m_forcedMove(SG_NULLPOINT),
      m_failed(false),
      m_map(SgArray<int,SG_MAXPOINT>(SG_NULLPOINT))
{
}
    
/** See SgStrategy::ExecuteMove */
void SgMiaiMap::ExecuteMove(SgPoint p, SgBlackWhite player)
{
    if (m_forcedMove != SG_NULLPOINT)
    {
        SG_ASSERT(m_forcedMove == p);
        m_forcedMove = SG_NULLPOINT;
    }
    else if (m_map[player][p] != SG_NULLPOINT)
    {
        m_forcedMove = m_map[player][p];
        m_map[player][p] = SG_NULLPOINT;
        m_map[player][m_forcedMove] = SG_NULLPOINT; // too early???
        
        SgBlackWhite opp = SgOppBW(player);
        SgPoint temp = m_map[opp][p];
        if (temp != SG_NULLPOINT)
        {
            SG_ASSERT(temp != SG_NULLPOINT);
            m_map[opp][p] = SG_NULLPOINT;
            SG_ASSERT(m_map[opp][temp] == p);
            m_map[opp][temp] = SG_NULLPOINT;
        }
    }
}

SgStrategyStatus SgMiaiMap::Status() const
{
    return m_failed ? SGSTRATEGY_FAILED :
           m_forcedMove != SG_NULLPOINT ? SGSTRATEGY_THREATENED :
           SGSTRATEGY_ACHIEVED;
}

void SgMiaiMap::ConvertFromSgMiaiStrategy(const SgMiaiStrategy& s)
{
    SG_ASSERT(! s.HasOverlappingMiaiPairs());
    
    const SgBlackWhite player = s.Player();
    for (SgVectorIterator<SgMiaiPair> it(s.MiaiStrategies()); it; ++it)
    {
        const SgPoint p1 = (*it).first;
        const SgPoint p2 = (*it).second;
        SG_ASSERT(m_map[player][p1] == SG_NULLPOINT);
        SG_ASSERT(m_map[player][p2] == SG_NULLPOINT);
        m_map[player][p1] = p2;
        m_map[player][p2] = p1;
    }
    const SgPoint m = s.OpenThreatMove();
    if (m != SG_NULLPOINT)
        m_forcedMove = m;
}

void SgMiaiMap::ConvertToSgMiaiStrategy(SgMiaiStrategy* s) const
{
    SG_UNUSED(s);
    SG_ASSERT(false);
}

//----------------------------------------------------------------------------
