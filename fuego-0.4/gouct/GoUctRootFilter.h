//----------------------------------------------------------------------------
/** @file GoUctRootFilter.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_ROOTFILTER_H
#define GOUCT_ROOTFILTER_H

#include <vector>
#include "SgPoint.h"

class GoBoard;

//----------------------------------------------------------------------------

/** Interface for using external knowledge to prune moves at the root node of
    a search.
*/
class GoUctRootFilter
{
public:
    virtual ~GoUctRootFilter();

    /** Get moves to filter in the current position.
        This function is invoked by the player before the search, it does not
        need to be thread-safe.
    */
    virtual std::vector<SgPoint> Get() = 0;
};

//----------------------------------------------------------------------------

#endif // GOUCT_ROOTFILTER_H
