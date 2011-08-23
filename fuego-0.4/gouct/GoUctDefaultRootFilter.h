//----------------------------------------------------------------------------
/** @file GoUctDefaultRootFilter.h */
//----------------------------------------------------------------------------

#ifndef GOUCT_DEFAULTROOTFILTER_H
#define GOUCT_DEFAULTROOTFILTER_H

#include "GoLadder.h"
#include "GoUctRootFilter.h"

class GoBoard;

//----------------------------------------------------------------------------

/** Default root filter used by GoUctPlayer. */
class GoUctDefaultRootFilter
    : public GoUctRootFilter
{
public:
    GoUctDefaultRootFilter(const GoBoard& bd);

    /** @name Pure virtual functions of GoUctRootFilter */
    // @{

    /** Get moves to filter in the current position.
        This function is invoked by the player before the search, it does not
        need to be thread-safe.
    */
    std::vector<SgPoint> Get();

    // @} // @name


    /** @name Parameters */
    // @{

    /** Prune unsuccesful ladder defense moves (unless the ladder would be
        short).
    */
    bool CheckLadders() const;

    /** See CheckLadders() */
    void SetCheckLadders(bool enable);

    // @} // @name

private:
    const GoBoard& m_bd;

    GoLadder m_ladder;

    /** See CheckLadders() */
    bool m_checkLadders;

    /** Minimum ladder length necessary to prune loosing ladder defense moves.
        @see m_checkLadders
    */
    int m_minLadderLength;

    /** Local variable in Get().
        Reused for efficiency.
    */
    mutable SgVector<SgPoint> m_ladderSequence;
};

inline bool GoUctDefaultRootFilter::CheckLadders() const
{
    return m_checkLadders;
}

inline void GoUctDefaultRootFilter::SetCheckLadders(bool enable)
{
    m_checkLadders = enable;
}

//----------------------------------------------------------------------------

#endif // GOUCT_DEFAULTROOTFILTER_H
