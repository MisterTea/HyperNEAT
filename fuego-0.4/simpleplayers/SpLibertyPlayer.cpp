//----------------------------------------------------------------------------
/** @file SpLibertyPlayer.cpp
    See SpLibertyPlayer.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SpLibertyPlayer.h"

#include "GoBoardUtil.h"
#include "SgConnCompIterator.h"

using GoBoardUtil::ExpandToBlocks;
using GoBoardUtil::MoveLegalAndNotAtari;

//----------------------------------------------------------------------------

int SpLibertyMoveGenerator::Score(SgPoint p)
// high score for playing liberties of weak blocks
// AR may be suicidal.
{
    SgPointSet nb;
    const int size = m_board.Size();
    nb.Include(p);
    nb = nb.Border(size) & m_board.Occupied();
    ExpandToBlocks(m_board, nb);

    int score(INT_MIN);

    if (MoveLegalAndNotAtari(m_board, p))
    {
        score = m_board.NumEmptyNeighbors(p);
        for (SgConnCompIterator it(nb, m_board.Size()); it; ++it)
        { 
            int nuLibs = ((*it).Border(size) & m_board.AllEmpty()).Size();
            if (nuLibs == 1)
                score += 20;
            else if (nuLibs == 2)
                score += 10;
            else if (nuLibs == 3)
                score += 5;
            else if (nuLibs == 4)
                score += 3;
            else
                ++score;
        }
    }
    return score;
}

/** counts surplus liberties of all blocks, those above 2.
    penalty for less than 2 liberties.
    @todo make threshold of 2 variable, experiment
*/
int LibertyMinus2(const GoBoard& board, SgBlackWhite color)
{
    int nuLibs = 0;
    const int size = board.Size();
    for (SgConnCompIterator it(board.All(color), board.Size()); it; ++it)
    {
        nuLibs += ((*it).Border(size) & board.AllEmpty()).Size() - 2;
    }
    return nuLibs;
}

