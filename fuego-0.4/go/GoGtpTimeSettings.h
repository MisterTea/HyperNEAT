//----------------------------------------------------------------------------
/** @file GoGtpTimeSettings.h
    Time settings as used in time_settings standard GTP command.
*/
//----------------------------------------------------------------------------

#ifndef GO_GTPTIMESETTINGS_H
#define GO_GTPTIMESETTINGS_H

//----------------------------------------------------------------------------

/** Time settings as used in time_settings standard GTP command. */
class GoGtpTimeSettings
{
public:
    /** Construct time settings with no time limit. */
    GoGtpTimeSettings();

    /** Construct time settings.
        Supports Canadian byo yomi, including absolute time (no byo yomi)
        as a special case.
        Setting mainTime=0 means that the engine immediately starts in byo
        yomi. Setting byoYomiTime means that if the main time is up before
        the game is over, the engine loses on time. Setting byoYomiTime>0
        and byoYomiStones=0 means no time limits.
        @param mainTime Main time measured in seconds.
        @param byoYomiTime Byo yomi time measured in seconds.
        @param byoYomiStones Number of stones per byo yomi period.
    */
    GoGtpTimeSettings(int mainTime, int byoYomiTime, int byoYomiStones);

    bool operator==(const GoGtpTimeSettings& timeSettings) const;

    int MainTime() const;

    int ByoYomiTime() const;

    int ByoYomiStones() const;

    /** Check if no time limits are used.
        Equivalent to byoYomiTime>0 and byoYomiStones=0.
    */
    bool NoTimeLimits() const;

private:
    /** Main time measured in seconds. */
    int m_mainTime;

    /** Byo yomi time measured in seconds. */
    int m_byoYomiTime;

    /** Number of stones per byo yomi period. */
    int m_byoYomiStones;
};

inline int GoGtpTimeSettings::MainTime() const
{
    return m_mainTime;
}

inline int GoGtpTimeSettings::ByoYomiTime() const
{
    return m_byoYomiTime;
}

inline int GoGtpTimeSettings::ByoYomiStones() const
{
    return m_byoYomiStones;
}

//----------------------------------------------------------------------------

#endif // GO_GTPTIMESETTINGS_H

