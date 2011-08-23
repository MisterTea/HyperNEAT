//----------------------------------------------------------------------------
/** @file GoBoardUtil.cpp
    See GoBoardUtil.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoBoardUtil.h"

#include <iomanip>
#include <sstream>
#include <string>
#include "GoBoard.h"
#include "GoModBoard.h"
#include "GoMoveExecutor.h"
#include "GoSafetySolver.h"
#include "SgDebug.h"
#include "SgException.h"
#include "SgNbIterator.h"
#include "SgProp.h"

using namespace std;
using SgPropUtil::PointToSgfString;

//----------------------------------------------------------------------------

namespace {

/** Function used in GoBoardUtil::CfgDistance() */
void CfgDistanceCheck(const GoBoard& bd, SgPointArray<int>& array,
                      GoPointList& pointList, int d, SgPoint p)
{
    if (! bd.IsBorder(p))
    {
        if (bd.Occupied(p))
            p = bd.Anchor(p);
        if (array[p] == numeric_limits<int>::max())
        {
            array[p] = d;
            pointList.PushBack(p);
        }
    }
}

/** Function used in GoBoardUtil::ScorePosition() */
void ScorePositionRecurse(const GoBoard& bd, SgPoint p,
                          const SgPointSet& deadStones, SgMarker& marker,
                          bool& isBlackAdjacent, bool& isWhiteAdjacent,
                          int& nuPoints, int& nuDeadWhite, int& nuDeadBlack)
{
    if (bd.IsBorder(p))
        return;
    SgEmptyBlackWhite c = bd.GetColor(p);
    if (c != SG_EMPTY && ! deadStones.Contains(p))
    {
        if (c == SG_BLACK)
            ++isBlackAdjacent;
        else
            ++isWhiteAdjacent;
        return;
    }
    if (! marker.NewMark(p))
        return;
    ++nuPoints;
    if (c == SG_BLACK)
        ++nuDeadBlack;
    if (c == SG_WHITE)
        ++nuDeadWhite;
    ScorePositionRecurse(bd, p + SG_NS, deadStones, marker, isBlackAdjacent,
                         isWhiteAdjacent, nuPoints, nuDeadWhite, nuDeadBlack);
    ScorePositionRecurse(bd, p - SG_NS, deadStones, marker, isBlackAdjacent,
                         isWhiteAdjacent, nuPoints, nuDeadWhite, nuDeadBlack);
    ScorePositionRecurse(bd, p + SG_WE, deadStones, marker, isBlackAdjacent,
                         isWhiteAdjacent, nuPoints, nuDeadWhite, nuDeadBlack);
    ScorePositionRecurse(bd, p - SG_WE, deadStones, marker, isBlackAdjacent,
                         isWhiteAdjacent, nuPoints, nuDeadWhite, nuDeadBlack);
}

} // namespace

//----------------------------------------------------------------------------

void GoBoardUtil::AddNeighborBlocksOfColor(const GoBoard& bd, SgPoint p,
                                           SgBlackWhite c,
                                           SgVector<SgPoint>& neighbors)
{
    if (bd.IsColor(p - SG_NS, c))
        neighbors.Include(bd.Anchor(p - SG_NS));
    if (bd.IsColor(p - SG_WE, c))
        neighbors.Include(bd.Anchor(p - SG_WE));
    if (bd.IsColor(p + SG_WE, c))
        neighbors.Include(bd.Anchor(p + SG_WE));
    if (bd.IsColor(p + SG_NS, c))
        neighbors.Include(bd.Anchor(p + SG_NS));
}

void GoBoardUtil::AddWall(GoBoard& bd,
                          SgBlackWhite color,
                          SgPoint start,
                          int length,
                          int direction)
{
    for (SgPoint p = start; length > 0; --length)
    {
        bd.Play(p, color);
        p += direction;
    }
}

