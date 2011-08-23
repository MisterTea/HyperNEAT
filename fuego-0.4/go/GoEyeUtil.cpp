//----------------------------------------------------------------------------
/** @file GoEyeUtil.cpp
    See GoEyeUtil.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoEyeUtil.h"

#include "GoBoardUtil.h"
#include "GoEyeCount.h"
#include "SgNbIterator.h"

//----------------------------------------------------------------------------
namespace
{

/** Count number of points on edge of board (Line 1) */
int NuEdgePoints(const GoBoard& bd, const SgPointSet& points)
{
    return (points & bd.LineSet(1)).Size();
}

/** Recognizes 2x2 block of points.
    Relies on the current implementation 
    where SgSetIterator produces set members in sorted order,
    such that bulky four points have values p, p+WE, p+NS, p+WE+NS
*/
bool IsBulkyFour(const SgPointSet& points)
{
    SG_ASSERT(points.IsSize(4));
    SgSetIterator it(points); 
    SgPoint p1 = *it;
    ++it;
    if (*it != p1 + SG_WE)
        return false;
    ++it;
    if (*it != p1 + SG_NS)
        return false;
    ++it;
    if (*it != p1 + SG_WE + SG_NS)
        return false;
    SG_ASSERT(GoEyeUtil::DegreeCode(points) == 400);
    return true;
}

bool IsTShape(const SgPointSet& block)
{
    return GoEyeUtil::DegreeCode(block) == 1030;
}

bool IsBulkyFive(const SgPointSet& block)
{
    return GoEyeUtil::DegreeCode(block) == 1310;
}

bool IsCross(const SgPointSet& block)
{
    return GoEyeUtil::DegreeCode(block) == 10040;
}

bool IsRabbitySix(const SgPointSet& block)
{
    return GoEyeUtil::DegreeCode(block) == 10320;
}
        
bool Is2x3Area(const SgPointSet& area)
{
    return GoEyeUtil::DegreeCode(area) == 2400;
}
        
/** Block has a shape that gives the opponent two eyes,
    if it gets captured by a single opponent surrounding block.
    @todo needs to check if all points in block 
    are adjacent to defender's stones - to avoid bent 4, 3x2 in corner etc.
*/
bool IsAliveBlock(const SgPointSet& block)
{
    const int code = GoEyeUtil::DegreeCode(block);
    
    return code == 220 // stretched 4 in a row (not alive in bent four)
        || code == 320 // streched 5 in a row
        || code == 1130 // T-shape or L-plus-foot
        || code == 2400 // solid 2x3 block (not alive in Corner)
        || code == 2220 // alive 6 point - not rabbity six
        ;
}

bool CheckAlwaysAlive8Code(int code)
{
    return    code == 11210 // T
           || code == 3110 // L with foot
           ;
}

/** Block has a shape that gives the opponent two eyes,
    and cannot be extended into a nakade shape.
    @todo needs to check if all points in block 
    are adjacent to defender's stones - to avoid bent 4, 3x2 in corner etc.
*/
bool IsAlwaysAliveBlock(const SgPointSet& block)
{
    const int code = GoEyeUtil::DegreeCode(block);
    
    return code == 320 // streched 5 in a row
        || (   code == 1130 // T-shape or L-plus-foot
            && CheckAlwaysAlive8Code(GoEyeUtil::DegreeCode8(block))
           )
        ;
}

/** Is single block, and has one of the standard nakade shapes */
bool IsNakadeBlock(const GoBoard& bd, 
                   const SgPointSet& block)
{
    if (! GoEyeUtil::IsNakadeShape(block))
        return false;
    return bd.NumStones(block.PointOf()) == block.Size();
}


/** area is all filled by stones, except for one empty point
    These stones are in an alive shape (two eyes for opponent).
*/
bool AlmostFilledByLivingShape(const GoBoard& bd, 
                          const SgPointSet& points,
                          SgBlackWhite stoneColor)
{
    // area must be surrounded by opponent.
    SG_ASSERT(points.Border(bd.Size()).SubsetOf(bd.All(SgOppBW(stoneColor))));

    return   (points & bd.AllEmpty()).IsSize(1)
          && IsAliveBlock(points & bd.All(stoneColor));
}

