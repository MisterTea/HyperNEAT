//----------------------------------------------------------------------------
/** @file SgRectTest.cpp
    Unit tests for SgRect.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgRect.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgRectTestAccessors)
{
    SgRect rect(2, 5, 7, 9);
    BOOST_CHECK_EQUAL(rect.Left(), 2);
    BOOST_CHECK_EQUAL(rect.Right(), 5);
    BOOST_CHECK_EQUAL(rect.Top(), 7);
    BOOST_CHECK_EQUAL(rect.Bottom(), 9);
}

BOOST_AUTO_TEST_CASE(SgRectTestCenter)
{
    SgRect rect(2, 6, 7, 9);
    SgPoint center = rect.Center();
    BOOST_CHECK_EQUAL(SgPointUtil::Col(center), 4);
    BOOST_CHECK_EQUAL(SgPointUtil::Row(center), 8);
}

BOOST_AUTO_TEST_CASE(SgRectTestContains)
{
    SgRect rect(2, 5, 7, 9);
    SgRect rect1(1, 4, 7, 9);
    BOOST_CHECK(! rect.Contains(rect1));
    SgRect rect2(4, 11, 7, 9);
    BOOST_CHECK(! rect.Contains(rect2));
    SgRect rect3(2, 5, 3, 9);
    BOOST_CHECK(! rect.Contains(rect3));
    SgRect rect4(2, 5, 7, 14);
    BOOST_CHECK(! rect.Contains(rect4));
    SgRect rect5(2, 5, 7, 9);
    BOOST_CHECK(rect.Contains(rect5));
    SgRect rect6(3, 4, 8, 9);
    BOOST_CHECK(rect.Contains(rect6));
    SgRect rect7(1, 9, 1, 12);
    BOOST_CHECK(! rect.Contains(rect7));
    BOOST_CHECK(rect7.Contains(rect));
}

BOOST_AUTO_TEST_CASE(SgRectTestDefaultConstructor)
{
    SgRect rect;
    BOOST_CHECK(rect.IsEmpty());
}

BOOST_AUTO_TEST_CASE(SgRectTestInRect)
{
    SgRect rect(2, 5, 7, 9);
    BOOST_CHECK(! rect.InRect(SgPointUtil::Pt(1, 8)));
    BOOST_CHECK(! rect.InRect(SgPointUtil::Pt(2, 6)));
    BOOST_CHECK(rect.InRect(SgPointUtil::Pt(2, 7)));
    BOOST_CHECK(rect.InRect(SgPointUtil::Pt(2, 8)));
    BOOST_CHECK(rect.InRect(SgPointUtil::Pt(2, 9)));
    BOOST_CHECK(! rect.InRect(SgPointUtil::Pt(2, 10)));
    BOOST_CHECK(! rect.InRect(SgPointUtil::Pt(3, 5)));
    BOOST_CHECK(rect.InRect(SgPointUtil::Pt(5, 7)));
    BOOST_CHECK(rect.InRect(SgPointUtil::Pt(5, 9)));
    BOOST_CHECK(! rect.InRect(SgPointUtil::Pt(5, 10)));
}

BOOST_AUTO_TEST_CASE(SgRectTestMirrorX)
{
    SgRect rect(2, 5, 7, 9);
    rect.MirrorX(19);
    BOOST_CHECK_EQUAL(rect.Left(), 15);
    BOOST_CHECK_EQUAL(rect.Right(), 18);
    BOOST_CHECK_EQUAL(rect.Top(), 7);
    BOOST_CHECK_EQUAL(rect.Bottom(), 9);
}

BOOST_AUTO_TEST_CASE(SgRectTestMirrorY)
{
    SgRect rect(2, 5, 7, 9);
    rect.MirrorY(19);
    BOOST_CHECK_EQUAL(rect.Left(), 2);
    BOOST_CHECK_EQUAL(rect.Right(), 5);
    BOOST_CHECK_EQUAL(rect.Top(), 11);
    BOOST_CHECK_EQUAL(rect.Bottom(), 13);
}

BOOST_AUTO_TEST_CASE(SgRectTestOverlaps)
{
    SgRect rect(2, 5, 7, 9);
    SgRect rect1(1, 4, 7, 9);
    BOOST_CHECK(rect.Overlaps(rect1));
    SgRect rect2(4, 11, 7, 9);
    BOOST_CHECK(rect.Overlaps(rect2));
    SgRect rect3(2, 5, 3, 9);
    BOOST_CHECK(rect.Overlaps(rect3));
    SgRect rect4(2, 5, 7, 14);
    BOOST_CHECK(rect.Overlaps(rect4));
    SgRect rect5(2, 5, 7, 9);
    BOOST_CHECK(rect.Overlaps(rect5));
    SgRect rect6(3, 4, 8, 9);
    BOOST_CHECK(rect.Overlaps(rect6));
    SgRect rect7(1, 9, 1, 12);
    BOOST_CHECK(rect.Overlaps(rect7));
    SgRect rect8(6, 9, 7, 9);
    BOOST_CHECK(! rect.Overlaps(rect8));
}

BOOST_AUTO_TEST_CASE(SgRectTestSwapXY)
{
    SgRect rect(2, 5, 7, 9);
    rect.SwapXY();
    BOOST_CHECK_EQUAL(rect.Left(), 7);
    BOOST_CHECK_EQUAL(rect.Right(), 9);
    BOOST_CHECK_EQUAL(rect.Top(), 2);
    BOOST_CHECK_EQUAL(rect.Bottom(), 5);
}

} // namespace

//----------------------------------------------------------------------------

