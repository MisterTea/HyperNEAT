//----------------------------------------------------------------------------
/** @file SgNbIterator.h
    Neighbor point iterators
*/
//----------------------------------------------------------------------------

#ifndef SG_NBITERATOR_H
#define SG_NBITERATOR_H

#include <algorithm>
#include "SgPoint.h"

//----------------------------------------------------------------------------

/** Iterator over all 4 neighbor points.
    Iterates in sorted order.
    See also SgNbIterator for an iterator that filters points off board, but
    needs access to current board.

    Vertical and horizontal neighbors (a subset of SgNb8Iterator).
    @verbatim
       0
       |
    1 -p- 2    e.g. next[2] = +WE
       |
       3
    @endverbatim
*/
class SgNb4Iterator
    : public SgArray<SgPoint,4>::Iterator
{
public:
    SgNb4Iterator(SgPoint p)
        : SgArray<SgPoint,4>::Iterator(s_precomp.m_nb[p])
    {
        SG_ASSERT(SgPointUtil::InBoardRange(p));
    }

private:
    /** Precomputed neighbors. */
    struct Precomp
    {
        Precomp();

        SgArray<SgArray<SgPoint,4>,SG_MAXPOINT> m_nb;
    };

    static const Precomp s_precomp;

    /** Not implemented. */
    SgNb4Iterator(const SgNb4Iterator&);

    /** Not implemented. */
    SgNb4Iterator& operator=(const SgNb4Iterator&);
};

//----------------------------------------------------------------------------

/** Iterator over all 4 diagonal neighbor points.
    Iterates in sorted order.
    Does not filter points off board, that would need access to current board

    Diagonal neighbors (a subset of SgNb8Iterator).
    @verbatim
    0     1
     \   /
       p       e.g. next[0] = -NS-WE
     /   \
    2     3
    @endverbatim
*/
class SgNb4DiagIterator
{
public:
    SgNb4DiagIterator(SgPoint p)
        : m_next(0),
          m_p(p)
    {
        SG_ASSERT(SgPointUtil::InBoardRange(p));
    }

    /** Advance the state of the iteration to the next element. */
    void operator++()
    {
        SG_ASSERT(m_next < 4);
        ++m_next;
    }

    /** Return the value of the current element. */
    SgPoint operator*() const
    {
        return m_p + s_diag[m_next];
    }

    /** Return true if iteration is valid, otherwise false. */
    operator bool() const
    {
        return m_next < 4;
    }

private:
    int m_next;

    SgPoint m_p;

    static const int s_diag[4];

    /** Not implemented. */
    SgNb4DiagIterator(const SgNb4DiagIterator&);

    /** Not implemented. */
    SgNb4DiagIterator& operator=(const SgNb4DiagIterator&);
};

//----------------------------------------------------------------------------

/** Iterator over all 8 neighbor points.
    Iterates in sorted order.
    Does not filter points off board, that would need access to current board

    Produces the neighbors of a point in all eight compass
    directions, in numerically ascending order.
    Directions 1, 3, 4 and 6 are vertical/horizontal, other are diagonal.
    @verbatim
    0  1  2
     \ | /
    3 -p- 4    e.g. next[1] = -NS
     / | \
    5  6  7
    @endverbatim
*/
class SgNb8Iterator
{
public:
    SgNb8Iterator(SgPoint p)
        : m_next(0),
          m_p(p)
    {
        SG_ASSERT(SgPointUtil::InBoardRange(p));
    }

    /** Advance the state of the iteration to the next element. */
    void operator++()
    {
        SG_ASSERT(m_next < 8);
        ++m_next;
    }

    /** Return the value of the current element. */
    SgPoint operator*() const
    {
        return m_p + s_nb8[m_next];
    }

    /** Return true if iteration is valid, otherwise false. */
    operator bool() const
    {
        return m_next < 8;
    }

    static int Direction(int i)
    {
        SG_ASSERTRANGE(i, 0, 7);
        return s_nb8[i];
    }

private:
    int m_next;

    SgPoint m_p;

    static const int s_nb8[8];

    /** Not implemented. */
    SgNb8Iterator(const SgNb8Iterator&);

    /** Not implemented. */
    SgNb8Iterator& operator=(const SgNb8Iterator&);
};

//----------------------------------------------------------------------------

#endif // SG_NBITERATOR_H