/** Area contains stones in an alive shape (two eyes for opponent).
    It cannot be extended into a nakade shape.
*/
bool ContainsLivingShape(const GoBoard& bd, 
                          const SgPointSet& points,
                          SgBlackWhite stoneColor)
{
    // area must be surrounded by opponent.
    SG_ASSERT(points.Border(bd.Size()).SubsetOf(bd.All(SgOppBW(stoneColor))));

    return IsAlwaysAliveBlock(points & bd.All(stoneColor));
}

/** area is all filled by stones, except for one empty point.
    These stones are in a nakade shape (only one eye).
 */
bool AlmostFilledByNakade(const GoBoard& bd, 
                          const SgPointSet& points,
                          SgBlackWhite stoneColor)
{
    // area must be surrounded by opponent.
    SG_ASSERT(points.Border(bd.Size()).SubsetOf(bd.All(SgOppBW(stoneColor))));

    if ((points & bd.AllEmpty()).IsSize(1))
    {
        return IsNakadeBlock(bd, points & bd.All(stoneColor));
    }
    return false;
}

/** Test for the case of a 3 stone block in a bulky 5 shape, which is not
    handled well by the standard heuristics.
    It is almost always nakade, except when one empty point is not
    a liberty of the block.
*/
GoEyeStatus BulkyFiveNakade(const GoBoard& bd, 
                            const SgPointSet& points,
                            SgBlackWhite stoneColor)
{
    const SgPointSet stones = points & bd.All(stoneColor);
    if (   IsBulkyFive(points) 
        && stones.IsSize(3)
       )
    {
        const SgPoint p = stones.PointOf();
        if (bd.NumStones(p) == 3) // single block
        // check if both empty points adjacent to block. if yes, nakade.
        // if no, alive.
        {
            SG_ASSERT((points & bd.AllEmpty()).IsSize(2));
            for (SgSetIterator it(points & bd.AllEmpty()); it; ++it)
            {
                if (bd.NumNeighbors(*it, stoneColor) == 0)
                {
                    return EYE_ONE_AND_HALF;
                }
            }
            return EYE_ONE;
        }
        else // 2 blocks (2 eyes) or 3 blocks (unsettled)
        {
            return GoEyeUtil::DegreeCode(stones) == 3 ? 
                 EYE_ONE_AND_HALF : // 3x degree 0 = 3 single stones
                 EYE_TWO; // 2 separate blocks in bulky five are alive shapes
        }

    }
    return EYE_UNKNOWN;
}

/** Exceptional 2x3 areas that are not handled correctly by heuristics */
GoEyeStatus Special2x3Cases(const GoBoard& bd, 
                            const SgPointSet& points,
                            SgBlackWhite stoneColor)
{ 
    const SgPointSet stones = points & bd.All(stoneColor);
    const int nuStones = stones.Size();
    if (nuStones > 0)
    {
        const int code = GoEyeUtil::DegreeCode(stones);
        const int code8 = GoEyeUtil::DegreeCode8(stones);
        // oo.
        // .oo
        if (code == 220 && code8 == 2200)
            return EYE_ONE_AND_HALF;
        // oo.
        // ..o
        if (code == 21 && code8 == 120)
            return EYE_TWO;
        if (NuEdgePoints(bd, points) == 3)
        {
            const SgPoint p = stones.PointOf();
            switch (nuStones)
            {
            case 1: // o.. Single stone on edge can kill by diagonal move
                     // ...
                if (bd.Line(p) == 1
                    && bd.HasNeighbors(p, SgOppBW(stoneColor)))
                    return EYE_ONE_AND_HALF;
                break;
            case 2: // o.o 1.5 eyes. B can make sente seki, W can kill
                // ...
                if (   bd.Line(p) == 1
                       && code == 2
                       && NuEdgePoints(bd, stones) == 2
                    )
                    return EYE_ONE_AND_HALF;
                // o.. followup from case 1: only 1 eye
                // .o.
                if (   code == 2
                    && code8 == 20
                       )
                {
                    const SgPoint p1 = (stones & bd.LineSet(1)).PointOf();
                    if (bd.HasNeighbors(p1, SgOppBW(stoneColor)))
                        return EYE_ONE;
                }
                break;
            case 3 : // o.o 1 eye
                     // .o.
                if (   code == 3
                    && code8 == 120
                    )
                {
                    const SgPoint p1 = (stones & bd.LineSet(1)).PointOf();
                    if (bd.HasNeighbors(p1, SgOppBW(stoneColor)))
                        return EYE_ONE;
                }
                break;
            }
        }
    }
    return EYE_UNKNOWN; // no special case. Default rules work, just use them.
}

