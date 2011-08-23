//----------------------------------------------------------------------------
/** @file GoTimeControl.h
    Time management.
*/
//----------------------------------------------------------------------------

#ifndef GO_TIMECONTROL_H
#define GO_TIMECONTROL_H

#include "SgTimeControl.h"

class GoBoard;

//----------------------------------------------------------------------------

/** Default time management for Go.
    Implements SgDefaultTimeControl. The remaining number of moves in the game
    is estimated by comparing the number of empty points to the expected final
    number of empty points (can be tuned with the SetFinalSpace() parameter;
    default = 0.75).
*/
class GoTimeControl
    : public SgDefaultTimeControl
{
public:
    GoTimeControl(const GoBoard& bd);


    /** @name Parameters */
    // @{

    /** Set estimated proportion of occupied points at the end of the game.
        See class description.
    */
    float FinalSpace() const;

    /** See FinalSpace() */
    void SetFinalSpace(float finalspace);

    // @} // @name Parameters


    void GetPositionInfo(SgBlackWhite& toPlay, int& movesPlayed,
                         int& estimatedRemainingMoves);

private:
    const GoBoard& m_bd;

    /** Estimated proportion of remaining space that will be played out */
    float m_finalSpace;
};

//----------------------------------------------------------------------------

#endif // GO_TIMECONTROL_H
