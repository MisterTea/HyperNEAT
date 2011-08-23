//----------------------------------------------------------------------------
/** @file SgPointTest.cpp
    Unit tests for SgPoint.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgPoint.h"

using namespace std;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgPointTest_Col)
{
    BOOST_CHECK_EQUAL(SgPointUtil::Col(SgPointUtil::Pt(1, 1)), 1);
    BOOST_CHECK_EQUAL(SgPointUtil::Col(SgPointUtil::Pt(5, 3)), 5);
    BOOST_CHECK_EQUAL(SgPointUtil::Col(SgPointUtil::Pt(19, 3)), 19);
    BOOST_CHECK_EQUAL(SgPointUtil::Col(SgPointUtil::Pt(19, 19)), 19);
}

BOOST_AUTO_TEST_CASE(SgPointTest_Pt)
{
    BOOST_CHECK_EQUAL(SgPointUtil::Pt(1, 1), 21);
    BOOST_CHECK_EQUAL(SgPointUtil::Pt(19, 19), 399);
}

BOOST_AUTO_TEST_CASE(SgPointTest_Row)
{
    BOOST_CHECK_EQUAL(SgPointUtil::Row(SgPointUtil::Pt(1, 1)), 1);
    BOOST_CHECK_EQUAL(SgPointUtil::Row(SgPointUtil::Pt(5, 3)), 3);
    BOOST_CHECK_EQUAL(SgPointUtil::Row(SgPointUtil::Pt(19, 3)), 3);
    BOOST_CHECK_EQUAL(SgPointUtil::Row(SgPointUtil::Pt(19, 19)), 19);
}

//----------------------------------------------------------------------------

void SgReadPointTest(SgPoint point, const char* s)
{
    istringstream in(s);
    SgPoint p;
    in >> SgReadPoint(p);
    BOOST_REQUIRE(! in.fail());
    BOOST_CHECK_EQUAL(p, point);
}

/** Read some valid points and pass. */
BOOST_AUTO_TEST_CASE(SgReadPointTest_AllValid)
{
    SgReadPointTest(Pt(1, 1), "A1");
    SgReadPointTest(Pt(8, 1), "H1");
    SgReadPointTest(Pt(9, 1), "J1");
    SgReadPointTest(Pt(19, 19), "T19");
    SgReadPointTest(SG_PASS, "PASS");
}

/** Check stream state after invalid input. */
BOOST_AUTO_TEST_CASE(SgReadPointTest_Invalid)
{
    istringstream in("123");
    SgPoint p;
    in >> SgReadPoint(p);
    BOOST_CHECK(! in);
}

/** Read two points from stream. */
BOOST_AUTO_TEST_CASE(SgReadPointTest_Two)
{
    istringstream in("A1 B2");
    SgPoint p1;
    SgPoint p2;
    in >> SgReadPoint(p1) >> SgReadPoint(p2);
    BOOST_REQUIRE(in);
    BOOST_CHECK_EQUAL(p1, Pt(1, 1));
    BOOST_CHECK_EQUAL(p2, Pt(2, 2));
}

//----------------------------------------------------------------------------

void SgWritePointTest(SgPoint p, const char* s)
{
    ostringstream out;
    out << SgWritePoint(p);
    BOOST_CHECK_EQUAL(out.str(), s);
}

BOOST_AUTO_TEST_CASE(SgWritePointTestAll)
{
    SgWritePointTest(Pt(1, 1), "A1");
    SgWritePointTest(Pt(8, 1), "H1");
    SgWritePointTest(Pt(9, 1), "J1");
    SgWritePointTest(Pt(19, 19), "T19");
    SgWritePointTest(SG_PASS, "PASS");
}

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgPointUtilTest_AreAdjacent)
{
    using SgPointUtil::AreAdjacent;
    BOOST_CHECK(AreAdjacent(Pt(1, 1), Pt(2, 1)));
    BOOST_CHECK(AreAdjacent(Pt(2, 1), Pt(1, 1)));
    BOOST_CHECK(AreAdjacent(Pt(1, 1), Pt(1, 2)));
    BOOST_CHECK(AreAdjacent(Pt(1, 2), Pt(1, 1)));
    BOOST_CHECK(! AreAdjacent(Pt(1, 1), Pt(2, 2)));
    BOOST_CHECK(! AreAdjacent(Pt(1, 1), Pt(1, 3)));
    BOOST_CHECK(! AreAdjacent(Pt(1, 1), Pt(1, SG_MAX_SIZE)));
}

BOOST_AUTO_TEST_CASE(SgPointUtilTest_Rotate)
{
    using SgPointUtil::Rotate;
    BOOST_CHECK_EQUAL(Rotate(0, Pt(1, 2), 9), Pt(1, 2));
    BOOST_CHECK_EQUAL(Rotate(1, Pt(1, 2), 9), Pt(9, 2));
    BOOST_CHECK_EQUAL(Rotate(2, Pt(1, 2), 9), Pt(1, 8));
    BOOST_CHECK_EQUAL(Rotate(3, Pt(1, 2), 9), Pt(2, 1));
    BOOST_CHECK_EQUAL(Rotate(4, Pt(1, 2), 9), Pt(8, 1));
    BOOST_CHECK_EQUAL(Rotate(5, Pt(1, 2), 9), Pt(2, 9));
    BOOST_CHECK_EQUAL(Rotate(6, Pt(1, 2), 9), Pt(9, 8));
    BOOST_CHECK_EQUAL(Rotate(7, Pt(1, 2), 9), Pt(8, 9));
}

BOOST_AUTO_TEST_CASE(SgPointUtilTest_InvRotation)
{
    using SgPointUtil::Rotate;
    using SgPointUtil::InvRotation;
    BOOST_CHECK_EQUAL(Rotate(InvRotation(0), Pt(1, 2), 9), Pt(1, 2));
    BOOST_CHECK_EQUAL(Rotate(InvRotation(1), Pt(9, 2), 9), Pt(1, 2));
    BOOST_CHECK_EQUAL(Rotate(InvRotation(2), Pt(1, 8), 9), Pt(1, 2));
    BOOST_CHECK_EQUAL(Rotate(InvRotation(3), Pt(2, 1), 9), Pt(1, 2));
    BOOST_CHECK_EQUAL(Rotate(InvRotation(4), Pt(8, 1), 9), Pt(1, 2));
    BOOST_CHECK_EQUAL(Rotate(InvRotation(5), Pt(2, 9), 9), Pt(1, 2));
    BOOST_CHECK_EQUAL(Rotate(InvRotation(6), Pt(9, 8), 9), Pt(1, 2));
    BOOST_CHECK_EQUAL(Rotate(InvRotation(7), Pt(8, 9), 9), Pt(1, 2));
}

//----------------------------------------------------------------------------

} // namespace


