//----------------------------------------------------------------------------
/** @file SgMiaiStrategyTest.cpp
    Unit tests for SgMiaiStrategy.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgMiaiStrategy.h"

#include "SgBlackWhite.h"
#include "SgPoint.h"
#include "SgPointSet.h"
#include "SgPointSetUtil.h"
#include "SgStrategy.h"

using SgPointUtil::Pt;
//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgMiaiStrategyTest_SgMiaiStrategy_1)
{
    SgMiaiStrategy m(SG_BLACK);
    BOOST_CHECK_EQUAL(m.Status(), SGSTRATEGY_ACHIEVED);
    BOOST_CHECK(m.OpenThreats().IsEmpty());
}

BOOST_AUTO_TEST_CASE(SgMiaiStrategyTest_SgMiaiStrategy_2)
{
    /* Set up simple miai strategy s with two points p1, p2 */
    SgPoint p1(Pt(3,3));
    SgPoint p2(Pt(5,3));
    SgMiaiPair p(p1, p2);
    SgMiaiStrategy s(SG_BLACK);
    s.AddPair(p);
    
    SgPointSet set1 = s.Dependency();
    SgPointSet set2;
    set2.Include(p1);
    set2.Include(p2);
    BOOST_CHECK_EQUAL(set1, set2);
    BOOST_CHECK_EQUAL(s.Status(), SGSTRATEGY_ACHIEVED);
    BOOST_CHECK(s.OpenThreats().IsEmpty());
    
    // check after playing one move in the pair
    s.ExecuteMove(p1, SG_WHITE);
    BOOST_CHECK_EQUAL(s.Status(), SGSTRATEGY_THREATENED);
    BOOST_CHECK(s.OpenThreats().IsLength(1));
    BOOST_CHECK_EQUAL(s.OpenThreatMove(), p2);

    // clone s to try playing both black and white followup moves
    SgMiaiStrategy s2(s);
    
    // answer threat - strategy achieved
    s.ExecuteMove(p2, SG_BLACK);
    BOOST_CHECK_EQUAL(s.Status(), SGSTRATEGY_ACHIEVED);
    set1 = s.Dependency();
    SgPointSet emptySet;
    BOOST_CHECK_EQUAL(set1, emptySet);
    
    // carry out threat - strategy failed
    s2.ExecuteMove(p2, SG_WHITE);
    BOOST_CHECK_EQUAL(s2.Status(), SGSTRATEGY_FAILED);
    set1 = s2.Dependency();
    BOOST_CHECK_EQUAL(set1, emptySet);
}

} // namespace

//----------------------------------------------------------------------------


