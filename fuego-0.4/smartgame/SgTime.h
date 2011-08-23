//----------------------------------------------------------------------------
/** @file SgTime.h
    Time measurement.
*/
//----------------------------------------------------------------------------

#ifndef SG_TIME_H
#define SG_TIME_H

#include <string>

//----------------------------------------------------------------------------

/** Time measurement mode used in SgTime. */
enum SgTimeMode
{
    /** Measure CPU-time.
        Returns the CPU time of the current thread including child processes.
    */
    SG_TIME_CPU,

    /** Measure real time. */
    SG_TIME_REAL
};

//----------------------------------------------------------------------------

/** Get current time, and formatting functions */
namespace SgTime
{
    /** Format time as MM:SS. */
    std::string Format(double time, bool minsAndSecs = true);

    /** Get time using the default mode. */
    double Get();

    /** Get time using explicit mode. */
    double Get(SgTimeMode mode);

    /** Return current default time mode. */
    SgTimeMode DefaultMode();

    /** Set default time mode. */
    void SetDefaultMode(SgTimeMode mode);

    std::string TodaysDate();
}

//----------------------------------------------------------------------------

#endif // SG_TIME_H
