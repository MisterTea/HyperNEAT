//----------------------------------------------------------------------------
/** @file SgMove.h
    Definitions for game-independent moves.
    Useful for games that can encode moves as positive integers (including 0).
    Negative integers are reserved for special meanings, like pass or null
    move and defined in this file.
    Don't make an assumption about the exact values of the special moves
    (apart that they are negative). If you do, at least document it with
    a static assertion.
    Classes that can work with moves of different games should only include
    this header.
*/
//----------------------------------------------------------------------------

#ifndef SG_MOVE_H
#define SG_MOVE_H

//----------------------------------------------------------------------------

typedef int SgMove;

/** A null move is an uninitialized move, not legal to execute.
    Not the same as a pass move or a null move in null-move-search.
*/
const SgMove SG_NULLMOVE = -1;

/** Used for coupon search.
    Indicates that no move was made in the game, but a coupon taken from
    the coupon stack.
*/
const SgMove SG_COUPONMOVE = -2;

/** Used for coupon search.
    Indicates that no move was made in the game, but a virtual coupon taken
    from the coupon stack.
*/
const SgMove SG_COUPONMOVE_VIRTUAL = -3;

/** Resign. */
const SgMove SG_RESIGN = -4;

//----------------------------------------------------------------------------

namespace SgMoveUtil
{
    /** Is move SG_COUPONMOVE or SG_COUPONMOVE_VIRTUAL? */
    bool IsCouponMove(SgMove move);
}

inline bool SgMoveUtil::IsCouponMove(SgMove move)
{
    return (move == SG_COUPONMOVE || move == SG_COUPONMOVE_VIRTUAL);
}

//----------------------------------------------------------------------------

#endif // SG_MOVE_H
