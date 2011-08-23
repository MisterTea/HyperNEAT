//----------------------------------------------------------------------------
/** @file SgSearchStatistics.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgSearchStatistics.h"

#include <iostream>
#include "SgTime.h"

using namespace std;

//----------------------------------------------------------------------------

SgSearchStatistics::SgSearchStatistics()
{
    Clear();
}

SgSearchStatistics::SgSearchStatistics(const SgSearchStatistics& stat)
    : m_numNodes(stat.m_numNodes),
      m_numEvals(stat.m_numEvals),
      m_numMoves(stat.m_numMoves),
      m_numPass(stat.m_numPass),
      m_depthReached(stat.m_depthReached),
      m_timeUsed(stat.m_timeUsed)
{
}

SgSearchStatistics::~SgSearchStatistics()
{
}

SgSearchStatistics&
SgSearchStatistics::operator=(const SgSearchStatistics& rhs)
{
    if (this != &rhs)
    {
        m_numNodes = rhs.m_numNodes;
        m_numEvals = rhs.m_numEvals;
        m_numMoves = rhs.m_numMoves;
        m_numPass = rhs.m_numPass;
        m_timeUsed = rhs.m_timeUsed;
        m_depthReached = rhs.m_depthReached;
    }
    return *this;
}

SgSearchStatistics&
SgSearchStatistics::operator+=(const SgSearchStatistics& rhs)
{
    m_numNodes += rhs.m_numNodes;
    m_numEvals += rhs.m_numEvals;
    m_numMoves += rhs.m_numMoves;
    m_numPass += rhs.m_numPass;
    m_timeUsed += rhs.m_timeUsed;
    if (m_depthReached < rhs.m_depthReached)
    {
        m_depthReached = rhs.m_depthReached;
    }
    return *this;
}

void SgSearchStatistics::Clear()
{
    m_numNodes = 0;
    m_numEvals = 0;
    m_numMoves = 0;
    m_numPass = 0;
    m_timeUsed = 0;
    m_depthReached = 0;
}

SgSearchStatistics* SgSearchStatistics::Duplicate() const
{
    return new SgSearchStatistics(*this);
}

double SgSearchStatistics::NumNodesPerSecond() const
{
    double used = TimeUsed();
    if (used == 0)
        return 0;
    return m_numNodes / used;
}

double SgSearchStatistics::NumEvalsPerSecond() const
{
    double used = TimeUsed();
    if (used == 0)
        return 0;
    return m_numEvals / used;
}

ostream& operator<<(ostream& stream, const SgSearchStatistics &s)
{
    stream << "SearchStatistics: "
           << s.NumNodes() << " Nodes, "
           << s.NumEvals() << " Evals, Time: "
           << SgTime::Format(s.TimeUsed())
           << " DepthReached: " << s.DepthReached() << ", " 
           << s.NumNodesPerSecond() << " Nodes/s, "
           << s.NumEvalsPerSecond() << " Evals/s\n";
    return stream;
}

//----------------------------------------------------------------------------

