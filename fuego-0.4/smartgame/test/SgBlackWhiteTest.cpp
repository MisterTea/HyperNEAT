//----------------------------------------------------------------------------
/** @file SgBlackWhiteTest.cpp
    Unit tests for SgColorIterator.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgBlackWhite.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgBWIteratorTest)
{
    SgBWIterator i;
    BOOST_CHECK(i);
    BOOST_CHECK_EQUAL(*i, SG_BLACK);
    ++i;
    BOOST_CHECK(i);
    BOOST_CHECK_EQUAL(*i, SG_WHITE);
    ++i;
    BOOST_CHECK(! i);
}

} // namespace

//----------------------------------------------------------------------------

