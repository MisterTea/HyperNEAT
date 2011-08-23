//----------------------------------------------------------------------------
/** @file GoLadder.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoLadder.h"

#include <algorithm>
#include <memory>
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoModBoard.h"
#include "SgVector.h"
#include "SgStack.h"

using namespace std;
using GoBoardUtil::NeighborsOfColor;
using GoBoardUtil::PlayIfLegal;

//----------------------------------------------------------------------------

namespace {

const int GOOD_FOR_PREY = 1000;

const int GOOD_FOR_HUNTER = -1000;

} // namespace

//----------------------------------------------------------------------------

GoLadder::GoLadder()
{
}

inline bool GoLadder::CheckMoveOverflow() const
{
    return (m_bd->MoveNumber() >= m_maxMoveNumber);
}

void GoLadder::InitMaxMoveNumber()
{
    // reserve is the maximum number of moves played before recursion
    // 5 is an optimistic bound
    const int RESERVE = 5;
    m_maxMoveNumber = min(m_bd->MoveNumber() + MAX_LADDER_MOVES,
                          GO_MAX_NUM_MOVES - RESERVE);
}

/** Marks all stones in the block p as part of the prey.
    If 'stones' is not 0, then append the stones to the existing list.
*/
void GoLadder::MarkStonesAsPrey(SgPoint p, SgVector<SgPoint>* stones)
{
    SG_ASSERT(m_bd->IsValidPoint(p));
    if (m_bd->Occupied(p))
    {
        for (GoBoard::StoneIterator it(*m_bd, p); it; ++it)
        {
            SgPoint p = *it;
            m_partOfPrey.Include(p);
            if (stones)
                stones->PushBack(p);
        }
    }
}

/** Filter out captured blocks, blocks not in atari, and blocks not adjacent
    to the prey.
    The latter are found by checking whether blocks adjacent to the block in
    question are the prey or not. Does not return the correct blocks if the
    prey has more than three liberties, but in that case, the prey has
    escaped anyway.
*/
void GoLadder::FilterAdjacent(GoPointList& adjBlocks)
{
    GoPointList temp;
    for (GoPointList::Iterator it(adjBlocks); it; ++it)
    {
        SgPoint block = *it;
        if (m_bd->IsColor(block, m_hunterColor)
            && m_bd->InAtari(block)
            && BlockIsAdjToPrey(block, 1))
            temp.PushBack(block);
    }
    ReduceToBlocks(temp);
    adjBlocks = temp;
}

bool GoLadder::PointIsAdjToPrey(SgPoint p)
{
    return  m_partOfPrey[p - SG_NS]
         || m_partOfPrey[p - SG_WE]
         || m_partOfPrey[p + SG_WE]
         || m_partOfPrey[p + SG_NS];
}

bool GoLadder::BlockIsAdjToPrey(SgPoint p, int numAdj)
{
    SG_ASSERT(m_bd->IsColor(p, m_hunterColor));
    for (GoBoard::StoneIterator it(*m_bd, p); it; ++it)
        if (PointIsAdjToPrey(*it) && --numAdj == 0)
            return true;
    return false;
}

/** Play hunter move and update all the relevant information.
    Play at one of the two liberties of the prey.
*/
int GoLadder::PlayHunterMove(int depth, SgPoint move, SgPoint lib1,
                             SgPoint lib2, const GoPointList& adjBlk,
                             SgVector<SgPoint>* sequence)
{
    SG_ASSERT(move == lib1 || move == lib2);
    // TODO: only pass move and otherLib
    int result = 0;
    if (PlayIfLegal(*m_bd, move, m_hunterColor))
    {
        // Find new adjacent blocks: only block just played can be new
        // in atari.
        // But other blocks previously in atari may have gained new liberties
        // because the move captured a stone, or the move may have extended a
        // block previously in atari.
        //   If it was in atari before, and the move doesn't capture
        // anything, then the block will still be in atari afterwards - no
        // need to check again.
        GoPointList newAdj;
        if (m_bd->InAtari(move))
            newAdj.PushBack(move);
        for (GoPointList::Iterator it(adjBlk); it; ++it)
        {
            SgPoint block = *it;
            if (! m_bd->AreInSameBlock(block, move))
            {
                if (! m_bd->CapturingMove() || m_bd->InAtari(block))
                    newAdj.PushBack(block);
            }
        }
        if (move == lib1)
            lib1 = lib2;
        result = PreyLadder(depth + 1, lib1, newAdj, sequence);
        if (sequence)
            sequence->PushBack(move);
        m_bd->Undo();
    }
    else
    {
        if (sequence)
            sequence->Clear();
        result = GOOD_FOR_PREY - depth;
    }
    return result;
}

