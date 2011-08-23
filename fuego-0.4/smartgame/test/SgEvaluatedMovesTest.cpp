//----------------------------------------------------------------------------
/** @file SgEvaluatedMovesTest.cpp
    Unit tests for SgEvaluatedMoves.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgEvaluatedMoves.h"

#include "SgPoint.h"
#include "SgPointSet.h"

using SgPointUtil::Pt;
//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgEvaluatedMovesTestEmpty)
{
    SgPointSet relevant;
    SgEvaluatedMoves moves(relevant);
    SgPointSet r = moves.Relevant();
    BOOST_CHECK(r.IsEmpty());
    SgVector<SgPoint> best;
    moves.BestMoves(best, 10);
    BOOST_CHECK(best.IsEmpty());
}

BOOST_AUTO_TEST_CASE(SgEvaluatedMovesTestOneMove)
{
    SgPointSet relevant;
    const SgPoint move1 = Pt(1,1);
    relevant.Include(move1);
    SgEvaluatedMoves moves(relevant);
    SgPointSet r = moves.Relevant();
    BOOST_CHECK(! r.IsEmpty());
    BOOST_CHECK_EQUAL(r.Size(), 1);
    BOOST_CHECK_EQUAL(moves.BestMove(), SG_PASS);
    SgVector<SgPoint> best;
    moves.BestMoves(best, 10);
    BOOST_CHECK(best.IsEmpty());
    moves.AddMove(move1, 345);
    BOOST_CHECK_EQUAL(moves.BestMove(), move1);
    BOOST_CHECK_EQUAL(moves.BestValue(), 345);
    moves.BestMoves(best, 10);
    BOOST_CHECK_EQUAL(best.Length(), 1);
    BOOST_CHECK_EQUAL(best[0], move1);
}

BOOST_AUTO_TEST_CASE(SgEvaluatedMovesTestTwoMoves)
{
    SgPointSet relevant;
    const SgPoint move1 = Pt(1,1);
    const SgPoint move2 = Pt(2,3);
    const SgPoint move3 = Pt(3,2);
    relevant.Include(move1);
    relevant.Include(move2);
    relevant.Include(move3);
    SgEvaluatedMoves moves(relevant);
    SgPointSet r = moves.Relevant();
    BOOST_CHECK(! r.IsEmpty());
    BOOST_CHECK_EQUAL(r.Size(), 3);
    BOOST_CHECK_EQUAL(moves.BestMove(), SG_PASS);
    moves.AddMove(move1, 345);
    moves.AddMove(move2, 99);
    BOOST_CHECK_EQUAL(moves.BestMove(), move1);
    BOOST_CHECK_EQUAL(moves.BestValue(), 345);
    SgVector<SgPoint> best;
    moves.BestMoves(best, 10);
    BOOST_CHECK_EQUAL(best.Length(), 1);
    BOOST_CHECK_EQUAL(best[0], move1);
    moves.AddMove(move3, 345);
    best.Clear();
    moves.BestMoves(best, 10);
    BOOST_CHECK_EQUAL(best.Length(), 2);
    BOOST_CHECK_EQUAL(best[0], move1);
    BOOST_CHECK_EQUAL(best[1], move3);
    BOOST_CHECK_EQUAL(moves.BestValue(), 345);
    moves.AddMove(move2, 999);
    best.Clear();
    moves.BestMoves(best, 10);
    BOOST_CHECK_EQUAL(best.Length(), 1);
    BOOST_CHECK_EQUAL(best[0], move2);
    BOOST_CHECK_EQUAL(moves.BestValue(), 999);
}

} // namespace

//----------------------------------------------------------------------------

