//----------------------------------------------------------------------------
/** @file GoUtilTest.cpp
    Unit tests for GoUtil.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoUtil.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(GoUtilTest_ScoreToString)
{
    BOOST_CHECK_EQUAL("0", GoUtil::ScoreToString(0));
    BOOST_CHECK_EQUAL("B+31", GoUtil::ScoreToString(31));
    BOOST_CHECK_EQUAL("W+2.5", GoUtil::ScoreToString(-2.5));
}

//----------------------------------------------------------------------------

} // namespace