/** Play prey move and update all the relevant information.
    Extend the prey by playing at its only liberty, or capture a block
    adjacent to the prey.
*/
int GoLadder::PlayPreyMove(int depth, SgPoint move, SgPoint lib1,
                           const GoPointList& adjBlk,
                           SgVector<SgPoint>* sequence)
{
    int result = 0;
    GoPointList newAdj(adjBlk);
    SgVector<SgPoint> newLib;
    SgVector<SgPoint> newStones;
    SgVector<SgPoint> neighbors;
    if (move == lib1)
    {
        NeighborsOfColor(*m_bd, move, m_preyColor, &neighbors);
        for (SgVectorIterator<SgPoint> iter(neighbors); iter; ++iter)
        {
            SgPoint block = *iter;
            if (! m_partOfPrey[block])
            {
                MarkStonesAsPrey(block, &newStones);
                GoPointList temp =
                    GoBoardUtil::AdjacentStones(*m_bd, block);
                newAdj.PushBackList(temp);
                for (GoBoard::LibertyIterator it(*m_bd, block); it; ++it)
                    newLib.Include(*it);
            }
        }
        m_partOfPrey.Include(move);
    }
    if (PlayIfLegal(*m_bd, move, m_preyColor))
    {
        if (move == lib1)
        {
            NeighborsOfColor(*m_bd, move, SG_EMPTY, &neighbors);
            for (SgVectorIterator<SgPoint> iter(newLib); iter; ++iter)
            {
                SgPoint point = *iter;
                // Test for Empty is necessary because newLib will include
                // the move just played.
                if (m_bd->IsEmpty(point))
                    neighbors.Include(point);
            }
        }
        else
        {
            neighbors.PushBack(lib1);
        }
        if (m_bd->CapturingMove())
        {   // Add the points at the captured stones that are adjacent to the
            // prey to the liberties, at least if exactly one stone captured.
            for (GoPointList::Iterator it(m_bd->CapturedStones()); it;
                 ++it)
            {
                SgPoint stone = *it;
                if (PointIsAdjToPrey(stone))
                    neighbors.Include(stone);
            }
        }
        SG_ASSERT(! neighbors.IsEmpty());
        lib1 = neighbors[0];
        SG_ASSERT(m_bd->IsEmpty(lib1));
        SgSList<SgPoint,4> temp =
            NeighborsOfColor(*m_bd, move, m_hunterColor);
        newAdj.PushBackList(temp);
        FilterAdjacent(newAdj);

        if (neighbors.Length() == 1)
            result = HunterLadder(depth + 1, lib1, newAdj, sequence);
        else if (neighbors.Length() == 2)
        {
            SgPoint lib2 = neighbors[1];
            SG_ASSERT(m_bd->IsEmpty(lib2));
            result = HunterLadder(depth + 1, lib1, lib2, newAdj, sequence);
        } 
        else // 3 <= numLib
        {
            if (sequence)
                sequence->Clear();
            result = GOOD_FOR_PREY - (depth + 1);
        }
        if (sequence)
            sequence->PushBack(move);
        m_bd->Undo();
    }
    else
    {
        if (sequence)
            sequence->Clear();
        result = GOOD_FOR_HUNTER + depth;
    }
    m_partOfPrey.Exclude(move);
    m_partOfPrey.Exclude(newStones);

    return result;
}

