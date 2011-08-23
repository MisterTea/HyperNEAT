//----------------------------------------------------------------------------
/** @file GoKomiTest.cpp
    Unit tests for GoKomi.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include "GoKomi.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(GoKomiTest_Assign)
{
    GoKomi komi;
    BOOST_CHECK_CLOSE((komi = GoKomi(0)).ToFloat(), 0.f, 1e-3f);
    BOOST_CHECK_CLOSE((komi = GoKomi(6.5)).ToFloat(), 6.5f, 1e-3f);
    BOOST_CHECK_CLOSE((komi = GoKomi(-1)).ToFloat(), -1.f, 1e-3f);
    BOOST_CHECK((komi = GoKomi()).IsUnknown());
}

BOOST_AUTO_TEST_CASE(GoKomiTest_Construct)
{
    BOOST_CHECK_CLOSE(GoKomi(0).ToFloat(), 0.f, 1e-3f);
    BOOST_CHECK_CLOSE(GoKomi("0").ToFloat(), 0.f, 1e-3f);
    BOOST_CHECK_CLOSE(GoKomi(" 0  ").ToFloat(), 0.f, 1e-3f);
    BOOST_CHECK_CLOSE(GoKomi("0.5").ToFloat(), 0.5f, 1e-3f);
    BOOST_CHECK_CLOSE(GoKomi("  6.5 ").ToFloat(), 6.5f, 1e-3f);
    BOOST_CHECK_CLOSE(GoKomi("-1").ToFloat(), -1.f, 1e-3f);
}

BOOST_AUTO_TEST_CASE(GoKomiTest_Equals)
{
    BOOST_CHECK(GoKomi() == GoKomi());
    BOOST_CHECK(GoKomi(0) == GoKomi("0"));
    BOOST_CHECK(GoKomi("6.5") == GoKomi(6.5));
    BOOST_CHECK(GoKomi() != GoKomi(0));
    BOOST_CHECK(GoKomi(5) != GoKomi(0.5));
    BOOST_CHECK(GoKomi(0) != GoKomi(0.5));
    BOOST_CHECK(GoKomi(-1) != GoKomi(0.5));
}

BOOST_AUTO_TEST_CASE(GoKomiTest_Invalid)
{
    BOOST_CHECK_THROW(GoKomi("foo"), GoKomi::InvalidKomi);
}

BOOST_AUTO_TEST_CASE(GoKomiTest_IsUnknown)
{
    BOOST_CHECK(GoKomi().IsUnknown());
    BOOST_CHECK(GoKomi("").IsUnknown());
    BOOST_CHECK(GoKomi("  ").IsUnknown());
    BOOST_CHECK(! GoKomi(0).IsUnknown());
    BOOST_CHECK(! GoKomi(6.5).IsUnknown());
    BOOST_CHECK(! GoKomi(-1.5).IsUnknown());
}

BOOST_AUTO_TEST_CASE(GoKomiTest_ToFloat)
{
    BOOST_CHECK_CLOSE(GoKomi().ToFloat(), 0.f, 1e-3f);
    BOOST_CHECK_CLOSE(GoKomi(0).ToFloat(), 0.f, 1e-3f);
    BOOST_CHECK_CLOSE(GoKomi(0.5).ToFloat(), 0.5f, 1e-3f);
    BOOST_CHECK_CLOSE(GoKomi(5).ToFloat(), 5.f, 1e-3f);
    BOOST_CHECK_CLOSE(GoKomi(6.5).ToFloat(), 6.5f, 1e-3f);
    BOOST_CHECK_CLOSE(GoKomi(-0.5).ToFloat(), -0.5f, 1e-3f);
    BOOST_CHECK_CLOSE(GoKomi(-1).ToFloat(), -1.f, 1e-3f);
    BOOST_CHECK_CLOSE(GoKomi(-1.5).ToFloat(), -1.5f, 1e-3f);
}

BOOST_AUTO_TEST_CASE(GoKomiTest_ToString)
{
    BOOST_CHECK_EQUAL(GoKomi().ToString(), "");
    BOOST_CHECK_EQUAL(GoKomi(0).ToString(), "0");
    BOOST_CHECK_EQUAL(GoKomi(0.5).ToString(), "0.5");
    BOOST_CHECK_EQUAL(GoKomi(5).ToString(), "5");
    BOOST_CHECK_EQUAL(GoKomi(6.5).ToString(), "6.5");
    BOOST_CHECK_EQUAL(GoKomi(-1.5).ToString(), "-1.5");
    BOOST_CHECK_EQUAL(GoKomi(-0.5).ToString(), "-0.5");
}

//----------------------------------------------------------------------------

} // namespace
