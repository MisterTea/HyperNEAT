//----------------------------------------------------------------------------
/** @file SpSafePlayer.cpp
    See SpSafePlayer.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SpSafePlayer.h"

#include "GoSafetySolver.h"

//----------------------------------------------------------------------------

int SpSafeMoveGenerator::Evaluate()
{   
    SgBWSet safe;
    GoSafetySolver s(m_board);
    s.FindSafePoints(&safe);
    
    // We are Opponent since this is after executing our move
    SgBlackWhite player = m_board.Opponent();
    return safe[player].Size() - safe[SgOppBW(player)].Size();
}


