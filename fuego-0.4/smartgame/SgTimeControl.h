//----------------------------------------------------------------------------
/** @file SgTimeControl.h */
//----------------------------------------------------------------------------

#ifndef SG_TIMECONTROL_H
#define SG_TIMECONTROL_H

#include "SgBlackWhite.h"

class SgTimeRecord;

//----------------------------------------------------------------------------

/** Basic interface for all time managers. */
class SgTimeControl
{
public:
    virtual ~SgTimeControl();

    /** Suggest a time for the current move.
        See class description.
        @param timeRecord Time settings and clock state of current game.
        @param quiet Don't print logging information to SgDebug()
        @return Time suggestion for current move in seconds.
    */
    virtual double TimeForCurrentMove(const SgTimeRecord& timeRecord,
                                      bool quiet = false) = 0;

};

//----------------------------------------------------------------------------

/** Time management.
    This class provides a reasonable default algorithm for time management.
    It queries the estimated number of remaining moves by a virtual
    function that needs to be implemented in the game-dependent subclass.
    -# If in an overtime period the remaining number of moves is used exactly
       as returned by GetPositionInfo().
    -# The time for a move is the remaining time (in the main time or current
       overtime period) divided by the number of remaining moves.
    -# During the opening (first 10 moves; can be controlled by
       SetFastOpen()), the time for a move reduced by multiplying it
       by a constant factor (default factor is 0.25).
    -# [A minimum time of 0.1 is also enforced, but this might become
        obsolete, see SetMinTime()]
    -# The parameter RemainingConstant() can be used to spend exponentially
       more time earlier in the game
*/
class SgDefaultTimeControl
    : public SgTimeControl
{
public:
    SgDefaultTimeControl();


    /** @name Parameters */
    // @{

    /** Set time reduction factor for fast opening moves.
        Default is 0.25.
        See class description.
    */
    double FastOpenFactor() const;

    /** See FastOpenFactor() */
    void SetFastOpenFactor(double factor);

    /** Set how many opening moves should be played quickly.
        Default is 0.
        See class description.
    */
    int FastOpenMoves() const;

    /** See FastOpenMoves() */
    void SetFastOpenMoves(int nummoves);

    /** Parameter to spend exponentially more time earlier in the game.
        This parameter cuts the number of expected remaining moves from
        the real expectation to a constant. Always expecting a constant number
        of remaining moves gives earlier moves more time. Since a good value
        depends on the average game length and therefore probably the board
        size, the constant is not given in moves, but as a fraction between 0
        and 1, and the constant is computed by multiplication with the
        expected number of total moves by the current player in the current
        game (moves played plus expected remaining moves). The default value
        is 1.0, which does not limit the number of expected remaining moves.
        The smaller the value, the more time is spent in the early phase of
        the game.
    */
    double RemainingConstant() const;

    /** See RemainingConstant() */
    void SetRemainingConstant(double value);

    /** Set minimum time for any move.
        Could be made obsolete? If the player cannot generate a meaningful
        move in less than a minimum time, he can decide itself to ignore
        the time limit.
    */
    void SetMinTime(double mintime);

    // @} // @name Parameters


    double TimeForCurrentMove(const SgTimeRecord& timeRecord,
                              bool quiet = false);

    /** Get game-specific information about the current position.
        @param[out] toPlay Current color to move.
        @param[out] movesPlayed Moves already played (by the current player)
        @param[out] estimatedRemainingMoves An estimate of the number of
        remaining moves (for the current player) in the game.
    */
    virtual void GetPositionInfo(SgBlackWhite& toPlay,
                                 int& movesPlayed,
                                 int& estimatedRemainingMoves) = 0;

private:
    /** See FastOpenFactor() */
    double m_fastOpenFactor;

    /** See FastOpenMoves() */
    int m_fastOpenMoves;

    /** See SetMinTime() */
    double m_minTime;

    /** See RemainingConstant() */
    double m_remainingConstant;
};

//----------------------------------------------------------------------------

/** Abstract interface for players (or other objects) that own an instance
    of SgDefaultTimeControl.
    Can be used to query a player at runtime (with a dynamic_cast), whether it
    uses such an object and set parameters.
*/
class SgObjectWithDefaultTimeControl
{
public:
    virtual ~SgObjectWithDefaultTimeControl();

    virtual SgDefaultTimeControl& TimeControl() = 0;

    virtual const SgDefaultTimeControl& TimeControl() const = 0;
};

//----------------------------------------------------------------------------

#endif // SG_TIMECONTROL_H
