//----------------------------------------------------------------------------
/** @file GoTimeControl.cpp
    See GoTimeControl.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoTimeControl.h"

#include "GoBoard.h"

using namespace std;

//----------------------------------------------------------------------------

GoTimeControl::GoTimeControl(const GoBoard& bd)
    : m_bd(bd),
      m_finalSpace(0.75f)
{
}

float GoTimeControl::FinalSpace() const
{
    return m_finalSpace;
}

void GoTimeControl::GetPositionInfo(SgBlackWhite& toPlay,
                                    int& movesPlayed,
                                    int& estimatedRemainingMoves)
{
    toPlay = m_bd.ToPlay();
    movesPlayed = m_bd.Occupied().Size() / 2;
    int finalNumEmpty =
        static_cast<int>(m_bd.AllPoints().Size() * (1 - m_finalSpace));
    estimatedRemainingMoves = max(m_bd.TotalNumEmpty() - finalNumEmpty, 0);
    estimatedRemainingMoves /= 2;
}

void GoTimeControl::SetFinalSpace(float finalspace)
{
    // Should be in [0:1]
    SG_ASSERT(finalspace > -0.1);
    SG_ASSERT(finalspace < 1.1);
    m_finalSpace = finalspace;
}

//----------------------------------------------------------------------------
