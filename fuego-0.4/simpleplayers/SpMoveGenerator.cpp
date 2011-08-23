//----------------------------------------------------------------------------
/** @file SpMoveGenerator.cpp
    See SpMoveGenerator.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SpMoveGenerator.h"

#include <limits>
#include "GoMoveExecutor.h"
#include "SgEvaluatedMoves.h"

using namespace std;

//----------------------------------------------------------------------------

void SpMoveGenerator::GenerateMoves(SgEvaluatedMoves& eval,
                                    SgBlackWhite toPlay)
{
    GoRestoreToPlay restoreToPlay(m_board);
    m_board.SetToPlay(toPlay);
    GoRestoreSuicide restoreSuicide(m_board, false);
    for (SgSetIterator it(eval.Relevant()); it; ++it)
    {
        SgPoint p(*it);
        int score = EvaluateMove(p);
        if (score > numeric_limits<int>::min())
            eval.AddMove(p, score);
    }
}

int Sp1PlyMoveGenerator::EvaluateMove(SgPoint p)
{
    GoMoveExecutor execute(m_board, p);
    if (execute.IsLegal())
        return Evaluate();
    else
        return numeric_limits<int>::min();
}

int SpStaticMoveGenerator::EvaluateMove(SgPoint p)
{
    if (m_board.IsLegal(p))
        return Score(p);
    else
        return numeric_limits<int>::min();
}


//----------------------------------------------------------------------------

