//----------------------------------------------------------------------------
/** @file SgBoardConst.h
    Constant data for a given board size.
*/
//----------------------------------------------------------------------------

#ifndef SG_BOARDCONST_H
#define SG_BOARDCONST_H

#include <boost/shared_ptr.hpp>
#include "SgArray.h"
#include "SgNbIterator.h"
#include "SgPoint.h"
#include "SgPointArray.h"
#include "SgPointIterator.h"
#include "SgPointSet.h"

//----------------------------------------------------------------------------

/** Constant data for a given board size for games with square boards.
    All boards created with the same board size share the same
    data to reduce memory consumption and increase cache hits.
*/
class SgBoardConst
{
public:
    SgBoardConst(SgGrid size);

    /** Change the information for this object to another size.
        Does nothing if 'newSize' is the same.
    */
    void ChangeSize(SgGrid newSize);

    /** Get board size. */
    SgGrid Size() const;

    /** Distance from the nearest border point.
        Points on the edge of the board have distance 1;
        border (off-board) points have distance 0.
    */
    SgGrid Line(SgPoint p) const;

    /** The distance to the second-closest border point.
        Points on the edge of the board have distance 1;
        border (off-board) points have distance 0.
    */
    SgGrid Pos(SgPoint p) const;

    /** Offset for neighboring point away from edge of board.
        @todo: the following seems weird, remove?
        Can also be a diagonal neighbor if no direct up-neighbor exists.
        If no up-neighbor exists at all, returns offset of 0.
    */
    int Up(SgPoint p) const;

    /** Offset for neighbor to the left, when looking in Up() direction */
    int Left(SgPoint p) const;

    /**  */
    int Right(SgPoint p) const;

    /** @todo */
    int Side(SgPoint p, int i) const;

    /** Set of points in corners of the board.
        On 12x12 boards and larger, this includes 5x5 area from corner
        except the 5,5 point itself (the classical opening moves and below).
        See SgBoardConst::BoardConstImpl::BoardConstImpl for details.
        On smaller boards, includes the 4x4 area except the 4,4 point itself.
    */
    const SgPointSet& Corners() const;

    /** Set of points on edge but not in corners of the board.
        On 12x12 boards and larger, this includes points on lines 1-4.
        On smaller boards, lines 1 to 3.
    */
    const SgPointSet& Edges() const;

    /** All points on board not in Corners() or Edges() */
    const SgPointSet& Centers() const;

    /** Points for standard side extensions on lines 2 to 4. */
    const SgPointSet& SideExtensions() const;

    /** Set of all points on given line. */
    const SgPointSet& LineSet(SgGrid line) const;

    /** Upper left corner of the board.
        Using FirstBoardPoint and LastBoardPoint the board points can be
        traversed with a for-loop. This avoids the border points before and
        after the board, but not the ones to the side of the board;
        one must still check for border fields within the loop.
    */
    int FirstBoardPoint() const;

    /** See FirstBoardPoint */
    int LastBoardPoint() const;

    /** List of on-board neighbor points terminated by SG_ENDPOINT.
        Points are sorted from low to high.
    */
    const SgPoint* NeighborIterAddress(SgPoint p) const;

    const SgPoint* BoardIterAddress() const;

    const SgPoint* BoardIterEnd() const;

    const SgPoint* LineIterAddress(SgGrid line) const;

    const SgPoint* CornerIterAddress() const;

private:
    struct BoardConstImpl
    {
        BoardConstImpl(SgGrid size);

        /** Size */
        SgGrid m_size;

        int m_firstBoardPoint;

        int m_lastBoardPoint;

        SgPointArray<SgGrid> m_gridToLine;

        SgPointArray<SgGrid> m_gridToPos;

        /** See SgBoardConst::Neighbors() */
        SgPoint m_neighbors[SG_MAXPOINT][5];

        SgPointArray<int> m_up;

        SgPointSet m_corners;

        SgPointSet m_edges;

        SgPointSet m_centers;

        SgPointSet m_sideExtensions;

        SgPoint m_cornerIter[4 + 1];

        SgPointSet m_line[(SG_MAX_SIZE / 2) + 1];

        SgPoint* m_lineIterAddress[(SG_MAX_SIZE / 2) + 1];

        SgPoint m_boardIter[SG_MAX_ONBOARD + 1];

        SgPoint* m_boardIterEnd;

        SgPoint m_lineIter[SG_MAX_SIZE * SG_MAX_SIZE + (SG_MAX_SIZE / 2) + 1];

        int m_side[2][2 * (SG_NS + SG_WE) + 1];
    };

    typedef SgArray<boost::shared_ptr<BoardConstImpl>,SG_MAX_SIZE + 1>
        BoardConstImplArray;

