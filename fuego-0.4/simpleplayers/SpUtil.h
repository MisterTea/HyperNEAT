//----------------------------------------------------------------------------
/** @file SpUtil.h
    Utility functions
*/
//----------------------------------------------------------------------------

#ifndef SP_UTIL_H
#define SP_UTIL_H

#include "SgBlackWhite.h"
#include "SgPointSet.h"

class GoBoard;

//----------------------------------------------------------------------------

namespace SpUtil
{
    /** Get all legal moves.
        Filters out moves into own safe territory with no opponent stones
        adjacent if useFilter.
    */
    SgPointSet GetRelevantMoves(GoBoard& bd, SgBlackWhite toPlay,
                                bool useFilter);
}

//----------------------------------------------------------------------------

#endif // SP_UTIL_H

