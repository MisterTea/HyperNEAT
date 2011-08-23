//----------------------------------------------------------------------------
/** @file SgArrayTest.cpp
    Unit tests for SgArray.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgArray.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgArrayTest_Assign)
{
    SgArray<int, 3> a(5);
    SgArray<int, 3> b;
    b = a;
    BOOST_CHECK_EQUAL(b[0], 5);
    BOOST_CHECK_EQUAL(b[1], 5);
    BOOST_CHECK_EQUAL(b[2], 5);
}

BOOST_AUTO_TEST_CASE(SgArrayTest_CopyConstructor)
{
    SgArray<int, 3> a(5);
    SgArray<int, 3> b(a);
    BOOST_CHECK_EQUAL(b[0], 5);
    BOOST_CHECK_EQUAL(b[1], 5);
    BOOST_CHECK_EQUAL(b[2], 5);
}

BOOST_AUTO_TEST_CASE(SgArrayTest_Elements)
{
    SgArray<int, 3> a;
    a[0] = 5;
    a[1] = 4;
    a[2] = 3;
    BOOST_CHECK_EQUAL(a[0], 5);
    BOOST_CHECK_EQUAL(a[1], 4);
    BOOST_CHECK_EQUAL(a[2], 3);
    const SgArray<int, 3>& b = a;
    BOOST_CHECK_EQUAL(b[0], 5);
    BOOST_CHECK_EQUAL(b[1], 4);
    BOOST_CHECK_EQUAL(b[2], 3);
}

BOOST_AUTO_TEST_CASE(SgSArrayTest_Iterator)
{
    SgArray<int,3> a;
    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
    SgArray<int,3>::Iterator i(a);
    BOOST_CHECK(i);
    BOOST_CHECK_EQUAL(*i, 1);
    ++i;
    BOOST_CHECK(i);
    BOOST_CHECK_EQUAL(*i, 2);
    ++i;
    BOOST_CHECK(i);
    BOOST_CHECK_EQUAL(*i, 3);
    ++i;
    BOOST_CHECK(! i);
}

BOOST_AUTO_TEST_CASE(SgArrayTest_MultiplyAssign)
{
    SgArray<int, 3> a;
    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
    a *= 10;
    BOOST_CHECK_EQUAL(a[0], 10);
    BOOST_CHECK_EQUAL(a[1], 20);
    BOOST_CHECK_EQUAL(a[2], 30);
}

BOOST_AUTO_TEST_CASE(SgSArrayTestNonConstIterator)
{
    SgArray<int,3> a;
    a[0] = 1;
    a[1] = 2;
    a[2] = 3;
    SgArray<int,3>::NonConstIterator i(a);
    BOOST_CHECK(i);
    BOOST_CHECK_EQUAL(*i, 1);
    *i = 0;
    ++i;
    BOOST_CHECK(i);
    BOOST_CHECK_EQUAL(*i, 2);
    *i = 0;
    ++i;
    BOOST_CHECK(i);
    BOOST_CHECK_EQUAL(*i, 3);
    *i = 0;
    ++i;
    BOOST_CHECK(! i);
    BOOST_CHECK_EQUAL(a[0], 0);
    BOOST_CHECK_EQUAL(a[1], 0);
    BOOST_CHECK_EQUAL(a[2], 0);
}

BOOST_AUTO_TEST_CASE(SgArrayTest_ValueConstructor)
{
    SgArray<int, 3> a(5);
    BOOST_CHECK_EQUAL(a[0], 5);
    BOOST_CHECK_EQUAL(a[1], 5);
    BOOST_CHECK_EQUAL(a[2], 5);
}

} // namespace

//----------------------------------------------------------------------------