/** is p+ns, p+we locally split? */
inline bool TestDiagonal(const SgPointSet& set, SgPoint p, int ns, int we)
{ 
    return     ! set[p+ns+we] // connected the short way
            && ! (   set[p+ns-we]
                  && set[p-we]
                  && set[p-ns-we] 
                  && set[p-ns]
                  && set[p-ns+we]
                 ); // connected the long way
}

/** is p+ns, p-ns locally split? */
inline bool TestOpposite(const SgPointSet& set, SgPoint p, int ns, int we)
{
    return    ! (set[p-ns-we] && set[p-we] && set[p+ns-we])  // connected west
           && ! (set[p-ns+we] && set[p+we] && set[p+ns+we]); // connected east
}


/** Check for bent four in the corner. brute force check of all eight cases. 
    @todo rewrite using the pattern symmetry functions
*/
bool IsBentFour(const SgPointSet& points, int boardSize, SgPoint* vital)
{
    SG_ASSERT(points.IsSize(4));

    if (    points.Contains(SgPointUtil::Pt(1, 1))
         && points.Contains(SgPointUtil::Pt(2, 1))
         && points.Contains(SgPointUtil::Pt(1, 2))
       )
    {
        if (points.Contains(SgPointUtil::Pt(3, 1)))
        {
            *vital = SgPointUtil::Pt(2, 1);
            return true;
        }
        else if (points.Contains(SgPointUtil::Pt(1, 3)))
        {
            *vital = SgPointUtil::Pt(1, 2);
            return true;
        }
    }
    if (    points.Contains(SgPointUtil::Pt(1, boardSize))
         && points.Contains(SgPointUtil::Pt(2, boardSize))
         && points.Contains(SgPointUtil::Pt(1, boardSize - 1))
       )
    {
        if (points.Contains(SgPointUtil::Pt(3, boardSize)))
        {
            *vital = SgPointUtil::Pt(2, boardSize);
            return true;
        }
        else if (points.Contains(SgPointUtil::Pt(1, boardSize - 2)))
        {
            *vital = SgPointUtil::Pt(1, boardSize - 1);
            return true;
        }
    }
    if (    points.Contains(SgPointUtil::Pt(boardSize, 1))
         && points.Contains(SgPointUtil::Pt(boardSize - 1, 1))
         && points.Contains(SgPointUtil::Pt(boardSize, 2))
       )
    {
        if (points.Contains(SgPointUtil::Pt(boardSize - 2, 1)))
        {
            *vital = SgPointUtil::Pt(boardSize - 1, 1);
            return true;
        }
        else if (points.Contains(SgPointUtil::Pt(boardSize, 3)))
        {
            *vital = SgPointUtil::Pt(boardSize, 2);
            return true;
        }
    }
    if (    points.Contains(SgPointUtil::Pt(boardSize, boardSize))
         && points.Contains(SgPointUtil::Pt(boardSize - 1, boardSize))
         && points.Contains(SgPointUtil::Pt(boardSize, boardSize - 1))
       )
    {
        if (points.Contains(SgPointUtil::Pt(boardSize - 2, boardSize)))
        {
            *vital = SgPointUtil::Pt(boardSize - 1, boardSize);
            return true;
        }
        else if (points.Contains(SgPointUtil::Pt(boardSize, boardSize - 2)))
        {
            *vital = SgPointUtil::Pt(boardSize, boardSize - 1);
            return true;
        }
    }
    
    return false;   
}

