//----------------------------------------------------------------------------
/** @file SgGameReaderTest.cpp
    Unit tests for SgGameReader.

    The relative paths for SGF file resources used by the SgGameReader tests
    expect that the working directory is project_explorer/linux/test.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <sstream>
#include <boost/test/auto_unit_test.hpp>
#include "SgGameReader.h"
#include "SgNode.h"

using namespace std;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgGameReaderTest_InvalidBoardSize)
{
    istringstream in("(;FF[4]SZ[999]");
    BOOST_REQUIRE(in);
    SgGameReader reader(in);
    SgNode* root = reader.ReadGame();
    BOOST_REQUIRE(root != 0);
    BOOST_CHECK(reader.GetWarnings().test(SgGameReader::INVALID_BOARDSIZE));
    root->DeleteTree();
}

BOOST_AUTO_TEST_CASE(SgGameReaderTest_NoWarnings)
{
    istringstream
        in("(;FF[4]CA[ISO8859_1]GN[no-warnings]SZ[9]KM[6.5]"
           ";B[ee];W[ce];B[ec];W[dg];B[fg];W[gc];B[gb];W[fb];B[fc];W[hb]"
           ";B[gd];W[hc];B[hd];W[dh];B[cd];W[bd];B[de];W[cf];B[eh];W[cc]"
           ";B[eb];W[ga];B[dd];W[eg];B[fh];W[id];B[gf];W[if];B[ef];W[ei]"
           ";B[fi];W[di];B[hg];W[ea];B[da];W[ia];B[ig];W[fa];B[cb];W[bc]"
           ";B[hf];W[ie];B[he];W[ic];B[bb];W[ab];B[ba];W[dc];B[db];W[ac]"
           ";B[aa];W[ae];B[df];W[cg];B[tt];W[tt])");
    BOOST_REQUIRE(in);
    SgGameReader reader(in);
    SgNode* root = reader.ReadGame();
    BOOST_REQUIRE(root != 0);
    BOOST_CHECK(reader.GetWarnings().none());
    root->DeleteTree();
}

/** Test that points are interpreted correctly if SZ comes after point
    values.
    Allowed by the SGF standard (?); used by some programs.
*/
BOOST_AUTO_TEST_CASE(SgGameReaderTest_SizeAfterPoints)
{
    istringstream in("(;AB[aa][ab]SZ[9])");
    SgGameReader reader(in);
    SgNode* root = reader.ReadGame();
    BOOST_REQUIRE(root != 0);
    SgPropAddStone* prop =
        dynamic_cast<SgPropAddStone*>(root->Get(SG_PROP_ADD_BLACK));
    BOOST_REQUIRE(prop);
    BOOST_REQUIRE_EQUAL(prop->Value().Length(), 2);
    BOOST_REQUIRE(prop->Value().Contains(Pt(1, 9)));
    BOOST_REQUIRE(prop->Value().Contains(Pt(1, 8)));
    root->DeleteTree();
}

} // namespace

//----------------------------------------------------------------------------

