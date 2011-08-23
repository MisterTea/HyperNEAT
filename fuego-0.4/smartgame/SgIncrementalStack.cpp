//----------------------------------------------------------------------------
/** @file SgIncrementalStack.cpp
    See SgIncrementalStack.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgIncrementalStack.h"

#include "SgBWSet.h"
#include "SgPointSet.h"

//----------------------------------------------------------------------------

void SgIncrementalStack::PushPts(int type, SgEmptyBlackWhite col,
                          const SgPointSet& pts)
// Events relevant for maintaining the state in an ExecuteMove or UndoMove
//   are stored in and retrieved from a stack. Each event consists of
//   1. an event type
//   2. the number of points for the event on the stack
//   3. a color (SG_EMPTY, SG_BLACK or SG_WHITE, meaning depends on event
//               type)
//   4. a list of points (as many as given in 2.)
// Events are pushed in reverse order, and popped in the right order
// (e.g. number of items before list of items)
{
    int nu = 0;
    for (SgSetIterator it(pts); it; ++it)
    {
        PushPoint(*it);
        ++nu;
    }
    PushInt(col);
    PushInt(nu);
    PushInt(type);
}

void SgIncrementalStack::PushPt(int type, SgEmptyBlackWhite col, SgPoint pt)
// same as PushPts for a single point AR: could be optimized for space by
// using different type tags for single and multiple point
{
    PushPoint(pt);
    PushInt(col);
    PushInt(1);// nu pts
    PushInt(type);
}

void SgIncrementalStack::PushPtrEvent(int type, void* ptr)
{
    PushPtr(ptr);
    PushInt(type);
}

void SgIncrementalStack::StartMoveInfo()
{
    PushInt(SG_NEXTMOVE);
}

void SgIncrementalStack::Clear()
{
    m_stack.Clear();
}

void SgIncrementalStack::SubtractPoints(SgPointSet* set)
{
    int nu = PopInt();
    SgEmptyBlackWhite col = PopInt();
    SG_UNUSED(col);
    for (int i = 1; i <= nu; ++i)
    {
        SgPoint p = PopPoint();
        set->Exclude(p);
    }
}

void SgIncrementalStack::AddPoints(SgPointSet* set)
{
    int nu = PopInt();
    SgEmptyBlackWhite col = PopInt();
    SG_UNUSED(col);
    for (int i = 1; i <= nu; ++i)
    {
        SgPoint p = PopPoint();
        set->Include(p);
    }
}

void SgIncrementalStack::SubtractPoints(SgBWSet* set)
{
    int nu = PopInt();
    SgBlackWhite col = PopInt();
    SgPointSet& s = (*set)[col];
    for (int i = 1; i <= nu; ++i)
    {
        SgPoint p = PopPoint();
        s.Exclude(p);
    }
}

void SgIncrementalStack::AddPoints(SgBWSet* set)
{
    int nu = PopInt();
    SgBlackWhite col = PopInt();
    SgPointSet& s = (*set)[col];
    for (int i = 1; i <= nu; ++i)
    {
        SgPoint p = PopPoint();
        s.Include(p);
    }
}

void SgIncrementalStack::SubtractAndAddPoints(SgBWSet* subtractset,
            SgBWSet* addset)
{
    int nu = PopInt();
    SgBlackWhite col = PopInt();
    SgPointSet& s1 = (*subtractset)[col];
    SgPointSet& s2 = (*addset)[col];
    for (int i = 1; i <= nu; ++i)
    {
        SgPoint p = PopPoint();
        s1.Exclude(p);
        s2.Include(p);
    }
}

void SgIncrementalStack::SubtractAndAddPoints(SgPointSet* subtractset,
                                       SgBWSet* addset)
{
    int nu = PopInt();
    SgBlackWhite col = PopInt();
    SgPointSet& s1 = (*subtractset);
    SgPointSet& s2 = (*addset)[col];
    for (int i = 1; i <= nu; ++i)
    {
        SgPoint p = PopPoint();
        s1.Exclude(p);
        s2.Include(p);
    }
}

void SgIncrementalStack::SubtractAndAddPoints(SgBWSet* subtractset,
                                       SgPointSet* addset)
{
    int nu = PopInt();
    SgBlackWhite col = PopInt();
    SgPointSet& s1 = (*subtractset)[col];
    SgPointSet& s2 = (*addset);
    for (int i = 1; i <= nu; ++i)
    {
        SgPoint p = PopPoint();
        s1.Exclude(p);
        s2.Include(p);
    }
}

//----------------------------------------------------------------------------