GoPointList GoBoardUtil::AdjacentStones(const GoBoard& bd, SgPoint point)
{
    SG_ASSERT(bd.IsValidPoint(point));
    SG_ASSERT(bd.Occupied(point));
    const SgBlackWhite other = SgOppBW(bd.GetStone(point));
    GoPointList result;
    SgMarker& mark = bd.m_userMarker;
    SgReserveMarker reserve(mark);
    SG_UNUSED(reserve);
    mark.Clear();
    for (GoBoard::StoneIterator it(bd, point); it; ++it)
    {
        if (bd.NumNeighbors(*it, other) > 0)
        {
            SgPoint p = *it;
            if (bd.IsColor(p - SG_NS, other) && mark.NewMark(p - SG_NS))
                result.PushBack(p - SG_NS);
            if (bd.IsColor(p - SG_WE, other) && mark.NewMark(p - SG_WE))
                result.PushBack(p - SG_WE);
            if (bd.IsColor(p + SG_WE, other) && mark.NewMark(p + SG_WE))
                result.PushBack(p + SG_WE);
            if (bd.IsColor(p + SG_NS, other) && mark.NewMark(p + SG_NS))
                result.PushBack(p + SG_NS);
        }
    };
    return result;
}

void GoBoardUtil::AdjacentBlocks(const GoBoard& bd, SgPoint p, int maxLib,
                                 SgVector<SgPoint>* blocks)
{
    SG_ASSERT(blocks);
    SgPoint a[SG_MAXPOINT];
    int n = bd.AdjacentBlocks(p, maxLib, a, SG_MAXPOINT);
    blocks->SetTo(a, n);
}

void GoBoardUtil::BlocksAdjacentToPoints(const GoBoard& bd,
                                         const SgVector<SgPoint>& points,
                                         SgBlackWhite c,
                                         SgVector<SgPoint>* blocks)
{
    // Mark all points to avoid n^2 algorithm.
    SgMarker& mark = bd.m_userMarker;
    SgReserveMarker reserve(mark);
    SG_UNUSED(reserve);
    mark.Clear();
    for (SgVectorIterator<SgPoint> it1(points); it1; ++it1)
        mark.Include(*it1);
    // Add the anchor of each adjacent block to the list of blocks.
    SgPoint a[SG_MAXPOINT];
    int n = 0;
    for (SgVectorIterator<SgPoint> it2(points); it2; ++it2)
    {
        SgPoint p = *it2;
        if (bd.NumNeighbors(p, c) > 0)
        {
            if (bd.IsColor(p - SG_NS, c)
                && mark.NewMark(bd.Anchor(p - SG_NS)))
                a[n++] = bd.Anchor(p - SG_NS);
            if (bd.IsColor(p - SG_WE, c)
                && mark.NewMark(bd.Anchor(p - SG_WE)))
                a[n++] = bd.Anchor(p - SG_WE);
            if (bd.IsColor(p + SG_WE, c)
                && mark.NewMark(bd.Anchor(p + SG_WE)))
                a[n++] = bd.Anchor(p + SG_WE);
            if (bd.IsColor(p + SG_NS, c)
                && mark.NewMark(bd.Anchor(p + SG_NS)))
                a[n++] = bd.Anchor(p + SG_NS);
        }
    }
    blocks->SetTo(a, n);
}

void GoBoardUtil::BlocksAdjacentToPoints(const GoBoard& bd,
                                         const SgPointSet& points,
                                         SgBlackWhite c,
                                         SgVector<SgPoint>* blocks)
{
    // exact copy from list version above
    SG_ASSERT(blocks);
    // Mark all points to avoid n^2 algorithm.
    SgMarker& mark = bd.m_userMarker;
    SgReserveMarker reserve(mark);
    SG_UNUSED(reserve);
    mark.Clear();
    for (SgSetIterator it1(points); it1; ++it1)
        mark.Include(*it1);
    // Add the anchor of each adjacent block to the list of blocks.
    SgPoint a[SG_MAXPOINT];
    int n = 0;
    for (SgSetIterator it2(points); it2; ++it2)
    {
        SgPoint p = *it2;
        if (bd.NumNeighbors(p, c) > 0)
        {
            if (bd.IsColor(p - SG_NS, c)
                && mark.NewMark(bd.Anchor(p - SG_NS)))
                a[n++] = bd.Anchor(p - SG_NS);
            if (bd.IsColor(p - SG_WE, c)
                && mark.NewMark(bd.Anchor(p - SG_WE)))
                a[n++] = bd.Anchor(p - SG_WE);
            if (bd.IsColor(p + SG_WE, c)
                && mark.NewMark(bd.Anchor(p + SG_WE)))
                a[n++] = bd.Anchor(p + SG_WE);
            if (bd.IsColor(p + SG_NS, c)
                && mark.NewMark(bd.Anchor(p + SG_NS)))
                a[n++] = bd.Anchor(p + SG_NS);
        }
    }
    blocks->SetTo(a, n);
}

