//----------------------------------------------------------------------------
/** @file SpGreedyPlayer.cpp
    See SpGreedyPlayer.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SpGreedyPlayer.h"

#include "GoInfluence.h"
#include "SpLibertyPlayer.h"

//----------------------------------------------------------------------------

int SpGreedyMoveGenerator::Evaluate()
{
    // We are Opponent since this is after executing our move
    SgBlackWhite player = m_board.Opponent();
    int score =   LibertyMinus2(m_board, player)
                - LibertyMinus2(m_board, SgOppBW(player))
                + GoInfluence::Influence(m_board, player, 7, 3);
    return score;
}


