//----------------------------------------------------------------------------
/** @file GoStaticLadder.cpp
    See GoStaticLadder.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoStaticLadder.h"

#include "GoBoard.h"

using namespace std;

//----------------------------------------------------------------------------

bool GoStaticLadder::IsEdgeLadder(const GoBoard& bd, SgPoint target,
                                  SgBlackWhite toPlay)
{
    SgBlackWhite defender = bd.GetColor(target);
    int nuLibs = bd.NumLiberties(target);
    if (nuLibs > 2)
        return false;

    // Get attack and defense point
    SgPoint attackPoint = SG_NULLMOVE;
    SgPoint defensePoint = SG_NULLMOVE;
    if (nuLibs == 1)
    {
        if (toPlay != defender)
            return true;
        SgPoint theLiberty = bd.TheLiberty(target);
        for (SgNb4Iterator it(theLiberty); it; ++it)
            if (bd.IsEmpty(*it))
            {
                if (attackPoint == SG_NULLMOVE)
                    attackPoint = *it;
                else
                {
                    SG_ASSERT(defensePoint == SG_NULLMOVE);
                    defensePoint = *it;
                }
            }
    }
    else
    {
        SG_ASSERT(nuLibs == 2);
        if (toPlay == defender)
            return false;
        GoBoard::LibertyIterator it(bd, target);
        defensePoint = *it;
        ++it;
        attackPoint = *it;
    }
    if (bd.Line(defensePoint) != 1)
    {
        if (bd.Line(attackPoint) != 1)
            return false;
        swap(defensePoint, attackPoint);
    }

    // Find direction to run ladder
    int col = SgPointUtil::Col(defensePoint);
    int delta;
    switch (defensePoint - attackPoint)
    {
    case SG_NS + SG_WE:
        delta = (col == bd.Size() ? SG_NS : SG_WE);
        break;
    case SG_NS - SG_WE:
        delta = (col == 1 ? SG_NS : -SG_WE);
        break;
    case -SG_NS + SG_WE:
        delta = (col == bd.Size() ? -SG_NS : SG_WE);
        break;
    case -SG_NS - SG_WE:
        delta = (col == 1 ? -SG_NS : -SG_WE);
        break;
    default:
        return false;
    }

    // @todo Check that no block in atari is adjacent to target block (?)

    // Compute ladder
    while (true)
    {
        SG_ASSERT(bd.IsEmpty(defensePoint));
        SG_ASSERT(bd.IsEmpty(attackPoint));
        int nuNeighborsDefender = bd.NumNeighbors(defensePoint, defender);
        if (nuNeighborsDefender > 1)
            return false;
        if (nuNeighborsDefender == 0
            && bd.NumEmptyNeighbors(defensePoint) < 3)
            return true;
        defensePoint += delta;
        attackPoint += delta;
        if (! bd.IsEmpty(defensePoint) || ! bd.IsEmpty(attackPoint))
            return false;
    }

    return true;
}

bool GoStaticLadder::IsLadder(const GoBoard& bd, SgPoint target,
                              SgBlackWhite toPlay)
{
    return IsEdgeLadder(bd, target, toPlay);
    // @todo Handle ladder with target not on edge
}

//----------------------------------------------------------------------------
