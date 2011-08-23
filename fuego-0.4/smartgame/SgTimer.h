//----------------------------------------------------------------------------
/** @file SgTimer.h
    Class SgTimer.
*/
//----------------------------------------------------------------------------

#ifndef SG_TIMER_H
#define SG_TIMER_H

#include "SgTime.h"

//----------------------------------------------------------------------------

/** Timer.
    For checking the elapsed time, without calling SgTime::Get each time.
*/
class SgTimer
{
public:
    /** Constructor.
        Also starts the timer.
    */
    SgTimer();

    /** Get elapsed time.
        Returns time since last start or between last start and stop if timer
        is stopped.
    */
    double GetTime() const;

    bool IsStopped() const;

    /** Check for timeout.
        This function can only be used with fixed parameters per instance
        of SgTimer.
        @param maxTime
        @param checkFreq Do the comparison only every n calls for efficiency.
        @todo The timeout functionality should be extracted to a separate
        class SgTimeout, which takes maxTime as constructor arguments.
    */
    bool IsTimeOut(double maxTime, std::size_t checkFreq = 16);

    /** Reset timer. */
    void Start();

    /** Stop timer. */
    void Stop();

private:
    bool m_isStopped;

    bool m_isTimeOut;

    /* For managing the frequency of calling SgTime::Get() in IsTimeOut(). */
    std::size_t m_counter;

    /* Time when we start searching. */
    double m_timeStart;

    /* Elapsed time when timer was stopped. */
    double m_timeStop;

    /** Not implemented */
    SgTimer(const SgTimer& timer);
};

inline SgTimer::SgTimer()
    : m_isStopped(false),
      m_isTimeOut(false),
      m_counter(0)
{
    Start();
}

inline double SgTimer::GetTime() const
{
    if (m_isStopped)
        return m_timeStop;
    return (SgTime::Get() -  m_timeStart);
}

inline bool SgTimer::IsStopped() const
{
    return m_isStopped;
}

inline bool SgTimer::IsTimeOut(double maxTime, std::size_t checkFreq)
{
    if (m_isTimeOut)
        return true;
    if (m_counter == 0)
    {
        double timeNow = SgTime::Get();
        if (timeNow - m_timeStart > maxTime)
        {
            m_isTimeOut = true;
            return true;
        }
        else
            m_counter = checkFreq;
    }
    else
        --m_counter;
    return false;
}

inline void SgTimer::Start()
{
    m_timeStart = SgTime::Get();
    m_isStopped = false;
}

inline void SgTimer::Stop()
{
    SG_ASSERT(! IsStopped());
    m_timeStop = (SgTime::Get() -  m_timeStart);
    m_isStopped = true;
}

//----------------------------------------------------------------------------

#endif // SG_TIMER_H
