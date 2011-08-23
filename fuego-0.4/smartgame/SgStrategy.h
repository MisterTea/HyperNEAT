//----------------------------------------------------------------------------
/** @file SgStrategy.h
    SgStrategy is a strategy for winning a specified goal.

    An example of a strategy: keep a territory safe that was proven safe
    earlier on. The strategy contains answers to any opponent threat.
    Strategies can be generated, tracked, and enforced during search.
*/
//----------------------------------------------------------------------------

#ifndef SG_STRATEGY_H
#define SG_STRATEGY_H

#include "SgArray.h"
#include "SgBlackWhite.h"
#include "SgBWArray.h"
#include "SgHash.h"
#include "SgPointSet.h"

//----------------------------------------------------------------------------

enum SgStrategyStatus
{
    SGSTRATEGY_ACHIEVED,
    SGSTRATEGY_THREATENED,
    SGSTRATEGY_UNKNOWN,
    SGSTRATEGY_FAILED,
    _SGSTRATEGY_COUNT
};

std::ostream& operator<<(std::ostream& stream, SgStrategyStatus s);

//----------------------------------------------------------------------------

/** Strategy for achieving a certain goal.
    Pure virtual class, see e.g. SgMiaiStrategy for an implementation.
*/
class SgStrategy
{
public:
    SgStrategy(SgBlackWhite player);
    
    virtual ~SgStrategy() {}

    /** See m_player */
    SgBlackWhite Player() const
    {
        return m_player;
    }

    /** remove all data, reset to empty strategy. */
    virtual void Clear();
    
    /** all points on board that can possibly affect strategy */
    virtual SgPointSet Dependency() const = 0;
    
    /** check success of strategy on given board */
    virtual SgStrategyStatus Status() const = 0;
    
    /** update strategy with move */
    virtual void ExecuteMove(SgMove p, SgBlackWhite player) = 0;
    
    /** go back to strategy state before move */
    virtual void UndoMove() = 0;
        
    /** Write object to stream. Do not call directly, use operator<< */
    virtual void Write(std::ostream& stream) const;

private:
    /** The player that this strategy is for */
    SgBlackWhite m_player;

    /** Hash code of board region for which strategy is defined */
    SgHashCode m_code;
};

std::ostream& operator<<(std::ostream& stream, const SgStrategy& s);

//----------------------------------------------------------------------------

#endif // SG_STRATEGY_H
