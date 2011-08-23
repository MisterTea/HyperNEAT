//----------------------------------------------------------------------------
/** @file GoNodeUtil.h
    Utility functions for Go trees.
*/
//----------------------------------------------------------------------------

#ifndef GO_NODEUTIL_H
#define GO_NODEUTIL_H

#include "GoKomi.h"
#include "SgBlackWhite.h"
#include "SgBWArray.h"
#include "SgPoint.h"
#include "SgVector.h"

class GoBoard;
class SgNode;

//----------------------------------------------------------------------------

namespace GoNodeUtil
{
    /** Create a root node containing a given board position. */
    template<class BOARD> SgNode* CreateRoot(const BOARD& board);

    /** Create a position with given size, toPlay, b and w points */
    SgNode* CreatePosition(int boardSize, SgBlackWhite toPlay,
                               const SgVector<SgPoint>& bPoints,
                               const SgVector<SgPoint>& wPoints);

    /** Find komi that is valid for this node.
        Search parent nodes until a node with a komi property is found.
    */
    GoKomi GetKomi(const SgNode* node);

    /** Find handicap that is valid for this node.
        Search parent nodes until a node with a handicap property is found.
    */
    int GetHandicap(const SgNode* node);
}

//----------------------------------------------------------------------------

template<class BOARD>
SgNode* GoNodeUtil::CreateRoot(const BOARD& board)
{
    SgBWArray<SgVector<SgPoint> > pointList;
    for (typename BOARD::Iterator it(board); it; ++it)
    {
        if (board.Occupied(*it))
            pointList[board.GetColor(*it)].PushBack(*it);
    }
    return CreatePosition(board.Size(), board.ToPlay(),
                pointList[SG_BLACK], pointList[SG_WHITE]);
}

//----------------------------------------------------------------------------

#endif // GO_NODEUTIL_H

