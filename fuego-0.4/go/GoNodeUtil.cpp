//----------------------------------------------------------------------------
/** @file GoNodeUtil.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoNodeUtil.h"

#include "GoBoard.h"
#include "SgNode.h"

using namespace std;

//----------------------------------------------------------------------------


SgNode* GoNodeUtil::CreatePosition(int boardSize, SgBlackWhite toPlay,
                               const SgVector<SgPoint>& bPoints,
                               const SgVector<SgPoint>& wPoints)
{
    SgNode* node = new SgNode();
    node->Add(new SgPropInt(SG_PROP_SIZE, boardSize));
    node->Add(new SgPropPlayer(SG_PROP_PLAYER, toPlay));
    node->Add(new SgPropAddStone(SG_PROP_ADD_BLACK, bPoints));
    node->Add(new SgPropAddStone(SG_PROP_ADD_WHITE, wPoints));
    return node;
}

GoKomi GoNodeUtil::GetKomi(const SgNode* node)
{
    while (node != 0)
    {
        if (node->HasProp(SG_PROP_KOMI))
        {
            try
            {
                return GoKomi(static_cast<float>
                              (node->GetRealProp(SG_PROP_KOMI)));
            }
            catch (const GoKomi::InvalidKomi&)
            {
                break;
            }
        }
        node = node->Father();
    }
    return GoKomi();
}

int GoNodeUtil::GetHandicap(const SgNode* node)
{
    while (node != 0)
    {
        if (node->HasProp(SG_PROP_HANDICAP))
            return node->GetIntProp(SG_PROP_HANDICAP);
        node = node->Father();
    }
    return 0;
}

//----------------------------------------------------------------------------

