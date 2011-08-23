//----------------------------------------------------------------------------
/** @file SgSearchTracer.cpp
    See SgSearchTracer.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgSearchTracer.h"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <sstream>
#include <math.h>
#include "SgDebug.h"
#include "SgHashTable.h"
#include "SgVector.h"
#include "SgMath.h"
#include "SgNode.h"
#include "SgSearchValue.h"
#include "SgTime.h"
#include "SgWrite.h"

using namespace std;

//----------------------------------------------------------------------------

SgSearchTracer::SgSearchTracer(SgNode* root) 
    : m_traceNode(root)
{
}

SgSearchTracer::~SgSearchTracer()
{
}

void SgSearchTracer::AddMoveProp(SgNode* node, SgMove move, 
                                 SgBlackWhite player)
{
    node->AddMoveProp(move, player);
}

void SgSearchTracer::AddTraceNode(SgMove move, SgBlackWhite player)
{
    if (m_traceNode != 0)
    {
        m_traceNode = m_traceNode->NewRightMostSon();
        AddMoveProp(m_traceNode, move, player);
    }
}

void SgSearchTracer::AppendTrace(SgNode* toNode)
{
    if (m_traceNode != 0)
    {
        m_traceNode->Root()->AppendTo(toNode);
        m_traceNode = 0;
    }
}

void SgSearchTracer::InitTracing(const string& type)
{
    SG_ASSERT(! m_traceNode);
    m_traceNode = new SgNode();
    m_traceNode->Add(new SgPropText(SG_PROP_COMMENT, type));
}

void SgSearchTracer::StartOfDepth(int depth)
{
    SG_ASSERT(m_traceNode != 0);
    if (depth > 0 && m_traceNode->HasFather())
    {
        // true for each depth except the very first
        // AR: the 0 should really be the depthMin parameter of iterated
        // search. this will break if depthMin != 0 and generate strange
        // trace trees.
        m_traceNode = m_traceNode->Father();
        // go from root of previous level to root
    }
    m_traceNode = m_traceNode->NewRightMostSon();
    SG_ASSERT(m_traceNode != 0);
    m_traceNode->SetIntProp(SG_PROP_MAX_DEPTH, depth);
    ostringstream stream;
    stream << "Iteration d = " << depth << ' ';
    // @todo: trace search.TimeUsed()
    m_traceNode->AddComment(stream.str());

    // @todo would be interesting to know time used for each depth,
    // create SG_PROP_TIME_USED property at EndOfDepth (doesn't exist yet)
}

void SgSearchTracer::TakeBackTraceNode()
{
    if (m_traceNode != 0)
        m_traceNode = m_traceNode->Father();
}

void SgSearchTracer::TraceComment(const char* comment) const
{
    if (m_traceNode != 0)
    {
        m_traceNode->AddComment(comment);
        m_traceNode->AddComment("\n");
    }
}

void SgSearchTracer::TraceValue(int value, SgBlackWhite toPlay) const
{
    SG_ASSERT(m_traceNode != 0);
    // The value needs to be recorded in absolute terms, not relative to
    // the current player.
    int v = (toPlay == SG_WHITE) ? -value : +value;
    m_traceNode->Add(new SgPropValue(SG_PROP_VALUE, v));
}

void SgSearchTracer::TraceValue(int value, SgBlackWhite toPlay,
                                const char* comment, bool isExact) const
{
    TraceValue(value, toPlay);
    if (comment != 0)
        TraceComment(comment);
    if (isExact)
    {
        m_traceNode->Add(new SgPropMultiple(SG_PROP_CHECK, 1));
        TraceComment("exact");
    }
}

//----------------------------------------------------------------------------

