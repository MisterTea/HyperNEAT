//----------------------------------------------------------------------------
/** @file GoBoardTest.cpp
    Unit tests for GoBoard.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "GoBoard.h"
#include "GoSetupUtil.h"
#include "SgWrite.h"

using namespace std;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

/** Test constructor with GoSetup argument. */
BOOST_AUTO_TEST_CASE(GoBoardTest_Constructor_Setup)
{
    GoSetup setup;
    setup.m_player = SG_WHITE;
    setup.AddWhite(Pt(1, 2));
    setup.AddWhite(Pt(2, 1));
    setup.AddWhite(Pt(2, 2));
    GoBoard bd(9, setup);
    BOOST_CHECK_EQUAL(bd.ToPlay(), SG_WHITE);
    BOOST_CHECK_EQUAL(bd.TotalNumStones(SG_WHITE), 3);
    BOOST_CHECK_EQUAL(bd.TotalNumStones(SG_BLACK), 0);
    BOOST_CHECK_EQUAL(bd.NumStones(Pt(1, 2)), 3);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 2)), 5);
    bd.CheckConsistency();
}

BOOST_AUTO_TEST_CASE(GoBoardTest_Anchor)
{
    GoSetup setup;
    setup.AddBlack(Pt(4, 1));
    setup.AddBlack(Pt(4, 2));
    setup.AddBlack(Pt(3, 2));
    setup.AddBlack(Pt(1, 2));
    GoBoard bd(19, setup);
    // Check that anchor returns smallest point
    BOOST_CHECK_EQUAL(bd.Anchor(Pt(4, 1)), Pt(4, 1));
    BOOST_CHECK_EQUAL(bd.Anchor(Pt(4, 2)), Pt(4, 1));
    BOOST_CHECK_EQUAL(bd.Anchor(Pt(3, 2)), Pt(4, 1));
    BOOST_CHECK_EQUAL(bd.Anchor(Pt(1, 2)), Pt(1, 2));
    bd.Play(Pt(2, 2), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.Anchor(Pt(4, 1)), Pt(4, 1));
    BOOST_CHECK_EQUAL(bd.Anchor(Pt(4, 2)), Pt(4, 1));
    BOOST_CHECK_EQUAL(bd.Anchor(Pt(3, 2)), Pt(4, 1));
    BOOST_CHECK_EQUAL(bd.Anchor(Pt(2, 2)), Pt(4, 1));
    BOOST_CHECK_EQUAL(bd.Anchor(Pt(1, 2)), Pt(4, 1));
}

/** Test GoBoard::Anchor if block-merging stone is new smallest point. */
BOOST_AUTO_TEST_CASE(GoBoardTest_Anchor_2)
{
    GoSetup setup;
    setup.AddBlack(Pt(1, 2));
    setup.AddBlack(Pt(2, 1));
    GoBoard bd(19, setup);
    BOOST_CHECK_EQUAL(bd.Anchor(Pt(1, 2)), Pt(1, 2));
    BOOST_CHECK_EQUAL(bd.Anchor(Pt(2, 1)), Pt(2, 1));
    bd.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.Anchor(Pt(1, 1)), Pt(1, 1));
    BOOST_CHECK_EQUAL(bd.Anchor(Pt(1, 2)), Pt(1, 1));
    BOOST_CHECK_EQUAL(bd.Anchor(Pt(2, 1)), Pt(1, 1));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_CanCapture)
{
    std::string s("XO..O.\n"
                  ".XOO..\n"
                  "......\n"
                  "......\n"
                  "......\n"
                  "......");
    int boardSize;
    GoSetup setup = GoSetupUtil::CreateSetupFromString(s, boardSize);
    setup.m_player = SG_BLACK;
    GoBoard bd(boardSize, setup);
    BOOST_CHECK(bd.IsColor(Pt(1, 6), SG_BLACK));
    BOOST_CHECK(bd.IsColor(Pt(2, 6), SG_WHITE));
    BOOST_CHECK(bd.CanCapture(Pt(3, 6), SG_BLACK));
    BOOST_CHECK(! bd.CanCapture(Pt(3, 6), SG_WHITE));
    BOOST_CHECK(! bd.CanCapture(Pt(4, 6), SG_BLACK));
    BOOST_CHECK(! bd.CanCapture(Pt(1, 5), SG_BLACK));
    BOOST_CHECK(bd.CanCapture(Pt(1, 5), SG_WHITE));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_CanUndo)
{
    GoBoard bd;
    BOOST_CHECK(! bd.CanUndo());
    bd.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK(bd.CanUndo());
    bd.Undo();
    BOOST_CHECK(! bd.CanUndo());
}

/** Test GoBoard::CapturedStones and GoBoard::NuCapturedStones and
    GoBoard::CapturingMove.
    Tests the following move sequence:
    @verbatim
    . . .
    O2. .
    X1O3.  X4 at 1
    @endverbatim
*/
BOOST_AUTO_TEST_CASE(GoBoardTest_CapturedStones)
{
    GoBoard bd;
    bd.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK(! bd.CapturingMove());
    BOOST_CHECK_EQUAL(bd.NuCapturedStones(), 0);
    BOOST_CHECK_EQUAL(bd.CapturedStones().Length(), 0);
    bd.Play(Pt(1, 2), SG_WHITE);
    BOOST_CHECK(! bd.CapturingMove());
    BOOST_CHECK_EQUAL(bd.NuCapturedStones(), 0);
    BOOST_CHECK_EQUAL(bd.CapturedStones().Length(), 0);
    bd.Play(Pt(2, 1), SG_WHITE);
    BOOST_CHECK(bd.CapturingMove());
    BOOST_CHECK_EQUAL(bd.NuCapturedStones(), 1);
    BOOST_CHECK(bd.CapturedStones().Contains(Pt(1, 1)));
    bd.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK(bd.CapturingMove());
    BOOST_CHECK_EQUAL(bd.NuCapturedStones(), 1);
    BOOST_CHECK(bd.CapturedStones().Contains(Pt(1, 1)));
}

