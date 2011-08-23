//----------------------------------------------------------------------------
/** @file GoSetupUtilTest.cpp
    Unit tests for GoSetupUtil.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <string>
#include <sstream>
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include "GoBoard.h"
#include "GoSetup.h"
#include "GoSetupUtil.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

/** Test empty input text. */
BOOST_AUTO_TEST_CASE(GoSetupUtilTest_EmptyText)
{
    std::stringbuf in("");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromStream(in, boardSize);
    BOOST_CHECK(setup.m_stones.BothEmpty());
    BOOST_CHECK_EQUAL(boardSize, 0);
}

/** Test empty 1x1 board. */
BOOST_AUTO_TEST_CASE(GoSetupUtilTest_EmptyBoard)
{
    std::stringbuf in(".");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromStream(in, boardSize);
    BOOST_CHECK(setup.m_stones.BothEmpty());
    BOOST_CHECK_EQUAL(boardSize, 1);
}


/** Test empty 3x3 board. */
BOOST_AUTO_TEST_CASE(GoSetupUtilTest_EmptyBoard_2)
{
    std::stringbuf in("...\n...\n...");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromStream(in, boardSize);
    BOOST_CHECK(setup.m_stones.BothEmpty());
    BOOST_CHECK_EQUAL(boardSize, 3);
}

/** Test occupied 3x3 board. */
BOOST_AUTO_TEST_CASE(GoSetupUtilTest_OccupiedBoard_1)
{
    std::stringbuf in("XXX\n...\n.OO");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromStream(in, boardSize);
 
    BOOST_CHECK_EQUAL(setup.m_stones[SG_BLACK].Size(), 3);
    BOOST_CHECK_EQUAL(setup.m_stones[SG_WHITE].Size(), 2);
    BOOST_CHECK_EQUAL(boardSize, 3);
    BOOST_CHECK(setup.m_stones[SG_BLACK].Contains(Pt(1, 3)));
    BOOST_CHECK(setup.m_stones[SG_BLACK].Contains(Pt(2, 3)));
    BOOST_CHECK(setup.m_stones[SG_BLACK].Contains(Pt(3, 3)));
    BOOST_CHECK(setup.m_stones[SG_WHITE].Contains(Pt(2, 1)));
    BOOST_CHECK(setup.m_stones[SG_WHITE].Contains(Pt(3, 1)));
}

/** Test occupied 3x3 board. */
BOOST_AUTO_TEST_CASE(GoSetupUtilTest_OccupiedBoard_2)
{
    std::stringbuf in("XO.\n...\n...");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromStream(in, boardSize);
    BOOST_CHECK_EQUAL(setup.m_stones[SG_BLACK].Size(), 1);
    BOOST_CHECK_EQUAL(setup.m_stones[SG_WHITE].Size(), 1);
    BOOST_CHECK_EQUAL(boardSize, 3);
    BOOST_CHECK(setup.m_stones[SG_BLACK].Contains(Pt(1, 3)));
    BOOST_CHECK(setup.m_stones[SG_WHITE].Contains(Pt(2, 3)));
}

/** Test using setup for GoBoard. */
BOOST_AUTO_TEST_CASE(GoSetupUtilTest_GoBoard)
{
    std::stringbuf in("XO.\n"
                      ".X.\n"
                      "...");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromStream(in, boardSize);
    GoBoard bd(boardSize, setup);
    BOOST_CHECK_EQUAL(bd.Size(), boardSize);
    BOOST_CHECK_EQUAL(bd.All(SG_BLACK), setup.m_stones[SG_BLACK]);
    BOOST_CHECK_EQUAL(bd.All(SG_WHITE), setup.m_stones[SG_WHITE]);
    BOOST_CHECK_EQUAL(bd.GetColor(Pt(1, 3)), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.GetColor(Pt(2, 3)), SG_WHITE);
    BOOST_CHECK_EQUAL(bd.GetColor(Pt(3, 3)), SG_EMPTY);
    BOOST_CHECK_EQUAL(bd.TotalNumStones(SG_BLACK), 2);
    BOOST_CHECK_EQUAL(bd.TotalNumStones(SG_WHITE), 1);
}

/** Test for CreateSetupFromString. */
BOOST_AUTO_TEST_CASE(GoSetupUtilTest_CreateSetupFromString)
{
    std::string s("XO.\n"
                  ".X.\n"
                  "...");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    GoBoard bd(boardSize, setup);
    BOOST_CHECK_EQUAL(bd.Size(), boardSize);
    BOOST_CHECK_EQUAL(bd.All(SG_BLACK), setup.m_stones[SG_BLACK]);
    BOOST_CHECK_EQUAL(bd.All(SG_WHITE), setup.m_stones[SG_WHITE]);
    BOOST_CHECK_EQUAL(bd.GetColor(Pt(1, 3)), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.GetColor(Pt(2, 3)), SG_WHITE);
    BOOST_CHECK_EQUAL(bd.GetColor(Pt(3, 3)), SG_EMPTY);
    BOOST_CHECK_EQUAL(bd.TotalNumStones(SG_BLACK), 2);
    BOOST_CHECK_EQUAL(bd.TotalNumStones(SG_WHITE), 1);
}
//----------------------------------------------------------------------------

} // namespace