/** The pattern with 2 diagonal stones is the only one that is unsettled.
    All others are nakade - only 1 eye
*/
bool TwoDiagonalStonesInBulkyFour(const GoBoard& bd, 
                                  const SgPointSet& points,
                                  SgBlackWhite stoneColor)
{
    // bulky four area must be surrounded by opponent.
    SG_ASSERT(points.Border(bd.Size()).SubsetOf(bd.All(SgOppBW(stoneColor))));
    
    if ((points & bd.All(stoneColor)).IsSize(2))
    {
        //2 stones, 2 empty
        SG_ASSERT((points & bd.AllEmpty()).IsSize(2));
        
        const SgPoint p = points.PointOf();
        if (bd.IsEmpty(p))
            return bd.NumNeighbors(p, stoneColor) == 2;
        else
            return bd.NumEmptyNeighbors(p) == 2;
    }
    return false;
}

inline bool ProcessStatus(GoEyeStatus status,
                          bool& isNakade,
                          bool& makeNakade)
{
    if (status == EYE_ONE)
        isNakade = true;
    else if (status == EYE_ONE_AND_HALF)
        makeNakade = true;
    return status != EYE_UNKNOWN;
}
} // namespace
//----------------------------------------------------------------------------

int GoEyeUtil::DegreeCode(const SgPointSet& points)
{
    int degrees[5] = {0,0,0,0,0};
    
    for (SgSetIterator it(points); it; ++it)
    {
        const SgPoint p(*it);
        int nbs = 0;
        for (SgNb4Iterator it(p); it; ++it)
        {
            if (points.Contains(*it))
                ++nbs;
        }
        ++(degrees[nbs]);
    }
    return          degrees[0] 
             + 10 * degrees[1]
            + 100 * degrees[2]
           + 1000 * degrees[3]
          + 10000 * degrees[4];
}

long GoEyeUtil::DegreeCode8(const SgPointSet& points)
{
    int degrees[9] = {0,0,0,0,0};
    
    for (SgSetIterator it(points); it; ++it)
    {
        const SgPoint p(*it);
        int nbs = 0;
        for (SgNb8Iterator it(p); it; ++it)
        {
            if (points.Contains(*it))
                ++nbs;
        }
        ++(degrees[nbs]);
    }
    return              degrees[0] 
                 + 10 * degrees[1]
                + 100 * degrees[2]
               + 1000 * degrees[3]
              + 10000 * degrees[4]
             + 100000 * degrees[5]
            + 1000000 * degrees[6]
           + 10000000 * degrees[7]
          + 100000000 * degrees[8];
}

bool GoEyeUtil::IsNakadeShape(const SgPointSet& area)
{
    switch (area.Size())
    {
        case 1:
        case 2:
        case 3: return true;
        case 4: return IsBulkyFour(area) || IsTShape(area);
        case 5: return IsBulkyFive(area) || IsCross(area);
        case 6: return IsRabbitySix(area);
        default: // too big
            return false;
    }
}

bool GoEyeUtil::IsSinglePointEye(const GoBoard& bd, SgPoint p,
                                 SgBlackWhite color)
{
    SG_ASSERT(bd.IsEmpty(p));
    const SgBlackWhite opp = SgOppBW(color);
    if (bd.HasEmptyNeighbors(p) || bd.HasNeighbors(p, opp))
        return false;
    if (bd.Line(p) == 1)
        return ! (bd.HasDiagonals(p, SG_EMPTY) || bd.HasDiagonals(p, opp));
    return (bd.NumDiagonals(p, SG_EMPTY) + bd.NumDiagonals(p, opp)) <= 1;
}