bool GoBoardUtil::BlockIsAdjacentTo(const GoBoard& bd, SgPoint block,
                                    const SgPointSet& walls)
{
    for (GoBoard::StoneIterator it(bd, block); it; ++it)
    {
        if (  walls.Contains((*it) + SG_NS)
           || walls.Contains((*it) - SG_NS)
           || walls.Contains((*it) + SG_WE)
           || walls.Contains((*it) - SG_WE)
           )
            return true;
    }
    return false;
}

SgPointArray<int> GoBoardUtil::CfgDistance(const GoBoard& bd, SgPoint p,
                                           int maxDist)
{
    SgPointArray<int> array(numeric_limits<int>::max());
    GoPointList pointList;
    if (bd.Occupied(p))
        p = bd.Anchor(p);
    pointList.PushBack(p);
    int begin = 0;
    int end = 1;
    int d = 0;
    array[p] = d;
    while (begin != end && d < maxDist)
    {
        ++d;
        for (int i = begin; i != end; ++i)
        {
            p = pointList[i];
            if (bd.Occupied(p))
            {
                for (GoBoard::StoneIterator it(bd, p); it; ++it)
                {
                    CfgDistanceCheck(bd, array, pointList, d, *it + SG_NS);
                    CfgDistanceCheck(bd, array, pointList, d, *it - SG_NS);
                    CfgDistanceCheck(bd, array, pointList, d, *it + SG_WE);
                    CfgDistanceCheck(bd, array, pointList, d, *it - SG_WE);
                }
            }
            else
            {
                CfgDistanceCheck(bd, array, pointList, d, p + SG_NS);
                CfgDistanceCheck(bd, array, pointList, d, p - SG_NS);
                CfgDistanceCheck(bd, array, pointList, d, p + SG_WE);
                CfgDistanceCheck(bd, array, pointList, d, p - SG_WE);
            }
        }
        begin = end;
        end = pointList.Length();
    }
    return array;
}

void GoBoardUtil::DumpBoard(const GoBoard& bd, std::ostream& out)
{
    const int size = bd.Size();
    out << bd;
    if (bd.MoveNumber() == 0)
        return;
    out << "(;SZ[" << size << "]\n";
    const GoSetup& setup = bd.Setup();
    if (! setup.IsEmpty())
    {
        for (SgBWIterator it; it; ++it)
        {
            SgBlackWhite c = *it;
            int stoneNumber = 0;
            out << (c == SG_BLACK ? "AB" : "AW");
            for (SgSetIterator it2(setup.m_stones[c]); it2; ++it2)
            {
                SgPoint p = *it2;
                ++stoneNumber;
                out << '[' << PointToSgfString(p, size, SG_PROPPOINTFMT_GO)
                    << ']';
                if (stoneNumber % 10 == 0)
                    out << '\n';
            }
            out << '\n';
        }
        out << "PL[" << (setup.m_player == SG_BLACK ? 'B' : 'W') << "]\n";
    }
    int moveNumber = 0;
    for (int i = 0; i < bd.MoveNumber(); ++i)
    {
        GoPlayerMove move = bd.Move(i);
        out << ';';
        out << (move.Color() == SG_BLACK ? "B" : "W");
        ++moveNumber;
        out << '[' << PointToSgfString(move.Point(), size, SG_PROPPOINTFMT_GO)
            << ']';
        if (moveNumber % 10 == 0)
            out << '\n';
    }
    out << ")\n";
}

void GoBoardUtil::ExpandToBlocks(const GoBoard& board, SgPointSet& pointSet)
{
    // @todo faster to use GoBoard::StoneIterator in GoBoard?
    SG_ASSERT(pointSet.SubsetOf(board.Occupied()));
    int size = board.Size();
    for (SgBlackWhite color = SG_BLACK; color <= SG_WHITE; ++color)
    {
        SgPointSet set = pointSet & board.All(color);
        bool change(true);
        while (change)
        {
            change = false;
            SgPointSet next = set | (set.Border(size) & board.All(color));
            if (next != set)
            {
                change = true;
                set = next;
            }
        }
        pointSet |= set;
    }
}

