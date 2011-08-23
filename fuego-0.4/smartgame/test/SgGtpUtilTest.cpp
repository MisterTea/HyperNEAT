//----------------------------------------------------------------------------
/** @file SgGtpUtilTest.cpp
    Unit tests for GtpSgUtil.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgGtpUtil.h"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "GtpEngine.h"
#include "SgPoint.h"
#include "SgPointSet.h"

using namespace std;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgGtpUtilTest_RespondPointSet)
{
    SgPointSet pointSet;
    pointSet.Include(Pt(1, 1));
    pointSet.Include(Pt(1, 2));
    pointSet.Include(Pt(2, 1));
    GtpCommand cmd;
    SgGtpUtil::RespondPointSet(cmd, pointSet);
    BOOST_CHECK_EQUAL(cmd.Response(), "A1 B1 A2");
}

} // namespace

//----------------------------------------------------------------------------

