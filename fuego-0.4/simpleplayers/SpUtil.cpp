//----------------------------------------------------------------------------
/** @file SpUtil.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SpUtil.h"

#include "GoBoard.h"
#include "GoSafetySolver.h"

using namespace std;

//----------------------------------------------------------------------------

SgPointSet SpUtil::GetRelevantMoves(GoBoard& bd, SgBlackWhite toPlay,
                                    bool useFilter)
{
    SgPointSet legal;
    for (SgSetIterator it(bd.AllEmpty()); it; ++it)
        if (bd.IsLegal(*it))
            legal.Include(*it);
    if (! useFilter)
        return legal;
    GoSafetySolver safetySolver(bd);
    SgBWSet safe;
    safetySolver.FindSafePoints(&safe);
    SgBlackWhite opp = SgOppBW(toPlay);
    SgPointSet moves;
    const GoRules& rules = bd.Rules();
    const bool captureDead = rules.CaptureDead();
    //const bool japaneseScoring = rules.JapaneseScoring();
    for (SgSetIterator it(legal); it; ++it)
    {
        SgPoint p = *it;
        const bool isSafe = safe[toPlay].Contains(p);
        const bool isSafeOpp = safe[opp].Contains(p);
        const bool hasOppNeighbors = bd.HasNeighbors(p, opp);
        if (  (! isSafe && ! isSafeOpp)
           || (isSafe && captureDead && hasOppNeighbors)
           // || (isSafeOpp && ! japaneseScoring)
           )
            moves.Include(p);
    }
    return moves;
}

//----------------------------------------------------------------------------