/** Check default settings in GoBoard.
    Checks the default values of settings in GoBoard that some other code
    does rely on.
*/
BOOST_AUTO_TEST_CASE(GoBoardTest_Defaults)
{
    GoBoard bd;
    BOOST_CHECK(bd.KoModifiesHash());
    BOOST_CHECK(! bd.KoRepetitionAllowed());
    BOOST_CHECK(! bd.AnyRepetitionAllowed());
}

/** Play and undo some moves and remember and compare the hash code. */
BOOST_AUTO_TEST_CASE(GoBoardTest_GetHashCode)
{
    GoBoard bd(19);
    bd.Play(Pt(1, 1), SG_BLACK);
    SgHashCode h1 = bd.GetHashCode();
    bd.Play(Pt(1, 2), SG_WHITE);
    SgHashCode h2 = bd.GetHashCode();
    BOOST_CHECK(h2 != h1);
    // capture
    bd.Play(Pt(2, 1), SG_WHITE);
    SgHashCode h3 = bd.GetHashCode();
    BOOST_CHECK(h3 != h1);
    BOOST_CHECK(h3 != h2);
    bd.Play(SG_PASS, SG_BLACK);
    SgHashCode h4 = bd.GetHashCode();
    BOOST_CHECK(h4 == h3);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.GetHashCode(), h3);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.GetHashCode(), h2);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.GetHashCode(), h1);
}

/** Tests that the hash code is 0 for the empty positions.
    If this is changed later, make sure no code relies on that fact.
*/
BOOST_AUTO_TEST_CASE(GoBoardTest_GetHashCode_EmptyPosition)
{
    {
        GoBoard bd(2);
        BOOST_CHECK(bd.GetHashCode().IsZero());
    }
    {
        GoBoard bd(8);
        BOOST_CHECK(bd.GetHashCode().IsZero());
    }
    {
        GoBoard bd(9);
        BOOST_CHECK(bd.GetHashCode().IsZero());
    }
    {
        GoBoard bd(19);
        BOOST_CHECK(bd.GetHashCode().IsZero());
    }
}

/** Play and undo some moves and remember and compare the hash code
    including to play.
*/
BOOST_AUTO_TEST_CASE(GoBoardTest_GetHashCodeInclToPlay)
{
    GoBoard bd(19);
    bd.Play(Pt(1, 1), SG_BLACK);
    SgHashCode h1 = bd.GetHashCodeInclToPlay();
    bd.Play(Pt(1, 2), SG_WHITE);
    SgHashCode h2 = bd.GetHashCodeInclToPlay();
    BOOST_CHECK(h2 != h1);
    // capture
    bd.Play(Pt(2, 1), SG_WHITE);
    SgHashCode h3 = bd.GetHashCodeInclToPlay();
    BOOST_CHECK(h3 != h1);
    BOOST_CHECK(h3 != h2);
    bd.Play(SG_PASS, SG_BLACK);
    SgHashCode h4 = bd.GetHashCodeInclToPlay();
    BOOST_CHECK(h4 != h1);
    BOOST_CHECK(h4 != h2);
    BOOST_CHECK(h4 != h3);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.GetHashCodeInclToPlay(), h3);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.GetHashCodeInclToPlay(), h2);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.GetHashCodeInclToPlay(), h1);
}

/** Test GoBoard::GetLastMove() and GoBoard::Get2ndLastMove() */
BOOST_AUTO_TEST_CASE(GoBoardTest_GetLastMove)
{
    GoBoard bd(19);
    BOOST_CHECK_EQUAL(bd.GetLastMove(), SG_NULLMOVE);
    BOOST_CHECK_EQUAL(bd.Get2ndLastMove(), SG_NULLMOVE);
    bd.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.GetLastMove(), Pt(1, 1));
    BOOST_CHECK_EQUAL(bd.Get2ndLastMove(), SG_NULLMOVE);
    bd.Play(Pt(2, 2), SG_WHITE);
    BOOST_CHECK_EQUAL(bd.GetLastMove(), Pt(2, 2));
    BOOST_CHECK_EQUAL(bd.Get2ndLastMove(), Pt(1, 1));
    bd.Play(SG_PASS, SG_BLACK);
    BOOST_CHECK_EQUAL(bd.GetLastMove(), SG_PASS);
    BOOST_CHECK_EQUAL(bd.Get2ndLastMove(), Pt(2, 2));
    bd.SetToPlay(SG_BLACK);
    BOOST_CHECK_EQUAL(bd.GetLastMove(), SG_NULLMOVE);
    BOOST_CHECK_EQUAL(bd.Get2ndLastMove(), SG_NULLMOVE);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.GetLastMove(), Pt(2, 2));
    BOOST_CHECK_EQUAL(bd.Get2ndLastMove(), Pt(1, 1));
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.GetLastMove(), Pt(1, 1));
    BOOST_CHECK_EQUAL(bd.Get2ndLastMove(), SG_NULLMOVE);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.GetLastMove(), SG_NULLMOVE);
    BOOST_CHECK_EQUAL(bd.Get2ndLastMove(), SG_NULLMOVE);
}

BOOST_AUTO_TEST_CASE(GoBoardTest_InAtari)
{
    GoSetup setup;
    setup.AddBlack(Pt(1, 1));
    GoBoard bd(19, setup);
    BOOST_CHECK(! bd.InAtari(Pt(1, 1)));
    setup.AddWhite(Pt(1, 2));
    bd.Init(19, setup);
    bd.SetToPlay(SG_BLACK);
    BOOST_CHECK(bd.InAtari(Pt(1, 1)));
    bd.SetToPlay(SG_WHITE);
    BOOST_CHECK(bd.InAtari(Pt(1, 1)));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_Init)
{
    GoBoard bd(19);
    bd.Play(Pt(3, 3), SG_BLACK);
    bd.Init(9);
    BOOST_CHECK_EQUAL(bd.Size(), 9);
    BOOST_CHECK(bd.IsBorder(Pt(10, 10)));
    for (GoBoard::Iterator it(bd); it; ++it)
        BOOST_CHECK_MESSAGE(bd.GetColor(*it) == SG_EMPTY,
                            "Point " << SgWritePoint(*it) << " not empty");
}

