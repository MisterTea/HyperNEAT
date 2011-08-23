//----------------------------------------------------------------------------
/** @file GoGtpTimeSettingsTest.cpp
    Unit tests for GoGtpTimeSettings.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoGtpTimeSettings.h"

#include <boost/test/auto_unit_test.hpp>

using namespace std;

//----------------------------------------------------------------------------

namespace {

/** Test if no time limits is properly encoded as defined in GTP standard. */
BOOST_AUTO_TEST_CASE(GoGtpTimeSettingsTest_DefaultConstructor)
{
    GoGtpTimeSettings timeSettings;
    BOOST_CHECK(timeSettings.ByoYomiTime() > 0);
    BOOST_CHECK_EQUAL(timeSettings.ByoYomiStones(), 0);    
}

BOOST_AUTO_TEST_CASE(GoGtpTimeSettingsTest_NoTimeLimits)
{
    GoGtpTimeSettings timeSettings;
    BOOST_CHECK(timeSettings.NoTimeLimits());
}

} // namespace

//----------------------------------------------------------------------------

