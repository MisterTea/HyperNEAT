//----------------------------------------------------------------------------
/** @file SgBoardConst.cpp
    See SgBoardConst.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgBoardConst.h"

#include <algorithm>
#include "SgInit.h"
#include "SgStack.h"

using namespace std;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

SgBoardConst::BoardConstImpl::BoardConstImpl(SgGrid size)
    : m_size(size),
      m_firstBoardPoint(Pt(1, 1)),
      m_lastBoardPoint(Pt(size, size))
{
    m_gridToLine.Fill(0);
    m_gridToPos.Fill(0);
    m_up.Fill(0);
    for (SgGrid line = 0; line <= SG_MAX_SIZE / 2; ++line)
        m_line[line].Clear();

    // Set up values for points on the board.
    m_boardIterEnd = m_boardIter;
    SgGrid halfSize = (m_size + 1) / 2;
    for (SgGrid row = 1; row <= m_size; ++row)
    {
        for (SgGrid col = 1; col <= m_size; ++col)
        {
            SgPoint p = Pt(col, row);
            SgGrid lineRow = row > halfSize ? m_size + 1 - row : row;
            SgGrid lineCol = col > halfSize ? m_size + 1 - col : col;
            SgGrid line = min(lineRow, lineCol);
            SgGrid pos = max(lineRow, lineCol);
            m_gridToLine[p] = line;
            m_gridToPos[p] = pos;
            m_line[line - 1].Include(p);
            const int MAX_EDGE = m_size > 11 ? 4 : 3;
            if (line <= MAX_EDGE)
            {
                if (pos <= MAX_EDGE + 1)
                    m_corners.Include(p);
                else
                    m_edges.Include(p);
            }
            else
                m_centers.Include(p);
            int nuNb = 0;
            if (row > 1)
                m_neighbors[p][nuNb++] = Pt(col, row - 1);
            if (col > 1)
                m_neighbors[p][nuNb++] = Pt(col - 1, row);
            if (col < m_size)
                m_neighbors[p][nuNb++] = Pt(col + 1, row);
            if (row < m_size)
                m_neighbors[p][nuNb++] = Pt(col, row + 1);
            m_neighbors[p][nuNb] = SG_ENDPOINT;
            *(m_boardIterEnd++) = p;
        }
    }

    // Set up direction to point on line above.
    for (SgPoint p = 0; p < SG_MAXPOINT; ++p)
    {
        if (m_gridToLine[p] != 0)
        {
            SgGrid line = m_gridToLine[p];
            if (m_gridToLine[p - SG_NS] == line + 1)
                m_up[p] = -SG_NS;
            if (m_gridToLine[p - SG_WE] == line + 1)
                m_up[p] = -SG_WE;
            if (m_gridToLine[p + SG_WE] == line + 1)
                m_up[p] = SG_WE;
            if (m_gridToLine[p + SG_NS] == line + 1)
                m_up[p] = SG_NS;

            // If no line above vertically or horizontally, try diagonally.
            if (m_up[p] == 0)
            {
                if (m_gridToLine[p - SG_NS - SG_WE] == line + 1)
                    m_up[p] = -SG_NS - SG_WE;
                if (m_gridToLine[p - SG_NS + SG_WE] == line + 1)
                    m_up[p] = -SG_NS + SG_WE;
                if (m_gridToLine[p + SG_NS - SG_WE] == line + 1)
                    m_up[p] = SG_NS - SG_WE;
                if (m_gridToLine[p + SG_NS + SG_WE] == line + 1)
                    m_up[p] = SG_NS + SG_WE;
            }
        }
    }

    // Set up direction to the sides, based on Up. Always list clockwise
    // direction first.
    {
        for (int i = 0; i <= 2 * (SG_NS + SG_WE); ++i)
        {
            m_side[0][i] = 0;
            m_side[1][i] = 0;
        }

        // When Up is towards center, sides are orthogonal to Up.
        m_side[0][-SG_NS + (SG_NS + SG_WE)] = -SG_WE;
        m_side[1][-SG_NS + (SG_NS + SG_WE)] = +SG_WE;
        m_side[0][-SG_WE + (SG_NS + SG_WE)] = +SG_NS;
        m_side[1][-SG_WE + (SG_NS + SG_WE)] = -SG_NS;
        m_side[0][+SG_WE + (SG_NS + SG_WE)] = -SG_NS;
        m_side[1][+SG_WE + (SG_NS + SG_WE)] = +SG_NS;
        m_side[0][+SG_NS + (SG_NS + SG_WE)] = +SG_WE;
        m_side[1][+SG_NS + (SG_NS + SG_WE)] = -SG_WE;

        // When Up is diagonal, sides are along different sides of board.
        m_side[0][-SG_NS - SG_WE + (SG_NS + SG_WE)] = -SG_WE;
        m_side[1][-SG_NS - SG_WE + (SG_NS + SG_WE)] = -SG_NS;
        m_side[0][-SG_NS + SG_WE + (SG_NS + SG_WE)] = -SG_NS;
        m_side[1][-SG_NS + SG_WE + (SG_NS + SG_WE)] = +SG_WE;
        m_side[0][+SG_NS - SG_WE + (SG_NS + SG_WE)] = +SG_NS;
        m_side[1][+SG_NS - SG_WE + (SG_NS + SG_WE)] = -SG_WE;
        m_side[0][+SG_NS + SG_WE + (SG_NS + SG_WE)] = +SG_WE;
        m_side[1][+SG_NS + SG_WE + (SG_NS + SG_WE)] = +SG_NS;
    }

    // Terminate board iterator.
    *m_boardIterEnd = SG_ENDPOINT;

    // Set up line iterators.
    {
        int lineIndex = 0;
        for (SgGrid line = 1; line <= (SG_MAX_SIZE / 2) + 1; ++line)
        {
            m_lineIterAddress[line-1] = &m_lineIter[lineIndex];
            for (SgPoint p = m_firstBoardPoint; p <= m_lastBoardPoint; ++p)
            {
                if (m_gridToLine[p] == line)
                    m_lineIter[lineIndex++] = p;
            }
            m_lineIter[lineIndex++] = SG_ENDPOINT;
            SG_ASSERT(lineIndex
                      <= SG_MAX_SIZE * SG_MAX_SIZE + (SG_MAX_SIZE / 2) + 1);
        }
        SG_ASSERT(lineIndex == m_size * m_size + (SG_MAX_SIZE / 2) + 1);
    }

    // Set up corner iterator.
    m_cornerIter[0] = Pt(1, 1);
    m_cornerIter[1] = Pt(m_size, 1);
    m_cornerIter[2] = Pt(1, m_size);
    m_cornerIter[3] = Pt(m_size, m_size);
    m_cornerIter[4] = SG_ENDPOINT;

    m_sideExtensions = m_line[2 - 1] | m_line[3 - 1] | m_line[4 - 1];
    // LineSet(line)  == m_line[line-1], see .h
    // exclude diagonals, so that different sides from corner are in different
    // sets.
    m_sideExtensions.Exclude(Pt(2, 2));
    m_sideExtensions.Exclude(Pt(2, m_size + 1 - 2));
    m_sideExtensions.Exclude(Pt(m_size + 1 - 2, 2));
    m_sideExtensions.Exclude(Pt(m_size + 1 - 2, m_size + 1 - 2));
    if (m_size > 2)
    {
        m_sideExtensions.Exclude(Pt(3, 3));
        m_sideExtensions.Exclude(Pt(3, m_size + 1 - 3));
        m_sideExtensions.Exclude(Pt(m_size + 1 - 3, 3));
        m_sideExtensions.Exclude(Pt(m_size + 1 - 3, m_size + 1 - 3));
        if (m_size > 3)
        {
            m_sideExtensions.Exclude(Pt(4, 4));
            m_sideExtensions.Exclude(Pt(4, m_size + 1 - 4));
            m_sideExtensions.Exclude(Pt(m_size + 1 - 4, 4));
            m_sideExtensions.Exclude(Pt(m_size + 1 - 4, m_size + 1 - 4));
        }
    }
}

//----------------------------------------------------------------------------

SgBoardConst::BoardConstImplArray SgBoardConst::s_const;

void SgBoardConst::Create(SgGrid size)
{
    SG_ASSERT_GRIDRANGE(size);
    if (! s_const[size])
        s_const[size] =
            boost::shared_ptr<BoardConstImpl>(new BoardConstImpl(size));
    m_const = s_const[size];
    SG_ASSERT(m_const);
}

SgBoardConst::SgBoardConst(SgGrid size)
{
    SG_ASSERT_GRIDRANGE(size);
    SgInitCheck();
    Create(size);
}

void SgBoardConst::ChangeSize(SgGrid newSize)
{
    SG_ASSERT_GRIDRANGE(newSize);
    SG_ASSERT(m_const);
    // Don't do anything if called with same size.
    SgGrid oldSize = m_const->m_size;
    if (newSize != oldSize)
        Create(newSize);
}

//----------------------------------------------------------------------------

