//----------------------------------------------------------------------------
/** @file SpLadderPlayer.cpp
    See SpLadderPlayer.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SpLadderPlayer.h"

#include "GoLadder.h"
#include "GoMoveExecutor.h"
#include "SgConnCompIterator.h"
#include "SgEvaluatedMoves.h"

using GoLadderUtil::LadderStatus;

//----------------------------------------------------------------------------

int SpLadderMoveGenerator::Score(SgPoint p)
{
    SG_UNUSED(p);
    // LadderMoveGenerator uses direct move generation, 
    // it does not work by executing moves and calling Score().
    SG_ASSERT(false);
    return INT_MIN;
}

void SpLadderMoveGenerator::GenerateMoves(SgEvaluatedMoves& eval,
                                          SgBlackWhite toPlay)
{
    // Don't permit player to kill its own groups.
    GoRestoreToPlay restoreToPlay(m_board);
    m_board.SetToPlay(toPlay);
    GoRestoreSuicide restoreSuicide(m_board, false);
    for (SgBlackWhite color = SG_BLACK; color <= SG_WHITE; ++color)
    {
        for (SgConnCompIterator it(m_board.All(color), m_board.Size());
             it; ++it)
        {
            SgPointSet block(*it);
            SgPoint p = block.PointOf(), toCapture, toEscape; 
            GoLadderStatus st = LadderStatus(m_board, p, false, &toCapture,
                                             &toEscape);
            if (st == GO_LADDER_UNSETTLED)
            {
                SgPoint move =
                    color == m_board.ToPlay() ? toEscape : toCapture;
                int size = 1000 * block.Size();
                eval.AddMove(move, size);
                if ((color == m_board.ToPlay()) && (move == SG_PASS))
                {
                    // try liberties
                    for (GoBoard::LibertyIterator it(m_board, p); it; ++it)
                    {
                        SgPoint lib = *it;
                        GoMoveExecutor m(m_board, lib, color);
                        if (m.IsLegal() && m_board.Occupied(p))
                        {
                            SgPoint toCapture2, toEscape2; 
                            GoLadderStatus st2 =
                                LadderStatus(m_board, p, false, &toCapture2,
                                             &toEscape2);
                            if (st2 == GO_LADDER_ESCAPED)
                                eval.AddMove(lib, size);
                        }
                    }
                }
            }
        }
    }
}

//----------------------------------------------------------------------------

