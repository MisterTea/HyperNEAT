//----------------------------------------------------------------------------
/** @file SgTime.cpp
    See SgTime.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgTime.h"
#include "SgException.h"

#include <cstring>
#include <ctime>
#include <iomanip>
#include <limits>
#include <iostream>
#include <sstream>
#include <cstdio>
#ifndef _MSC_VER
#include <errno.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#else

#include < time.h >
struct timezone
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};

// From http://www.openasthra.com/c-tidbits/gettimeofday-function-for-windows/

#include <windows.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
  #define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;

  if (NULL != tv)
  {
    GetSystemTimeAsFileTime(&ft);

    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;

    /*converting file time to unix epoch*/
    tmpres /= 10;  /*convert into microseconds*/
    tmpres -= DELTA_EPOCH_IN_MICROSECS;
    tv->tv_sec = (long)(tmpres / 1000000UL);
    tv->tv_usec = (long)(tmpres % 1000000UL);
  }

  if (NULL != tz)
  {
    if (!tzflag)
    {
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }

  return 0;
}

#endif

using namespace std;

//----------------------------------------------------------------------------

namespace {

SgTimeMode g_defaultMode = SG_TIME_REAL;

bool g_isInitialized = false;

clock_t g_ticksPerSecond;

clock_t g_ticksPerMinute;

void Init()
{
    int ticksPerSecond = CLOCKS_PER_SEC;
    if (ticksPerSecond < 0) // Shouldn't happen
    {
        throw SgException("Could not get _SC_CLK_TCK.");
    }
    g_ticksPerSecond = static_cast<clock_t>(ticksPerSecond);
    g_ticksPerMinute = 60 * g_ticksPerSecond;
    g_isInitialized = true;
}

} // namespace

//----------------------------------------------------------------------------

string SgTime::Format(double time, bool minsAndSecs)
{
    ostringstream out;
    if (minsAndSecs)
    {
        int mins = static_cast<int>(time / 60);
        int secs = static_cast<int>(time - mins * 60);
        out << setw(2) << mins << ':' << setw(2) << setfill('0') 
            << secs;
    }
    else
        out << setprecision(2) << fixed << time;
    return out.str();
}

double SgTime::Get()
{
    return Get(g_defaultMode);
}

double SgTime::Get(SgTimeMode mode)
{
    if (! g_isInitialized)
        Init();
    switch (mode)
    {
    case SG_TIME_CPU:
        {
            clock_t clockTicks = clock();
            return static_cast<double>(clockTicks) / g_ticksPerSecond;
        }
    case SG_TIME_REAL:
        {
            struct timeval timeVal;
            if (gettimeofday(&timeVal, 0) != 0)
                throw SgException(string("gettimeofday: ") + strerror(errno));
            return (timeVal.tv_sec
                    + static_cast<double>(timeVal.tv_usec) / 1e6);
        }
    default:
        SG_ASSERT(false);
        return 0;
    }
}

SgTimeMode SgTime::DefaultMode()
{
    return g_defaultMode;
}

void SgTime::SetDefaultMode(SgTimeMode mode)
{
    g_defaultMode = mode;
}

string SgTime::TodaysDate()
{
    time_t systime = time(0);
    struct tm* currtime = localtime(&systime);
    const int BUF_SIZE = 14;
    char buf[BUF_SIZE];
    strftime(buf, BUF_SIZE - 1, "%Y-%m-%d", currtime);
    return string(buf);
}

//----------------------------------------------------------------------------

