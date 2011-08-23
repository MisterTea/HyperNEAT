//----------------------------------------------------------------------------
/** @file GoSetup.h
    Initial setup information for GoBoard.
*/
//----------------------------------------------------------------------------

#ifndef GO_SETUP_H
#define GO_SETUP_H

#include "SgBlackWhite.h"
#include "SgBWSet.h"
#include "SgPoint.h"

//----------------------------------------------------------------------------

/** Setup information for start position in GoBoard. */
class GoSetup
{
public:
    SgBWSet m_stones;

    SgBlackWhite m_player;

    /** Create empty setup.
        Contains no setup stones. Black is to move.
    */
    GoSetup();

    bool operator==(const GoSetup& setup) const;

    bool operator!=(const GoSetup& setup) const;

    /** Add a black stone.
        Convenience function. Equivalent to @c m_stones[SG_BLACK].Include(p)
    */
    void AddBlack(SgPoint p);

    /** Add a black stone.
        Convenience function. Equivalent to @c m_stones[SG_WHITE].Include(p)
    */
    void AddWhite(SgPoint p);

    /** Does the setup contain no setup stones and is the player Black. */
    bool IsEmpty() const;
};

inline GoSetup::GoSetup()
    : m_player(SG_BLACK)
{
}

inline bool GoSetup::operator==(const GoSetup& setup) const
{
    return (m_stones == setup.m_stones && m_player == setup.m_player);
}

inline bool GoSetup::operator!=(const GoSetup& setup) const
{
    return ! operator==(setup);
}

inline void GoSetup::AddBlack(SgPoint p)
{
    m_stones[SG_BLACK].Include(p);
}

inline void GoSetup::AddWhite(SgPoint p)
{
    m_stones[SG_WHITE].Include(p);
}

inline bool GoSetup::IsEmpty() const
{
    return (m_stones.BothEmpty() && m_player == SG_BLACK);
}

//----------------------------------------------------------------------------

#endif // GO_SETUP_H

