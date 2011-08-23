//----------------------------------------------------------------------------
/** @file SgBoardConstTest.cpp
    Unit tests for SgBoardConst.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <sstream>
#include <boost/test/auto_unit_test.hpp>
#include "SgBoardConst.h"

using namespace std;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgBoardConstTest_Line)
{
    SgBoardConst boardConst(8);
    BOOST_CHECK_EQUAL(boardConst.Line(Pt(1, 1) - SG_WE), 0);
    BOOST_CHECK_EQUAL(boardConst.Line(Pt(1, 1)), 1);
    BOOST_CHECK_EQUAL(boardConst.Line(Pt(1, 8)), 1);
    BOOST_CHECK_EQUAL(boardConst.Line(Pt(8, 1)), 1);
    BOOST_CHECK_EQUAL(boardConst.Line(Pt(8, 8)), 1);
    BOOST_CHECK_EQUAL(boardConst.Line(Pt(2, 5)), 2);
    BOOST_CHECK_EQUAL(boardConst.Line(Pt(5, 5)), 4);
}

BOOST_AUTO_TEST_CASE(SgBoardConstTest_Pos)
{
    SgBoardConst boardConst(8);
    BOOST_CHECK_EQUAL(boardConst.Pos(Pt(1, 1) - SG_WE), 0);
    BOOST_CHECK_EQUAL(boardConst.Pos(Pt(1, 1)), 1);
    BOOST_CHECK_EQUAL(boardConst.Pos(Pt(1, 8)), 1);
    BOOST_CHECK_EQUAL(boardConst.Pos(Pt(8, 1)), 1);
    BOOST_CHECK_EQUAL(boardConst.Pos(Pt(8, 8)), 1);
    BOOST_CHECK_EQUAL(boardConst.Pos(Pt(2, 5)), 4);
    BOOST_CHECK_EQUAL(boardConst.Pos(Pt(5, 5)), 4);
}

BOOST_AUTO_TEST_CASE(SgBoardConstTest_Size)
{
    BOOST_CHECK_EQUAL(SgBoardConst(19).Size(), 19);
    BOOST_CHECK_EQUAL(SgBoardConst(6).Size(), 6);
}

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgNbIteratorTest_Corner)
{
    SgBoardConst boardConst(19);
    SgNbIterator it(boardConst, Pt(1, 1));
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(*it, Pt(2, 1));
    ++it;
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(*it, Pt(1, 2));
    ++it;
    BOOST_CHECK(! it);
}

BOOST_AUTO_TEST_CASE(SgNbIteratorTest_Edge)
{
    SgBoardConst boardConst(9);
    SgNbIterator it(boardConst, Pt(9, 5));
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(*it, Pt(9, 4));
    ++it;
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(*it, Pt(8, 5));
    ++it;
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(*it, Pt(9, 6));
    ++it;
    BOOST_CHECK(! it);
}

BOOST_AUTO_TEST_CASE(SgNbIteratorTest_Middle)
{
    SgBoardConst boardConst(13);
    SgNbIterator it(boardConst, Pt(3, 3));
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(*it, Pt(3, 2));
    ++it;
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(*it, Pt(2, 3));
    ++it;
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(*it, Pt(4, 3));
    ++it;
    BOOST_CHECK(it);
    BOOST_CHECK_EQUAL(*it, Pt(3, 4));
    ++it;
    BOOST_CHECK(! it);
}

//----------------------------------------------------------------------------

} // namespace

