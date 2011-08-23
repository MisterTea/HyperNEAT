//----------------------------------------------------------------------------
/** @file SgPointArrayTest.cpp
    Unit tests for SgPointArray.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgPointArray.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

/** Test constructor of SgPointArray<int>(int).
    Tests that all elements are initialized with 0.
*/
BOOST_AUTO_TEST_CASE(SgArrayTestConstructorDefault)
{
    // Use placement new to avoid memory accidentally initialized with 0
    const size_t size = sizeof(SgPointArray<int>);
    char buffer[size];
    for (size_t i = 0; i < size; ++i)
        buffer[i] = 1;
    SgPointArray<int>* array = new (buffer) SgPointArray<int>(0);
    BOOST_CHECK_EQUAL((*array)[0], 0);
    BOOST_CHECK_EQUAL((*array)[SG_MAXPOINT - 1], 0);
}

} // namespace

//----------------------------------------------------------------------------