int GoLadder::PreyLadder(int depth, SgPoint lib1,
                         const GoPointList& adjBlk,
                         SgVector<SgPoint>* sequence)
{
    if (CheckMoveOverflow())
        return GOOD_FOR_PREY;
    int result = 0;
    for (GoPointList::Iterator iter(adjBlk); iter; ++iter)
    {
        SgPoint block = *iter;
        SgPoint move = *GoBoard::LibertyIterator(*m_bd, block);
        if (BlockIsAdjToPrey(block, 2))
        {
            if (sequence)
                sequence->SetTo(move);
            result = GOOD_FOR_PREY - depth;
        }
        else if (move != lib1)
        {
            result = PlayPreyMove(depth, move, lib1, adjBlk, sequence);
        }
        if (0 < result)
            break;
    }
    if (result <= 0)
    {
        if (sequence)
        {
            SgVector<SgPoint> seq2;
            int result2 = PlayPreyMove(depth, lib1, lib1, adjBlk, &seq2);
            if (result < result2 || result == 0)
            {
                result = result2;
                sequence->SwapWith(&seq2);
            }
        }
        else
        {
            int result2 = PlayPreyMove(depth, lib1, lib1, adjBlk, 0);
            if (result < result2 || result == 0)
                result = result2;
        }
    }
    return result;
}

int GoLadder::HunterLadder(int depth, SgPoint lib1, const GoPointList& adjBlk,
                           SgVector<SgPoint>* sequence)
{
    SG_UNUSED(adjBlk);
    if (CheckMoveOverflow())
        return GOOD_FOR_PREY;
    if (sequence)
        sequence->SetTo(lib1);
    // TODO: should probably test for IsSnapback here, but don't have
    // the right information available.
    return GOOD_FOR_HUNTER + depth;
}

int GoLadder::HunterLadder(int depth, SgPoint lib1, SgPoint lib2, 
                           const GoPointList& adjBlk,
                           SgVector<SgPoint>* sequence)
{
    if (CheckMoveOverflow())
        return GOOD_FOR_PREY;
    int result = 0;
    if (m_bd->NumEmptyNeighbors(lib1) < m_bd->NumEmptyNeighbors(lib2))
    {
        swap(lib1, lib2);
    }
    if (m_bd->NumEmptyNeighbors(lib1) == 3
        && ! SgPointUtil::AreAdjacent(lib1, lib2))
    {
        // If not playing at lib1, then prey will play at lib1 and
        // get three liberties; little to update in this case.
        m_bd->Play(lib1, m_hunterColor);
        result = PreyLadder(depth + 1, lib2, adjBlk, sequence);
        if (sequence)
            sequence->PushBack(lib1);
        m_bd->Undo();
    }
    else
    {
        // Two liberties, hunter to play, but not standard case.
        if (! adjBlk.IsEmpty()
            && *GoBoard::LibertyIterator(*m_bd, adjBlk[0]) == lib2)
        {
            swap(lib1, lib2); // protect hunter blocks in atari
        }
        result = PlayHunterMove(depth, lib1, lib1, lib2,
                                adjBlk, sequence);
        if (0 <= result) // escaped
        {
            if (sequence)
            {
                SgVector<SgPoint> seq2;
                int result2 = PlayHunterMove(depth, lib2, lib1, lib2,
                                             adjBlk, &seq2);
                if (result2 < result)
                {   result = result2;
                    sequence->SwapWith(&seq2);
                }
            }
            else
            {
                int result2 = PlayHunterMove(depth, lib2, lib1, lib2,
                                             adjBlk, 0);
                if (result2 < result)
                    result = result2;
            }
        }
    }
    return result;
}

