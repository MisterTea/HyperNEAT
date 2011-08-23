//----------------------------------------------------------------------------
/** @file GoGameRecordTest.cpp
    Unit tests for GoGameRecord.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoGame.h"
#include "GoPlayer.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

/** Test player to test that GoGame calls GoPlayer::UpdateSubscriber */
class TestPlayer
    : public GoPlayer
{
public:
    int m_nuBoardChangeCalls;

    TestPlayer(GoBoard& bd);

    SgMove GenMove(const SgTimeRecord& time, SgBlackWhite toPlay);

protected:
    /** Implementation of GoBoardSynchronizer::OnBoardChange */
    void OnBoardChange();
};

TestPlayer::TestPlayer(GoBoard& bd)
    : GoPlayer(bd)
{
    m_nuBoardChangeCalls = 0;
}

SgMove TestPlayer::GenMove(const SgTimeRecord& time, SgBlackWhite toPlay)
{
    SG_UNUSED(time);
    SG_UNUSED(toPlay);
    return SG_NULLPOINT;
}

void TestPlayer::OnBoardChange()
{
    ++m_nuBoardChangeCalls;
}

//----------------------------------------------------------------------------

/** Test that GoGame::Init updates the players. */
BOOST_AUTO_TEST_CASE(GoGameTest_UpdatePlayers)
{
    GoBoard bd(19);
    GoGame game(bd);
    TestPlayer* player = new TestPlayer(bd);
    BOOST_CHECK_EQUAL(player->m_nuBoardChangeCalls, 0);
    game.SetPlayer(SG_BLACK, player);
    BOOST_CHECK_EQUAL(player->m_nuBoardChangeCalls, 0); // no boardsize change
    game.Init(9, GoRules());
    BOOST_CHECK_EQUAL(player->m_nuBoardChangeCalls, 1);
}

//----------------------------------------------------------------------------

} // namespace
