//----------------------------------------------------------------------------
/** @file GoBoardSynchronizerTest.cpp
    Unit tests for GoBoardSynchronizer.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoBoard.h"
#include "GoBoardSynchronizer.h"
#include "SgWrite.h"

using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(GoBoardSynchronizerTest_Update)
{
    GoBoard publisher;
    GoBoard subscriber;
    GoBoardSynchronizer synchronizer(publisher);
    synchronizer.SetSubscriber(subscriber);
    publisher.Play(Pt(1, 1), SG_BLACK);
    publisher.Play(Pt(2, 2), SG_WHITE);
    synchronizer.UpdateSubscriber();
    BOOST_CHECK_EQUAL(subscriber.MoveNumber(), 2);
    BOOST_CHECK_EQUAL(subscriber.Move(0), GoPlayerMove(SG_BLACK, Pt(1, 1)));
    BOOST_CHECK_EQUAL(subscriber.Move(1), GoPlayerMove(SG_WHITE, Pt(2, 2)));
}

/** Test GoBoardSynchronizer::Update, if setup changed. */
BOOST_AUTO_TEST_CASE(GoBoardSynchronizerTest_Update_Setup)
{
    GoBoard publisher(19);
    GoBoard subscriber(19);
    GoBoardSynchronizer synchronizer(publisher);
    synchronizer.SetSubscriber(subscriber);
    publisher.Play(Pt(1, 1), SG_BLACK);
    synchronizer.UpdateSubscriber();
    GoSetup setup;
    setup.AddBlack(Pt(2, 2));
    publisher.Init(19, setup);
    publisher.Play(Pt(1, 1), SG_BLACK);
    synchronizer.UpdateSubscriber();
    BOOST_CHECK_EQUAL(subscriber.MoveNumber(), 1);
    BOOST_CHECK_EQUAL(subscriber.Move(0), GoPlayerMove(SG_BLACK, Pt(1, 1)));
    BOOST_CHECK_EQUAL(subscriber.GetColor(Pt(2, 2)), SG_BLACK);
}

BOOST_AUTO_TEST_CASE(GoBoardSynchronizerTest_Update_Init)
{
    GoBoard publisher(19);
    GoBoard subscriber(19);
    GoBoardSynchronizer synchronizer(publisher);
    synchronizer.SetSubscriber(subscriber);
    publisher.Play(Pt(1, 1), SG_BLACK);
    publisher.Init(9);
    synchronizer.UpdateSubscriber();
    BOOST_CHECK_EQUAL(subscriber.MoveNumber(), 0);
    BOOST_CHECK_EQUAL(subscriber.Size(), 9);
}

BOOST_AUTO_TEST_CASE(GoBoardSynchronizerTest_Update_ToPlay)
{
    GoBoard publisher;
    GoBoard subscriber;
    GoBoardSynchronizer synchronizer(publisher);
    synchronizer.SetSubscriber(subscriber);
    publisher.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK_EQUAL(publisher.ToPlay(), SG_WHITE);
    publisher.SetToPlay(SG_BLACK);
    BOOST_CHECK_EQUAL(publisher.ToPlay(), SG_BLACK);
    synchronizer.UpdateSubscriber();
    BOOST_CHECK_EQUAL(subscriber.MoveNumber(), 1);
    BOOST_CHECK_EQUAL(subscriber.Move(0), GoPlayerMove(SG_BLACK, Pt(1, 1)));
    BOOST_CHECK_EQUAL(subscriber.ToPlay(), SG_BLACK);
}

BOOST_AUTO_TEST_CASE(GoBoardSynchronizerTest_Update_Undo)
{
    GoBoard publisher;
    GoBoard subscriber;
    GoBoardSynchronizer synchronizer(publisher);
    synchronizer.SetSubscriber(subscriber);
    publisher.Play(Pt(1, 1), SG_BLACK);
    publisher.Play(Pt(2, 2), SG_WHITE);
    publisher.Undo();
    publisher.Play(Pt(3, 3), SG_WHITE);
    publisher.Play(Pt(4, 4), SG_WHITE);
    synchronizer.UpdateSubscriber();
    BOOST_CHECK_EQUAL(subscriber.MoveNumber(), 3);
    BOOST_CHECK_EQUAL(subscriber.Move(0), GoPlayerMove(SG_BLACK, Pt(1, 1)));
    BOOST_CHECK_EQUAL(subscriber.Move(1), GoPlayerMove(SG_WHITE, Pt(3, 3)));
    BOOST_CHECK_EQUAL(subscriber.Move(2), GoPlayerMove(SG_WHITE, Pt(4, 4)));
}

} // namespace

//----------------------------------------------------------------------------