BOOST_AUTO_TEST_CASE(GoBoardTest_IsFirst)
{
    GoBoard bd;
    BOOST_CHECK(bd.IsFirst(Pt(1, 1)));
    bd.Play(Pt(2, 2), SG_BLACK);
    BOOST_CHECK(bd.IsFirst(Pt(1, 1)));
    bd.Play(Pt(2, 1), SG_WHITE);
    bd.Play(Pt(3, 1), SG_BLACK);
    bd.Play(Pt(1, 2), SG_WHITE);
    bd.Play(Pt(1, 1), SG_BLACK);
    bd.Play(SG_PASS, SG_WHITE);
    bd.Play(SG_PASS, SG_BLACK);
    bd.Play(Pt(2, 1), SG_WHITE);
    BOOST_CHECK(! bd.IsFirst(Pt(1, 1)));
    bd.Play(SG_PASS, SG_BLACK);
    bd.Play(SG_PASS, SG_WHITE);
    BOOST_CHECK(! bd.IsFirst(Pt(1, 1)));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_IsLegal)
{
    GoBoard bd;
    BOOST_CHECK(bd.IsLegal(Pt(1, 1), SG_BLACK));
    BOOST_CHECK(bd.IsLegal(Pt(1, 1), SG_WHITE));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_IsLegal_Pass)
{
    GoBoard bd;
    BOOST_CHECK(bd.IsLegal(SG_PASS, SG_BLACK));
    BOOST_CHECK(bd.IsLegal(SG_PASS, SG_WHITE));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_IsLegal_Ko)
{
    GoSetup setup;
    setup.AddWhite(Pt(1, 1));
    setup.AddWhite(Pt(2, 2));
    setup.AddWhite(Pt(3, 1));
    setup.AddBlack(Pt(1, 2));
    GoBoard bd(19, setup);
    bd.Play(Pt(2, 1), SG_BLACK);
    BOOST_CHECK(! bd.IsLegal(Pt(1, 1)));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_IsLegal_KoRepetition)
{
    GoSetup setup;
    setup.AddWhite(Pt(1, 2));
    setup.AddWhite(Pt(2, 1));
    setup.AddBlack(Pt(2, 2));
    setup.AddBlack(Pt(3, 1));
    GoBoard bd(19, setup);
    BOOST_REQUIRE(! bd.KoRepetitionAllowed());
    bd.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK(! bd.IsLegal(Pt(2, 1), SG_WHITE));
    bd.AllowKoRepetition(true);
    BOOST_CHECK(bd.IsLegal(Pt(2, 1), SG_WHITE));
}

/** Test for GoBoard::IsIllegal for Ko move.
    Test for catching a bug that occured in the incremental update of the
    illegal ko point.
*/
BOOST_AUTO_TEST_CASE(GoBoardTest_IsLegal_KoRepetition_2)
{
    GoSetup setup;
    setup.AddWhite(Pt(1, 2));
    setup.AddWhite(Pt(2, 1));
    setup.AddBlack(Pt(2, 2));
    setup.AddBlack(Pt(3, 1));
    GoBoard bd(19, setup);
    BOOST_REQUIRE(! bd.KoRepetitionAllowed());
    bd.Rules().SetKoRule(GoRules::SIMPLEKO);
    bd.Play(Pt(1, 1), SG_BLACK);
    // Now the illegal ko point is (2, 1)
    BOOST_CHECK(! bd.IsLegal(Pt(2, 1), SG_WHITE));
    // Play an unrelated move and undo:
    bd.Play(Pt(5, 5), SG_WHITE);
    bd.Undo();
    // (2, 1) should still be illegal
    BOOST_CHECK(! bd.IsLegal(Pt(2, 1), SG_WHITE));
}

/** Test for GoBoard::IsIllegal for Ko move.
    Checks that a capture-back move with the capturing move resulting in a
    single-stone block with more than one liberty does not violate Ko.
    This bug occured in the incremental update of the illegal Ko point.
*/
BOOST_AUTO_TEST_CASE(GoBoardTest_IsLegal_KoRepetition_3)
{
    // |. O O O O .
    // |O O . X O .
    // |. X O X O .
    // +-----------
    GoSetup setup;
    setup.AddWhite(Pt(1, 2));
    setup.AddWhite(Pt(2, 2));
    setup.AddWhite(Pt(2, 3));
    setup.AddWhite(Pt(3, 1));
    setup.AddWhite(Pt(3, 3));
    setup.AddWhite(Pt(4, 3));
    setup.AddWhite(Pt(5, 1));
    setup.AddWhite(Pt(5, 2));
    setup.AddWhite(Pt(5, 3));
    setup.AddBlack(Pt(2, 1));
    setup.AddBlack(Pt(4, 1));
    setup.AddBlack(Pt(4, 2));
    GoBoard bd(19, setup);
    BOOST_REQUIRE(bd.IsLegal(Pt(3, 2), SG_BLACK));
    bd.Play(Pt(3, 2), SG_BLACK);
    // Capturing back does not violate the Ko rule
    BOOST_CHECK(bd.IsLegal(Pt(3, 1), SG_WHITE));
}

/** Test that positional superko is dealt with correctly */
BOOST_AUTO_TEST_CASE(GoBoardTest_IsLegal_PositionalSuperko)
{
    GoBoard bd(9);
    bd.Play(Pt(2, 8), SG_BLACK);
    bd.Play(Pt(1, 8), SG_WHITE);
    bd.Play(Pt(3, 8), SG_BLACK);
    bd.Play(Pt(2, 9), SG_WHITE);
    bd.Play(Pt(4, 9), SG_BLACK);
    bd.Play(Pt(3, 9), SG_WHITE);
    bd.Play(Pt(1, 9), SG_BLACK);

    // White (2, 9) is positional superko only
    bd.Rules().SetKoRule(GoRules::POS_SUPERKO);
    BOOST_CHECK(! bd.IsLegal(Pt(2, 9), SG_WHITE));
    bd.Rules().SetKoRule(GoRules::SUPERKO);
    BOOST_CHECK(bd.IsLegal(Pt(2, 9), SG_WHITE));
    bd.Rules().SetKoRule(GoRules::SIMPLEKO);
    BOOST_CHECK(bd.IsLegal(Pt(2, 9), SG_WHITE));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_IsLegal_Occupied)
{
    GoSetup setup;
    setup.AddWhite(Pt(1, 2));
    GoBoard bd(19, setup);
    BOOST_CHECK(! bd.IsLegal(Pt(1, 2), SG_BLACK));
    BOOST_CHECK(! bd.IsLegal(Pt(1, 2), SG_WHITE));
}

