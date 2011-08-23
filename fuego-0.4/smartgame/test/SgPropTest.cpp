//----------------------------------------------------------------------------
/** @file SgPropTest.cpp
    Unit tests for classes in SgProp.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <sstream>
#include <boost/test/auto_unit_test.hpp>
#include "SgProp.h"

using namespace std;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

void SgPropMoveTest_ToString_Go(SgPoint p, int boardSize, int fileFormat,
                                const char* targetValue)
{
    vector<string> values;
    SgPropMove prop(SG_PROP_MOVE, p);
    prop.ToString(values, boardSize, SG_PROPPOINTFMT_GO, fileFormat);
    BOOST_REQUIRE_EQUAL(values.size(), 1u);
    BOOST_CHECK_EQUAL(values[0], targetValue);
}

BOOST_AUTO_TEST_CASE(SgPropMoveTest_ToString_GoAll)
{
    SgPropMoveTest_ToString_Go(Pt(1, 19), 19, 4, "aa");
    SgPropMoveTest_ToString_Go(Pt(19, 19), 19, 4, "sa");
    SgPropMoveTest_ToString_Go(Pt(1, 1), 19, 4, "as");
    SgPropMoveTest_ToString_Go(Pt(19, 1), 19, 4, "ss");
    SgPropMoveTest_ToString_Go(Pt(1, 9), 9, 4, "aa");
    SgPropMoveTest_ToString_Go(Pt(9, 9), 9, 4, "ia");
    SgPropMoveTest_ToString_Go(Pt(1, 1), 9, 4, "ai");
    SgPropMoveTest_ToString_Go(Pt(9, 1), 9, 4, "ii");
    SgPropMoveTest_ToString_Go(SG_PASS, 19, 3, "tt");
    SgPropMoveTest_ToString_Go(SG_PASS, 9, 3, "tt");
    SgPropMoveTest_ToString_Go(SG_PASS, 19, 4, "");
}

void SgPropMoveTest_ToString_Hex(SgPoint p, int boardSize,
                                 const char* targetValue)
{
    vector<string> values;
    SgPropMove prop(SG_PROP_MOVE, p);
    int fileFormat = 4;
    prop.ToString(values, boardSize, SG_PROPPOINTFMT_HEX, fileFormat);
    BOOST_REQUIRE_EQUAL(values.size(), 1u);
    BOOST_CHECK_EQUAL(values[0], targetValue);
}

BOOST_AUTO_TEST_CASE(SgPropMoveTest_ToString_HexAll)
{
    SgPropMoveTest_ToString_Hex(Pt(1, 19), 19, "a1");
    SgPropMoveTest_ToString_Hex(Pt(19, 19), 19, "s1");
    SgPropMoveTest_ToString_Hex(Pt(1, 1), 19, "a19");
    SgPropMoveTest_ToString_Hex(Pt(19, 1), 19, "s19");
    SgPropMoveTest_ToString_Hex(Pt(10, 10), 10, "j1");
    SgPropMoveTest_ToString_Hex(Pt(1, 9), 9, "a1");
    SgPropMoveTest_ToString_Hex(Pt(9, 9), 9, "i1");
    SgPropMoveTest_ToString_Hex(Pt(1, 1), 9, "a9");
    SgPropMoveTest_ToString_Hex(Pt(9, 1), 9, "i9");
}

void SgPropMoveTest_FromString_Go(string s, int boardSize,
                                  SgPoint targetValue)
{
    SgPropMove prop(SG_PROP_MOVE);
    vector<string> values(1, s);
    bool success = prop.FromString(values, boardSize, SG_PROPPOINTFMT_GO);
    BOOST_CHECK(success);
    ostringstream msg;
    msg << "Property value '" << s << "' [" << prop.Value() << "!= "
        << targetValue << "]";
    BOOST_CHECK_MESSAGE(prop.Value() == targetValue, msg.str());
}

BOOST_AUTO_TEST_CASE(SgPropMoveTest_FromString_GoAll)
{
    SgPropMoveTest_FromString_Go("aa", 19, Pt(1, 19));
    SgPropMoveTest_FromString_Go("sa", 19, Pt(19, 19));
    SgPropMoveTest_FromString_Go("as", 19, Pt(1, 1));
    SgPropMoveTest_FromString_Go("ss", 19, Pt(19, 1));
    SgPropMoveTest_FromString_Go("aa", 9, Pt(1, 9));
    SgPropMoveTest_FromString_Go("ia", 9, Pt(9, 9));
    SgPropMoveTest_FromString_Go("ai", 9, Pt(1, 1));
    SgPropMoveTest_FromString_Go("ii", 9, Pt(9, 1));
    SgPropMoveTest_FromString_Go("tt", 19, SG_PASS);
    SgPropMoveTest_FromString_Go("tt", 9, SG_PASS);
    SgPropMoveTest_FromString_Go("", 19, SG_PASS);
}

void SgPropMoveTest_FromString_Hex(string s, int boardSize,
                                   SgPoint targetValue)
{
    SgPropMove prop(SG_PROP_MOVE);
    vector<string> values(1, s);
    bool success = prop.FromString(values, boardSize, SG_PROPPOINTFMT_HEX);
    BOOST_CHECK(success);
    ostringstream msg;
    msg << "Property value '" << s << "' [" << prop.Value() << "!= "
        << targetValue << "]";
    BOOST_CHECK_MESSAGE(prop.Value() == targetValue, msg.str());
}

BOOST_AUTO_TEST_CASE(SgPropMoveTest_FromString_HexAll)
{
    SgPropMoveTest_FromString_Hex("a1", 19, Pt(1, 19));
    SgPropMoveTest_FromString_Hex("s1", 19, Pt(19, 19));
    SgPropMoveTest_FromString_Hex("a19", 19, Pt(1, 1));
    SgPropMoveTest_FromString_Hex("s19",19, Pt(19, 1));
    SgPropMoveTest_FromString_Hex("j1", 10, Pt(10, 10));
    SgPropMoveTest_FromString_Hex("a1", 9, Pt(1, 9));
    SgPropMoveTest_FromString_Hex("i1", 9, Pt(9, 9));
    SgPropMoveTest_FromString_Hex("a9", 9, Pt(1, 1));
    SgPropMoveTest_FromString_Hex("i9", 9, Pt(9, 1));
}

/** Test SgProp::Player() and SgProp::IsPlayer() */
BOOST_AUTO_TEST_CASE(SgPropMoveTest_Player)
{
    auto_ptr<SgProp> prop;
    prop.reset(SgProp::CreateProperty(SgProp::GetIDOfLabel("PB")));
    BOOST_CHECK_EQUAL(SG_BLACK, prop->Player());
    BOOST_CHECK(prop->IsPlayer(SG_BLACK));
    prop.reset(SgProp::CreateProperty(SgProp::GetIDOfLabel("PW")));
    BOOST_CHECK_EQUAL(SG_WHITE, prop->Player());
    BOOST_CHECK(prop->IsPlayer(SG_WHITE));
}

