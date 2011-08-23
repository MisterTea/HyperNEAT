//----------------------------------------------------------------------------
/** @file GoUtil.h
    General Go related utility functions.
*/
//----------------------------------------------------------------------------

#ifndef GO_UTIL_H
#define GO_UTIL_H

#include <string>

//----------------------------------------------------------------------------

namespace GoUtil
{
    /** Convert a score from a float to a string.
        The string representation is compatible to the one used in the SGF
        file format ant the GTP final_score command (e.g. W+2.5 if white wins,
        B+31 if black wins, and just 0 if the game ends in a draw).
    */
    std::string ScoreToString(float score);
}

//----------------------------------------------------------------------------

#endif // GO_UTIL_H