/** Test GoBoard::IsLegal(SgPoint, SgBlackWhite) for suicide move.
    Create position with add stones.
 */
BOOST_AUTO_TEST_CASE(GoBoardTest_IsLegal_Suicide)
{
    GoSetup setup;
    setup.AddWhite(Pt(1, 2));
    setup.AddWhite(Pt(2, 1));
    GoBoard bd(19, setup);
    BOOST_REQUIRE(! bd.Rules().AllowSuicide());
    BOOST_CHECK(! bd.IsLegal(Pt(1, 1), SG_BLACK));
}

/** Test GoBoard::IsLegal(SgPoint) for suicide move.
    Create position with play commands.
 */
BOOST_AUTO_TEST_CASE(GoBoardTest_IsLegal_Suicide_2)
{
    GoBoard bd;
    BOOST_REQUIRE(! bd.Rules().AllowSuicide());
    bd.Play(Pt(1, 1), SG_BLACK);
    bd.Play(Pt(1, 2), SG_WHITE);
    bd.Play(Pt(1, 3), SG_BLACK);
    bd.Play(Pt(2, 1), SG_WHITE);
    BOOST_CHECK(! bd.IsLegal(Pt(1, 1)));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_IsLibertyOfBlock)
{
    GoSetup setup;
    setup.AddWhite(Pt(1, 2));
    setup.AddWhite(Pt(2, 1));
    setup.AddBlack(Pt(2, 2));
    GoBoard bd(19, setup);
    BOOST_CHECK(bd.IsLibertyOfBlock(Pt(1, 1), bd.Anchor(Pt(1, 2))));
    BOOST_CHECK(bd.IsLibertyOfBlock(Pt(1, 1), bd.Anchor(Pt(2, 1))));
    BOOST_CHECK(! bd.IsLibertyOfBlock(Pt(1, 1), bd.Anchor(Pt(2, 2))));
    BOOST_CHECK(bd.IsLibertyOfBlock(Pt(3, 2), bd.Anchor(Pt(2, 2))));
    BOOST_CHECK(bd.IsLibertyOfBlock(Pt(2, 3), bd.Anchor(Pt(2, 2))));
    BOOST_CHECK(! bd.IsLibertyOfBlock(Pt(2, 3), bd.Anchor(Pt(1, 2))));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_IsSuicide_1)
{
    GoSetup setup;
    setup.AddWhite(Pt(1, 2));
    setup.AddWhite(Pt(2, 1));
    GoBoard bd(19, setup);
    bd.SetToPlay(SG_BLACK);
    BOOST_CHECK(bd.IsSuicide(Pt(1, 1)));
    BOOST_CHECK(! bd.IsSuicide(Pt(3, 1)));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_IsSuicide_2)
{
    GoBoard bd(9);
    bd.Play(Pt(1, 1), SG_BLACK);
    bd.Play(Pt(2, 1), SG_WHITE);
    bd.Play(Pt(1, 2), SG_WHITE);
    BOOST_CHECK(bd.IsSuicide(Pt(1, 1)));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_IsSuicide_3)
{
    GoBoard bd(9);
    bd.Play(Pt(1, 2), SG_BLACK);
    bd.Play(Pt(1, 1), SG_WHITE);
    bd.Play(Pt(2, 2), SG_BLACK);
    bd.Play(Pt(3, 1), SG_BLACK);
    BOOST_CHECK(bd.IsSuicide(Pt(2, 1)));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_IsValidPoint)
{
    GoBoard bd(19);
    BOOST_CHECK(bd.IsValidPoint(Pt(1, 1)));
    BOOST_CHECK(bd.IsValidPoint(Pt(1, 19)));
    BOOST_CHECK(bd.IsValidPoint(Pt(19, 1)));
    BOOST_CHECK(bd.IsValidPoint(Pt(19, 19)));
    BOOST_CHECK(! bd.IsValidPoint(Pt(1, 1) - SG_NS));
    BOOST_CHECK(! bd.IsValidPoint(Pt(1, 1) - SG_WE));
    BOOST_CHECK(! bd.IsValidPoint(Pt(1, 19) + SG_NS));
    BOOST_CHECK(! bd.IsValidPoint(Pt(1, 19) - SG_WE));
    BOOST_CHECK(! bd.IsValidPoint(Pt(19, 1) - SG_NS));
    BOOST_CHECK(! bd.IsValidPoint(Pt(19, 1) + SG_WE));
    BOOST_CHECK(! bd.IsValidPoint(Pt(19, 19) + SG_NS));
    BOOST_CHECK(! bd.IsValidPoint(Pt(19, 19) + SG_WE));
    BOOST_CHECK(! bd.IsValidPoint(0));
    BOOST_CHECK(! bd.IsValidPoint(SG_PASS));
    BOOST_CHECK(! bd.IsValidPoint(SG_ENDPOINT));
    BOOST_CHECK(! bd.IsValidPoint(SG_NULLMOVE));
    BOOST_CHECK(! bd.IsValidPoint(SG_NULLPOINT));
    BOOST_CHECK(! bd.IsValidPoint(2 * SG_MAXPOINT));
}

