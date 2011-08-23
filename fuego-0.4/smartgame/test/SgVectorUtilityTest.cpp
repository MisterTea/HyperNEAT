//----------------------------------------------------------------------------
/** @file SgVectorUtilityTest.cpp
    Unit tests for SgVectorUtility.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgVectorUtility.h"

//----------------------------------------------------------------------------
namespace {
//----------------------------------------------------------------------------

void AddToVector(int from, int to, SgVector<int>& vector)
{
    for (int i = from; i <= to; ++i)
        vector.PushBack(i);
}

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgVectorUtilityTestDifference)
{
    SgVector<int> a;
    a.PushBack(5);
    a.PushBack(6);
    a.PushBack(7);
    a.PushBack(8);
    a.PushBack(-56);
    a.PushBack(9);
    a.PushBack(10);
    SgVector<int> b;
    b.PushBack(8);
    b.PushBack(-56);
    b.PushBack(9);
    b.PushBack(10);
    b.PushBack(11);
    b.PushBack(12);
    SgVectorUtility::Difference(&a, b);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    BOOST_CHECK_EQUAL(a[0], 5);
    BOOST_CHECK_EQUAL(a[1], 6);
    BOOST_CHECK_EQUAL(a[2], 7);
}

BOOST_AUTO_TEST_CASE(SgVectorUtilityTestIntersection)
{
    SgVector<int> a;
    AddToVector(5,10,a);
    SgVector<int> b;
    AddToVector(8,12,b);
    SgVectorUtility::Intersection(&a, b);
    BOOST_CHECK_EQUAL(a.Length(), 3);
    BOOST_CHECK_EQUAL(a[0], 8);
    BOOST_CHECK_EQUAL(a[1], 9);
    BOOST_CHECK_EQUAL(a[2], 10);
}

//----------------------------------------------------------------------------
} // namespace
//----------------------------------------------------------------------------

