//----------------------------------------------------------------------------
/** @file SgEBWArrayTest.cpp
    Unit tests for SgEBWArray.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgEBWArray.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgEBWArrayTest_Constructor1)
{
    SgEBWArray<int> array(1);
    BOOST_CHECK_EQUAL(array[SG_EMPTY], 1);
    BOOST_CHECK_EQUAL(array[SG_BLACK], 1);
    BOOST_CHECK_EQUAL(array[SG_WHITE], 1);
}

BOOST_AUTO_TEST_CASE(SgEBWArrayTest_Constructor2)
{
    SgEBWArray<int> array(1, 2, 3);
    BOOST_CHECK_EQUAL(array[SG_EMPTY], 1);
    BOOST_CHECK_EQUAL(array[SG_BLACK], 2);
    BOOST_CHECK_EQUAL(array[SG_WHITE], 3);
}

BOOST_AUTO_TEST_CASE(SgEBWArrayTest_Element)
{
    SgEBWArray<int> array;
    array[SG_EMPTY] = 1;
    array[SG_BLACK] = 2;
    array[SG_WHITE] = 3;
    BOOST_CHECK_EQUAL(array[SG_EMPTY], 1);
    BOOST_CHECK_EQUAL(array[SG_BLACK], 2);
    BOOST_CHECK_EQUAL(array[SG_WHITE], 3);
    const SgEBWArray<int>& constArray = array;
    BOOST_CHECK_EQUAL(constArray[SG_EMPTY], 1);
    BOOST_CHECK_EQUAL(constArray[SG_BLACK], 2);
    BOOST_CHECK_EQUAL(constArray[SG_WHITE], 3);
}

} // namespace

//----------------------------------------------------------------------------