/** Test KoLevel and isIllegal flags depending on ko repetition allowed and
    ko modifies hash.
    @see sgboardko
*/
void GoBoardTest_Ko(bool allowKoRepetition, bool koModifiesHash)
{
    // 3 . . . .
    // 2 @ O . .
    // 1 O . O .
    //   A B C D
    GoSetup setup;
    setup.AddBlack(Pt(1, 2));
    setup.AddWhite(Pt(1, 1));
    setup.AddWhite(Pt(2, 2));
    setup.AddWhite(Pt(3, 1));
    GoBoard bd(19, setup);
    bd.AllowKoRepetition(allowKoRepetition);
    bd.SetKoModifiesHash(koModifiesHash);
    bd.SetKoLoser(SG_BLACK);
    BOOST_CHECK_EQUAL(bd.KoLevel(), 0);
    const int maxKoLevel = GoBoard::MAX_KOLEVEL;
    for (int i = 1; i < maxKoLevel + 5; ++i)
    {
        bd.Play(Pt(2, 1), SG_BLACK);
        if (i > 1)
        {
            BOOST_CHECK(bd.LastMoveInfo(GO_MOVEFLAG_REPETITION));
            BOOST_CHECK(bd.LastMoveInfo(GO_MOVEFLAG_ILLEGAL));
        }
        bd.Play(Pt(1, 1), SG_WHITE);
        BOOST_CHECK(bd.LastMoveInfo(GO_MOVEFLAG_REPETITION));
        if (! allowKoRepetition
            || (koModifiesHash && i > maxKoLevel))
            BOOST_CHECK(bd.LastMoveInfo(GO_MOVEFLAG_ILLEGAL));
        else
            BOOST_CHECK(! bd.LastMoveInfo(GO_MOVEFLAG_ILLEGAL));
        if (! allowKoRepetition)
            BOOST_CHECK_EQUAL(bd.KoLevel(), 0);
        else if (koModifiesHash && i >  maxKoLevel)
            BOOST_CHECK_EQUAL(bd.KoLevel(), maxKoLevel);
        else
            BOOST_CHECK_EQUAL(bd.KoLevel(), i);
    }
}

BOOST_AUTO_TEST_CASE(GoBoardTest_Ko_RepetitionModifyHash)
{
    GoBoardTest_Ko(true, true);
}

BOOST_AUTO_TEST_CASE(GoBoardTest_Ko_RepetitionNoModifyHash)
{
    GoBoardTest_Ko(true, false);
}

BOOST_AUTO_TEST_CASE(GoBoardTest_Ko_NoRepetitionModifyHash)
{
    GoBoardTest_Ko(false, true);
}

BOOST_AUTO_TEST_CASE(GoBoardTest_Ko_NoRepetitionNoModifyHash)
{
    GoBoardTest_Ko(false, false);
}

BOOST_AUTO_TEST_CASE(GoBoardTest_KoModifiesHash)
{
    GoSetup setup;
    setup.AddWhite(Pt(1, 2));
    setup.AddWhite(Pt(2, 1));
    setup.AddBlack(Pt(2, 2));
    setup.AddBlack(Pt(3, 1));
    GoBoard bd(19, setup);
    bd.AllowKoRepetition(true);
    BOOST_CHECK(bd.KoModifiesHash());
    SgHashCode code = bd.GetHashCode();
    bd.Play(Pt(1, 1), SG_BLACK);
    bd.Play(Pt(2, 1), SG_WHITE);
    BOOST_CHECK(bd.GetHashCode() != code);
    bd.Undo();
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.GetHashCode(), code);
    bd.SetKoModifiesHash(false);
    bd.Play(Pt(1, 1), SG_BLACK);
    bd.Play(Pt(2, 1), SG_WHITE);
    BOOST_CHECK_EQUAL(bd.GetHashCode(), code);
}

/** Test that isCapturing flag is set after capturing. */
BOOST_AUTO_TEST_CASE(GoBoardTest_LastMoveInfo_IsCapturing_1)
{
    GoSetup setup;
    setup.AddBlack(Pt(1, 1));
    setup.AddWhite(Pt(1, 2));
    GoBoard bd(19, setup);
    bd.Play(Pt(2, 1), SG_WHITE);
    BOOST_CHECK(bd.LastMoveInfo(GO_MOVEFLAG_CAPTURING));
}

/** Test that isCapturing flag is not set after suicide. */
BOOST_AUTO_TEST_CASE(GoBoardTest_LastMoveInfo_IsCapturing_2)
{
    GoSetup setup;
    setup.AddBlack(Pt(1, 2));
    setup.AddBlack(Pt(2, 1));
    GoBoard bd(19, setup);
    bd.Play(Pt(1, 1), SG_WHITE);
    BOOST_CHECK(! bd.LastMoveInfo(GO_MOVEFLAG_CAPTURING));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_LastMoveInfo_IsIllegal)
{
    GoBoard bd;
    bd.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK(! bd.LastMoveInfo(GO_MOVEFLAG_ILLEGAL));
    bd.Play(Pt(2, 2), SG_WHITE);
    BOOST_CHECK(! bd.LastMoveInfo(GO_MOVEFLAG_ILLEGAL));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_LastMoveInfo_IsIllegal_KoRepetition)
{
    GoSetup setup;
    setup.AddWhite(Pt(1, 2));
    setup.AddWhite(Pt(2, 1));
    setup.AddBlack(Pt(2, 2));
    setup.AddBlack(Pt(3, 1));
    GoBoard bd(19, setup);
    BOOST_REQUIRE(! bd.KoRepetitionAllowed());
    bd.Play(Pt(1, 1), SG_BLACK);
    bd.Play(Pt(2, 1), SG_WHITE);
    // Check if move is illegal
    BOOST_CHECK(bd.LastMoveInfo(GO_MOVEFLAG_ILLEGAL));
    bd.Undo();
    // Check if move is still illegal after playing/undoing a different move
    bd.Play(Pt(5, 5), SG_WHITE);
    bd.Undo();
    bd.Play(Pt(2, 1), SG_WHITE);
    BOOST_CHECK(bd.LastMoveInfo(GO_MOVEFLAG_ILLEGAL));
    bd.Undo();
    // Check that move is legal if ko repetition allowed
    bd.AllowKoRepetition(true);
    bd.Play(Pt(2, 1), SG_WHITE);
    BOOST_CHECK(! bd.LastMoveInfo(GO_MOVEFLAG_ILLEGAL));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_LastMoveInfo_IsIllegal_Suicide)
{
    GoSetup setup;
    setup.AddWhite(Pt(1, 2));
    setup.AddWhite(Pt(2, 1));
    GoBoard bd(19, setup);
    bd.Rules().SetAllowSuicide(false);
    bd.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK(bd.LastMoveInfo(GO_MOVEFLAG_ILLEGAL));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_LastMoveInfo_IsRepetition)
{
    GoBoard bd;
    bd.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK(! bd.LastMoveInfo(GO_MOVEFLAG_REPETITION));
    bd.Play(Pt(1, 2), SG_WHITE);
    BOOST_CHECK(! bd.LastMoveInfo(GO_MOVEFLAG_REPETITION));
    bd.Play(Pt(2, 2), SG_BLACK);
    BOOST_CHECK(! bd.LastMoveInfo(GO_MOVEFLAG_REPETITION));
    bd.Play(Pt(2, 1), SG_WHITE);
    BOOST_CHECK(! bd.LastMoveInfo(GO_MOVEFLAG_REPETITION));
    bd.Play(Pt(3, 1), SG_BLACK);
    BOOST_CHECK(! bd.LastMoveInfo(GO_MOVEFLAG_REPETITION));
    bd.Play(SG_PASS, SG_WHITE);
    BOOST_CHECK(! bd.LastMoveInfo(GO_MOVEFLAG_REPETITION));
    bd.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK(! bd.LastMoveInfo(GO_MOVEFLAG_REPETITION));
    bd.Play(SG_PASS, SG_BLACK);
    bd.Play(Pt(2, 1), SG_WHITE);
    BOOST_CHECK(bd.LastMoveInfo(GO_MOVEFLAG_REPETITION));
    bd.Play(Pt(3, 2), SG_BLACK);
    BOOST_CHECK(! bd.LastMoveInfo(GO_MOVEFLAG_REPETITION));
}