bool GoEyeUtil::IsPossibleEye(const GoBoard& board, SgBlackWhite color,
                              SgPoint p)
{
    bool isPossibleEye = false;
    SG_ASSERT(board.GetColor(p) != color);
    const SgBlackWhite opp = SgOppBW(color);
    if (board.Line(p) == 1) // corner or edge
    {
        const int nuOwn = (board.Pos(p) == 1) ? 2 : 4;
        if ( board.Num8Neighbors(p, color) == nuOwn
             && board.Num8EmptyNeighbors(p) == 1
           )
        {     
            isPossibleEye = true;
        }
    }
    else // in center
    {
        // have all neighbors, and 2 diagonals, and can get a third
        if (    board.NumNeighbors(p, color) == 4
             && board.NumDiagonals(p, color) == 2
             && board.NumEmptyDiagonals(p) > 0
           )
        {     
            isPossibleEye = true;
        }
        // have 3 of 4 neighbors, can get the 4th, and have enough diagonals
        else if (   board.NumNeighbors(p, color) == 3
                 && board.NumNeighbors(p, opp) == 0
                 && board.NumDiagonals(p, color) >= 3 
                )
        {
            isPossibleEye = true;
        }
    }
    
    return isPossibleEye;
}

bool GoEyeUtil::NumberOfMoveToEye(const GoBoard& board, SgBlackWhite color,
                                  SgPoint p, int& number)
{
    SG_ASSERT(board.IsEmpty(p));
    SgBlackWhite att = SgOppBW(color);  // attacker

    if ( board.Line(p) == 1) // corner or edge
    {
        if ( board.Num8Neighbors(p, att) > 0 )
            return false;
        else
        {
            number = board.Num8EmptyNeighbors(p);
            return true;
        }
    }
    else // in center
    {
        if ( board.Num8Neighbors(p, att) >= 2 )
            return false;
        else if ( board.NumNeighbors(p, att) > 0 )
            return false;
        else // only 0 or 1 attacker point and not in NB4
        {
            number = board.Num8EmptyNeighbors(p);
            return true;
        }
    }
    
}

bool GoEyeUtil::IsSinglePointEye2(const GoBoard& board, SgPoint p, 
                                  SgBlackWhite color, SgVector<SgPoint>& eyes)
{
    // Must be an empty point
    if (! board.IsColor(p, SG_EMPTY))
        return false;
    // All adjacent neighbours must be friendly
    SgBoardColor opp = SgOppBW(color);
    for (SgNb4Iterator adj(p); adj; ++adj)
    {
        SgBoardColor adjColor = board.GetColor(*adj);
        if (adjColor == opp || adjColor == SG_EMPTY)
            return false;
    }
    // All diagonals (with up to one exception) must be friendly or an eye
    int baddiags = 0;
    int maxbaddiags = (board.Line(p) == 1 ? 0 : 1);
    for (SgNb4DiagIterator it(p); it; ++it)
    {
        if (board.IsColor(*it, opp))
            ++baddiags;
        if (board.IsColor(*it, SG_EMPTY) && ! eyes.Contains(*it))
        {
            // Assume this point is an eye and recurse
            eyes.PushBack(p);
            if (! IsSinglePointEye2(board, *it, color, eyes))
                ++baddiags;
            eyes.PopBack();
        }
        if (baddiags > maxbaddiags)
            return false;
    }
    return true;
}

bool GoEyeUtil::IsSinglePointEye2(const GoBoard& board, SgPoint p, 
                                  SgBlackWhite color)
{
    SgVector<SgPoint> emptylist;
    return IsSinglePointEye2(board, p, color, emptylist);
}

