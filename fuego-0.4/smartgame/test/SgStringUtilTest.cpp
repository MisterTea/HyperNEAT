//----------------------------------------------------------------------------
/** @file SgStringUtilTest.cpp
    Unit tests for SgStringUtil.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgStringUtil.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgStringUtilTest_SplitArguments_1)
{
    vector<string> s
        = SgStringUtil::SplitArguments("one two \"three four\"");
    BOOST_CHECK_EQUAL(3u, s.size());
    BOOST_CHECK_EQUAL("one", s[0]);
    BOOST_CHECK_EQUAL("two", s[1]);
    BOOST_CHECK_EQUAL("three four", s[2]);
}

BOOST_AUTO_TEST_CASE(SgStringUtilTest_SplitArguments_2)
{
    vector<string> s
        = SgStringUtil::SplitArguments("one \"two \\\"three four\\\"\"");
    BOOST_CHECK_EQUAL(2u, s.size());
    BOOST_CHECK_EQUAL("one", s[0]);
    BOOST_CHECK_EQUAL("two \\\"three four\\\"", s[1]);
}

} // namespace

//----------------------------------------------------------------------------