BOOST_AUTO_TEST_CASE(GoBoardTest_Line_9)
{
    GoBoard bd(9);
    BOOST_CHECK_EQUAL(bd.Line(Pt(1, 1)), 1);
    BOOST_CHECK_EQUAL(bd.Line(Pt(3, 4)), 3);
    BOOST_CHECK_EQUAL(bd.Line(Pt(8, 5)), 2);
    BOOST_CHECK_EQUAL(bd.Line(Pt(5, 8)), 2);
    BOOST_CHECK_EQUAL(bd.Line(Pt(9, 9)), 1);
}

BOOST_AUTO_TEST_CASE(GoBoardTest_Line_10)
{
    GoBoard bd(10);
    BOOST_CHECK_EQUAL(bd.Line(Pt(5, 4)), 4);
    BOOST_CHECK_EQUAL(bd.Line(Pt(5, 5)), 5);
    BOOST_CHECK_EQUAL(bd.Line(Pt(5, 6)), 5);
    BOOST_CHECK_EQUAL(bd.Line(Pt(5, 7)), 4);
    BOOST_CHECK_EQUAL(bd.Line(Pt(4, 5)), 4);
    BOOST_CHECK_EQUAL(bd.Line(Pt(5, 5)), 5);
    BOOST_CHECK_EQUAL(bd.Line(Pt(6, 5)), 5);
    BOOST_CHECK_EQUAL(bd.Line(Pt(7, 5)), 4);
}

BOOST_AUTO_TEST_CASE(GoBoardTest_Line_19)
{
    GoBoard bd(19);
    BOOST_CHECK_EQUAL(bd.Line(Pt(1, 1)), 1);
    BOOST_CHECK_EQUAL(bd.Line(Pt(3, 4)), 3);
    BOOST_CHECK_EQUAL(bd.Line(Pt(18, 5)), 2);
    BOOST_CHECK_EQUAL(bd.Line(Pt(5, 18)), 2);
    BOOST_CHECK_EQUAL(bd.Line(Pt(19, 19)), 1);
}

/** Test GoBoard::NumLiberties after a combination of setup and play */
BOOST_AUTO_TEST_CASE(GoBoardTest_NumLiberties_1)
{
    GoSetup setup;
    setup.AddBlack(Pt(1, 3));
    GoBoard bd(19, setup);
    bd.Play(Pt(2, 2), SG_WHITE);
    bd.Play(Pt(1, 2), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 2)), 3);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(2, 2)), 3);
}

/** Test GoBoard::NumLiberties after c combination of Play and Undo */
BOOST_AUTO_TEST_CASE(GoBoardTest_NumLiberties_2)
{
    GoBoard bd(19);
    bd.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 1)), 2);
    bd.Undo();
    bd.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 1)), 2);
    bd.Play(Pt(1, 2), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 1)), 3);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 2)), 3);
    bd.Undo();
    bd.Play(Pt(1, 2), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 1)), 3);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 2)), 3);
}

/** Test GoBoard::NumLiberties after capturing a stone */
BOOST_AUTO_TEST_CASE(GoBoardTest_NumLiberties_3)
{
    GoSetup setup;
    setup.AddBlack(Pt(2, 2));
    setup.AddBlack(Pt(3, 1));
    setup.AddWhite(Pt(1, 2));
    setup.AddWhite(Pt(2, 1));
    GoBoard bd(9, setup);
    bd.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 1)), 1);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 2)), 1);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(2, 2)), 3);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(3, 1)), 3);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 2)), 2);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(2, 1)), 1);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(2, 2)), 2);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(3, 1)), 2);
}

/** Test GoBoard::NumLiberties after adding a stone to en existing block. */
BOOST_AUTO_TEST_CASE(GoBoardTest_NumLiberties_4)
{
    GoBoard bd(9);
    bd.Play(Pt(5, 5), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(5, 5)), 4);
    bd.Play(Pt(4, 4), SG_WHITE);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(4, 4)), 4);
    bd.Play(Pt(5, 4), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(5, 5)), 5);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(5, 4)), 5);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(4, 4)), 3);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(4, 4)), 4);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(5, 5)), 4);
}

/** Test GoBoard::NumLiberties after merging blocks. */
BOOST_AUTO_TEST_CASE(GoBoardTest_NumLiberties_5)
{
    GoBoard bd(9);
    bd.Play(Pt(5, 5), SG_BLACK);
    bd.Play(Pt(6, 5), SG_BLACK);
    bd.Play(Pt(8, 5), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(5, 5)), 6);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(6, 5)), 6);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(8, 5)), 4);
    bd.Play(Pt(7, 5), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(5, 5)), 10);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(6, 5)), 10);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(7, 5)), 10);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(8, 5)), 10);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(5, 5)), 6);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(6, 5)), 6);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(8, 5)), 4);
}