void GoBoardUtil::DiagonalsOfColor(const GoBoard& bd, SgPoint p, int c,
                                   SgVector<SgPoint>* diagonals)
{
    diagonals->Clear();
    if (bd.IsColor(p - SG_NS - SG_WE, c))
        diagonals->PushBack(p - SG_NS - SG_WE);
    if (bd.IsColor(p - SG_NS + SG_WE, c))
        diagonals->PushBack(p - SG_NS + SG_WE);
    if (bd.IsColor(p + SG_NS - SG_WE, c))
        diagonals->PushBack(p + SG_NS - SG_WE);
    if (bd.IsColor(p + SG_NS + SG_WE, c))
        diagonals->PushBack(p + SG_NS + SG_WE);
}

bool GoBoardUtil::EndOfGame(const GoBoard& bd)
{
    SgBlackWhite toPlay = bd.ToPlay();
    GoPlayerMove passToPlay(toPlay, SG_PASS);
    GoPlayerMove passOpp(SgOppBW(toPlay), SG_PASS);
    int moveNumber = bd.MoveNumber();
    if (bd.Rules().TwoPassesEndGame())
    {
        return moveNumber >= 2
            && bd.Move(moveNumber - 1) == passOpp
            && bd.Move(moveNumber - 2) == passToPlay;
    }
    else // Three passes in a row end the game.
    {
        return moveNumber >= 3
            && bd.Move(moveNumber - 1) == passOpp
            && bd.Move(moveNumber - 2) == passToPlay
            && bd.Move(moveNumber - 3) == passOpp;
    }
}


bool GoBoardUtil::GenerateIfLegal(const GoBoard& bd, SgPoint move,
                                  SgVector<SgPoint>* moves)
{
    if (bd.IsLegal(move))
    {
        if (moves)
            moves->Include(move);
        /* */ return true; /* */
    }
    return false;
}

void GoBoardUtil::GetCoordString(SgMove p, std::string* s, int boardSize)
{
    SG_ASSERT(s);
    SG_ASSERT(p != SG_NULLMOVE);
    if (p == SG_PASS)
        *s = "Pass";
    else if (p == SG_COUPONMOVE)
        *s = "Coupon";
    else
    {
        int col = SgPointUtil::Col(p);
        int row = SgPointUtil::Row(p);
        if (9 <= col)
            ++col; // skip 'I'
        ostringstream o;
        o << static_cast<char>('A' + col - 1) << (boardSize + 1 - row);
        *s = o.str();
    }
}

bool GoBoardUtil::HasAdjacentBlocks(const GoBoard& bd, SgPoint p,
                                    int maxLib)
{
    SG_ASSERT(bd.Occupied(p));
    const SgBlackWhite other = SgOppBW(bd.GetStone(p));
    for (GoBoard::StoneIterator stone(bd, p); stone; ++stone)
        for (SgNb4Iterator nb(*stone); nb; ++nb)
            if (bd.IsColor(*nb, other) && bd.AtMostNumLibs(*nb, maxLib))
                return true;
    return false;
}

bool GoBoardUtil::IsHandicapPoint(SgGrid size, SgGrid col, SgGrid row)
{
    SgGrid line1;
    SgGrid line3;
    if (size < 9)
        return false;
    if (size <= 11)
    {
        line1 = 3;
        line3 = size - 2;
    }
    else
    {
        line1 = 4;
        line3 = size - 3;
    }
    if (size > 11 && size % 2 != 0) // mark mid points
    {
        SgGrid line2 = size / 2 + 1;
        return (row == line1 || row == line2 || row == line3)
            && (col == line1 || col == line2 || col == line3);
    }
    else
        return (row == line1 || row == line3)
            && (col == line1 || col == line3);
}

bool GoBoardUtil::IsSimpleEyeOfBlock(const GoBoard& bd, SgPoint lib,
                                     SgPoint blockAnchor,
                                     const SgVector<SgPoint>& eyes)
{
    SgBlackWhite color = bd.GetStone(blockAnchor);
    // need IsColor test for nbs because might be off board.
    if (bd.IsColor(lib - SG_NS, color)
        && bd.Anchor(lib - SG_NS) != blockAnchor)
        return false;
    if (bd.IsColor(lib + SG_NS, color)
        && bd.Anchor(lib + SG_NS) != blockAnchor)
        return false;
    if (bd.IsColor(lib - SG_WE, color)
        && bd.Anchor(lib - SG_WE) != blockAnchor)
        return false;
    if (bd.IsColor(lib + SG_WE, color)
        && bd.Anchor(lib + SG_WE) != blockAnchor)
        return false;
    int nuForFalse = (bd.Line(lib) == 1) ? 1 : 2;
    // no need to check diagonals for same block since direct neighbors are.
    for (SgNb4DiagIterator it(lib); it; ++it)
    {
        SgPoint nb(*it);
        if (! bd.IsBorder(nb) && ! bd.IsColor(nb, color)
            && ! eyes.Contains(nb))
            if (--nuForFalse <= 0)
                return false;
    }
    return true;
}

