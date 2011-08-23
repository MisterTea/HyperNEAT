//----------------------------------------------------------------------------
/** @file SgMathTest.cpp
    Unit tests for SgMath.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgMath.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgMathTest_RoundToInt)
{
    BOOST_CHECK_EQUAL(SgMath::RoundToInt(-0.8), -1);
    BOOST_CHECK_EQUAL(SgMath::RoundToInt(-0.3), 0);
    BOOST_CHECK_EQUAL(SgMath::RoundToInt(0.3), 0);
    BOOST_CHECK_EQUAL(SgMath::RoundToInt(0.8), 1);
}

} // namespace

//----------------------------------------------------------------------------

