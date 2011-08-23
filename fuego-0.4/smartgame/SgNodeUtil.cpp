//----------------------------------------------------------------------------
/** @file SgNodeUtil.cpp
    See SgNodeUtil.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgNodeUtil.h"

#include <vector>
#include "SgNode.h"
#include "SgTimeRecord.h"

using namespace std;

//----------------------------------------------------------------------------

int SgNodeUtil::GetMoveNumber(const SgNode* node)
{
    int nuMoves = 0;
    while (node != 0)
    {
        if (node->HasProp(SG_PROP_MOVE))
            ++nuMoves;
        if (node->HasProp(SG_PROP_ADD_BLACK)
            || node->HasProp(SG_PROP_ADD_WHITE)
            || node->HasProp(SG_PROP_ADD_EMPTY))
            break;
        node = node->Father();
    }
    return nuMoves;
}

void SgNodeUtil::UpdateTime(SgTimeRecord& time, const SgNode* node)
{
    vector<const SgNode*> nodes;
    while (node != 0)
    {
        nodes.push_back(node);
        node = node->Father();
    }
    for (vector<const SgNode*>::reverse_iterator it = nodes.rbegin();
         it != nodes.rend(); ++it)
    {
        const SgNode* node = *it;
        if (node->HasProp(SG_PROP_LOSE_TIME))
            time.SetLoseOnTime(true);
        if (node->HasProp(SG_PROP_OT_NU_MOVES))
            time.SetOTNumMoves(node->GetIntProp(SG_PROP_OT_NU_MOVES));
        if (node->HasProp(SG_PROP_OT_PERIOD))
            time.SetOTPeriod(node->GetRealProp(SG_PROP_OT_PERIOD));
        if (node->HasProp(SG_PROP_OVERHEAD))
            time.SetOverhead(node->GetRealProp(SG_PROP_OVERHEAD));
    }
}

//----------------------------------------------------------------------------
