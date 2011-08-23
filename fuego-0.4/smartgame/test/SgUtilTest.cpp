//----------------------------------------------------------------------------
/** @file SgUtilTest.cpp
    Unit tests for SgUtil.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgUtil.h"

using namespace std;
using namespace SgUtil;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgUtil_ForceInRange)
{
    int i(1200);
    ForceInRange(0,&i,2000);
    BOOST_CHECK_EQUAL(i, 1200);
    ForceInRange(0,&i,1000);
    BOOST_CHECK_EQUAL(i, 1000);
    ForceInRange(5000,&i,10000);
    BOOST_CHECK_EQUAL(i, 5000);
    
    double d(1.5);
    const double epsilon(1.0e-6);
    ForceInRange(0.0,&d,2.0);
    BOOST_CHECK(d < 1.5 + epsilon);
    BOOST_CHECK(d > 1.5 - epsilon);
    ForceInRange(0.0,&d,1.0);
    BOOST_CHECK(d < 1.0 + epsilon);
    BOOST_CHECK(d > 1.0 - epsilon);
}

BOOST_AUTO_TEST_CASE(SgUtil_InRange)
{
    int a(1), b(2), c(3);
    BOOST_CHECK(InRange(b, a, c));
    BOOST_CHECK(! InRange(a, b, c));
    BOOST_CHECK(! InRange(c, a, b));
}

BOOST_AUTO_TEST_CASE(SgBalancer_1)
{
    SgBalancer b(3);
    BOOST_CHECK(b.Play(SG_BLACK));
    BOOST_CHECK(b.Play(SG_BLACK));
    BOOST_CHECK(b.Play(SG_BLACK));
    BOOST_CHECK(! b.Play(SG_BLACK));
    BOOST_CHECK_EQUAL(b.Balance(), 3);
    BOOST_CHECK_EQUAL(b.NuPlayed(SG_BLACK), 3);
    BOOST_CHECK_EQUAL(b.NuRejected(SG_BLACK), 1);
    BOOST_CHECK(b.Play(SG_WHITE));
    BOOST_CHECK(b.Play(SG_BLACK));
    BOOST_CHECK(! b.Play(SG_BLACK));
    BOOST_CHECK_EQUAL(b.Balance(), 3);
    BOOST_CHECK_EQUAL(b.NuPlayed(SG_BLACK), 4);
    BOOST_CHECK_EQUAL(b.NuRejected(SG_BLACK), 2);
    BOOST_CHECK_EQUAL(b.NuPlayed(SG_WHITE), 1);
    BOOST_CHECK_EQUAL(b.NuRejected(SG_WHITE), 0);
    BOOST_CHECK(b.Play(SG_WHITE));
    BOOST_CHECK(b.Play(SG_WHITE));
    BOOST_CHECK(b.Play(SG_WHITE));
    BOOST_CHECK(b.Play(SG_WHITE));
    BOOST_CHECK(b.Play(SG_WHITE));
    BOOST_CHECK(b.Play(SG_WHITE));
    BOOST_CHECK(! b.Play(SG_WHITE));
    BOOST_CHECK_EQUAL(b.NuPlayed(SG_WHITE), 7);
    BOOST_CHECK_EQUAL(b.NuRejected(SG_WHITE), 1);
}

} // namespace

//----------------------------------------------------------------------------

