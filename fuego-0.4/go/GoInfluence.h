//----------------------------------------------------------------------------
/** @file GoInfluence.h
    Simple functions for influence computation
*/
//----------------------------------------------------------------------------

#ifndef GO_INFLUENCE_H
#define GO_INFLUENCE_H

#include "GoBoard.h"
#include "GoGame.h"
#include "SgPointArray.h"

//----------------------------------------------------------------------------

namespace GoInfluence {

/** Compute influence by nuExpand expansions followed by nuShrink
    contractions.
    Starts from the player's stones.
    Expand: add the neighbor points
    Shrink: take away the points on the border of the set
*/
void FindInfluence(const GoBoard& board, int nuExpand, int nuShrink,
                   SgBWSet* result);

/** Influence of a single player. @see FindInfluence.
    If possible use FindInfluence directly instead of calling this twice
    for black and white.
*/
int Influence(const GoBoard& board, SgBlackWhite color, int nuExpand,
              int nuShrink);

/** Compute influence, stopping at stopPts */
void ComputeInfluence(const GoBoard& board, const SgBWSet& stopPts,
                      SgBWArray<SgPointArray<int> >* influence);

} // namespace GoInfluence

//----------------------------------------------------------------------------

#endif // GO_INFLUENCE_H

