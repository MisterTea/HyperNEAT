//----------------------------------------------------------------------------
/** @file GoSafetyUtil.h
    Utility functions for the static and search-based safety solvers.
*/
//----------------------------------------------------------------------------

#ifndef GO_SAFETYUTIL_H
#define GO_SAFETYUTIL_H

#include "SgBlackWhite.h"
#include "SgMiaiStrategy.h"
#include "SgPoint.h"
#include "SgVector.h"

class GoBoard;
class GoRegion;
class GoRegionBoard;
class SgBWSet;
class SgPointSet;

//----------------------------------------------------------------------------

namespace GoSafetyUtil
{

    /** Add pts to *safe[color] */
    void AddToSafe(const GoBoard& board, const SgPointSet& pts,
                   SgBlackWhite color, SgBWSet* safe, const char* reason,
                   int depth, bool addBoundary);

    /** Stronger version of IsTerritory that uses region information
        This version checks for opponent nakade inside the area.
        Useful for proving safe semeai test cases after resolving semeai.
    */
    bool ExtendedIsTerritory(const GoBoard& board, GoRegionBoard* regions,
                             const SgPointSet& pts,
                             const SgPointSet& safe, SgBlackWhite color);

    /** See FindDameAndUnsurroundablePoints */
    SgPointSet FindDamePoints(const GoBoard& board, const SgPointSet& empty,
                              const SgBWSet& safe);

    /** Find dame and unsurroundable points.
        Given sets of empty points and safe points, compute subset of dame
        points. Given safe B+W stones, find empty which are surely dame,
        using the algorithm of [Mueller1995].
        Unsurroundable points are empty points that can not be surrounded
        by either player because they are adjacent to both player's safe
        stones. However, they can potentially have an effect on unsafe stones
        or on other empty points. Dame points are a subset of unsurroundable
        points that have no effect on other points - no matter if they will
        be occupied by Black, White, or remain empty.
    */
    void FindDameAndUnsurroundablePoints(const GoBoard& bd,
                                         const SgPointSet& empty,
                                         const SgBWSet& safe,
                                         SgPointSet* dame,
                                         SgPointSet* unsurroundable);

    /** Simple static territory check for surrounded area */
    bool IsTerritory(const GoBoard& board, const SgPointSet& pts,
                     const SgPointSet& safe, SgBlackWhite color);

    /** Given set of stones, reduce to block anchors */
    void ReduceToAnchors(const GoBoard& board, const SgPointSet& stones,
                         SgVector<SgPoint>* anchors);

    /** Helper function for 1-vitality test.
        Try to find two matching liberties for point p, subtract them from
        libs if found.
    */
    bool Find2Libs(SgPoint p, SgPointSet* libs);

    /** Helper function for 1-vitality test.
        Similar to Find2Libs(), but try to find miaiPair of two best liberties
        (not shared with other interior points).
    */
    bool Find2BestLibs(SgPoint p, const SgPointSet& libs,
                       SgPointSet interior, SgMiaiPair* miaiPair);

    /** Extended version of MightMakeLife.
        Recognizes some nakade shapes as dead. Useful mostly for semeai
        solver.
    */
    bool ExtendedMightMakeLife(const GoBoard& board, GoRegionBoard* regions,
                               const SgPointSet& area, const SgPointSet& safe,
                               SgBlackWhite color);

    /** Test whether color can make 2 eyes inside a surrounded area.
        Precondition: area surrounded by safe stones of opponent.
        Basic test, handles 1 and 2 point eyes only.
    */
    bool MightMakeLife(const GoBoard& board, const SgPointSet& area,
                       const SgPointSet& safe, SgBlackWhite color);

    /** Write statistics about the safe points */
    void WriteStatistics(const std::string& heading,
                      const GoRegionBoard* regions,
                      const SgBWSet* safe);
                      
} // namespace GoSafetyUtil

//----------------------------------------------------------------------------

#endif // GO_SAFETYUTIL_H
