//----------------------------------------------------------------------------
/** @file GoBensonSolver.cpp
    See GoBensonSolver.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoBensonSolver.h"

#include "GoBlock.h"
#include "GoSafetyUtil.h"
#include "SgDebug.h"

namespace {

	const bool DEBUG_BENSON = false;
    
}

//----------------------------------------------------------------------------
 
void GoBensonSolver::FindSafePoints(SgBWSet* safe)
{
    safe->Clear();
    GoStaticSafetySolver::FindSafePoints(safe);

    if (DEBUG_BENSON)
        GoSafetyUtil::WriteStatistics("GoBensonSolver", Regions(), safe);
}

