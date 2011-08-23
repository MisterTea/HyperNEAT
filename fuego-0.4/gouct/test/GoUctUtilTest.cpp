//----------------------------------------------------------------------------
/** @file GoUctUtilTest.cpp
    Unit tests for GoUctUtil.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoBoard.h"
#include "GoSetupUtil.h"
#include "GoUctUtil.h"

using namespace std;
using GoUctUtil::DoSelfAtariCorrection;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

/** Test GoUctUtil::DoSelfAtariCorrection (self-atari) */
BOOST_AUTO_TEST_CASE(GoUctUtilTest_DoSelfAtariCorrection_1)
{
    // 3 . . . .
    // 2 X X O .
    // 1 . O . .
    //   A B C D
    GoSetup setup;
    setup.AddBlack(Pt(1, 2));
    setup.AddBlack(Pt(2, 2));
    setup.AddWhite(Pt(2, 1));
    setup.AddWhite(Pt(3, 2));
    setup.m_player = SG_WHITE;
    GoBoard bd(19, setup);
    SgPoint p = Pt(1, 1);
    DoSelfAtariCorrection(bd, p);
    BOOST_CHECK_EQUAL(p, Pt(3, 1));
    DoSelfAtariCorrection(bd, p);
    BOOST_CHECK_EQUAL(p, Pt(3, 1));
    bd.SetToPlay(SG_BLACK);
    p = Pt(1, 1);
    DoSelfAtariCorrection(bd, p);
    BOOST_CHECK_EQUAL(p, Pt(1, 1));
}

/** Test GoUctUtil::DoSelfAtariCorrection (no self-atari; capture) */
BOOST_AUTO_TEST_CASE(GoUctUtilTest_DoSelfAtariCorrection_2)
{
    // 3 O O . .
    // 2 X X O .
    // 1 . O . .
    //   A B C D
    GoSetup setup;
    setup.AddBlack(Pt(1, 2));
    setup.AddBlack(Pt(2, 2));
    setup.AddWhite(Pt(1, 3));
    setup.AddWhite(Pt(2, 1));
    setup.AddWhite(Pt(2, 3));
    setup.AddWhite(Pt(3, 2));
    setup.m_player = SG_WHITE;
    GoBoard bd(19, setup);
    SgPoint p = Pt(1, 1);
    DoSelfAtariCorrection(bd, p);
    BOOST_CHECK_EQUAL(p, Pt(1, 1));
}

/** Test GoUctUtil::DoSelfAtariCorrection (single stone) */
BOOST_AUTO_TEST_CASE(GoUctUtilTest_DoSelfAtariCorrection_3)
{
    // 3 . .
    // 2 X .
    // 1 . .
    //   A B C D
    GoSetup setup;
    setup.AddBlack(Pt(1, 2));
    setup.m_player = SG_WHITE;
    GoBoard bd(19, setup);
    // single stone self-atari is replaced by adjacent empty point
    SgPoint p = Pt(1, 1);
    DoSelfAtariCorrection(bd, p);
    BOOST_CHECK_EQUAL(p, Pt(2, 1));
}

/** Test GoUctUtil::DoSelfAtariCorrection (single stone and capture) */
BOOST_AUTO_TEST_CASE(GoUctUtilTest_DoSelfAtariCorrection_4)
{
    std::string s("..X.\n"
                  "OX..\n"
                  "XOX.\n"
                  "....");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    GoBoard bd(boardSize, setup);
    // single stone capture is not replaced
    SgPoint p = Pt(1, 4);
    DoSelfAtariCorrection(bd, p);
    BOOST_CHECK_EQUAL(p, Pt(1, 4));
    p = Pt(1, 1); // extension replaced by capture
    DoSelfAtariCorrection(bd, p);
    BOOST_CHECK_EQUAL(p, Pt(2, 1));
}

/** Test GoUctUtil::DoSelfAtariCorrection (single stone and capture) */
BOOST_AUTO_TEST_CASE(GoUctUtilTest_DoSelfAtariCorrection_5)
{
    std::string s("XO..O.\n"
                  ".XOO..\n"
                  "......\n"
                  "......\n"
                  "......\n"
                  "......");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    GoBoard bd(boardSize, setup);
    // replace single stone selfatari by capture
    SgPoint p = Pt(4, 6);
    DoSelfAtariCorrection(bd, p);
    BOOST_CHECK_EQUAL(p, Pt(3, 6));
}

//----------------------------------------------------------------------------

/** Test that a move on a 4-4 point is generated on an empty 19x19 board. */
BOOST_AUTO_TEST_CASE(GoUctUtilTest_GenForcedOpeningMove_EmptyBoard)
{
    GoBoard bd(19);
    SgPoint p = GoUctUtil::GenForcedOpeningMove(bd);
    BOOST_CHECK(p == Pt(4, 4) || p == Pt(4, 16) || p == Pt(16, 4)
                || p == Pt(16, 16));
}

/** Test that no move is generated if no corner is empty. */
BOOST_AUTO_TEST_CASE(GoUctUtilTest_GenForcedOpeningMove_NoEmptyCorner_1)
{
    string s(". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . O . . . . . . . . . . . . X . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . X . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . O . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    GoBoard bd(boardSize, setup);
    BOOST_CHECK_EQUAL(GoUctUtil::GenForcedOpeningMove(bd), SG_NULLMOVE);
}

/** Test that no move is generated if no corner is empty. */
BOOST_AUTO_TEST_CASE(GoUctUtilTest_GenForcedOpeningMove_NoEmptyCorner_2)
{
    string s(". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . X . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . X . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . X . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . O . . . . . . . . . . . O . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_WHITE;
    GoBoard bd(boardSize, setup);
    BOOST_CHECK_EQUAL(GoUctUtil::GenForcedOpeningMove(bd), SG_NULLMOVE);
}

/** Test that a move on a 4-4 point is generated in only empty corner. */
BOOST_AUTO_TEST_CASE(GoUctUtilTest_GenForcedOpeningMove_OneEmptyCorner)
{
    string s(". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . X . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . O . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . X . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup. m_player = SG_WHITE;
    GoBoard bd(boardSize, setup);
    BOOST_CHECK_EQUAL(GoUctUtil::GenForcedOpeningMove(bd), Pt(4, 4));
}

/** Test that no moves are generated on a board smaller than size 13. */
BOOST_AUTO_TEST_CASE(GoUctUtilTest_GenForcedOpeningMove_SmallBoard)
{
    GoBoard bd(12);
    BOOST_CHECK_EQUAL(GoUctUtil::GenForcedOpeningMove(bd), SG_NULLMOVE);
}

/** Test that no moves are generated if more than 5 stones of a color are
    already on a board.
*/
BOOST_AUTO_TEST_CASE(GoUctUtilTest_GenForcedOpeningMove_SetupStones)
{
    string s(". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             ". . . . . . . . . . . . . . . . . . .\n"
             "X X X X . . . . . . . . . . . . . . .\n"
             ". . . X . . . . . . . . . . . . . . .\n"
             ". . . X . . . . . . . . . . . . . . .\n");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_WHITE;
    GoBoard bd(boardSize, setup);
    BOOST_CHECK_EQUAL(GoUctUtil::GenForcedOpeningMove(bd), SG_NULLMOVE);
}

//----------------------------------------------------------------------------

} // namespace
