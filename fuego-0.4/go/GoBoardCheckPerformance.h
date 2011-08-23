//----------------------------------------------------------------------------
/** @file GoBoardCheckPerformance.h
    Check performance of the GoBoard class.

    @todo check performance on different kinds of positions.
    check execute/undo move, full board repetition, etc.
*/
//----------------------------------------------------------------------------

#ifndef GO_BOARDCHECKPERFORMANCE_H
#define GO_BOARDCHECKPERFORMANCE_H

#include <iosfwd>
#include "GoBoard.h"

//----------------------------------------------------------------------------

namespace GoBoardCheckPerformance
{

/** Performance check of class GoBoard.
    Looping through all board points is fastest using board iterator for small
    boards, and only slightly slower than if skipping dependency check for
    19*19.

    @verbatim
    19*19
    -----
    Time1: 570 For 0..SG_MAXPOINT
    Time2: 521 First/LastBoardPoint
    Time3: 210 GoBoard::Iterator
    Time4: 170 For 0..SG_MAXPOINT, no dependency
    Time5: 150 First/LastBoardPoint, no dependency

    13*13
    -----
    Time1: 561 For 0..SG_MAXPOINT
    Time2: 341 First/LastBoardPoint
    Time3: 90 GoBoard::Iterator
    Time4: 160 For 0..SG_MAXPOINT, no dependency
    Time5: 100 First/LastBoardPoint, no dependency

    9*9
    ---
    Time1: 580 For 0..SG_MAXPOINT
    Time2: 230 First/LastBoardPoint
    Time3: 50 GoBoard::Iterator
    Time4: 150 For 0..SG_MAXPOINT, no dependency
    Time5: 60 First/LastBoardPoint, no dependency
    @endverbatim
*/
void CheckPerformance(const GoBoard& board, std::ostream& out);

} // namespace GoBoardCheckPerformance

//----------------------------------------------------------------------------

#endif // GO_BOARDCHECKPERFORMANCE_H
