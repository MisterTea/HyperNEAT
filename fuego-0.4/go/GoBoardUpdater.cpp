//----------------------------------------------------------------------------
/** @file GoBoardUpdater.cpp */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoBoardUpdater.h"

#include "GoBoard.h"
#include "GoSetupUtil.h"
#include "SgNode.h"
#include "SgUtil.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

SgEmptyBlackWhite GetPlayer(const SgNode* node)
{
    if (! node->HasProp(SG_PROP_PLAYER))
        return SG_EMPTY;
    SgPropPlayer* prop =
        dynamic_cast<SgPropPlayer*>(node->Get(SG_PROP_PLAYER));
    return prop->Value();
}

} // namespace

//----------------------------------------------------------------------------

void GoBoardUpdater::Update(const SgNode* node, GoBoard& bd)
{
    SG_ASSERT(node != 0);
    m_nodes.clear();
    while (node != 0)
    {
        m_nodes.push_back(node);
        node = node->Father();
    }
    const SgNode* root = m_nodes[m_nodes.size() - 1];
    int size = GO_DEFAULT_SIZE;
    SgPropInt* boardSize = static_cast<SgPropInt*>(root->Get(SG_PROP_SIZE));
    if (boardSize)
    {
        size = boardSize->Value();
        SG_ASSERT(SgUtil::InRange(size, SG_MIN_SIZE, SG_MAX_SIZE));
    }
    bd.Init(size);
    for (vector<const SgNode*>::reverse_iterator it = m_nodes.rbegin();
         it != m_nodes.rend(); ++it)
    {
        const SgNode* node = *it;
        SgEmptyBlackWhite player = GetPlayer(node);
        if (node->HasProp(SG_PROP_ADD_EMPTY)
            || node->HasProp(SG_PROP_ADD_BLACK)
            || node->HasProp(SG_PROP_ADD_WHITE))
        {
            // Compute the new initial setup position to re-initialize the
            // board with
            GoSetup setup = GoSetupUtil::CurrentPosSetup(bd);
            if (player != SG_EMPTY)
                setup.m_player = player;
            if (node->HasProp(SG_PROP_ADD_BLACK))
            {
                SgPropAddStone* prop =
                  dynamic_cast<SgPropAddStone*>(node->Get(SG_PROP_ADD_BLACK));
                const SgVector<SgPoint>& addBlack = prop->Value();
                for (SgVectorIterator<SgPoint> it2(addBlack); it2; ++it2)
                {
                    SgPoint p = *it2;
                    setup.m_stones[SG_WHITE].Exclude(p);
                    if (! setup.m_stones[SG_BLACK].Contains(p))
                        setup.AddBlack(p);
                }
            }
            if (node->HasProp(SG_PROP_ADD_WHITE))
            {
                SgPropAddStone* prop =
                  dynamic_cast<SgPropAddStone*>(node->Get(SG_PROP_ADD_WHITE));
                const SgVector<SgPoint>& addWhite = prop->Value();
                for (SgVectorIterator<SgPoint> it2(addWhite); it2; ++it2)
                {
                    SgPoint p = *it2;
                    setup.m_stones[SG_BLACK].Exclude(p);
                    if (! setup.m_stones[SG_WHITE].Contains(p))
                        setup.AddWhite(p);
                }
            }
            if (node->HasProp(SG_PROP_ADD_EMPTY))
            {
                SgPropAddStone* prop =
                  dynamic_cast<SgPropAddStone*>(node->Get(SG_PROP_ADD_EMPTY));
                const SgVector<SgPoint>& addEmpty = prop->Value();
                for (SgVectorIterator<SgPoint> it2(addEmpty); it2; ++it2)
                {
                    SgPoint p = *it2;
                    setup.m_stones[SG_BLACK].Exclude(p);
                    setup.m_stones[SG_WHITE].Exclude(p);
                }
            }
            bd.Init(bd.Size(), setup);
        }
        else if (player != SG_EMPTY)
            bd.SetToPlay(player);
        if (node->HasProp(SG_PROP_MOVE))
        {
            SgPropMove* prop =
                dynamic_cast<SgPropMove*>(node->Get(SG_PROP_MOVE));
            SgPoint p = prop->Value();
            if (p == SG_PASS || ! bd.Occupied(p))
                bd.Play(p, prop->Player());
        }
    }
}

//----------------------------------------------------------------------------
