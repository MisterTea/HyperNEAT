//----------------------------------------------------------------------------
/** @file SgPointSetTest.cpp
    Unit tests for SgSet.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <sstream>
#include <boost/test/auto_unit_test.hpp>
#include "SgPointSet.h"

using namespace std;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgPointSetTest_AllAdjacentTo)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(1, 1));
    BOOST_CHECK(! b.AllAdjacentTo(a));
    b.Exclude(Pt(1, 1));
    b.Include(Pt(1, SG_MAX_SIZE));
    BOOST_CHECK(! b.AllAdjacentTo(a));
    b.Clear();
    b.Include(Pt(1, 2));
    BOOST_CHECK(b.AllAdjacentTo(a));
    b.Include(Pt(3, 2));
    BOOST_CHECK(b.AllAdjacentTo(a));
    b.Include(Pt(5, 5));
    BOOST_CHECK(! b.AllAdjacentTo(a));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Adjacent)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(1, 1));
    BOOST_CHECK(! b.Adjacent(a));
    b.Include(Pt(1, SG_MAX_SIZE));
    BOOST_CHECK(! b.Adjacent(a));
    b.Include(Pt(1, 2));
    BOOST_CHECK(b.Adjacent(a));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_AdjacentOnlyTo)
{
    SgPointSet a;
    a.Include(Pt(19, 1));
    a.Include(Pt(19, 2));
    SgPointSet b;
    b.Include(Pt(19, 1));
    b.Include(Pt(19, 2));
    b.Include(Pt(19, 3));
    b.Include(Pt(18, 1));
    BOOST_CHECK(! a.AdjacentOnlyTo(b, 19));
    b.Include(Pt(18, 2));
    BOOST_CHECK(a.AdjacentOnlyTo(b, 19));
    b.Include(Pt(18, 3));
    BOOST_CHECK(a.AdjacentOnlyTo(b, 19));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_AdjacentTo)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    BOOST_CHECK(! a.AdjacentTo(Pt(1, 1)));
    BOOST_CHECK(! a.AdjacentTo(Pt(1, SG_MAX_SIZE)));
    BOOST_CHECK(a.AdjacentTo(Pt(1, 2)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Adjacent8To)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    BOOST_CHECK(! a.Adjacent8To(Pt(1, SG_MAX_SIZE)));
    BOOST_CHECK(a.Adjacent8To(Pt(1, 1)));
    BOOST_CHECK(a.Adjacent8To(Pt(1, 2)));
    BOOST_CHECK(a.Adjacent8To(Pt(3, 3)));
}

void SgPointSetTestAllPointsAtSize(int boardSize)
{
    const SgPointSet& s = SgPointSet::AllPoints(boardSize);
    BOOST_CHECK_EQUAL(s.Size(), boardSize * boardSize);
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_AllPoints)
{
    SgPointSetTestAllPointsAtSize(SG_MIN_SIZE);
    SgPointSetTestAllPointsAtSize(9);
    SgPointSetTestAllPointsAtSize(10);
    SgPointSetTestAllPointsAtSize(SG_MAX_SIZE);
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_And)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(1, 1));
    SgPointSet c(a & b);
    BOOST_CHECK_EQUAL(c.Size(), 1);
    BOOST_CHECK(c.Contains(Pt(1, 1)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_AndAssign)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(1, 1));
    a &= b;
    BOOST_CHECK_EQUAL(a.Size(), 1);
    BOOST_CHECK(a.Contains(Pt(1, 1)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Assign)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(3, 3));
    b = a;
    BOOST_CHECK_EQUAL(b.Size(), 2);
    BOOST_CHECK(a.Contains(Pt(1, 1)));
    BOOST_CHECK(a.Contains(Pt(2, 2)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Border)
{
    SgPointSet a;
    a.Include(Pt(19, 1));
    a.Include(Pt(19, 2));
    SgPointSet b = a.Border(19);
    BOOST_CHECK_EQUAL(b.Size(), 3);
    BOOST_CHECK(b.Contains(Pt(19, 3)));
    BOOST_CHECK(b.Contains(Pt(18, 1)));
    BOOST_CHECK(b.Contains(Pt(18, 2)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Border8)
{
    SgPointSet a;
    a.Include(Pt(19, 1));
    a.Include(Pt(19, 2));
    SgPointSet b = a.Border8(19);
    BOOST_CHECK_EQUAL(b.Size(), 4);
    BOOST_CHECK(b.Contains(Pt(19, 3)));
    BOOST_CHECK(b.Contains(Pt(18, 1)));
    BOOST_CHECK(b.Contains(Pt(18, 2)));
    BOOST_CHECK(b.Contains(Pt(18, 3)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Center)
{
    SgPointSet a;
    a.Include(Pt(19, 1));
    a.Include(Pt(19, 2));
    a.Include(Pt(19, 3));
    BOOST_CHECK_EQUAL(a.Center(), Pt(19, 2));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Clear)
{
    SgPointSet a;
    a.Include(Pt(19, 1));
    a.Include(Pt(19, 2));
    a.Include(Pt(19, 3));
    a.Clear();
    BOOST_CHECK_EQUAL(a.Size(), 0);
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Component)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(1, 2));
    a.Include(Pt(2, 1));
    a.Include(Pt(1, SG_MAX_SIZE));
    SgPointSet b = a.Component(Pt(1, 1));
    BOOST_CHECK_EQUAL(b.Size(), 3);
    BOOST_CHECK(a.Contains(Pt(1, 1)));
    BOOST_CHECK(a.Contains(Pt(1, 2)));
    BOOST_CHECK(a.Contains(Pt(2, 1)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_ConnComp)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(1, 2));
    a.Include(Pt(2, 1));
    a.Include(Pt(1, SG_MAX_SIZE));
    SgPointSet b = a.ConnComp(Pt(1, 1));
    BOOST_CHECK_EQUAL(b.Size(), 3);
    BOOST_CHECK(a.Contains(Pt(1, 1)));
    BOOST_CHECK(a.Contains(Pt(1, 2)));
    BOOST_CHECK(a.Contains(Pt(2, 1)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Disjoint)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    BOOST_CHECK(a.Disjoint(b));
    b.Include(Pt(3, 3));
    BOOST_CHECK(a.Disjoint(b));
    b.Include(Pt(2, 2));
    BOOST_CHECK(! a.Disjoint(b));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_EnclosingRect)
{
    SgPointSet a;
    a.Include(Pt(19, 1));
    a.Include(Pt(19, 3));
    a.Include(Pt(18, 2));
    BOOST_CHECK_EQUAL(a.EnclosingRect(), SgRect(18, 19, 1, 3));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Equals)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(1, 1));
    b.Include(Pt(2, 2));
    BOOST_CHECK(a == b);
    BOOST_CHECK(! (a != b));
    b.Exclude(Pt(2, 2));
    BOOST_CHECK(! (a == b));
    BOOST_CHECK(a != b);
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Grow)
{
    SgPointSet a;
    a.Include(Pt(8, 1));
    a.Include(Pt(8, 2));
    a.Include(Pt(9, 1));
    a.Include(Pt(9, 2));
    a.Grow(9);
    BOOST_CHECK_EQUAL(a.Size(), 8);
    BOOST_CHECK(a.Contains(Pt(7, 1)));
    BOOST_CHECK(a.Contains(Pt(7, 2)));
    BOOST_CHECK(a.Contains(Pt(8, 1)));
    BOOST_CHECK(a.Contains(Pt(8, 2)));
    BOOST_CHECK(a.Contains(Pt(8, 3)));
    BOOST_CHECK(a.Contains(Pt(9, 1)));
    BOOST_CHECK(a.Contains(Pt(9, 2)));
    BOOST_CHECK(a.Contains(Pt(9, 3)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_GrowNewArea)
{
    SgPointSet a;
    a.Include(Pt(8, 1));
    a.Include(Pt(8, 2));
    a.Include(Pt(9, 1));
    a.Include(Pt(9, 2));
    SgPointSet newArea;
    a.Grow(&newArea, 9);
    BOOST_CHECK_EQUAL(a.Size(), 8);
    BOOST_CHECK(a.Contains(Pt(7, 1)));
    BOOST_CHECK(a.Contains(Pt(7, 2)));
    BOOST_CHECK(a.Contains(Pt(8, 1)));
    BOOST_CHECK(a.Contains(Pt(8, 2)));
    BOOST_CHECK(a.Contains(Pt(8, 3)));
    BOOST_CHECK(a.Contains(Pt(9, 1)));
    BOOST_CHECK(a.Contains(Pt(9, 2)));
    BOOST_CHECK(a.Contains(Pt(9, 3)));
    BOOST_CHECK_EQUAL(newArea.Size(), 4);
    BOOST_CHECK(newArea.Contains(Pt(7, 1)));
    BOOST_CHECK(newArea.Contains(Pt(7, 2)));
    BOOST_CHECK(newArea.Contains(Pt(8, 3)));
    BOOST_CHECK(newArea.Contains(Pt(9, 3)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Grow8)
{
    SgPointSet a;
    a.Include(Pt(8, 1));
    a.Include(Pt(8, 2));
    a.Include(Pt(9, 1));
    a.Include(Pt(9, 2));
    a.Grow8(9);
    BOOST_CHECK_EQUAL(a.Size(), 9);
    BOOST_CHECK(a.Contains(Pt(7, 1)));
    BOOST_CHECK(a.Contains(Pt(7, 2)));
    BOOST_CHECK(a.Contains(Pt(7, 3)));
    BOOST_CHECK(a.Contains(Pt(8, 1)));
    BOOST_CHECK(a.Contains(Pt(8, 2)));
    BOOST_CHECK(a.Contains(Pt(8, 3)));
    BOOST_CHECK(a.Contains(Pt(9, 1)));
    BOOST_CHECK(a.Contains(Pt(9, 2)));
    BOOST_CHECK(a.Contains(Pt(9, 3)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_IsCloseTo)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    BOOST_CHECK(a.IsCloseTo(Pt(1, 1)));
    BOOST_CHECK(a.IsCloseTo(Pt(1, 2)));
    BOOST_CHECK(a.IsCloseTo(Pt(1, 3)));
    BOOST_CHECK(a.IsCloseTo(Pt(1, 4)));
    BOOST_CHECK(! a.IsCloseTo(Pt(1, 5)));
    BOOST_CHECK(a.IsCloseTo(Pt(1, 1)));
    BOOST_CHECK(a.IsCloseTo(Pt(2, 1)));
    BOOST_CHECK(a.IsCloseTo(Pt(3, 1)));
    BOOST_CHECK(a.IsCloseTo(Pt(4, 1)));
    BOOST_CHECK(! a.IsCloseTo(Pt(5, 1)));
    BOOST_CHECK(a.IsCloseTo(Pt(4, 4)));
    BOOST_CHECK(! a.IsCloseTo(Pt(5, 5)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_IsConnected)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(1, 2));
    a.Include(Pt(2, 1));
    BOOST_CHECK(a.IsConnected());
    a.Include(Pt(1, SG_MAX_SIZE));
    BOOST_CHECK(! a.IsConnected());
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_IsEmpty)
{
    SgPointSet a;
    BOOST_CHECK(a.IsEmpty());
    a.Include(Pt(1, 1));
    BOOST_CHECK(! a.IsEmpty());
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Kernel)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(1, 2));
    a.Include(Pt(1, 3));
    a.Include(Pt(2, 1));
    a.Include(Pt(2, 2));
    a.Include(Pt(2, 3));
    a.Include(Pt(3, 1));
    a.Include(Pt(3, 2));
    a.Include(Pt(3, 3));
    SgPointSet k = a.Kernel(19);
    BOOST_CHECK_EQUAL(k.Size(), 4);
    BOOST_CHECK(k.Contains(Pt(1, 1)));
    BOOST_CHECK(k.Contains(Pt(1, 2)));
    BOOST_CHECK(k.Contains(Pt(2, 1)));
    BOOST_CHECK(k.Contains(Pt(2, 2)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_MaxOverlap)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(2, 2));
    b.Include(Pt(3, 3));
    BOOST_CHECK(! a.MaxOverlap(b, 0));
    BOOST_CHECK(a.MaxOverlap(b, 1));
    BOOST_CHECK(a.MaxOverlap(b, 2));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_MinOverlap)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(2, 2));
    b.Include(Pt(3, 3));
    BOOST_CHECK(a.MinOverlap(b, 0));
    BOOST_CHECK(a.MinOverlap(b, 1));
    BOOST_CHECK(! a.MinOverlap(b, 2));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Minus)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(1, 1));
    SgPointSet c(a - b);
    BOOST_CHECK_EQUAL(c.Size(), 1);
    BOOST_CHECK(c.Contains(Pt(2, 2)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_MinusAssign)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(1, 1));
    a -= b;
    BOOST_CHECK_EQUAL(a.Size(), 1);
    BOOST_CHECK(a.Contains(Pt(2, 2)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Or)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(1, 1));
    SgPointSet c(a | b);
    BOOST_CHECK_EQUAL(c.Size(), 2);
    BOOST_CHECK(c.Contains(Pt(1, 1)));
    BOOST_CHECK(c.Contains(Pt(2, 2)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_OrAssign)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(1, 1));
    a |= b;
    BOOST_CHECK_EQUAL(a.Size(), 2);
    BOOST_CHECK(a.Contains(Pt(1, 1)));
    BOOST_CHECK(a.Contains(Pt(2, 2)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Overlaps)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(3, 3));
    BOOST_CHECK(! a.Overlaps(b));
    BOOST_CHECK(! b.Overlaps(a));
    b.Include(Pt(2, 2));
    BOOST_CHECK(a.Overlaps(b));
    BOOST_CHECK(b.Overlaps(a));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_PointOf)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    BOOST_CHECK_EQUAL(a.PointOf(), Pt(1, 1));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_SubsetOf)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    BOOST_CHECK(b.SubsetOf(a));
    b.Include(Pt(1, 1));
    BOOST_CHECK(b.SubsetOf(a));
    b.Include(Pt(2, 2));
    BOOST_CHECK(b.SubsetOf(a));
    b.Include(Pt(3, 3));
    BOOST_CHECK(! b.SubsetOf(a));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_SgPointSet_SgVector)
{
    SgVector<SgPoint> a;
    SgPointSet b(a);
    BOOST_CHECK_EQUAL(b.Size(), 0);
    a.PushBack(Pt(1, 1));
    a.PushBack(Pt(2, 2));
    a.PushBack(Pt(3, 3));
    SgPointSet c(a);
    BOOST_CHECK_EQUAL(c.Size(), 3);
    SgVector<SgPoint> d;
    BOOST_CHECK_EQUAL(d.Length(), 0);
    c.ToVector(&d);
    BOOST_CHECK_EQUAL(d.Length(), 3);
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_SupersetOf)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    BOOST_CHECK(! b.SupersetOf(a));
    b.Include(Pt(1, 1));
    BOOST_CHECK(! b.SupersetOf(a));
    b.Include(Pt(2, 2));
    BOOST_CHECK(b.SupersetOf(a));
    b.Include(Pt(3, 3));
    BOOST_CHECK(b.SupersetOf(a));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Swap)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(3, 3));
    b.Include(Pt(4, 4));
    b.Include(Pt(5, 5));
    a.Swap(b);
    BOOST_CHECK_EQUAL(a.Size(), 3);
    BOOST_REQUIRE(a.Size() == 3);
    BOOST_CHECK(a.Contains(Pt(3, 3)));
    BOOST_CHECK(a.Contains(Pt(4, 4)));
    BOOST_CHECK(a.Contains(Pt(5, 5)));
    BOOST_CHECK_EQUAL(b.Size(), 2);
    BOOST_REQUIRE(b.Size() == 2);
    BOOST_CHECK(b.Contains(Pt(1, 1)));
    BOOST_CHECK(b.Contains(Pt(2, 2)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Write)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    ostringstream out;
    a.Write(out, 5);
    BOOST_CHECK_EQUAL(out.str(),
                      "-----\n"
                      "-----\n"
                      "-----\n"
                      "-@---\n"
                      "@----\n");
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_Xor)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(1, 1));
    SgPointSet c(a ^ b);
    BOOST_CHECK_EQUAL(c.Size(), 1);
    BOOST_CHECK(c.Contains(Pt(2, 2)));
}

BOOST_AUTO_TEST_CASE(SgPointSetTest_XorAssign)
{
    SgPointSet a;
    a.Include(Pt(1, 1));
    a.Include(Pt(2, 2));
    SgPointSet b;
    b.Include(Pt(1, 1));
    a ^= b;
    BOOST_CHECK_EQUAL(a.Size(), 1);
    BOOST_CHECK(a.Contains(Pt(2, 2)));
}

} // namespace

//----------------------------------------------------------------------------

