//----------------------------------------------------------------------------
/** @file SgConnCompIterator.h
    Classes ConnCompIterator and ConnComp8Iterator
*/
//----------------------------------------------------------------------------

#ifndef SG_CONNCOMPITERATOR_H
#define SG_CONNCOMPITERATOR_H

#include "SgPointSet.h"

//----------------------------------------------------------------------------

/** Iterate through all connected components of a given set.
    Example: compute all blocks of a given color:
    <pre>
    for (ConnCompIterator it(board.All(color), board); it; ++it) DO
    {
    ...define block with points '*it' and color 'color'...
    }
    </pre>
*/
class SgConnCompIterator
{
public:
    /** Create an iterator to iterate through set on 'board'. */
    SgConnCompIterator(const SgPointSet& set, int boardSize);

    void operator++();

    const SgPointSet& operator*() const;

    operator bool() const
    {
        return m_nextPoint <= m_lastBoardPoint;
    }

private:
    SgPointSet m_set;

    SgPointSet m_nextSet;

    int m_nextPoint;

    int m_lastBoardPoint;
};

//----------------------------------------------------------------------------

/** Iterate through all 8-connected components of a given set. */
class SgConnComp8Iterator
{
public:
    /** Create an iterator to iterate through set on 'board'. */
    SgConnComp8Iterator(const SgPointSet& set, int boardSize);

    void operator++();

    const SgPointSet& operator*() const;

    operator bool() const
    {
        return m_nextPoint <= m_lastBoardPoint;
    }

private:
    SgPointSet m_set;

    SgPointSet m_nextSet;

    int m_nextPoint;

    int m_lastBoardPoint;
};

//----------------------------------------------------------------------------

#endif // SG_CONNCOMPITERATOR_H
