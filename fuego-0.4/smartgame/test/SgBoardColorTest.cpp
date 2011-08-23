//----------------------------------------------------------------------------
/** @file SgBoardColorTest.cpp
    Unit tests for SgColorIterator.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgBoardColor.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgEBWIteratorTest)
{
    SgEBWIterator i;
    BOOST_CHECK(i);
    BOOST_CHECK_EQUAL(*i, SG_BLACK);
    ++i;
    BOOST_CHECK(i);
    BOOST_CHECK_EQUAL(*i, SG_WHITE);
    ++i;
    BOOST_CHECK(i);
    BOOST_CHECK_EQUAL(*i, SG_EMPTY);
    ++i;
    BOOST_CHECK(! i);
}

} // namespace

//----------------------------------------------------------------------------

