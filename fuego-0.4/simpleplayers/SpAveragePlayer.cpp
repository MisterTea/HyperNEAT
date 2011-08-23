//----------------------------------------------------------------------------
/** @file SpAveragePlayer.cpp
    See SpAveragePlayer.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SpAveragePlayer.h"

#include "SgConnCompIterator.h"

//----------------------------------------------------------------------------

int LibertyAveragex10(const GoBoard& board, SgBlackWhite color)
{
    int nuLibs = 0, nuBlocks = 0;
    const int size = board.Size();
    for (SgConnCompIterator it(board.All(color), board.Size()); it; ++it)
    {
        ++nuBlocks;
        nuLibs += ((*it).Border(size) & board.AllEmpty()).Size();
    }
    return (nuBlocks == 0) ? 0 : 10 * nuLibs / nuBlocks;
}
//----------------------------------------------------------------------------

int SpAverageMoveGenerator::Evaluate()
{
    // We are Opponent since this is after executing our move
    SgBlackWhite player = m_board.Opponent();
    int score = LibertyAveragex10(m_board, player)
              - LibertyAveragex10(m_board, SgOppBW(player));
        
    return score;
}

