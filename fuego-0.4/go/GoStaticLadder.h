//----------------------------------------------------------------------------
/** @file GoStaticLadder.h
*/
//----------------------------------------------------------------------------

#ifndef GO_STATICLADDER_H
#define GO_STATICLADDER_H

#include "SgBlackWhite.h"
#include "SgPoint.h"

class GoBoard;

//----------------------------------------------------------------------------

/** Fast ladder computations that do not play actual moves on the board.
    Can be used where speed is of high importance (e.g. MC playouts).
    They detect only the subset of the ladders detected in GoLadder,
    in which the ladder path is free of stones.
*/
namespace GoStaticLadder
{
    /** Return true, if block can be captured in a ladder running along the
        edge.
    */
    bool IsEdgeLadder(const GoBoard& bd, SgPoint target, SgBlackWhite toPlay);

    /** Return true, if block can be captured in a ladder. */
    bool IsLadder(const GoBoard& bd, SgPoint target, SgBlackWhite toPlay);
}

//----------------------------------------------------------------------------

#endif // GO_STATICLADDER_H
