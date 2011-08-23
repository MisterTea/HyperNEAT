//----------------------------------------------------------------------------
/** @file GoBookTest.cpp
    Unit tests for GoBook.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <sstream>
#include <boost/test/auto_unit_test.hpp>
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoBook.h"

using namespace std;
using GoBoardUtil::UndoAll;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(GoBookTest_Add)
{
    istringstream in("9 C3 C7 E5 | G7\n");
    GoBook book;
    book.Read(in);
    GoBoard bd(9);

    bd.Play(Pt(7, 3));
    bd.Play(Pt(3, 3));
    bd.Play(Pt(5, 5));
    book.Add(bd, Pt(7, 7));

    UndoAll(bd);
    bd.Play(Pt(3, 3));
    bd.Play(Pt(3, 7));
    bd.Play(Pt(5, 5));

    vector<SgPoint> moves = book.LookupAllMoves(bd);
    BOOST_REQUIRE_EQUAL(moves.size(), 2u);
    BOOST_CHECK_EQUAL(moves[0], Pt(7, 7));
    BOOST_CHECK_EQUAL(moves[1], Pt(7, 3));
}

BOOST_AUTO_TEST_CASE(GoBookTest_Add_NewPosition)
{
    GoBook book;
    GoBoard bd(9);

    bd.Play(Pt(7, 3));
    bd.Play(Pt(3, 3));
    bd.Play(Pt(5, 5));
    book.Add(bd, Pt(7, 7));

    UndoAll(bd);
    bd.Play(Pt(3, 3));
    bd.Play(Pt(3, 7));
    bd.Play(Pt(5, 5));

    vector<SgPoint> moves = book.LookupAllMoves(bd);
    BOOST_REQUIRE_EQUAL(moves.size(), 1u);
    BOOST_CHECK_EQUAL(moves[0], Pt(7, 3));
}

BOOST_AUTO_TEST_CASE(GoBookTest_Delete)
{
    istringstream in("9 C3 C7 E5 | G3 G7\n");
    GoBook book;
    book.Read(in);
    GoBoard bd(9);

    bd.Play(Pt(7, 3));
    bd.Play(Pt(3, 3));
    bd.Play(Pt(5, 5));
    book.Delete(bd, Pt(7, 7));

    UndoAll(bd);
    bd.Play(Pt(3, 3));
    bd.Play(Pt(3, 7));
    bd.Play(Pt(5, 5));

    vector<SgPoint> moves = book.LookupAllMoves(bd);
    BOOST_REQUIRE_EQUAL(moves.size(), 1u);
    BOOST_CHECK_EQUAL(moves[0], Pt(7, 7));
}

/** Test that a position in a book matches rotated/mirrored/transposed
    positions.
*/
BOOST_AUTO_TEST_CASE(GoBookTest_RotatedAndTransposedMatches)
{
    istringstream in("9 C3 C7 E5 | G3 G7");
    GoBook book;
    book.Read(in);
    GoBoard bd(9);
    vector<SgPoint> moves;

    bd.Play(Pt(3, 3));
    bd.Play(Pt(3, 7));
    bd.Play(Pt(5, 5));
    moves = book.LookupAllMoves(bd);
    BOOST_REQUIRE_EQUAL(moves.size(), 2u);
    BOOST_CHECK_EQUAL(moves[0], Pt(7, 3));
    BOOST_CHECK_EQUAL(moves[1], Pt(7, 7));

    UndoAll(bd);
    bd.Play(Pt(7, 3));
    bd.Play(Pt(3, 3));
    bd.Play(Pt(5, 5));
    moves = book.LookupAllMoves(bd);
    BOOST_REQUIRE_EQUAL(moves.size(), 2u);
    BOOST_CHECK_EQUAL(moves[0], Pt(7, 7));
    BOOST_CHECK_EQUAL(moves[1], Pt(3, 7));

    // @todo Add more transformations and transpositions
}

/** Test that positions are matched including the color to play. */
BOOST_AUTO_TEST_CASE(GoBookTest_MatchInclToPlay)
{
    istringstream in("9 | E5");
    GoBook book;
    book.Read(in);
    GoBoard bd(9);
    vector<SgPoint> moves;
    moves = book.LookupAllMoves(bd);
    BOOST_REQUIRE_EQUAL(moves.size(), 1u);
    BOOST_CHECK_EQUAL(moves[0], Pt(5, 5));
    bd.SetToPlay(SG_WHITE);
    moves = book.LookupAllMoves(bd);
    BOOST_REQUIRE_EQUAL(moves.size(), 0u);
}

} // namespace

//----------------------------------------------------------------------------
