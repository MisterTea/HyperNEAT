//----------------------------------------------------------------------------
/** @file SgBWArrayTest.cpp
    Unit tests for SgBWArray.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgBWArray.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgBWArrayTest_Constructor1)
{
    SgBWArray<int> array(1);
    BOOST_CHECK_EQUAL(array[SG_BLACK], 1);
    BOOST_CHECK_EQUAL(array[SG_WHITE], 1);
}

BOOST_AUTO_TEST_CASE(SgBWArrayTest_Constructor2)
{
    SgBWArray<int> array(1, 2);
    BOOST_CHECK_EQUAL(array[SG_BLACK], 1);
    BOOST_CHECK_EQUAL(array[SG_WHITE], 2);
}

BOOST_AUTO_TEST_CASE(SgBWArrayTest_Element)
{
    SgBWArray<int> array;
    array[SG_BLACK] = 1;
    array[SG_WHITE] = 2;
    BOOST_CHECK_EQUAL(array[SG_BLACK], 1);
    BOOST_CHECK_EQUAL(array[SG_WHITE], 2);
    const SgBWArray<int>& constArray = array;
    BOOST_CHECK_EQUAL(constArray[SG_BLACK], 1);
    BOOST_CHECK_EQUAL(constArray[SG_WHITE], 2);
}

} // namespace

//----------------------------------------------------------------------------