    boost::shared_ptr<BoardConstImpl> m_const;

    void Create(SgGrid size);

    static BoardConstImplArray s_const;
};

inline SgGrid SgBoardConst::Size() const
{
    return m_const->m_size;
}

inline SgGrid SgBoardConst::Line(SgPoint p) const
{
    return m_const->m_gridToLine[p];
}

inline const SgPoint* SgBoardConst::NeighborIterAddress(SgPoint p) const
{
    return m_const->m_neighbors[p];
}

inline SgGrid SgBoardConst::Pos(SgPoint p) const
{
    return m_const->m_gridToPos[p];
}

inline int SgBoardConst::Up(SgPoint p) const
{
    return m_const->m_up[p];
}

inline int SgBoardConst::Left(SgPoint p) const
{
    return m_const->m_side[0][m_const->m_up[p] + (SG_NS + SG_WE)];
}

inline int SgBoardConst::Right(SgPoint p) const
{
    return m_const->m_side[1][m_const->m_up[p] + (SG_NS + SG_WE)];
}

inline int SgBoardConst::Side(SgPoint p, int i) const
{
    return m_const->m_side[i][m_const->m_up[p] + (SG_NS + SG_WE)];
}

inline const SgPointSet& SgBoardConst::Corners() const
{
    return m_const->m_corners;
}

inline const SgPointSet& SgBoardConst::Edges() const
{
    return m_const->m_edges;
}

inline const SgPointSet& SgBoardConst::Centers() const
{
    return m_const->m_centers;
}

inline const SgPointSet& SgBoardConst::SideExtensions() const
{
    return m_const->m_sideExtensions;
}

inline const SgPointSet& SgBoardConst::LineSet(SgGrid line) const
{
    return m_const->m_line[line - 1];
}

inline int SgBoardConst::FirstBoardPoint() const
{
    return m_const->m_firstBoardPoint;
}

inline int SgBoardConst::LastBoardPoint() const
{
    return m_const->m_lastBoardPoint;
}

inline const SgPoint* SgBoardConst::BoardIterAddress() const
{
    return m_const->m_boardIter;
}

inline const SgPoint* SgBoardConst::BoardIterEnd() const
{
    return m_const->m_boardIterEnd;
}

inline const SgPoint* SgBoardConst::LineIterAddress(SgGrid line) const
{
    return m_const->m_lineIterAddress[line - 1];
}

inline const SgPoint* SgBoardConst::CornerIterAddress() const
{
    return m_const->m_cornerIter;
}

//----------------------------------------------------------------------------

/** Iterate through all points on a specific line on the given board.
    e.g. all points on the third line.
*/
class SgLineIterator
    : public SgPointIterator
{
public:
    SgLineIterator(const SgBoardConst& boardConst, SgGrid line)
        : SgPointIterator(boardConst.LineIterAddress(line))
    { }
};

//----------------------------------------------------------------------------

/** Iterate through all four corner point of the given board. */
class SgCornerIterator
    : public SgPointIterator
{
public:
    SgCornerIterator(const SgBoardConst& boardConst)
        : SgPointIterator(boardConst.CornerIterAddress())
    { }
};

//----------------------------------------------------------------------------

/** Iterate through the two directions along the sides for the given point.
    Returns the offset to the left (clockwise) first, then to the right.
*/
class SgSideIterator
{
public:
    SgSideIterator(const SgBoardConst& boardConst, SgPoint p)
        : m_boardConst(boardConst),
          m_p(p),
          m_index(0)
    {
        SG_ASSERT(m_boardConst.Side(m_p, 0) != 0);
    }

    /** Advance the state of the iteration to the next element. */
    void operator++()
    {
        ++m_index;
    }

    /** Return the value of the current element. */
    int operator*() const
    {
        return m_boardConst.Side(m_p, m_index);
    }

    /** Return true if iteration is valid, otherwise false. */
    operator bool() const
    {
        return m_index <= 1;
    }

private:
    const SgBoardConst& m_boardConst;

    SgPoint m_p;

    int m_index;

    /** Not implemented. */
    SgSideIterator(const SgSideIterator&);

    /** Not implemented. */
    SgSideIterator& operator=(const SgSideIterator&);
};

//----------------------------------------------------------------------------

/** Iterate over all on-board neighbor points.
    The points are iterated in sorted order (from low to high).
*/
class SgNbIterator
    : public SgPointIterator
{
public:
    SgNbIterator(const SgBoardConst& boardConst, SgPoint p);
};

inline SgNbIterator::SgNbIterator(const SgBoardConst& boardConst, SgPoint p)
    : SgPointIterator(boardConst.NeighborIterAddress(p))
{
}

//----------------------------------------------------------------------------

#endif // SG_BOARDCONST_H