/** Test GoBoard::NumLiberties adding stones twice to an existing block,
    and removing a liberty.
*/
BOOST_AUTO_TEST_CASE(GoBoardTest_NumLiberties_6)
{
    GoBoard bd(9);
    bd.Play(Pt(5, 5), SG_BLACK);
    bd.Play(Pt(6, 5), SG_BLACK);
    bd.Play(Pt(7, 5), SG_BLACK);
    bd.Play(Pt(6, 6), SG_WHITE);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(5, 5)), 7);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(6, 5)), 7);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(7, 5)), 7);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(6, 6)), 3);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(5, 5)), 8);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(6, 5)), 8);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(7, 5)), 8);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(5, 5)), 6);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(6, 5)), 6);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(5, 5)), 4);
}

/** Test GoBoard::NumLiberties merging blocks and capture.
    @verbatim
    . . . .
    . O . .
    O @ O .
    O . O .
    @endverbatim
*/
BOOST_AUTO_TEST_CASE(GoBoardTest_NumLiberties_7)
{
    GoBoard bd(9);
    bd.Play(Pt(1, 1), SG_WHITE);
    bd.Play(Pt(3, 1), SG_WHITE);
    bd.Play(Pt(1, 2), SG_WHITE);
    bd.Play(Pt(3, 2), SG_WHITE);
    bd.Play(Pt(2, 3), SG_WHITE);
    bd.Play(Pt(2, 2), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 1)), 2);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(3, 1)), 4);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 2)), 2);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(3, 2)), 4);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(2, 3)), 3);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(2, 2)), 1);
    bd.Play(Pt(2, 1), SG_WHITE);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 1)), 5);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(2, 1)), 5);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(3, 1)), 5);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(3, 2)), 5);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(2, 3)), 4);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 1)), 2);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(3, 1)), 4);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(1, 2)), 2);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(3, 2)), 4);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(2, 3)), 3);
    BOOST_CHECK_EQUAL(bd.NumLiberties(Pt(2, 2)), 1);
}

BOOST_AUTO_TEST_CASE(GoBoardTest_NumPrisoners)
{
    GoBoard bd(9);
    BOOST_CHECK_EQUAL(bd.NumPrisoners(SG_BLACK), 0);
    BOOST_CHECK_EQUAL(bd.NumPrisoners(SG_WHITE), 0);
    bd.Play(Pt(1, 2), SG_BLACK);
    bd.Play(Pt(1, 1), SG_WHITE);
    bd.Play(Pt(2, 1), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.NumPrisoners(SG_BLACK), 0);
    BOOST_CHECK_EQUAL(bd.NumPrisoners(SG_WHITE), 1);
    bd.Play(Pt(9, 8), SG_WHITE);
    bd.Play(Pt(9, 9), SG_BLACK);
    bd.Play(Pt(8, 9), SG_WHITE);
    BOOST_CHECK_EQUAL(bd.NumPrisoners(SG_BLACK), 1);
    BOOST_CHECK_EQUAL(bd.NumPrisoners(SG_WHITE), 1);
    bd.Undo();
    bd.Undo();
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.NumPrisoners(SG_BLACK), 0);
    BOOST_CHECK_EQUAL(bd.NumPrisoners(SG_WHITE), 1);
    bd.Undo();
    bd.Undo();
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.NumPrisoners(SG_BLACK), 0);
    BOOST_CHECK_EQUAL(bd.NumPrisoners(SG_WHITE), 0);

    // Check suicide
    bd.Play(Pt(1, 2), SG_BLACK);
    bd.Play(Pt(9, 9), SG_WHITE);
    bd.Play(Pt(2, 1), SG_BLACK);
    bd.Play(Pt(1, 1), SG_WHITE);
    BOOST_CHECK_EQUAL(bd.NumPrisoners(SG_BLACK), 0);
    BOOST_CHECK_EQUAL(bd.NumPrisoners(SG_WHITE), 1);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.NumPrisoners(SG_BLACK), 0);
    BOOST_CHECK_EQUAL(bd.NumPrisoners(SG_WHITE), 0);
}

BOOST_AUTO_TEST_CASE(GoBoardTest_Occupied)
{
    GoSetup setup;
    setup.AddWhite(Pt(1, 2));
    setup.AddBlack(Pt(2, 1));
    GoBoard bd(19, setup);
    BOOST_CHECK(bd.Occupied(Pt(1, 2)));
    BOOST_CHECK(bd.Occupied(Pt(2, 1)));
    // Occupied should return false for border points
    SgPoint borderPoint = Pt(1, 1) - SG_WE;
    BOOST_REQUIRE(bd.IsBorder(borderPoint));
    BOOST_CHECK(! bd.Occupied(borderPoint));
}

/** Test some data after creating a single stone block with a Play */
BOOST_AUTO_TEST_CASE(GoBoardTest_Play_SingleStone)
{
    GoBoard bd(19);
    SgPoint p = Pt(1, 2);
    bd.Play(p, SG_BLACK);
    BOOST_CHECK_EQUAL(bd.Anchor(p), p);
    BOOST_CHECK_EQUAL(bd.NumLiberties(p), 3);
    BOOST_CHECK_EQUAL(bd.NumStones(p), 1);
}

BOOST_AUTO_TEST_CASE(GoBoardTest_Pos_9)
{
    GoBoard bd(9);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(1, 1)), 1);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(3, 4)), 4);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(8, 5)), 5);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(5, 8)), 5);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(9, 9)), 1);
}

BOOST_AUTO_TEST_CASE(GoBoardTest_Pos_10)
{
    GoBoard bd(10);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(5, 4)), 5);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(5, 5)), 5);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(5, 6)), 5);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(5, 7)), 5);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(4, 5)), 5);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(5, 5)), 5);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(6, 5)), 5);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(7, 5)), 5);
}

