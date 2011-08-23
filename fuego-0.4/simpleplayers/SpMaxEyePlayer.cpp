//----------------------------------------------------------------------------
/** @file SpMaxEyePlayer.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SpMaxEyePlayer.h"

#include "GoEyeUtil.h"

//----------------------------------------------------------------------------

using namespace std;

//----------------------------------------------------------------------------

float SpMaxEyeMoveGenerator::Heuristic(SgPoint p, SgBlackWhite colour)
{
    // Score is inversely proportional to number of moves to make eye
    int nummoves;
    bool eyepossible = 
        GoEyeUtil::NumberOfMoveToEye2(m_board, colour, p, nummoves);

    // Impossible eyes are not urgent
    if (! eyepossible)
        return 0.0f;

    // Otherwise the fewer moves to completion, the more urgent to play
    if (colour == m_board.Opponent())
        return 1.0f / (static_cast<float>(nummoves) + 1.0f);

    // Opponent is to play, so urgency is half a move greater
    else
        return 1.0f / (static_cast<float>(nummoves) + 0.5f);
}

int SpMaxEyeMoveGenerator::Evaluate()
{
    // We are Opponent since this is after executing our move
    SgBlackWhite player = m_board.Opponent();
    SgBlackWhite opponent = m_board.ToPlay();
 
    float sign = 0;
    float maxeyescore = -1;
    float totaleyescore = 0;
    
    for (GoBoard::Iterator iBoard(m_board); iBoard; ++iBoard)
    {
        float playerscore = Heuristic(*iBoard, player);
        float oppscore = Heuristic(*iBoard, opponent);

        totaleyescore += playerscore - oppscore;

        if (playerscore > maxeyescore)
        {
            maxeyescore = playerscore;
            sign = 1;
        }
            
        if (oppscore > maxeyescore)
        {
            maxeyescore = oppscore;
            sign = -1;
        }
    }

    // Find the point with the maximum eye score
    // Tie-break using the total eye score
    if (m_eyeGo)
        return static_cast<int>(
            maxeyescore * sign * 10000.0f + totaleyescore * 100.0f);

    // Play to score the most eye-ishly overall
    else
        return static_cast<int>(
            totaleyescore * 100.0f);
}

//----------------------------------------------------------------------------