bool GoBoardUtil::IsSnapback(const GoBoard& constBd, SgPoint p)
{
    SG_ASSERT(constBd.IsValidPoint(p));
    SG_ASSERT(constBd.Occupied(p));

    bool snapback = false;
    if (constBd.IsSingleStone(p) && constBd.InAtari(p))
    {
        const SgPoint lib = constBd.TheLiberty(p);
        GoModBoard mbd(constBd);
        GoBoard& bd = mbd.Board();
        const bool isLegal =
            GoBoardUtil::PlayIfLegal(bd, lib, SgOppBW(bd.GetStone(p)));
        if (  isLegal
           && bd.InAtari(lib)
           && ! bd.IsSingleStone(lib)
           )
            snapback = true;
        if (isLegal)
            bd.Undo();
    }
    return snapback;
}

bool GoBoardUtil::ManySecondaryLibs(const GoBoard& bd, SgPoint block)
{
    // was always 8, not enough for loose ladder in CAPTURES.SGB, problem 2
    // one liberty can have 3 new secondary, total of 4 which are taken by
    // opp. move.
    // current value is just a guess, experiment.
    const int LIBERTY_LIMIT = 9;
    static SgMarker m;
    m.Clear();
    int nu = 0;
    for (GoBoard::LibertyIterator it(bd, block); it; ++it)
    {
        SgPoint p(*it);
        if (m.NewMark(p))
            if (++nu >= LIBERTY_LIMIT)
                return true;
        for (SgNb4Iterator itn(p); itn; ++itn)
        {
            if (bd.IsEmpty(*itn) && m.NewMark(*itn))
                if (++nu >= LIBERTY_LIMIT)
                    return true;
        }
    }
    return (nu >= LIBERTY_LIMIT);
}

SgSList<SgPoint,4> GoBoardUtil::NeighborsOfColor(const GoBoard& bd, SgPoint p,
                                                 int c)
{
    SgSList<SgPoint,4> result;
    if (bd.IsColor(p - SG_NS, c))
        result.PushBack(p - SG_NS);
    if (bd.IsColor(p - SG_WE, c))
        result.PushBack(p - SG_WE);
    if (bd.IsColor(p + SG_WE, c))
        result.PushBack(p + SG_WE);
    if (bd.IsColor(p + SG_NS, c))
        result.PushBack(p + SG_NS);
    return result;
}

void GoBoardUtil::NeighborsOfColor(const GoBoard& bd, SgPoint p, int c,
                                   SgVector<SgPoint>* neighbors)
{
    neighbors->Clear();
    if (bd.IsColor(p - SG_NS, c))
        neighbors->PushBack(p - SG_NS);
    if (bd.IsColor(p - SG_WE, c))
        neighbors->PushBack(p - SG_WE);
    if (bd.IsColor(p + SG_WE, c))
        neighbors->PushBack(p + SG_WE);
    if (bd.IsColor(p + SG_NS, c))
        neighbors->PushBack(p + SG_NS);
}

bool GoBoardUtil::PassWins(const GoBoard& bd, SgBlackWhite toPlay)
{
    if (toPlay != bd.ToPlay())
        // Not defined if non-alternating moves
        return false;
    if (! bd.Rules().CaptureDead() || bd.Rules().JapaneseScoring())
        return false;
    if (bd.GetLastMove() != SG_PASS)
        return false;
    float komi = bd.Rules().Komi().ToFloat();
    float score = GoBoardUtil::TrompTaylorScore(bd, komi);
    if ((score > 0 && toPlay == SG_BLACK)
        || (score < 0 && toPlay == SG_WHITE))
        return true;
    return false;
}

bool GoBoardUtil::PlayIfLegal(GoBoard& bd, SgPoint p, SgBlackWhite player)
{
    if (p != SG_PASS && p != SG_COUPONMOVE)
    {
        if (! bd.IsEmpty(p))
            return false;
        if (! bd.Rules().AllowSuicide() && bd.IsSuicide(p, player))
            return false;
    }
    bd.Play(p, player);
    if (bd.LastMoveInfo(GO_MOVEFLAG_ILLEGAL))
    {
        bd.Undo();
        return false;
    }
    return true;
}