void GoLadder::ReduceToBlocks(GoPointList& stones)
{
    // Single block is frequent case, don't compute block.
    if (stones.IsEmpty())
        ; // nothing to do
    else if (stones.Length() <= 1)
    {
        if (m_bd->IsEmpty(stones[0]))
            stones.Clear();
    }
    else
    {
        GoPointList visited;
        // TODO: create SgMarker member in GoLadder and use it for visited
        // points
        GoPointList result;
        for (GoPointList::Iterator it(stones); it; ++it)
        {
            SgPoint stone = *it;
            if (m_bd->Occupied(stone) && ! visited.Contains(stone))
            {
                result.PushBack(stone);
                for (GoBoard::StoneIterator it(*m_bd, stone); it; ++it)
                    visited.PushBack(*it);
            }
        }
        stones = result;
    }
}

/** Main ladder routine */
int GoLadder::Ladder(const GoBoard& bd, SgPoint prey, SgBlackWhite toPlay,
                     SgVector<SgPoint>* sequence, bool twoLibIsEscape)
{
    GoModBoard modBoard(bd);
    m_bd = &modBoard.Board();
    InitMaxMoveNumber();
    if (sequence)
        sequence->Clear();
    if (! m_bd->Occupied(prey))
        return 0;
    if (CheckMoveOverflow())
        return GOOD_FOR_PREY;
    int result = 0;
    m_preyColor = m_bd->GetStone(prey);
    m_hunterColor = SgOppBW(m_preyColor);
    int numLib = m_bd->NumLiberties(prey);
    if (2 < numLib)
        result = GOOD_FOR_PREY;
    else
    {
        GoBoard::LibertyIterator libit(*m_bd, prey);
        SgPoint lib1 = *libit;
        m_partOfPrey.Clear();
        MarkStonesAsPrey(prey);
        GoPointList adjBlk = GoBoardUtil::AdjacentStones(*m_bd, prey);
        FilterAdjacent(adjBlk);
        if (toPlay == m_preyColor)
        {
            if (numLib == 1)
                result = PreyLadder(0, lib1, adjBlk, sequence);
            else if (twoLibIsEscape) // prey to play, numLib >= 2
                // For example, Explorer cannot treat this case as a ladder,
                // it messes up the logic
                result = GOOD_FOR_PREY;
            else
            {
                // Prey to play, two liberties. This is usually good, but
                // need to prove that there is some move that really
                //  escapes laddercapture.
                // Try liberties of adjacent blocks with at most two
                // liberties, try lib1 and lib2, and try moves one away
                // from the two liberties.
                // Good example with three blocks that test this case:
                // (;GM[1]SZ[19]FF[3]
                // AB[qa][pa][pb][pd][pc][qe][re][rd][rc][se]
                // AW[pe][pf][qf][qd][qc][rb][qb][sa][sc][rf][rg][sg])
                SgVector<SgPoint> movesToTry;

                // Liberties of adj. blocks with at most two liberties.
                adjBlk = GoBoardUtil::AdjacentStones(*m_bd, prey);
                ReduceToBlocks(adjBlk);
                for (GoPointList::Iterator iterAdj(adjBlk); iterAdj;
                     ++iterAdj)
                {
                    SgPoint block = *iterAdj;
                    SG_ASSERT(m_bd->IsColor(block, m_hunterColor));
                    SG_ASSERT(BlockIsAdjToPrey(block, 1));
                    if (m_bd->NumLiberties(block) <= 2)
                        for (GoBoard::LibertyIterator it(*m_bd, block); it;
                             ++it)
                            movesToTry.PushBack(*it);
                }

                // Liberties of blocks.
                ++libit;
                SgPoint lib2 = *libit;
                movesToTry.PushBack(lib1);
                movesToTry.PushBack(lib2);

                // Moves one away from liberties.
                SgVector<SgPoint> neighbors;
                NeighborsOfColor(*m_bd, lib1, SG_EMPTY, &neighbors);
                movesToTry.Concat(&neighbors);
                NeighborsOfColor(*m_bd, lib2, SG_EMPTY, &neighbors);
                movesToTry.Concat(&neighbors);

                // Try whether any of these moves lead to escape.
                for (SgVectorIterator<SgPoint> it(movesToTry); it; ++it)
                {
                    if (PlayIfLegal(*m_bd, *it, m_preyColor))
                    {
                        if (Ladder(bd, prey, m_hunterColor, 0, twoLibIsEscape)
                            > 0)
                        {
                            if (sequence)
                                sequence->PushBack(*it); 
                            result = GOOD_FOR_PREY;
                        }
                        m_bd->Undo();
                    }
                    if (result != 0)
                        break;
                }

                // If none of those moves worked, prey can't escape.
                // This is a bit pessimistic, there may be other moves
                // that do lead to escape (e.g. approach moves), but
                // ladder algorithm doesn't know about those.
                if (result == 0)
                    result = GOOD_FOR_HUNTER;
            }
        }
        else
        {
            if (IsSnapback(prey))
                result = GOOD_FOR_PREY;
            else
            {
                ++libit;
                if (libit) // two liberties
                    result = HunterLadder(0, lib1, *libit, adjBlk, sequence);
                else // one liberty
                    result = HunterLadder(0, lib1, adjBlk, sequence);
            }
        }
    }
    if (sequence)
        sequence->Reverse(); // built as a stack, with first move at end.
    return result;
}

