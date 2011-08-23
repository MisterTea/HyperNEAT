//----------------------------------------------------------------------------
/** @file GoRegionTest.cpp
    Unit tests for GoRegion.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoBoard.h"
#include "GoRegion.h"
#include "GoRegionBoard.h"
#include "GoSetupUtil.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(GoRegionTest_IsInteriorBlock)
{
    std::string s(
    ". X . . . . . . . \n"
    "X X . . . . . . . \n"
    ". X X X X X X X . \n"
    "X X X O O O O X X \n"
    "O O O . . . X O X \n"
    "O . O O O . O O X \n"
    ". O O . . O O . O \n"
    "O O . X X O . O O \n"
    ". X X X X O O O . \n"
    );
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    GoBoard bd(boardSize, setup);
    GoRegionBoard rbd(bd);
    rbd.GenBlocksRegions();
    BOOST_CHECK_EQUAL(rbd.AllBlocks(SG_BLACK).Length(), 3);
    BOOST_CHECK_EQUAL(rbd.AllBlocks(SG_WHITE).Length(), 3);
    BOOST_CHECK_EQUAL(rbd.AllRegions(SG_BLACK).Length(), 4);
    BOOST_CHECK_EQUAL(rbd.AllRegions(SG_WHITE).Length(), 8);
    
    GoRegion* r = rbd.RegionAt(Pt(5, 5), SG_WHITE);
    GoBlock* b = rbd.BlockAt(Pt(5, 6));
    BOOST_CHECK_EQUAL(r->IsInteriorBlock(b), false);
    b = rbd.BlockAt(Pt(1, 5));
    BOOST_CHECK_EQUAL(r->IsInteriorBlock(b), false);
    b = rbd.BlockAt(Pt(8, 5));
    BOOST_CHECK_EQUAL(r->IsInteriorBlock(b), false);
    
    r = rbd.RegionAt(Pt(1, 1), SG_BLACK);
    b = rbd.BlockAt(Pt(2, 1));
    BOOST_CHECK_EQUAL(r->IsInteriorBlock(b), true);
    b = rbd.BlockAt(Pt(1, 6));
    BOOST_CHECK_EQUAL(r->IsInteriorBlock(b), false);
}

} // namespace