void GoBoardUtil::ReduceToAnchors(const GoBoard& bd,
                                  SgVector<SgPoint>* stones)
{
    SG_ASSERT(stones);
    SgVector<SgPoint> result;
    for (SgVectorIterator<SgPoint> stone(*stones); stone; ++stone)
        if (bd.Occupied(*stone))
            result.Insert(bd.Anchor(*stone));
    stones->SwapWith(&result);
}

void GoBoardUtil::ReduceToAnchors(const GoBoard& bd,
                                  const SgVector<SgPoint>& stones,
                                  SgSList<SgPoint,SG_MAXPOINT>& anchors)
{
    anchors.Clear();
    for (SgVectorIterator<SgPoint> it(stones); it; ++it)
        if (bd.Occupied(*it))
            anchors.Include(bd.Anchor(*it));
}

void GoBoardUtil::RegionCode(const GoBoard& bd,
                             const SgVector<SgPoint>& region,
                             SgHashCode* c)
{
    BOOST_STATIC_ASSERT(SG_BLACK < 2);
    BOOST_STATIC_ASSERT(SG_WHITE < 2);

    c->Clear();
    for (SgVectorIterator<SgPoint> it(region); it; ++it)
    {
        SgPoint p = *it;
        if (bd.Occupied(p))
            SgHashUtil::XorZobrist(*c, p + bd.GetStone(p) * SG_MAXPOINT);
    }
}

bool GoBoardUtil::RemainingChineseHandicap(const GoBoard& bd)
{
    const GoRules& rules = bd.Rules();
    return (! rules.JapaneseHandicap()
            && rules.Handicap() > bd.TotalNumStones(SG_BLACK));
}

float GoBoardUtil::ScoreSimpleEndPosition(const GoBoard& bd, float komi,
                                          bool noCheck)
{
    int score = 0;
    for (GoBoard::Iterator it(bd); it; ++it)
        score += ScorePoint(bd, *it, noCheck);
    return (score - komi);
}

void GoBoardUtil::SharedLiberties(const GoBoard& bd,
                                  SgPoint block1,
                                  SgPoint block2,
                                  SgVector<SgPoint>* sharedLibs)
{
    SG_ASSERT(sharedLibs);
    SG_ASSERT(bd.Occupied(block1));
    SG_ASSERT(bd.Occupied(block2));
    block1 = bd.Anchor(block1);
    block2 = bd.Anchor(block2);
    sharedLibs->Clear();
    for (GoBoard::LibertyIterator libIter(bd, block1); libIter; ++libIter)
    {
        SgPoint lib = *libIter;
        if (bd.IsLibertyOfBlock(lib, block2))
            sharedLibs->PushBack(lib);
    }
}

void GoBoardUtil::SharedLibertyBlocks(const GoBoard& bd, SgPoint anchor,
                                      int maxLib, SgVector<SgPoint>* blocks)
{
    SG_ASSERT(blocks);
    // Mark all points and previous blocks.
    SgMarker& mark = bd.m_userMarker;
    SgReserveMarker reserve(mark);
    SG_UNUSED(reserve);
    mark.Clear();
    for (GoBoard::StoneIterator it1(bd, anchor); it1; ++it1)
        mark.Include(*it1);
    for (SgVectorIterator<SgPoint> it(*blocks); it; ++it)
    {
        SgPoint a = *it;
        for (GoBoard::StoneIterator it(bd, a); it; ++it)
            mark.Include(*it);
    }
    SgBlackWhite c = bd.GetStone(anchor);
    // Add the anchor of each adjacent block to the list of blocks.
    for (GoBoard::LibertyIterator it2(bd, anchor); it2; ++it2)
    {
        SgPoint p = *it2;
        if (bd.NumNeighbors(p, c) > 0)
        {
            if (bd.IsColor(p - SG_NS, c) && mark.NewMark(bd.Anchor(p - SG_NS))
                && bd.AtMostNumLibs(p - SG_NS, maxLib))
                blocks->PushBack(bd.Anchor(p - SG_NS));
            if (bd.IsColor(p - SG_WE, c) && mark.NewMark(bd.Anchor(p - SG_WE))
                && bd.AtMostNumLibs(p - SG_WE, maxLib))
                blocks->PushBack(bd.Anchor(p - SG_WE));
            if (bd.IsColor(p + SG_WE, c) && mark.NewMark(bd.Anchor(p + SG_WE))
                && bd.AtMostNumLibs(p + SG_WE, maxLib))
                blocks->PushBack(bd.Anchor(p + SG_WE));
            if (bd.IsColor(p + SG_NS, c) && mark.NewMark(bd.Anchor(p + SG_NS))
                && bd.AtMostNumLibs(p + SG_NS, maxLib))
                blocks->PushBack(bd.Anchor(p + SG_NS));
        }
    }
}