//----------------------------------------------------------------------------

void SgPropTextTest_ToString(const char* text, const char* targetValue)
{
    vector<string> values;
    SgPropText prop(SG_PROP_UNKNOWN, text);
    const int boardSize = 19;
    const int fileFormat = 3;
    prop.ToString(values, boardSize, SG_PROPPOINTFMT_GO, fileFormat);
    BOOST_REQUIRE_EQUAL(values.size(), 1u);
    BOOST_CHECK_EQUAL(values[0], targetValue);
}

BOOST_AUTO_TEST_CASE(SgPropTextTest_ToString_All)
{
    SgPropTextTest_ToString("abc", "abc");
    SgPropTextTest_ToString("ab]c", "ab\\]c");
    SgPropTextTest_ToString("ab\\c", "ab\\\\c");
}

BOOST_AUTO_TEST_CASE(SgPropTextListTest)
{
    SgPropTextList list(SG_PROP_UNKNOWN);
    SgPoint p = Pt(3,5);
    string s;
    bool hasString = list.GetStringAtPoint(p, &s);
    BOOST_CHECK_EQUAL(hasString, false);
    SgVector<SgPoint> pv = list.GetPointsWithText();
    BOOST_CHECK_EQUAL(pv.Length(), 0);
    list.AddStringAtPoint(p, "bla");
    hasString = list.GetStringAtPoint(p, &s);
    BOOST_CHECK_EQUAL(hasString, true);
    BOOST_CHECK_EQUAL(s, "bla");
    pv = list.GetPointsWithText();
    BOOST_CHECK_EQUAL(pv.Length(), 1);
    list.AppendToStringAtPoint(p, "blu");
    hasString = list.GetStringAtPoint(p, &s);
    BOOST_CHECK_EQUAL(hasString, true);
    BOOST_CHECK_EQUAL(s, "blablu");
    list.ClearStringAtPoint(p);
    hasString = list.GetStringAtPoint(p, &s);
    BOOST_CHECK_EQUAL(hasString, false);
    pv = list.GetPointsWithText();
    BOOST_CHECK_EQUAL(pv.Length(), 0);
    list.AppendToStringAtPoint(p, "blu");
    hasString = list.GetStringAtPoint(p, &s);
    BOOST_CHECK_EQUAL(hasString, true);
    BOOST_CHECK_EQUAL(s, "blu");
    pv = list.GetPointsWithText();
    BOOST_CHECK_EQUAL(pv.Length(), 1);
}


void SgPropRealTest_ToString(double value, const char* targetValue,
                             int precision)
{
    vector<string> values;
    SgPropReal prop(SG_PROP_UNKNOWN, value, precision);
    const int boardSize = 19;
    const int fileFormat = 3;
    prop.ToString(values, boardSize, SG_PROPPOINTFMT_GO, fileFormat);
    BOOST_REQUIRE_EQUAL(values.size(), 1u);
    BOOST_CHECK_EQUAL(values[0], targetValue);
}

BOOST_AUTO_TEST_CASE(SgPropRealTest_ToString_All)
{
    // 0: default precision for C++ streams (=6)
    SgPropRealTest_ToString(10.5, "10.500000", 0);
    SgPropRealTest_ToString(10.123456, "10.1", 1);
    SgPropRealTest_ToString(10.123456, "10.12346", 5);
}

//----------------------------------------------------------------------------

} // namespace

//----------------------------------------------------------------------------
