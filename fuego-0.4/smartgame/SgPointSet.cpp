//----------------------------------------------------------------------------
/** @file SgPointSet.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgPointSet.h"

#include <algorithm>
#include <iostream>
#include "SgNbIterator.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

int MaxDistance(SgPoint p1, SgPoint p2)
{
    return max(abs(SgPointUtil::Row(p1) - SgPointUtil::Row(p2)),
               abs(SgPointUtil::Col(p1) - SgPointUtil::Col(p2)));
}

} // namespace

//----------------------------------------------------------------------------

SgPointSet::PrecompAllPoints::PrecompAllPoints()
{
    for (int size = SG_MIN_SIZE; size <= SG_MAX_SIZE; ++size)
    {
        m_allPoints[size].reset(new SgPointSet());
        for (int col = 1; col <= size; ++col)
            for (int row = 1; row <= size; ++row)
                m_allPoints[size]->Include(SgPointUtil::Pt(col, row));
    }
}

//----------------------------------------------------------------------------

SgPointSet::PrecompAllPoints SgPointSet::s_allPoints;

SgPointSet SgPointSet::Border(int boardSize) const
{
    return (BorderNoClip() & AllPoints(boardSize));
}

SgPointSet SgPointSet::BorderNoClip() const
{
    SgPointSet bd = (*this >> SG_NS);
    bd |= (*this << SG_NS);
    bd |= (*this >> SG_WE);
    bd |= (*this << SG_WE);
    bd -= (*this);
    return bd;
}

SgPointSet SgPointSet::Component(SgPoint p) const
{
    SgPointSet set1, set2;
    set1.Include(p);
    SgPointSet* a = &set1, *b = &set2;
    do
    {
        *b = *a | (a->BorderNoClip() & (*this));
        swap(a, b); // swap pointers, not sets.
    } while (set1 != set2);
    return set1;
}

SgPointSet SgPointSet::ConnComp(SgPoint p) const
{
    // alternative connected component algorithm for large diameter sets
    SgPointSet out, in = (*this);
    SgPoint stack[SG_MAXPOINT]; // AR: use Stack template
    out.Include(p);
    in.Exclude(p);
    int current = 0;
    stack[current] = p;
    while (current >= 0)
    {
        SgPoint q = stack[current--];
        for (SgNb4Iterator it(q); it; ++it)
        {
            SgPoint nb = *it;
            if (in.Contains(nb))
            {
                out.Include(nb);
                in.Exclude(nb);
                stack[++current] = nb;
            }
        }
    }
    return out;
}

SgPointSet SgPointSet::ConnComp8(SgPoint p) const
{
    SG_ASSERT(Contains(p));
    SgPointSet out, in = (*this);
    SgPoint stack[SG_MAXPOINT]; // AR: use Stack template
    out.Include(p);
    in.Exclude(p);
    int current = 0;
    stack[current] = p;
    while (current >= 0)
    {
        SgPoint q = stack[current--];
        for (SgNb8Iterator it(q); it; ++it)
        {
            SgPoint nb = *it;
            if (in.Contains(nb))
            {
                out.Include(nb);
                in.Exclude(nb);
                stack[++current] = nb;
            }
        }
    }
    SG_ASSERT(SupersetOf(out));
    return out;
}

SgPointSet& SgPointSet::Exclude(const SgVector<SgPoint>& vector)
{
    for (SgVectorIterator<SgPoint> it(vector); it; ++it)
        Exclude(*it);
    return (*this);
}

SgPointSet::SgPointSet(const SgVector<SgPoint>& vector)
{
    Clear();
    for (SgVectorIterator<SgPoint> it(vector); it; ++it)
        Include(*it);
}

void SgPointSet::ToVector(SgVector<SgPoint>* list) const
{
    list->Clear();
    for (SgSetIterator si(*this); si; ++si)
        list->PushBack(*si);
}

void SgPointSet::Write(ostream& out, int boardSize) const
{
    for (int row = boardSize; row >= 1; --row)
    {
        for (int col = 1; col <= boardSize; ++col)
            out << (Contains(SgPointUtil::Pt(col, row)) ? '@' : '-');
        out << '\n';
    }
}

void SgPointSet::Grow(int boardSize)
{
    SgPointSet bd = (*this >> SG_NS);
    bd |= (*this << SG_NS);
    bd |= (*this >> SG_WE);
    bd |= (*this << SG_WE);
    bd &= AllPoints(boardSize);
    *this |= bd;
}

void SgPointSet::Grow(SgPointSet* newArea, int boardSize)
{
    *newArea  = (*this >> SG_NS);
    *newArea |= (*this << SG_NS);
    *newArea |= (*this >> SG_WE);
    *newArea |= (*this << SG_WE);
    *newArea &= AllPoints(boardSize);
    *newArea ^= (*this);
    *this |= *newArea;
}

void SgPointSet::Grow8(int boardSize)
{
    SgPointSet bd = (*this >> SG_NS);
    bd |= (*this << SG_NS);
    bd |= (*this >> SG_WE);
    bd |= (*this << SG_WE);
    bd |= (*this >> (SG_NS + SG_WE));
    bd |= (*this << (SG_NS + SG_WE));
    bd |= (*this >> (SG_NS - SG_WE));
    bd |= (*this << (SG_NS - SG_WE));
    bd &= AllPoints(boardSize);
    *this |= bd;
}

SgPointSet SgPointSet::Border8(int boardSize) const
{
    SgPointSet bd = (*this >> SG_NS);
    bd |= (*this << SG_NS);
    bd |= (*this >> SG_WE);
    bd |= (*this << SG_WE);
    bd |= (*this >> (SG_NS + SG_WE));
    bd |= (*this << (SG_NS + SG_WE));
    bd |= (*this >> (SG_NS - SG_WE));
    bd |= (*this << (SG_NS - SG_WE));
    bd -= (*this);
    bd &= AllPoints(boardSize);
    return bd;
}

bool SgPointSet::IsSize(int size) const
{
    SG_ASSERT(size >= 0);
    return (Size() == size);
}

bool SgPointSet::MinSetSize(int size) const
{
    return Size() >= size;
}

bool SgPointSet::MaxSetSize(int size) const
{
    return Size() <= size;
}

bool SgPointSet::Adjacent(const SgPointSet& s) const
{
    return BorderNoClip().Overlaps(s);
}

bool SgPointSet::AllAdjacentTo(const SgPointSet& s) const
{
    return SubsetOf(s.BorderNoClip());
}

bool SgPointSet::AdjacentOnlyTo(const SgPointSet& s, int boardSize) const
{
    return Border(boardSize).SubsetOf(s);
}

bool SgPointSet::IsCloseTo(SgPoint p) const
{
    const int MAX_CLOSE_DISTANCE = 3;
    if (Contains(p))
        return true;
    
    for (SgSetIterator it(*this); it; ++it)
    {
        if (MaxDistance(*it, p) <= MAX_CLOSE_DISTANCE)
            return true;
    }
    return false;
}

SgPointSet SgPointSet::Kernel(int boardSize) const
{
    // Kernel is computed by growing the complement,
    // and subtracting that from the given set.
    // AR: would direct implementation be faster?
    SgPointSet k = AllPoints(boardSize) - (*this);
    SgPointSet bd = (k >> SG_NS);
    bd |= (k << SG_NS);
    bd |= (k >> SG_WE);
    bd |= (k << SG_WE);
    return (*this) - bd;
}

SgPoint SgPointSet::PointOf() const
{
    // slight misuse of iterator, only see if can get one point
    SgSetIterator it(*this);
    if (it)
        return *it;
    else
        return SG_NULLPOINT;
}

SgPoint SgPointSet::Center() const
{ 
    SgRect rect = EnclosingRect();
    if (rect.IsEmpty())
        return SG_NULLPOINT;

    SgPoint idealCenter = rect.Center();

    if (Contains(idealCenter))
        return idealCenter;

    int minDist = 4 * SG_MAX_SIZE;
    SgPoint center = SG_NULLPOINT;
    for (SgSetIterator it(*this); it; ++it)
    {
        int dist =
            MaxDistance(*it, idealCenter)
            + SgPointUtil::Distance(*it, idealCenter);
        if (dist < minDist)
        {
            center  = *it;
            minDist = dist;
        }
    }
    return center;
}

SgRect SgPointSet::EnclosingRect() const
{
    SgRect r;
    for (SgSetIterator it(*this); it; ++it)
        r.Include(*it);
    return r;
}

bool SgPointSet::IsConnected() const
{
    SgPoint p = PointOf();
    if (p == SG_NULLPOINT)
        return true;
    else
        return (Component(p) == (*this));
}

bool SgPointSet::Is8Connected() const
{ // AR: slow.
    SgPoint p = PointOf();
    if (p == SG_NULLPOINT)
        return true; // Empty set
    else 
        return (ConnComp8(p) == (*this));
}

//----------------------------------------------------------------------------

