//----------------------------------------------------------------------------
/** @file SgGtpUtil.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgGtpUtil.h"

#include "SgPointSet.h"
#include "GtpEngine.h"

using namespace std;

//----------------------------------------------------------------------------

void SgGtpUtil::RespondPointSet(GtpCommand& cmd, const SgPointSet& pointSet)
{
    bool isFirst = true;
    for (SgSetIterator it(pointSet); it; ++it)
    {
        if (! isFirst)
            cmd << ' ';
        isFirst = false;
        cmd << SgWritePoint(*it);
    }
}

//----------------------------------------------------------------------------

