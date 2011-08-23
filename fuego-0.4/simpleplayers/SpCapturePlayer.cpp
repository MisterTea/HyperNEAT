//----------------------------------------------------------------------------
/** @file SpCapturePlayer.cpp
    See SpCapturePlayer.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SpCapturePlayer.h"

//----------------------------------------------------------------------------

int SpCaptureMoveGenerator::Evaluate()
{
    int numstones = m_board.NuCapturedStones();
    if (numstones == 0)
        return INT_MIN;
    else
        return numstones;
}

//----------------------------------------------------------------------------

