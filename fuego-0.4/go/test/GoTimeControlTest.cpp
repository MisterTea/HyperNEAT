//----------------------------------------------------------------------------
/** @file GoTimeControlTest.cpp
    Unit tests for GoTimeControl.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include "GoBoard.h"
#include "GoTimeControl.h"
#include "SgDebug.h"
#include "SgTimeRecord.h"

using namespace std;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

/** Test that not the full remaining time is used, if close to the
    expected final number of stones on the board, but no overtime is used.
*/
BOOST_AUTO_TEST_CASE(GoTimeControlTest_AbsoluteTime)
{
    // Fill the board to > 70%, so that expected remaining number of moves
    // is close to zero
    GoSetup setup;
    for (int row = 1; row <= 19; row +=2)
        for (int col = 1; col <= 19; ++col)
            setup.AddBlack(Pt(row, col));
    for (int row = 2; row <= 18; row +=2)
        for (int col = 1; col <= 19; col += 3)
            setup.AddBlack(Pt(row, col));
    GoBoard bd(19, setup);

    GoTimeControl timeControl(bd);
    timeControl.SetFastOpenMoves(0);
    timeControl.SetFinalSpace(0.7);

    SgTimeRecord timeRecord;
    double timeLeft = 10;
    timeRecord.SetTimeLeft(SG_BLACK, timeLeft);
    timeRecord.SetOTNumMoves(0); // no overtime

    // Arbitrarily check that time for move < 0.5 * timeLeft. In reality
    // it should be even smaller, because there could still be many moves
    // to play
    double timeForMove = timeControl.TimeForCurrentMove(timeRecord,
                                                        /*quiet*/true);
    BOOST_CHECK_MESSAGE(timeForMove < 0.5 * timeLeft,
                        "timeForMove is " << timeForMove);
}

/** Test that the expected move time is returned for an empty board.
    See the time management algorithm documentation in GoTimeControl.
*/
BOOST_AUTO_TEST_CASE(GoTimeControlTest_EmptyBoard)
{
    GoBoard bd(19);
    GoTimeControl timeControl(bd);
    timeControl.SetFastOpenMoves(0);
    SgTimeRecord timeRecord;
    double timeLeft = 100;
    timeRecord.SetTimeLeft(SG_BLACK, timeLeft);
    timeRecord.SetOTNumMoves(0); // no overtime
    double timeForMove = timeControl.TimeForCurrentMove(timeRecord,
                                                        /*quiet*/true);
    // Expected time for move:
    // Final space parameter 0.75 (the documented default value)
    // expectedRemaining = (19 * 19 - 19 * 19 * (1 - 0.75)) / 2 = 135
    // timeForMove = 100 / 135 = 0.7407
    BOOST_CHECK_CLOSE(timeForMove, 0.7407, 1e-2);
}

//----------------------------------------------------------------------------

} // namespace