BOOST_AUTO_TEST_CASE(GoBoardTest_Pos_19)
{
    GoBoard bd(19);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(1, 1)), 1);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(3, 4)), 4);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(18, 5)), 5);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(5, 18)), 5);
    BOOST_CHECK_EQUAL(bd.Pos(Pt(19, 19)), 1);
}

/** Test GoBoard:::SetToPlay. */
BOOST_AUTO_TEST_CASE(GoBoardTest_SetToPlay)
{
    GoBoard bd(19);
    bd.Play(Pt(1, 1), SG_BLACK);
    bd.SetToPlay(SG_BLACK);
    BOOST_CHECK_EQUAL(bd.ToPlay(), SG_BLACK);
    bd.Play(Pt(2, 1), SG_BLACK);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.ToPlay(), SG_BLACK);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.ToPlay(), SG_BLACK);
}

BOOST_AUTO_TEST_CASE(GoBoardTest_ToPlay)
{
    GoBoard bd(19);
    BOOST_CHECK_EQUAL(bd.ToPlay(), SG_BLACK);
    bd.Play(Pt(1, 1), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.ToPlay(), SG_WHITE);
    bd.Play(Pt(2, 1), SG_WHITE);
    BOOST_CHECK_EQUAL(bd.ToPlay(), SG_BLACK);
    bd.Play(Pt(3, 1), SG_BLACK);
    BOOST_CHECK_EQUAL(bd.ToPlay(), SG_WHITE);
    bd.Play(SG_PASS, SG_WHITE);
    BOOST_CHECK_EQUAL(bd.ToPlay(), SG_BLACK);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.ToPlay(), SG_WHITE);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.ToPlay(), SG_BLACK);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.ToPlay(), SG_WHITE);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.ToPlay(), SG_BLACK);
}

/** Check that board state is correctly restored after an undo. */
BOOST_AUTO_TEST_CASE(GoBoardTest_Undo)
{
    GoBoard bd(19);
    BOOST_REQUIRE(bd.ToPlay() == SG_BLACK);
    bd.Play(Pt(1, 1), SG_WHITE);
    bd.Undo();
    BOOST_CHECK_EQUAL(bd.ToPlay(), SG_BLACK);
}

//----------------------------------------------------------------------------

void GoBoardIteratorTest_AtSize(int size)
{
    GoBoard bd(size);
    SgSList<SgPoint,SG_MAXPOINT> points;
    for (GoBoard::Iterator it(bd); it; ++it)
        points.PushBack(*it);
    BOOST_CHECK_EQUAL(points.Length(), size * size);
    for (int x = 1; x <= size; ++x)
        for (int y = 1; y <= size; ++y)
        {
            SgPoint p = Pt(x, y);
            BOOST_CHECK_MESSAGE(points.Contains(p),
                                "GoBoard::Iterator size " << size
                                << " missing " << SgWritePoint(p));
        }
}

BOOST_AUTO_TEST_CASE(GoBoardIteratorTest_All)
{
    GoBoardIteratorTest_AtSize(9);
    GoBoardIteratorTest_AtSize(10);
    GoBoardIteratorTest_AtSize(13);
    GoBoardIteratorTest_AtSize(19);
}

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(GoBoardLibertyIteratorTest)
{
    GoSetup setup;
    setup.AddBlack(Pt(1, 3));
    GoBoard bd(19, setup);
    bd.Play(Pt(2, 2), SG_WHITE);
    bd.Play(Pt(1, 2), SG_BLACK);
    SgSList<SgPoint,SG_MAXPOINT> libs;
    for (GoBoard::LibertyIterator it(bd, Pt(1, 2)); it; ++it)
        libs.PushBack(*it);
    BOOST_CHECK_EQUAL(libs.Length(), 3);
    BOOST_CHECK(libs.Contains(Pt(1, 1)));
    BOOST_CHECK(libs.Contains(Pt(1, 4)));
    BOOST_CHECK(libs.Contains(Pt(2, 3)));
    libs.Clear();
    for (GoBoard::LibertyIterator it(bd, Pt(2, 2)); it; ++it)
        libs.PushBack(*it);
    BOOST_CHECK_EQUAL(libs.Length(), 3);
    BOOST_CHECK(libs.Contains(Pt(2, 1)));
    BOOST_CHECK(libs.Contains(Pt(2, 3)));
    BOOST_CHECK(libs.Contains(Pt(3, 2)));
}

//----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(GoBoardStoneIteratorTest)
{
    GoSetup setup;
    setup.AddBlack(Pt(3, 2));
    setup.AddBlack(Pt(4, 3));
    GoBoard bd(19, setup);
    bd.Play(Pt(1, 1), SG_BLACK);
    bd.Play(Pt(1, 3), SG_WHITE);
    bd.Play(Pt(1, 2), SG_BLACK);
    bd.Play(Pt(2, 3), SG_WHITE);
    bd.Play(Pt(3, 3), SG_WHITE);
    bd.Play(Pt(2, 2), SG_BLACK);
    SgSList<SgPoint,SG_MAXPOINT> stones;
    for (GoBoard::StoneIterator it(bd, Pt(2, 2)); it; ++it)
        stones.PushBack(*it);
    BOOST_CHECK_EQUAL(stones.Length(), 4);
    BOOST_CHECK(stones.Contains(Pt(1, 1)));
    BOOST_CHECK(stones.Contains(Pt(1, 2)));
    BOOST_CHECK(stones.Contains(Pt(2, 2)));
    BOOST_CHECK(stones.Contains(Pt(3, 2)));
    stones.Clear();
    for (GoBoard::StoneIterator it(bd, Pt(4, 3)); it; ++it)
        stones.PushBack(*it);
    BOOST_CHECK_EQUAL(stones.Length(), 1);
    BOOST_CHECK(stones.Contains(Pt(4, 3)));
    stones.Clear();
    for (GoBoard::StoneIterator it(bd, Pt(1, 3)); it; ++it)
        stones.PushBack(*it);
    BOOST_CHECK_EQUAL(stones.Length(), 3);
    BOOST_CHECK(stones.Contains(Pt(1, 3)));
    BOOST_CHECK(stones.Contains(Pt(2, 3)));
    BOOST_CHECK(stones.Contains(Pt(3, 3)));
}

//----------------------------------------------------------------------------

} // namespace

