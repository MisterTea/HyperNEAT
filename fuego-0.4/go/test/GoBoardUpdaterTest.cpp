//----------------------------------------------------------------------------
/** @file GoBoardUpdaterTest.cpp
    Unit tests for GoBoardUpdater.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoBoard.h"
#include "GoBoardUpdater.h"
#include "SgNode.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(GoBoardUpdaterTest_Play)
{
    SgNode* root = new SgNode();
    SgNode* child = new SgNode();
    child->AddMoveProp(Pt(1, 1), SG_BLACK);
    child->AppendTo(root);
    GoBoard bd(19);
    GoBoardUpdater updater;
    updater.Update(child, bd);
    root->DeleteTree();
    BOOST_CHECK_EQUAL(SG_BLACK, bd.GetColor(Pt(1, 1)));
}

BOOST_AUTO_TEST_CASE(GoBoardUpdaterTest_Setup)
{
    SgNode* root = new SgNode();
    SgPropAddStone* addBlack = new SgPropAddStone(SG_PROP_ADD_BLACK);
    SgPropAddStone* addWhite = new SgPropAddStone(SG_PROP_ADD_WHITE);
    addBlack->PushBack(Pt(1, 1));
    addBlack->PushBack(Pt(1, 2));
    addBlack->PushBack(Pt(1, 3));
    addBlack->PushBack(Pt(1, 4));
    addBlack->PushBack(Pt(1, 5));
    root->Add(addBlack);
    root->Add(addWhite);
    SgNode* child = new SgNode();
    addBlack = new SgPropAddStone(SG_PROP_ADD_BLACK);
    addWhite = new SgPropAddStone(SG_PROP_ADD_WHITE);
    addWhite->PushBack(Pt(2, 1));
    addWhite->PushBack(Pt(2, 2));
    addWhite->PushBack(Pt(2, 3));
    addWhite->PushBack(Pt(2, 4));
    addWhite->PushBack(Pt(2, 5));
    addBlack->PushBack(Pt(3, 1));
    child->Add(addBlack);
    child->Add(addWhite);
    child->AppendTo(root);
    GoBoard bd(19);
    GoBoardUpdater updater;
    updater.Update(child, bd);
    root->DeleteTree();
    BOOST_CHECK_EQUAL(SG_BLACK, bd.GetColor(Pt(1, 1)));
    BOOST_CHECK_EQUAL(SG_BLACK, bd.GetColor(Pt(1, 2)));
    BOOST_CHECK_EQUAL(SG_BLACK, bd.GetColor(Pt(1, 3)));
    BOOST_CHECK_EQUAL(SG_BLACK, bd.GetColor(Pt(1, 4)));
    BOOST_CHECK_EQUAL(SG_BLACK, bd.GetColor(Pt(1, 5)));
    BOOST_CHECK_EQUAL(SG_WHITE, bd.GetColor(Pt(2, 1)));
    BOOST_CHECK_EQUAL(SG_WHITE, bd.GetColor(Pt(2, 2)));
    BOOST_CHECK_EQUAL(SG_WHITE, bd.GetColor(Pt(2, 3)));
    BOOST_CHECK_EQUAL(SG_WHITE, bd.GetColor(Pt(2, 4)));
    BOOST_CHECK_EQUAL(SG_WHITE, bd.GetColor(Pt(2, 5)));
    BOOST_CHECK_EQUAL(SG_BLACK, bd.GetColor(Pt(3, 1)));
}

/** Test that setting a stone on an point occupied by the second move
    in a game works.
    Checks for a bug that was only triggered, if the changed stone color
    was the second move played, not the first move,
*/
BOOST_AUTO_TEST_CASE(GoBoardUpdaterTest_SetupOnOccupiedMoveTwo)
{
    SgNode* root = new SgNode();
    SgNode* node1 = new SgNode();
    node1->AppendTo(root);
    node1->AddMoveProp(Pt(4, 4), SG_BLACK);
    SgNode* node2 = new SgNode();
    node2->AppendTo(node1);
    node2->AddMoveProp(Pt(3, 3), SG_WHITE);
    SgNode* node3 = new SgNode();
    node3->AppendTo(node2);
    SgPropAddStone* addBlack = new SgPropAddStone(SG_PROP_ADD_BLACK);
    addBlack->PushBack(Pt(3, 3));
    node3->Add(addBlack);
    GoBoard bd(19);
    GoBoardUpdater updater;
    updater.Update(node3, bd);
    root->DeleteTree();
    BOOST_CHECK_EQUAL(SG_BLACK, bd.GetColor(Pt(3, 3)));
}

} // namespace

//----------------------------------------------------------------------------