/** Check whether the block at 'prey' is caught in a snapback.
    Snapback means that it can be captured, but it's only a single stone, and
    the prey can capture right back.
*/
bool GoLadder::IsSnapback(SgPoint prey)
{
    bool isSnapback = false;
    if (m_bd->IsSingleStone(prey) && m_bd->InAtari(prey))
    {
        SgPoint liberty = *GoBoard::LibertyIterator(*m_bd, prey);
        if (PlayIfLegal(*m_bd, liberty, SgOppBW(m_bd->GetStone(prey))))
        {
            isSnapback = (m_bd->InAtari(liberty)
                          && ! m_bd->IsSingleStone(liberty));
            m_bd->Undo();
        }
    }
    return isSnapback;
}

//----------------------------------------------------------------------------

bool GoLadderUtil::Ladder(const GoBoard& bd, SgPoint prey,
                          SgBlackWhite toPlay, bool twoLibIsEscape,
                          SgVector<SgPoint>* sequence)
{
    SG_ASSERT(bd.IsValidPoint(prey));
    SG_ASSERT(bd.Occupied(prey));
    // AR: from Martin: for an unsettled block with 2 liberties, it
    // immediately says it can escape, but does not return a move.
    // Sequence is empty.  So I have to special case this and look for
    // moves that escape from ladder myself.
    // ---> need to tell Martin if I find this
#ifndef NDEBUG
    SgHashCode oldHash = bd.GetHashCode();
#endif
    GoLadder ladder;
    int result = ladder.Ladder(bd, prey, toPlay, sequence, twoLibIsEscape);
#ifndef NDEBUG
    // Make sure Ladder didn't change the board position.
    SG_ASSERT(oldHash == bd.GetHashCode());
#endif
    SG_ASSERT(result != 0);
    return (result < 0);
}

