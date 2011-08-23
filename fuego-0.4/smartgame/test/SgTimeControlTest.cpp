//----------------------------------------------------------------------------
/** @file SgTimeControlTest.cpp
    Unit tests for SgTimeControl.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgTimeControl.h"
#include "SgTimeRecord.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

/** Concrete subclass of SgDefaultTimeControl for testing purposes. */
class TimeControl
    : public SgDefaultTimeControl
{
public:
    void GetPositionInfo(SgBlackWhite& toPlay, int& movesPlayed,
                         int& estimatedRemainingMoves);

    /** Set values returned by the next call of GetPositionInfo() */
    void SetPositionInfo(SgBlackWhite toPlay, int movesPlayed,
                         int estimatedRemainingMoves);

private:
    SgBlackWhite m_toPlay;

    int m_movesPlayed;

    int m_estimatedRemainingMoves;
};

inline void TimeControl::GetPositionInfo(SgBlackWhite& toPlay,
                                         int& movesPlayed,
                                         int& estimatedRemainingMoves)
{
    toPlay = m_toPlay;
    movesPlayed = m_movesPlayed;
    estimatedRemainingMoves = m_estimatedRemainingMoves;
}

inline void TimeControl::SetPositionInfo(SgBlackWhite toPlay, int movesPlayed,
                                         int estimatedRemainingMoves)
{
    m_toPlay = toPlay;
    m_movesPlayed = movesPlayed;
    m_estimatedRemainingMoves = estimatedRemainingMoves;
}

//----------------------------------------------------------------------------

/** Test border cases in which bugs could make SgDefaultTimeControl return
    infinity.
*/
BOOST_AUTO_TEST_CASE(SgDefaultTimeControlTest_Infinity)
{
    TimeControl control;
    SgTimeRecord timeRecord;
    const double epsilon = 1.0;
    double timeMove;

    // Played moves and remaining moves is 0, in main time, no overtime
    timeRecord.SetTimeLeft(SG_BLACK, 100.0);
    timeRecord.SetMovesLeft(SG_BLACK, 0);
    control.SetPositionInfo(SG_BLACK, 0, 0);
    timeMove = control.TimeForCurrentMove(timeRecord, true/*quiet*/);
    //BOOST_CHECK(timeMove < 100.0 + epsilon);

    // Remaining moves is 0, in main time, with overtime
    timeRecord.SetTimeLeft(SG_BLACK, 100.0);
    timeRecord.SetMovesLeft(SG_BLACK, 0);
    timeRecord.SetOTPeriod(50.0);
    timeRecord.SetOTNumMoves(5);
    control.SetPositionInfo(SG_BLACK, 10, 0);
    timeMove = control.TimeForCurrentMove(timeRecord, true/*quiet*/);
    BOOST_CHECK(timeMove < 100.0 + epsilon);
}

//----------------------------------------------------------------------------

} // namespace