bool GoEyeUtil::NumberOfMoveToEye2(const GoBoard& board, SgBlackWhite color,
                                   SgPoint p, int& nummoves)
{
    nummoves = 0;
    bool capturing = false;
    SgVector<SgPoint> usedpoints;
    usedpoints.PushBack(p);
    SgPointSet counted;

    // Can never turn own stone into an eye
    if (board.IsColor(p, color))
        return false;
    
    // If opponent stone then it must be captured to make eye
    if (board.IsColor(p, SgOppBW(color)))
    {
        capturing = true;
    
        // If it is obviously safe then it can never be an eye
        if (SinglePointSafe2(board, p)) // Quick, naive safety test
            return false;

        for (GoBoard::LibertyIterator libit(board, p); libit; ++libit)
            counted.Include(*libit);
    }

    // Count immediate adjacencies
    for (SgNb4Iterator nb(p); nb; ++nb)
    {
        SgPoint adj = *nb;
        
        // Empty points must be filled
        if (board.IsColor(adj, SG_EMPTY))
        {
            counted.Include(adj);
        }
        
        // If adjacent opponent then can never be an eye
        else if (board.IsColor(adj, SgOppBW(color)))
        {
            if (capturing)
                counted.Include(adj); // must capture and then fill
            else
                return false;
        }
    }

    // Up to one diagonal can be ignored: estimate most costly
    SgPoint toignore = SG_NULLPOINT;
    int maxcost = 0;
    int infcost = 1000;
    if (board.Line(p) > 1)
    {
        for (SgNb4DiagIterator nbd(p); nbd; ++nbd)
        {
            SgPoint diag = *nbd;
            int cost = 0;

            if (  board.IsColor(diag, SG_EMPTY)
               && ! IsSinglePointEye2(board, diag, color, usedpoints))
            {
                cost = 1;
            }
            
            else if (board.IsColor(diag, SgOppBW(color)))
            {
                // quick safety test
                if (SinglePointSafe2(board, diag))
                    cost = infcost;
                else
                    cost = board.NumLiberties(diag);
            }

            if (cost > maxcost)
            {
                maxcost = cost;
                toignore = diag;
            }
        }
    }

    // Now mark points that must be played to secure diagonals
    for (SgNb4DiagIterator nbd(p); nbd; ++nbd)
    {
        SgPoint diag = *nbd;
        if (diag == toignore)
            continue;
        
        // Empty points must be filled (unless they are eyes)
        if (  board.IsColor(diag, SG_EMPTY)
           && ! IsSinglePointEye2(board, diag, color, usedpoints))
        {
            counted.Include(diag);
        }
        
        // Opponent stones on diagonals must be captured and filled
        else if (board.IsColor(diag, SgOppBW(color)))
        {
            if (SinglePointSafe2(board, diag))
                return false;
            else
            {
                counted.Include(diag);
                for (GoBoard::LibertyIterator libit(board, diag); libit;
                     ++libit)
                    counted.Include(*libit);
            }
        }
    }

    nummoves = counted.Size();
    return true;
}

int GoEyeUtil::CountSinglePointEyes2(const GoBoard& board, SgPoint p)
{
    if (! board.Occupied(p))
        return 0;

    SgBlackWhite color = board.GetColor(p);
    int numeyes = 0;

    for (GoBoard::LibertyIterator lib(board, p); lib; ++lib)
    {
        if (IsSinglePointEye2(board, *lib, color))
            numeyes++;
    }
    
    return numeyes;
}

bool GoEyeUtil::SinglePointSafe2(const GoBoard& board, SgPoint p)
{
    int numeyes = CountSinglePointEyes2(board, p);
    return numeyes >= 2;
}