void GoBoardUtil::UndoAll(GoBoard& bd)
{
    while (bd.CanUndo())
        bd.Undo();
}

bool GoBoardUtil::AtLeastTwoSharedLibs(const GoBoard& bd, SgPoint block1,
                                       SgPoint block2)
{
    SG_ASSERT(bd.Occupied(block1));
    SG_ASSERT(bd.Occupied(block2));
    //block1 = bd.Anchor(block1);
    block2 = bd.Anchor(block2);
    bool fHasOneShared = false;
    for (GoBoard::LibertyIterator libIter(bd, block1); libIter; ++libIter)
    {
        if (bd.IsLibertyOfBlock(*libIter, block2))
        {
            if (fHasOneShared)
                return true;
            fHasOneShared = true;
        }
    }
    return false;
}

void GoBoardUtil::TestForChain(GoBoard& bd, SgPoint block, SgPoint block2,
                               SgPoint lib, SgVector<SgPoint>* extended)
{
    if (AtLeastTwoSharedLibs(bd, block, block2))
        extended->PushBack(block);
    else // protected lib.
    {
        GoRestoreToPlay r(bd);
        bd.SetToPlay(SgOppBW(bd.GetStone(block)));
        if (MoveNotLegalOrAtari(bd, lib))
            extended->PushBack(block);
    }
}

bool GoBoardUtil::HasStonesOfBothColors(const GoBoard& bd,
                                        const SgVector<SgPoint>& stones)
{
    SgBWArray<bool> has(false);
    for (SgVectorIterator<SgPoint> it(stones); it; ++it)
    {
        if (bd.Occupied(*it))
        {
            SgBlackWhite color(bd.GetStone(*it));
            has[color] = true;
            if (has[SgOppBW(color)])
                return true;
        }
    }
    return false;
}

bool GoBoardUtil::MoveNotLegalOrAtari(GoBoard& bd, SgPoint move)
{
    GoMoveExecutor execute(bd, move);
    return (! execute.IsLegal() || bd.InAtari(move));
}

bool GoBoardUtil::MoveLegalAndNotAtari(GoBoard& bd, SgPoint move)
{
    GoMoveExecutor execute(bd, move);
    return (execute.IsLegal() && ! bd.InAtari(move));
}

bool GoBoardUtil::ScorePosition(const GoBoard& bd,
                                const SgPointSet& deadStones, float& score)
{
    SgMarker marker;
    int stones = 0;
    int territory = 0;
    int dead = 0;
    for (GoBoard::Iterator it(bd); it; ++it)
    {
        SgPoint p = *it;
        if (bd.Occupied(p) && ! deadStones.Contains(p))
        {
            if (bd.GetColor(p) == SG_BLACK)
                ++stones;
            else
                --stones;
            continue;
        }
        if (marker.Contains(p))
            continue;
        int nuPoints = 0;
        int nuDeadWhite = 0;
        int nuDeadBlack = 0;
        bool isBlackAdjacent = false;
        bool isWhiteAdjacent = false;
        ScorePositionRecurse(bd, p, deadStones, marker, isBlackAdjacent,
                             isWhiteAdjacent, nuPoints, nuDeadWhite,
                             nuDeadBlack);
        if ((nuDeadWhite > 0 && nuDeadBlack > 0)
            || (isBlackAdjacent && nuDeadBlack > 0)
            || (isWhiteAdjacent && nuDeadWhite > 0))
            return false;
        dead += nuDeadBlack;
        dead -= nuDeadWhite;
        if (isBlackAdjacent && ! isWhiteAdjacent)
            territory += nuPoints;
        else if (isWhiteAdjacent && ! isBlackAdjacent)
            territory -= nuPoints;
    }
    int prisoners = bd.NumPrisoners(SG_BLACK) - bd.NumPrisoners(SG_WHITE);
    if (bd.Rules().JapaneseScoring())
        score = territory - dead - prisoners - bd.Rules().Komi().ToFloat();
    else
        score = territory + stones - bd.Rules().Komi().ToFloat();
    return true;
}