GoLadderStatus GoLadderUtil::LadderStatus(const GoBoard& bd, SgPoint prey,
                                          bool twoLibIsEscape,
                                          SgPoint* toCapture,
                                          SgPoint* toEscape)
{
    SG_ASSERT(bd.IsValidPoint(prey));
    SG_ASSERT(bd.Occupied(prey));
#ifndef NDEBUG
    SgHashCode oldHash = bd.GetHashCode();
#endif
    // Unsettled only if can capture when hunter plays first, and can escape
    // if prey plays first.
    GoLadder ladder;
    SgBlackWhite preyColor = bd.GetStone(prey);
    SgVector<SgPoint> captureSequence;
    GoLadderStatus status = GO_LADDER_ESCAPED;
    if (ladder.Ladder(bd, prey, SgOppBW(preyColor), &captureSequence,
                      twoLibIsEscape) < 0)
    {
        SgVector<SgPoint> escapeSequence;
        if (ladder.Ladder(bd, prey, preyColor, &escapeSequence,
                          twoLibIsEscape) < 0)
            status = GO_LADDER_CAPTURED;
        else
        {
            status = GO_LADDER_UNSETTLED;
            // Unsettled = ladder depends on who plays first, so there must
            // be a move that can be played.
            SG_ASSERT(captureSequence.NonEmpty());
            // escapeSequence can be empty in 2 libs, prey to play case
            SG_ASSERT(twoLibIsEscape || escapeSequence.NonEmpty());
            if (toCapture)
                *toCapture = captureSequence.Front();
            if (toEscape)
                *toEscape = escapeSequence.IsEmpty() ? SG_PASS :
                                                       escapeSequence.Front();
        }
    }
#ifndef NDEBUG
    // Make sure Ladder didn't change the board position.
    SG_ASSERT(oldHash == bd.GetHashCode());
#endif
    return status;
}

bool GoLadderUtil::IsProtectedLiberty(const GoBoard& bd, SgPoint liberty,
                                      SgBlackWhite color)
{
    bool ignoreLadder;
    bool ignoreKo;
    return IsProtectedLiberty(bd, liberty, color, ignoreLadder, ignoreKo,
                              true);
}

bool GoLadderUtil::IsProtectedLiberty(const GoBoard& bd1, SgPoint liberty,
                                      SgBlackWhite col, bool& byLadder,
                                      bool& isKoCut, bool tryLadder)
{
    byLadder = false;
    isKoCut = false;
    GoModBoard mbd(bd1);
    GoBoard& bd = mbd.Board();

    const SgBlackWhite toPlay = bd1.ToPlay();
    bd.SetToPlay(SgOppBW(col));
    bool isProtected;
    if (! PlayIfLegal(bd, liberty))
        isProtected = bd.LastMoveInfo(GO_MOVEFLAG_SUICIDE);
        // opponent cannot play there
    else
    {
        if (bd.LastMoveInfo(GO_MOVEFLAG_SUICIDE))
           isProtected = true;
        else
        {
            if (bd.InAtari(liberty))
            {
                if (bd.NumStones(liberty) > 1)
                    isProtected = true;
                else
                {
                    SgPoint p = bd.TheLiberty(liberty);
                    if (PlayIfLegal(bd, p))
                    {
                        isProtected =    (bd.NumStones(p) != 1)
                                      || (bd.NumLiberties(p) != 1);
                                      // yes, can re-capture there
                        bd.Undo();
                    }
                    else
                        isProtected = false;

                    if (! isProtected)
                        isKoCut = true;
                }
            }
            else if (tryLadder)
            {
                isProtected = Ladder(bd, liberty, bd.ToPlay(), true);
                if (isProtected)
                    byLadder = true;
            }
            else // don't try ladder
                isProtected = false;
        }
        bd.Undo();
    }
    bd.SetToPlay(toPlay);
    return isProtected;
}

/** try to escape/capture prey block
    Possible return values:
    - SG_PASS if already escaped/captured
    - the point to play
    - SG_NULLMOVE in case of failure
*/
SgPoint GoLadderUtil::TryLadder(const GoBoard& bd, SgPoint prey,
                                SgBlackWhite firstPlayer)
{
    SgVector<SgPoint> sequence;
    bool isCaptured = Ladder(bd, prey, firstPlayer, true, &sequence);
    // if move is same color as prey, we want to escape
    // else we want to capture.
    SgPoint p;
    if (isCaptured != (firstPlayer == bd.GetStone(prey)))
        p = sequence.IsEmpty() ? SG_PASS : sequence.Front();
    else
        p = SG_NULLMOVE;
    return p;
}

//----------------------------------------------------------------------------

