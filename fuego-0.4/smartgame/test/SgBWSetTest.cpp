//----------------------------------------------------------------------------
/** @file SgBWSetTest.cpp
    Unit tests for SgBWSet.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <sstream>
#include <boost/test/auto_unit_test.hpp>
#include "SgBWSet.h"

using namespace std;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgBWSetTest_Equals)
{
    SgBWSet set1;
    SgBWSet set2;
    SgPoint p = Pt(5, 5);
    BOOST_CHECK_EQUAL(set1, set2);
    set1[SG_BLACK].Include(p);
    BOOST_CHECK(set1 != set2);
    set2[SG_WHITE].Include(p);
    BOOST_CHECK(set1 != set2);
    set1[SG_WHITE].Include(p);
    BOOST_CHECK(set1 != set2);
    set2[SG_BLACK].Include(p);
    BOOST_CHECK_EQUAL(set1, set2);
}

} // namespace

//----------------------------------------------------------------------------