bool GoEyeUtil::IsLocalSplitPt(SgPoint p, const SgPointSet& set)
{
    int nuNb = 0;
    for (SgNb4Iterator it(p); it; ++it)
    {
        if (set.Contains(*it))
        {
            ++nuNb;
            if (nuNb >= 2)
                break;
        }
    }
    if (nuNb >= 2)
    {
        if (set[p - SG_NS])
        {
            if (set[p - SG_WE] && TestDiagonal(set, p, -SG_NS, -SG_WE))
                return true;
            if (set[p + SG_NS] && TestOpposite(set, p, SG_NS, SG_WE))
                return true;
            if (set[p + SG_WE] && TestDiagonal(set, p, -SG_NS, +SG_WE))
                return true;
        }
        if (set[p + SG_NS])
        {
            if (set[p - SG_WE] && TestDiagonal(set, p, +SG_NS, -SG_WE))
                return true;
            if (set[p + SG_WE] && TestDiagonal(set, p, +SG_NS, +SG_WE))
                return true;
        }
        if (set[p - SG_WE] && set[p + SG_WE]
            && TestOpposite(set, p, SG_WE, SG_NS))
            return true;
    }
    return false; // no local split found.
}

bool GoEyeUtil::IsSplitPt(SgPoint p, const SgPointSet& points)
{
    SG_ASSERT(points[p]);
    if (! IsLocalSplitPt(p, points))
        return false;
    SgPointSet s(points);
    s.Exclude(p);
    return ! s.IsConnected();
}

bool GoEyeUtil::CanBecomeSinglePointEye (const GoBoard& board, SgPoint p, 
                              const SgPointSet& oppSafe)
{
    SG_ASSERT(! oppSafe[p]);

    for (SgNb4Iterator it(p); it; ++it)
    {
        if (oppSafe[*it])
            return false;
    }

    int nu = 0;
    for (SgNb4DiagIterator dit(p); dit; ++dit)
    {
        if (oppSafe[*dit])
        {
            if (board.Line(p) == 1)
                return false;
            ++nu;
            if (nu > 1)
                return false;
        }
    }
    
    return true;
}


void GoEyeUtil::TestNakade(const SgPointSet& points,
                           const GoBoard& bd,
                           SgBlackWhite color,
                           bool isFullyEnclosed,
                           bool& isNakade,
                           bool& makeNakade,
                           bool& makeFalse,
                           bool& maybeSeki,
                           bool& sureSeki,
                           SgPoint* vital)
{   // handles case
    // of more than 1 point passing vital point test.
    // passes back vital point if exactly one is found.
    // @todo handle case where vital is illegal or suicide (eye within eye?).
    // also test bigger, would-be-alive shapes in atari.
    // @todo handle seki.
    
    SG_UNUSED(makeFalse);
    isNakade = makeNakade = maybeSeki = sureSeki = false;
    SgPoint vitalP(SG_NULLPOINT);
    const SgBlackWhite opp = SgOppBW(color);
    const int nuPoints = points.Size();
    
    SG_ASSERT(nuPoints >= 3); // don't call for smaller areas, no need,
    // and results in isNakade = false which is confusing.
    
    if (nuPoints == 4) // special case 4 point areas
    {
        if (IsBulkyFour(points))
        {
            if (   isFullyEnclosed
                && TwoDiagonalStonesInBulkyFour(bd, points, opp)
               )
                makeNakade = true;
            else
                isNakade = true;
            /* */ return; /* */
        }
        else if (   isFullyEnclosed
                 && points.SubsetOf(bd.AllEmpty())
                 && IsBentFour(points, bd.Size(), vital)
                )
        {
            makeNakade = true;
            /* */ return; /* */
        }
    }
    else if (isFullyEnclosed && nuPoints == 5) // special case 5 point areas
    {
        const GoEyeStatus status = BulkyFiveNakade(bd, points, opp);
        if (ProcessStatus(status, isNakade, makeNakade))
            /* */ return; /* */
    }
    else if (isFullyEnclosed && nuPoints == 6) // special case 6 point areas
    {
        if (Is2x3Area (points))
        {
            GoEyeStatus status = Special2x3Cases(bd, points, opp);
            if (ProcessStatus(status, isNakade, makeNakade))
                /* */ return; /* */
        }   
    }
    if (   isFullyEnclosed
        && AlmostFilledByNakade(bd, points, opp)
       )
    {
        isNakade = true;
        /* */ return; /* */
    }
        
    if (   isFullyEnclosed
        && (   AlmostFilledByLivingShape(bd, points, opp)
            || ContainsLivingShape(bd, points, opp)
           )
       )
    {   // not nakade, 2 eyes
        /* */ return; /* */
    }
        
    int nuMakeNakade = 0;
    int nuVitalOccupied = 0;
    bool hasDivider = false;
    // counting number of nakade fixes bug with stretched 5 pt eye 
    // that had 3 vital pts,
    // one of them occupied. was classified as 'isNakade7 = 1 eye.
    // see sgf/ld200,#20
    for (SgSetIterator it(points); it; ++it)
    {
        SgPoint p(*it);
        if (IsVitalPt(points, p, opp, bd))
        {
            if (bd.IsEmpty(p))
            {
                if (bd.IsLegal(p, opp))
                {
                    ++nuMakeNakade;
                    vitalP = p;
                }
                else
                    hasDivider = true;
            }
            else
                ++nuVitalOccupied;
        }
    }
    
    if (hasDivider)
    { // alive 
    }
    else if (nuMakeNakade == 1) // exactly one way to make nakade here.
    {
        makeNakade = true;
        *vital = vitalP;
    }
    else if (nuMakeNakade > 0) 
        isNakade = false;
    else if (nuVitalOccupied < 3)
        isNakade = true;
    else
    {
        maybeSeki = true;
        // @todo if (IsSureSekiShape(...)) sureSeki = true;
    }
}

