//----------------------------------------------------------------------------
/** @file SgMiaiMap.h
    Efficient implementation of a consistent (not broken) SgMiaiStrategy.

*/
//----------------------------------------------------------------------------

#ifndef SG_MIAIMAP_H
#define SG_MIAIMAP_H

#include "SgArray.h"
#include "SgBlackWhite.h"
#include "SgBWArray.h"
#include "SgMiaiStrategy.h"
#include "SgPoint.h"

//----------------------------------------------------------------------------

/** Efficient implementation of a SgMiaiStrategy.
    The miai strategy must be achievable (not broken).
    Uses two boards, one for each color.
    Board entries either point to the miai point, 
    or are set to SG_NULLPOINT if the point is not part of a miai.
    Can only be used going forward through a game - does not support undo.
*/
class SgMiaiMap
{
public:
    /** Initialize to empty map. */
    SgMiaiMap();
    
    /** See SgStrategy::ExecuteMove */
    void ExecuteMove(SgPoint p, SgBlackWhite player);
    
    /** See m_forcedMove */
    SgPoint ForcedMove() const {return m_forcedMove;}

    /** See SgStrategyStatus */
    SgStrategyStatus Status() const;

    /** Convert SgMiaiStrategy into SgMiaiMap. 
        The SgMiaiStrategy must not have overlapping threats.
    */
    void ConvertFromSgMiaiStrategy(const SgMiaiStrategy& s);
    
    /** Convert SgMiaiMap into SgMiaiStrategy. 
    */
    void ConvertToSgMiaiStrategy(SgMiaiStrategy* s) const;
private:
    /** Move that must be played to stop or convert an open threat.
        Returns SG_NULLPOINT if no such move.
    */
    SgPoint m_forcedMove;
    
    /** Strategy has failed if opponent has occupied both points in a pair */
    bool m_failed;
    
    /** The maps, one for each color. */
    SgBWArray<SgArray<int,SG_MAXPOINT> > m_map;
};

std::ostream& operator<<(std::ostream& stream, const SgStrategy& s);

//----------------------------------------------------------------------------

#endif // SG_MIAIMAP_H
