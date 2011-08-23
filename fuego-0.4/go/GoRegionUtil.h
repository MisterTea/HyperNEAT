//----------------------------------------------------------------------------
/** @file GoRegionUtil.h
    Utility functions for GoRegion and GoRegionBoard.
*/
//----------------------------------------------------------------------------

#ifndef GO_REGIONUTIL_H
#define GO_REGIONUTIL_H

#include "SgBlackWhite.h"
#include "SgVector.h"
#include "SgPoint.h"

class GoBoard;
class SgPointSet;

//----------------------------------------------------------------------------

namespace GoRegionUtil
{
    /** Check for 1-vitality, as explained in [Mueller 95, p.****] */
    bool StaticIs1VitalAndConnected(const GoBoard& board,
                                    const SgPointSet& pts,
                                    SgBlackWhite color);

    /** Test if a point set is 2-vital for color */
    bool Has2SureLiberties(const GoBoard& board, const SgPointSet& pts,
                           SgBlackWhite color,
                           const SgVector<SgPoint>& boundaryAnchors);

    /** Test if pts has 2 IP or eyes.
        Has2IPorEyes is called after all interior points are 2conn proved.
        It returns true if two intersection points are found or single
        boundary block forms two separate eyes.
    */
    bool Has2IPorEyes(const GoBoard& board, const SgPointSet& pts,
                      SgBlackWhite color,
                      const SgVector<SgPoint>& boundaryAnchors);

    /** Do pts belong to only one block?
        pts must be non-empty.
    */
    bool IsSingleBlock(const GoBoard& board, const SgPointSet& pts,
                       SgBlackWhite color);

    /** A region is called small iff its interior is completely filled
        by opponent stones (or there is no interior).
        This definition (and the name "small") is due to Benson.
        pts must be a region completely surrounded by opponent stones.
    */
    bool IsSmallRegion(const GoBoard& board, const SgPointSet& pts,
                       SgBlackWhite opp);

    /** Find anchors of all blocks represented by origAnchors
        that are present in current board position.
        Some of the origAnchors may have been captured in the meantime, and
        some anchors may have changed by extending and/or merging of blocks.
    */
    void FindCurrentAnchors(const GoBoard& board,
                            const SgVector<SgPoint>& origAnchors,
                            SgVector<SgPoint>* currentAnchors);

} // namespace GoRegionUtil

//----------------------------------------------------------------------------

#endif // GO_REGIONUTIL_H