bool GoEyeUtil::IsVitalPt(const SgPointSet& points, SgPoint p,
                SgBlackWhite opp,
                const GoBoard& bd)
{
    // in corridors a vital point has 2 nbs, in big ones it may have 3 or 4.
    // but: 2 in following
    // example: unsettled nakade, non-flat points are all occupied by opp.
    // .a       a is vital Point.
    //  o.
    //  .
    int numNb = bd.NumEmptyNeighbors(p) + bd.NumNeighbors(p, opp);
    if (numNb >= 2)
    {
        if (numNb >= 4)
            /* */ return true; /* */
        int nu = IsTreeShape(points) ? 2 : 3;
        if (numNb >= nu)
        {
            if (numNb == 2 && bd.IsEmpty(p))
                return IsSplitPt(p, points);
            else
                return true;
        }
    }
    return false;
}

bool GoEyeUtil::CheckInterior(const GoBoard& bd, const SgPointSet& area,
                   SgBlackWhite opp, bool checkBlocks)
{
    bool hasSingleNbPoint = false;
    int nuPoints = 0;
    for (SgSetIterator it(area); it; ++it)
    {
        const SgPoint p(*it);
        if (bd.IsEmpty(p))
        {
            int nuNbs = 0;
            if (area.Contains(p + SG_NS))
                ++nuNbs;
            if (area.Contains(p - SG_NS))
                ++nuNbs;
            if (area.Contains(p + SG_WE))
                ++nuNbs;
            if (area.Contains(p - SG_WE))
                ++nuNbs;
            if (nuNbs == 1)
                hasSingleNbPoint = true;
            else if (nuNbs > 2)
                return false;
        }
        else if (p == bd.Anchor(p))
        {
            if (bd.GetColor(p) != opp)
            // if own stones on inside: not a tree shape.
                return false;
            int nuLibs = bd.NumLiberties(p);
            if (nuLibs == 1)
                hasSingleNbPoint = true;
            else if (checkBlocks && nuLibs > 2)
                return false;
        }
        ++nuPoints;
    }
    return nuPoints == 1 || hasSingleNbPoint;
}

bool GoEyeUtil::IsTreeShape(const SgPointSet& area)
{
    for (SgSetIterator it(area); it; ++it)
    {
        const SgPoint p(*it);
        if (   area.Contains(p + SG_NS)
            && area.Contains(p + SG_WE)
            && area.Contains(p + SG_NS + SG_WE)
           )
           return false;
    }
    return true;
}
