//----------------------------------------------------------------------------
/** @file GoGameRecordTest.cpp
    Unit tests for GoGameRecord.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include "GoGame.h"

using namespace std;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

/** Test executing and undoing a node with setup stones. */
BOOST_AUTO_TEST_CASE(GoGameRecordTest_Setup)
{
    GoBoard bd;
    GoGameRecord game(bd);
    SgNode* node = game.CurrentNode()->NewRightMostSon();
    SgPropAddStone* addBlack = new SgPropAddStone(SG_PROP_ADD_BLACK);
    addBlack->PushBack(Pt(1, 1));
    SgPropAddStone* addWhite = new SgPropAddStone(SG_PROP_ADD_WHITE);
    addWhite->PushBack(Pt(2, 2));
    addWhite->PushBack(Pt(3, 3));
    node->Add(addBlack);
    node->Add(addWhite);
    game.GoToNode(node);
    BOOST_CHECK_EQUAL(SG_BLACK, bd.GetColor(Pt(1, 1)));
    BOOST_CHECK_EQUAL(SG_WHITE, bd.GetColor(Pt(2, 2)));
    BOOST_CHECK_EQUAL(SG_WHITE, bd.GetColor(Pt(3, 3)));
    game.GoToNode(node->Father());
    BOOST_CHECK_EQUAL(SG_EMPTY, bd.GetColor(Pt(1, 1)));
    BOOST_CHECK_EQUAL(SG_EMPTY, bd.GetColor(Pt(2, 2)));
    BOOST_CHECK_EQUAL(SG_EMPTY, bd.GetColor(Pt(3, 3)));
}

//----------------------------------------------------------------------------

} // namespace