int GoBoardUtil::Stones(const GoBoard& bd, SgPoint p, SgPoint stones[])
{
    SG_ASSERT(bd.IsValidPoint(p));
    SG_ASSERT(bd.Occupied(p));
    if (bd.IsSingleStone(p))
    {
        stones[0] = p;
        return 1;
    }
    else
    {
        int nm = 0;
        for (GoBoard::StoneIterator it(bd, bd.Anchor(p)); it; ++it)
            stones[nm++] = p;
        return nm;
    }
}

bool GoBoardUtil::TwoPasses(const GoBoard& bd)
{
    SgBlackWhite toPlay = bd.ToPlay();
    GoPlayerMove passToPlay(toPlay, SG_PASS);
    GoPlayerMove passOpp(SgOppBW(toPlay), SG_PASS);
    int moveNumber = bd.MoveNumber();
    return (  moveNumber >= 2
           && bd.Move(moveNumber - 1) == passOpp
           && bd.Move(moveNumber - 2) == passToPlay
           );
}

SgPointSet GoBoardUtil::Lines(const GoBoard& bd, SgGrid from, SgGrid to)
{
    SG_ASSERT(from >= 1);
    SG_ASSERT(from <= to);
    SG_ASSERT(to <= (bd.Size() + 1) / 2);
    SgPointSet lines;
    for (SgGrid i = from; i <= to; ++i)
        lines |= bd.LineSet(i);
    return lines;
}

SgRect GoBoardUtil::GetDirtyRegion(const GoBoard& bd, SgMove move,
                                   SgBlackWhite colour, bool checklibs,
                                   bool premove)
{
    SgRect dirty;
    if (move == SG_PASS)
        return dirty;

    SgBlackWhite opp = SgOppBW(colour);

    // Point played has changed
    dirty.Include(move);

    SgPointSet blocks;

    // This move adjusts libs for all adjacent blocks
    if (checklibs)
    {
        for (SgNb4Iterator inb(move); inb; ++inb)
            if (bd.Occupied(*inb))
                for (GoBoard::StoneIterator istone(bd, *inb); istone;
                     ++istone)
                    dirty.Include(*istone);
    }

    // Check if this move will make a capture
    if (premove)
    {
        for (SgNb4Iterator inb(move); inb; ++inb)
        {
            if (bd.IsColor(*inb, opp) && bd.NumLiberties(*inb) == 1)
            {
                for (GoBoard::StoneIterator icap(bd, *inb); icap; ++icap)
                {
                    dirty.Include(*icap);

                    // Track blocks who gain libs as a result of capture
                    if (checklibs)
                    {
                        for (SgNb4Iterator inb2(*icap); inb2; ++inb2)
                            if (bd.IsColor(*inb2, colour))
                                blocks.Include(bd.Anchor(*inb2));
                    }
                }
            }
        }
    }

    // Check if this move did make a capture
    if (! premove && bd.CapturingMove())
    {
        for (GoPointList::Iterator icaptures(bd.CapturedStones()); icaptures;
             ++icaptures)
        {
            dirty.Include(*icaptures);

            // Track blocks who gained liberties as a result of a capture
            if (checklibs)
            {
                for (SgNb4Iterator inb(*icaptures); inb; ++inb)
                    if (bd.IsColor(*inb, colour))
                        blocks.Include(bd.Anchor(*inb));
            }
        }
    }

    // Now mark all stones of blocks that gained liberties
    if (checklibs)
    {
        for (SgSetIterator iblocks(blocks); iblocks; ++iblocks)
            for (GoBoard::StoneIterator istone(bd, *iblocks); istone;
                 ++istone)
                dirty.Include(*istone);
    }

    return dirty;
}

int GoBoardUtil::Approx2Libs(const GoBoard& board, SgPoint block,
    SgPoint p, SgBlackWhite color)
{
    int libs2 = 0;
    for (SgNb4Iterator inb(p); inb; ++inb)
    {
        SgPoint nb = *inb;
        if (board.IsEmpty(nb))
            libs2++;
        else if (board.IsColor(nb, color)
            && board.Anchor(nb) != board.Anchor(block))
            libs2 += board.NumLiberties(nb); // May double count libs
    }

    return libs2;
}

//----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const GoBoardWrite::WriteMap& w)
{
    w.Points().Write(out, w.Board().Size());
    return out;
}

//----------------------------------------------------------------------------
