//----------------------------------------------------------------------------
/** @file SgStackTest.cpp
    Unit tests for SgStack.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgStack.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgStackTestIsEmpty)
{
    SgStack<int, 5> s;
    BOOST_CHECK(s.IsEmpty());
    s.Push(3);
    BOOST_CHECK(! s.IsEmpty());
}

BOOST_AUTO_TEST_CASE(SgStackTestNonEmpty)
{
    SgStack<int, 5> s;
    BOOST_CHECK(! s.NonEmpty());
    s.Push(3);
    BOOST_CHECK(s.NonEmpty());
}

BOOST_AUTO_TEST_CASE(SgStackTestPushPop)
{
    SgStack<int, 5> s;
    BOOST_CHECK(s.Size() == 0);
    s.Push(3);
    BOOST_CHECK_EQUAL(s[0],  3);
    BOOST_CHECK_EQUAL(s.Top(),  3);
    BOOST_CHECK(s.Size() == 1);
    s.Push(5);
    BOOST_CHECK(s.Size() == 2);
    BOOST_CHECK_EQUAL(s[0],  3);
    BOOST_CHECK_EQUAL(s[1],  5);
    BOOST_CHECK_EQUAL(s.Top(),  5);
    s.Pop();
    BOOST_CHECK(s.Size() == 1);
    BOOST_CHECK_EQUAL(s[0],  3);
    BOOST_CHECK_EQUAL(s.Top(),  3);
    s.Pop();
    BOOST_CHECK(s.Size() == 0);
}

BOOST_AUTO_TEST_CASE(SgStackTestCopyFrom)
{
    SgStack<int, 5> s1;
    SgStack<int, 5> s2;
    s1.Push(1);
    s1.Push(2);
    s1.Push(3);
    s2.Push(4);
    s2.Push(5);
    BOOST_CHECK_EQUAL(s1.Size(), 3);
    BOOST_CHECK_EQUAL(s2.Size(), 2);
    s1.CopyFrom(s2);
    BOOST_CHECK_EQUAL(s1.Size(), 2);
    BOOST_CHECK_EQUAL(s1[0],  4);
    BOOST_CHECK_EQUAL(s1[1],  5);
}

BOOST_AUTO_TEST_CASE(SgStackTestPushAll)
{
    SgStack<int, 5> s1;
    SgStack<int, 5> s2;
    s1.Push(1);
    s1.Push(2);
    s1.Push(3);
    s2.Push(4);
    s2.Push(5);
    BOOST_CHECK_EQUAL(s1.Size(), 3);
    BOOST_CHECK_EQUAL(s2.Size(), 2);
    s1.PushAll(s2);
    BOOST_CHECK_EQUAL(s1.Size(), 5);
    BOOST_CHECK_EQUAL(s1[0],  1);
    BOOST_CHECK_EQUAL(s1[1],  2);
    BOOST_CHECK_EQUAL(s1[2],  3);
    BOOST_CHECK_EQUAL(s1[3],  4);
    BOOST_CHECK_EQUAL(s1[4],  5);
}

BOOST_AUTO_TEST_CASE(SgStackTestSwapWith)
{
    SgStack<int, 5> s1;
    SgStack<int, 5> s2;
    s1.Push(1);
    s1.Push(2);
    s1.Push(3);
    s2.Push(4);
    s2.Push(5);
    BOOST_CHECK_EQUAL(s1.Size(), 3);
    BOOST_CHECK_EQUAL(s2.Size(), 2);
    s1.SwapWith(s2);
    BOOST_CHECK_EQUAL(s1.Size(), 2);
    BOOST_CHECK_EQUAL(s2.Size(), 3);
    BOOST_CHECK_EQUAL(s1[0],  4);
    BOOST_CHECK_EQUAL(s1[1],  5);
    BOOST_CHECK_EQUAL(s2[0],  1);
    BOOST_CHECK_EQUAL(s2[1],  2);
    BOOST_CHECK_EQUAL(s2[2],  3);
    s1.SwapWith(s2);
    BOOST_CHECK_EQUAL(s2.Size(), 2);
    BOOST_CHECK_EQUAL(s1.Size(), 3);
    BOOST_CHECK_EQUAL(s2[0],  4);
    BOOST_CHECK_EQUAL(s2[1],  5);
    BOOST_CHECK_EQUAL(s1[0],  1);
    BOOST_CHECK_EQUAL(s1[1],  2);
    BOOST_CHECK_EQUAL(s1[2],  3);
    s2.SwapWith(s1);
    BOOST_CHECK_EQUAL(s1.Size(), 2);
    BOOST_CHECK_EQUAL(s2.Size(), 3);
    BOOST_CHECK_EQUAL(s1[0],  4);
    BOOST_CHECK_EQUAL(s1[1],  5);
    BOOST_CHECK_EQUAL(s2[0],  1);
    BOOST_CHECK_EQUAL(s2[1],  2);
    BOOST_CHECK_EQUAL(s2[2],  3);
    s2.SwapWith(s1);
    BOOST_CHECK_EQUAL(s2.Size(), 2);
    BOOST_CHECK_EQUAL(s1.Size(), 3);
    BOOST_CHECK_EQUAL(s2[0],  4);
    BOOST_CHECK_EQUAL(s2[1],  5);
    BOOST_CHECK_EQUAL(s1[0],  1);
    BOOST_CHECK_EQUAL(s1[1],  2);
    BOOST_CHECK_EQUAL(s1[2],  3);
}


} // namespace

//----------------------------------------------------------------------------

