//----------------------------------------------------------------------------
/** @file SgHashTest.cpp
    Unit tests for SgHash.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgHash.h"

using namespace std;
using namespace SgHashUtil;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgHashCodeTest_Clear)
{
    SgHashCode hash = SgHashCode::Random();
    hash.Clear();
    BOOST_CHECK(hash.IsZero());
}

/** SgHashCode constructor test.
    Checks that hash code is initialized with zero.
*/
BOOST_AUTO_TEST_CASE(SgHashCodeTest_Constructor)
{
    SgHashCode hash;
    BOOST_CHECK(hash.IsZero());
}

BOOST_AUTO_TEST_CASE(SgHashCodeTest_Constructor2)
{
    SgHashCode hash1(12435);
    BOOST_CHECK(! hash1.IsZero());
    SgHashCode hash2(23456);
    BOOST_CHECK(! hash2.IsZero());
    BOOST_CHECK(hash1 != hash2);
}

BOOST_AUTO_TEST_CASE(SgHashCodeTest_Random)
{
    SgHashCode hash = SgHashCode::Random();
    BOOST_CHECK(! hash.IsZero());
}

BOOST_AUTO_TEST_CASE(SgHashCodeTest_Roll)
{
    SgHashCode hash(12345);
    for (int i = 0; i < SgHashCode::Size(); ++i)
    {
        SgHashCode newhash(12345);
        newhash.RollLeft(i);
        if (i > 0)
            BOOST_CHECK(hash != newhash);
        newhash.RollRight(i);
        BOOST_CHECK(hash == newhash);
    }
}

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(SgHashUtilTest_XorZobrist)
{
    SgHashCode origHash;
    SgHashCode hash(origHash);
    XorZobrist(hash, 5);
    BOOST_CHECK(hash != origHash);
    XorZobrist(hash, 3);
    BOOST_CHECK(hash != origHash);
    XorZobrist(hash, 10);
    BOOST_CHECK(hash != origHash);
    XorZobrist(hash, 10);
    BOOST_CHECK(hash != origHash);
    XorZobrist(hash, 3);
    BOOST_CHECK(hash != origHash);
    XorZobrist(hash, 5);
    BOOST_CHECK_EQUAL(hash, origHash);
}

BOOST_AUTO_TEST_CASE(SgHashUtilTest_XorInteger)
{
    SgHashCode origHash;
    SgHashCode hash(origHash);
    XorInteger(hash, 5);
    BOOST_CHECK(hash != origHash);
    XorInteger(hash, 3);
    BOOST_CHECK(hash != origHash);
    XorInteger(hash, 10);
    BOOST_CHECK(hash != origHash);
    XorInteger(hash, 10);
    BOOST_CHECK(hash != origHash);
    XorInteger(hash, 3);
    BOOST_CHECK(hash != origHash);
    XorInteger(hash, 5);
    BOOST_CHECK_EQUAL(hash, origHash);
}

//----------------------------------------------------------------------------

/** Entries in Zobrist table should be distinct and non-zero.
    Tests only consecutive values for distinctness, not all pairs.
*/
BOOST_AUTO_TEST_CASE(SgHashZobristTest_Initialization)
{
    SgHashZobrist<64> t;
    SgHash<64> emptyHash;
    SgHash<64> prev = t.Get(0);
    BOOST_CHECK(prev != emptyHash);
    for (int i = 1; i < SgHashZobrist<64>::MAX_HASH_INDEX; ++i)
    {
        SgHash<64> hash = t.Get(i);
        BOOST_CHECK(hash != prev);
        BOOST_CHECK(hash != emptyHash);
        prev = hash;
    }
}

//----------------------------------------------------------------------------

} // namespace

