//----------------------------------------------------------------------------
/** @file SpMinLibPlayer.cpp
    See SpMinLibPlayer.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SpMinLibPlayer.h"

#include "SpAveragePlayer.h"
#include "SgConnCompIterator.h"

//----------------------------------------------------------------------------

/** Value corresponding to minimum liberties for EITHER color */
int SpMinLibMoveGenerator::LibertyMinimum(SgBlackWhite toplay)
{
    int minlibs = INT_MAX;
    float sign = 1;
    SgBlackWhite opponent = SgOpp(toplay);
    const int size = m_board.Size();        
    for (SgConnCompIterator it(m_board.All(toplay), m_board.Size()); it; ++it)
    {
        // Subtract a little from liberties, 
        // breaking ties to save own stones rather than attacking
        int libs = ((*it).Border(size) & m_board.AllEmpty()).Size() * 2 - 1;
        if (libs < minlibs)
        {
            minlibs = libs;
            sign = -1; // If we have minimum liberties that's bad
        }
    }

    for (SgConnCompIterator it(m_board.All(opponent), m_board.Size());
         it; ++it)
    {
        int libs = ((*it).Border(size) & m_board.AllEmpty()).Size() * 2;
        if (libs < minlibs)
        {
            minlibs = libs;
            sign = 1; // If opponent has minimum liberties that's good
        }
    }
        
    // Evaluation is inversely proportional to minlibs value
    // with appropriate sign
    SG_ASSERT(minlibs != 0);
    float eval = 100 * sign / minlibs; // in range [-100, 100]
    return static_cast<int>(eval);
}

int SpMinLibMoveGenerator::Evaluate()
{    
    // We are Opponent since this is after executing our move
    SgBlackWhite player = m_board.Opponent();
    int minlibscore = LibertyMinimum(player);
 
    int avlibscore = 
          ::LibertyAveragex10(m_board, player)
        - ::LibertyAveragex10(m_board, SgOppBW(player));

    // Select moves in priority order according to:
    // a) Minimum liberties (for either player)
    // b) Average liberties x 10
    
    // Note that the 1/x scaling of minlibs means that avlibs can matter more
    // when |minlibs| is very high. Which is probably a good thing!
    int score = minlibscore * 100 + avlibscore;
    // roughly in range [-10000,10000]
    return score;
}

//----------------------------------------------------------------------------

