//----------------------------------------------------------------------------
/** @file GoSetupUtil.h
    utilities to create setup information for GoBoard.
*/
//----------------------------------------------------------------------------

#ifndef GO_SETUPUTIL_H
#define GO_SETUPUTIL_H

#include <streambuf>
#include "GoBoard.h"
#include "GoSetup.h"

//----------------------------------------------------------------------------
namespace GoSetupUtil
{

    /** Create a setup instance from text stream. 
        Recognizes '0','o','O' for white stones,
                   'x', 'X', '@' for black stones,
                   '.' and '+' for empty points.
        Blanks ' ' and tabs are ignored.
        Lines must be separated by a '\n' character.
    */
    GoSetup CreateSetupFromStream(std::streambuf& in, int& boardSize);

    GoSetup CreateSetupFromString(std::string& in, int& boardSize);

    /** Get a setup instance with the current position on board. */
    GoSetup CurrentPosSetup(const GoBoard& bd);

} // namespace GoSetupUtil

//----------------------------------------------------------------------------

#endif // GO_SETUPUTIL_H

