//----------------------------------------------------------------------------
/** @file SgConnCompIterator.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgConnCompIterator.h"

using namespace std;

//----------------------------------------------------------------------------

SgConnCompIterator::SgConnCompIterator(const SgPointSet& set, int boardSize)
    : m_set(set),
      m_nextPoint(SgPointUtil::Pt(1, 1) - 1),
      m_lastBoardPoint(SgPointUtil::Pt(boardSize, boardSize))
{
    SG_ASSERTRANGE(boardSize, 1, SG_MAX_SIZE);
    operator++(); // find first.
}

void SgConnCompIterator::operator++()
{
    ++m_nextPoint;
    while ((m_nextPoint <= m_lastBoardPoint) && !(m_set[m_nextPoint]))
        ++m_nextPoint;
    if (m_nextPoint <= m_lastBoardPoint)
    {
        m_nextSet = m_set.ConnComp(m_nextPoint);
        m_set -= m_nextSet;
    }
}

const SgPointSet& SgConnCompIterator::operator*() const
{
    SG_ASSERT(m_nextPoint <= m_lastBoardPoint);
    return m_nextSet;
}

//----------------------------------------------------------------------------

SgConnComp8Iterator::SgConnComp8Iterator(const SgPointSet& set, int boardSize)
    : m_set(set),
      m_nextPoint(SgPointUtil::Pt(1, 1) - 1),
      m_lastBoardPoint(SgPointUtil::Pt(boardSize, boardSize))
{
    SG_ASSERTRANGE(boardSize, 1, SG_MAX_SIZE);
    operator++(); // find first.
}

void SgConnComp8Iterator::operator++()
{
    ++m_nextPoint;
    while ((m_nextPoint <= m_lastBoardPoint) && !(m_set[m_nextPoint]))
        ++m_nextPoint;
    if (m_nextPoint <= m_lastBoardPoint)
    {
        m_nextSet = m_set.ConnComp8(m_nextPoint);
        m_set -= m_nextSet;
    }
}

const SgPointSet& SgConnComp8Iterator::operator*() const
{
    SG_ASSERT(m_nextPoint <= m_lastBoardPoint);
    return m_nextSet;
}

//----------------------------------------------------------------------------

