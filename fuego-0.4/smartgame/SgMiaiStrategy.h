//----------------------------------------------------------------------------
/** @file SgMiaiStrategy.h
    Simple strategy for winning a specified goal using paired moves.
*/
//----------------------------------------------------------------------------

#ifndef SG_MIAISTRATEGY_H
#define SG_MIAISTRATEGY_H

#include <utility>
#include "SgArray.h"
#include "SgBlackWhite.h"
#include "SgBWArray.h"
#include "SgHash.h"
#include "SgPointSet.h"
#include "SgStrategy.h"
#include "SgVector.h"

//----------------------------------------------------------------------------

typedef std::pair<SgPoint, SgPoint> SgMiaiPair;

//----------------------------------------------------------------------------

namespace SgMiaiPairUtil
{
    SgPoint Other(const SgMiaiPair& pair, SgPoint p);
}

//----------------------------------------------------------------------------

/** Pair of moves strategy - if opponent plays one we must play other */
class SgMiaiStrategy
    : public SgStrategy
{
public:
    /** Initialized to empty strategy; use AddPair */
    SgMiaiStrategy(SgBlackWhite player)
        : SgStrategy(player),
          m_failed(false)
    { }

    /** Add a miai strategy on pair of points */
    void AddPair(const SgMiaiPair& miaiPair);
    
    /** Set whole strategy */
    void SetStrategy(const SgVector<SgMiaiPair>& miaiStrategies)
    {
        //SG_ASSERT(m_miaiStrategies.IsEmpty());
        m_miaiStrategies = miaiStrategies;
    }
    
    /** See m_miaiStrategies */
    const SgVector<SgMiaiPair>& MiaiStrategies() const
    {
        return m_miaiStrategies;
    }
    
    /** Pairs that overlap give a double threat */
    bool HasOverlappingMiaiPairs() const;
    
    /** All points on which this strategy depends */
    SgPointSet Dependency() const;

    /** See SgStrategyStatus */
    SgStrategyStatus Status() const;
    
    /** See m_openThreats */
    const SgVector<SgPoint>& OpenThreats() const;
    
    /** If exactly one open threat, return that move.
        If no active threat, return SG_NULLPOINT.
    */
    SgPoint OpenThreatMove() const;
 
    /** See SgStrategy::ExecuteMove */
    void ExecuteMove(SgPoint p, SgBlackWhite player);
    
    /** See SgStrategy::UndoMove */
    void UndoMove();
    
    /** See SgStrategy::Clear */
    void Clear();
       
    /** Write object to stream. Do not call directly, use operator<< */
    void Write(std::ostream& stream) const;
private:
    /** strategy has failed - set m_failed flag and release resources */
    void StrategyFailed();

    /** move pairs - must play one in each pair */
    SgVector<SgMiaiPair> m_miaiStrategies;
    
    /** open threats must be answered to achieve strategy.
        More than one open threat means strategy cannot be achieved.
    */
    SgVector<SgPoint> m_openThreats;
    
    /** Strategy has failed if opponent has occupied both points in a pair */
    bool m_failed;
};

//----------------------------------------------------------------------------

#endif // SG_MIAISTRATEGY_H
