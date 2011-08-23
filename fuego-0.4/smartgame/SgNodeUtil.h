//----------------------------------------------------------------------------
/** @file SgNodeUtil.h */
//----------------------------------------------------------------------------

#ifndef SG_NODEUTIL_H
#define SG_NODEUTIL_H

class SgNode;
class SgTimeRecord;

//----------------------------------------------------------------------------

namespace SgNodeUtil
{
    /** Get number of moves since root or last node with setup properties. */
    int GetMoveNumber(const SgNode* node);

    /** Update the current time information corresponding to a node.
        Visits all nodes in the path from root to the target node and applies
        the following properties to the time record: SG_PROP_LOSE_TIME,
        SG_PROP_OT_NU_MOVES, SG_PROP_OT_NU_MOVES, SG_PROP_OT_PERIOD,
        SG_PROP_OVERHEAD.
        @todo Initial time settings property @c TM is not handled,
        SgTimeRecord does presently not support main time (without a finite
        number of moves left)
    */
    void UpdateTime(SgTimeRecord& time, const SgNode* node);
}

//----------------------------------------------------------------------------

#endif // SG_NODEUTIL_H
