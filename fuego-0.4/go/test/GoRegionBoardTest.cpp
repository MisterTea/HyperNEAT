//----------------------------------------------------------------------------
/** @file GoRegionBoardTest.cpp
    Unit tests for GoRegionBoard.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoBoard.h"
#include "GoRegionBoard.h"
#include "SgNode.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(GoRegionBoardTest_Setup)
{
    GoSetup setup;
    setup.AddBlack(Pt(4, 5));
    setup.AddBlack(Pt(5, 4));
    setup.AddWhite(Pt(1, 2));
    setup.AddWhite(Pt(2, 2));
    GoBoard bd(5, setup);
    GoRegionBoard r(bd);
    r.GenBlocksRegions();
    BOOST_CHECK_EQUAL(r.AllBlocks(SG_BLACK).Length(), 2);
    BOOST_CHECK_EQUAL(r.AllBlocks(SG_WHITE).Length(), 1);
    BOOST_CHECK_EQUAL(r.AllRegions(SG_BLACK).Length(), 2);
    BOOST_CHECK_EQUAL(r.AllRegions(SG_WHITE).Length(), 1);
}

} // namespace

