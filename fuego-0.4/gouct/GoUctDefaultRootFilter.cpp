//----------------------------------------------------------------------------
/** @file GoUctDefaultRootFilter.cpp
    See GoUctDefaultRootFilter.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctDefaultRootFilter.h"

#include "GoBensonSolver.h"
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoModBoard.h"
#include "GoSafetySolver.h"
#include "SgWrite.h"

using namespace std;

//----------------------------------------------------------------------------

/** Return true, if point is on edge line and no stone is within a
    Manhattan distance of 4.
*/
bool IsEmptyEdge(const GoBoard& bd, SgPoint p)
{
    if (bd.Occupied(p))
        return false;
    int size = bd.Size();
    int col = SgPointUtil::Col(p);
    int row = SgPointUtil::Row(p);
    if (! (col == 1 || col == size || row == 1 || row == size))
        return false;
    for (int deltaCol = -4; deltaCol <= 4; ++deltaCol)
        for (int deltaRow = -4; deltaRow <= 4; ++deltaRow)
        {
            if (col + deltaCol < 1 || col + deltaCol > size
                || row + deltaRow < 1 || row + deltaRow > size
                || abs(deltaCol) + abs(deltaRow) > 4)
                continue;
            if (bd.Occupied(SgPointUtil::Pt(col + deltaCol,
                                            row + deltaRow)))
                return false;
        }
    return true;
}

//----------------------------------------------------------------------------

GoUctDefaultRootFilter::GoUctDefaultRootFilter(const GoBoard& bd)
    : m_bd(bd),
      m_checkLadders(true),
      m_minLadderLength(6)
{
}

vector<SgPoint> GoUctDefaultRootFilter::Get()
{
    vector<SgPoint> rootFilter;
    SgBlackWhite toPlay = m_bd.ToPlay();
    SgBlackWhite opp = SgOppBW(toPlay);

    // Safe territory

    GoModBoard modBoard(m_bd);
    GoBoard& bd = modBoard.Board();

    SgBWSet alternateSafe;
    bool isAllAlternateSafe = false;
    // Alternate safety is used to prune moves only in opponent territory
    // and only if everything is alive under alternate play. This ensures that
    // capturing moves that are not liberties of dead blocks and ko threats
    // will not be pruned. This alternate safety pruning is not going to
    // improve or worsen playing strength, but may cause earlier passes,
    // which is nice in games against humans
    GoSafetySolver safetySolver(bd);
    safetySolver.FindSafePoints(&alternateSafe);
    isAllAlternateSafe = (alternateSafe.Both() == bd.AllPoints());

    // Benson solver guarantees that capturing moves of dead blocks are
    // liberties of the dead blocks and that no move in Benson safe territory
    // is a ko threat
    GoBensonSolver bensonSolver(bd);
    SgBWSet unconditionalSafe;
    bensonSolver.FindSafePoints(&unconditionalSafe);

    for (GoBoard::Iterator it(bd); it; ++it)
    {
        SgPoint p = *it;
        if (m_bd.IsLegal(p))
        {
            bool isUnconditionalSafe = unconditionalSafe[toPlay].Contains(p);
            bool isUnconditionalSafeOpp = unconditionalSafe[opp].Contains(p);
            bool isAlternateSafeOpp = alternateSafe[opp].Contains(p);
            bool hasOppNeighbors = bd.HasNeighbors(p, opp);
            // Always generate capturing moves in own safe territory, even
            // if current rules do no use CaptureDead(), because the UCT
            // player always scores with Tromp-Taylor after two passes in the
            // in-tree phase
            if ((isAllAlternateSafe && isAlternateSafeOpp)
                || isUnconditionalSafeOpp
                || (isUnconditionalSafe && ! hasOppNeighbors))
                rootFilter.push_back(p);
        }
    }

    // Loosing ladder defense moves
    if (m_checkLadders)
        for (GoBlockIterator it(m_bd); it; ++it)
        {
            SgPoint p = *it;
            if (m_bd.GetStone(p) == toPlay && m_bd.InAtari(p))
            {
                if (m_ladder.Ladder(m_bd, p, toPlay, &m_ladderSequence,
                                    false/*twoLibIsEscape*/) < 0)
                {
                    if (m_ladderSequence.Length() >= m_minLadderLength)
                        rootFilter.push_back(m_bd.TheLiberty(p));
                }
            }

        }

    // Moves on edge line, if no stone is near
    for (GoBoard::Iterator it(m_bd); it; ++it)
    {
        SgPoint p = *it;
        if (IsEmptyEdge(m_bd, p))
            rootFilter.push_back(p);
    }

    return rootFilter;
}

//----------------------------------------------------------------------------
