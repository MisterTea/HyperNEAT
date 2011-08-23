//----------------------------------------------------------------------------
/** @file GoGtpTimeSettings.cpp
    See GoGtpTimeSettings.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoGtpTimeSettings.h"

//----------------------------------------------------------------------------

GoGtpTimeSettings::GoGtpTimeSettings()
    : m_mainTime(0),
      m_byoYomiTime(1),
      m_byoYomiStones(0)
{
    SG_ASSERT(NoTimeLimits());
}

GoGtpTimeSettings::GoGtpTimeSettings(int mainTime, int byoYomiTime,
                                 int byoYomiStones)
    : m_mainTime(mainTime),
      m_byoYomiTime(byoYomiTime),
      m_byoYomiStones(byoYomiStones)
{
    SG_ASSERT(mainTime >= 0);
    SG_ASSERT(byoYomiTime >= 0);
    SG_ASSERT(byoYomiStones >= 0);
}

bool GoGtpTimeSettings::operator==(const GoGtpTimeSettings& timeSettings)
    const
{
    return (timeSettings.m_mainTime == m_mainTime
            && timeSettings.m_byoYomiTime == m_byoYomiTime
            && timeSettings.m_byoYomiStones == m_byoYomiStones);
}

bool GoGtpTimeSettings::NoTimeLimits() const
{
    return (m_byoYomiTime > 0 && m_byoYomiStones == 0);
}

//----------------------------------------------------------------------------


