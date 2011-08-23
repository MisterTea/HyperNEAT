//----------------------------------------------------------------------------
/** @file SgUtil.cpp
    See SgUtil.h
 */
//----------------------------------------------------------------------------
#include "SgSystem.h"

#include <iostream>
#include "SgBlackWhite.h"
#include "SgUtil.h"
//----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const SgBalancer& balancer)
{
    stream 
            << balancer.NuCalls() << " calls, "
            << balancer.Balance() << " balance, "
            << balancer.Margin() << " margin, "
            << balancer.NuPlayed(SG_BLACK) << " played(B), "
            << balancer.NuPlayed(SG_WHITE) << " played(W), "
            << balancer.NuRejected(SG_BLACK) << " rejected(B), "
            << balancer.NuRejected(SG_WHITE) << " rejected(W).\n";
    return stream;
}

//----------------------------------------------------------------------------

