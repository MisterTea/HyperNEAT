//----------------------------------------------------------------------------
/** @file SgMiaiMapTest.cpp
    Unit tests for SgMiaiMap.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include "SgMiaiMap.h"
#include "SgMiaiStrategy.h"
#include "SgStrategy.h"

#include "SgBlackWhite.h"
#include "SgPoint.h"
#include "SgPointSet.h"
#include "SgPointSetUtil.h"

using SgPointUtil::Pt;
//----------------------------------------------------------------------------

namespace {


BOOST_AUTO_TEST_CASE(SgStrategyTest_SgMiaiMap)
{
    /* Set up simple miai strategy with two points p1, p2 */
    SgPoint p1(Pt(3,3));
    SgPoint p2(Pt(5,3));
    SgMiaiPair p(p1, p2);
    SgMiaiStrategy s(SG_BLACK);
    s.AddPair(p);

    SgMiaiMap m;
    m.ConvertFromSgMiaiStrategy(s);
    BOOST_CHECK_EQUAL(m.Status(), SGSTRATEGY_ACHIEVED);
    m.ExecuteMove(p1, SG_BLACK);
    BOOST_CHECK_EQUAL(m.Status(), SGSTRATEGY_THREATENED);
    BOOST_CHECK_EQUAL(m.ForcedMove(), p2);
    
    // todo: convert strategy with open threat.
}

} // namespace
//----------------------------------------------------------------------------

