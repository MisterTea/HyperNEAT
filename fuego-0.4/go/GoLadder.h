//----------------------------------------------------------------------------
/** @file GoLadder.h
    Fast ladder algorithm, computes ladders and snapbacks.

    @note
    Will become obsolete when class GoBoard is fast enough for computing
    ladders.
*/
//----------------------------------------------------------------------------

#ifndef GO_LADDER_H
#define GO_LADDER_H

#include "GoBoard.h"
#include "SgBoardColor.h"
#include "GoModBoard.h"
#include "SgPoint.h"
#include "SgPointSet.h"
#include "SgVector.h"

//----------------------------------------------------------------------------

enum GoLadderStatus
{
    /** Don't know anything about the status of this block. */
    GO_LADDER_UNKNOWN,

    /** Definitely captured, regardless of who plays first. */
    GO_LADDER_CAPTURED,

    /** Capture or escape depends on who plays first. */
    GO_LADDER_UNSETTLED,

    /** Definitely escaped, regardless of who plays first. */
    GO_LADDER_ESCAPED
};

//----------------------------------------------------------------------------

/** This class contains all the ladder-specific stuff. */
class GoLadder
{
public:
    GoLadder();

    /** Main ladder routine.
        twoLibIsEscape: if prey is to play and has two libs, does it count as
        an immediate escape, or shall we keep trying to capture?
        @return Positive value if ladder is good for prey, negative, if good
        for hunter.
    */
    int Ladder(const GoBoard& bd, SgPoint prey, SgBlackWhite toPlay,
               SgVector<SgPoint>* sequence, bool twoLibIsEscape = false);

private:
    /** Maximum number of moves in ladder.
        If board has simple ko rule, ladders could not terminate.
    */
    static const int MAX_LADDER_MOVES = 200;

    /** Maximum move number before ladder should be aborted. */
    int m_maxMoveNumber;

    GoBoard* m_bd;

    SgPointSet m_partOfPrey;

    SgBlackWhite m_preyColor;

    SgBlackWhite m_hunterColor;

    bool CheckMoveOverflow() const;

    void InitMaxMoveNumber();

    bool PointIsAdjToPrey(SgPoint p);

    bool BlockIsAdjToPrey(SgPoint p, int numAdj);

    void MarkStonesAsPrey(SgPoint p, SgVector<SgPoint>* stones = 0);

    void FilterAdjacent(GoPointList& adjBlocks);

    int PlayHunterMove(int depth, SgPoint move, SgPoint lib1, SgPoint lib2,
                       const GoPointList& adjBlk,
                       SgVector<SgPoint>* sequence);

    int PlayPreyMove(int depth, SgPoint move, SgPoint lib1,
                     const GoPointList& adjBlk, SgVector<SgPoint>* sequence);

    bool IsSnapback(SgPoint prey);

    int PreyLadder(int depth, SgPoint lib1, const GoPointList& adjBlk,
                   SgVector<SgPoint>* sequence);

    int HunterLadder(int depth, SgPoint lib1, const GoPointList& adjBlk, 
                     SgVector<SgPoint>* sequence);

    int HunterLadder(int depth, SgPoint lib1, SgPoint lib2,
                     const GoPointList& adjBlk, SgVector<SgPoint>* sequence);

    void ReduceToBlocks(GoPointList& stones);
};

//----------------------------------------------------------------------------

namespace GoLadderUtil {

/** Return whether or not the block at 'prey' can be captured in a ladder when
    'toPlay' plays first.
    True means capture, false means escape. If 'sequence' is not 0, return a
    sequence of moves to capture or escape (need not be the optimal sequence).
    Return an empty sequence if the prey is already captured or has escaped,
    without needing to play a move.
    If the prey can be temporarily removed from the board but can capture
    back immediately (snapback), return that the prey cannot be captured.
*/
bool Ladder(const GoBoard& board, SgPoint prey, SgBlackWhite toPlay,
            bool fTwoLibIsEscape = false, SgVector<SgPoint>* sequence = 0);

/** Return whether the block at 'prey' is captured, escaped, or unsettled
    with regards to capture in a ladder.
    If it is unsettled, set '*toCapture' and '*toEscape' (if not 0) to the
    capturing/escaping move to play.
    Otherwise, leave '*toCapture' and '*toEscape' unchanged. The point at
    'prey' must be occupied.
*/
GoLadderStatus LadderStatus(const GoBoard& bd, SgPoint prey,
                            bool fTwoLibIsEscape = false,
                            SgPoint* toCapture = 0, SgPoint* toEscape = 0);

/** Check if this is a chain connection point, or a ko cut point.
    Try to play there as opponent, then check:
    - Suicide
    - self removal, self atari
    - can be captured by ladder?
    - is it a Ko?
*/
bool IsProtectedLiberty(const GoBoard& bd, SgPoint liberty, SgBlackWhite col,
                        bool& byLadder, bool& isKoCut, bool tryLadder = true);

/** Simple form, calls the complex form and ignores bool results */
bool IsProtectedLiberty(const GoBoard& bd, SgPoint liberty, SgBlackWhite col);

SgPoint TryLadder(const GoBoard& bd, SgPoint prey, SgBlackWhite firstPlayer);

} // namespace GoLadderUtil

//----------------------------------------------------------------------------

#endif // GO_LADDER_H

