//----------------------------------------------------------------------------
/** @file GoUctEstimatorStat.h
    Utility classes to gather statistics for estimators (like RAVE) used in
    SgUctSearch.
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_ESTIMATORSTAT_H
#define GOUCT_ESTIMATORSTAT_H

#include <cstddef>
#include <string>
#include <vector>

class GoBoard;
class GoUctSearch;

//----------------------------------------------------------------------------

namespace GoUctEstimatorStat
{
    /** Get statistics for estimators.
        First plays each move and performs a search with a large number of
        games to estimate the true value of the move. Then performs a search
        at the root node and saves a sample of the values and counts after
        each game in the search.
        @param search
        @param trueValueMaxGames Number of games to use for the search to
        determine the true value
        @param maxGames Maximum number of games for actual search.
        @param stepSize Interval of games played to save the values.
        @param fileName File to append the resulting samples to
    */
    void Compute(GoUctSearch& search, std::size_t trueValueMaxGames,
                 std::size_t maxGames, std::size_t stepSize,
                 const std::string& fileName);
}

//----------------------------------------------------------------------------

#endif // GOUCT_ESTIMATORSTAT_H
