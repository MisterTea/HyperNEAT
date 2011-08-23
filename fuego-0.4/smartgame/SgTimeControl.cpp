//----------------------------------------------------------------------------
/** @file SgTimeControl.cpp
    See SgTimeControl.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgTimeControl.h"

#include "SgDebug.h"
#include "SgTimeRecord.h"

using namespace std;

//----------------------------------------------------------------------------

SgTimeControl::~SgTimeControl()
{
}

//----------------------------------------------------------------------------

SgDefaultTimeControl::SgDefaultTimeControl()
    : m_fastOpenFactor(0.25),
      m_fastOpenMoves(0),
      m_minTime(0),
      m_remainingConstant(1.0)
{
}

double SgDefaultTimeControl::FastOpenFactor() const
{
    return m_fastOpenFactor;
}

int SgDefaultTimeControl::FastOpenMoves() const
{
    return m_fastOpenMoves;
}

double SgDefaultTimeControl::RemainingConstant() const
{
    return m_remainingConstant;
}

void SgDefaultTimeControl::SetRemainingConstant(double value)
{
    m_remainingConstant = value;
}

void SgDefaultTimeControl::SetFastOpenFactor(double factor)
{
    m_fastOpenFactor = factor;
}

void SgDefaultTimeControl::SetFastOpenMoves(int nummoves)
{
    m_fastOpenMoves = nummoves;
}

void SgDefaultTimeControl::SetMinTime(double mintime)
{
    m_minTime = mintime;
}

double SgDefaultTimeControl::TimeForCurrentMove(const SgTimeRecord& time,
                                                bool quiet)
{
    SgBlackWhite toPlay;
    int estimatedRemainingMoves;
    int movesPlayed;
    GetPositionInfo(toPlay, movesPlayed, estimatedRemainingMoves);
    const bool useOvertime = time.UseOvertime();
    const bool isInOvertime = (useOvertime && time.MovesLeft(toPlay) > 0);
    double remainingMoves = 0.0;
    double timeForMove = 0.0;
    double timeLeft = time.TimeLeft(toPlay);
    if (isInOvertime)
    {
        remainingMoves = time.MovesLeft(toPlay);
        timeForMove = timeLeft / remainingMoves - time.Overhead();
    }
    else
    {
        double estimatedTotalMoves = movesPlayed + estimatedRemainingMoves;
        remainingMoves = m_remainingConstant * estimatedTotalMoves;
        if (useOvertime && remainingMoves > estimatedRemainingMoves)
            remainingMoves = estimatedRemainingMoves;
        remainingMoves = max(remainingMoves, 1.0);
        timeForMove = time.TimeLeft(toPlay) / remainingMoves - time.Overhead();
        if (useOvertime)
        {
            double firstOvertimeMove = time.OTPeriod() / time.OTNumMoves();
            if (timeForMove < firstOvertimeMove)
                timeForMove = firstOvertimeMove;
        }
    }

    // Don't use quite as much time for the first few moves, makes no real
    // difference in the quality of the moves.
    if (m_fastOpenMoves > 0 && movesPlayed <= m_fastOpenMoves)
        timeForMove *= m_fastOpenFactor;

    timeForMove = max(timeForMove, m_minTime);

    if (! quiet)
    {
        SgDebug() << "SgDefaultTimeControl: timeLeft=" << timeLeft;
        if (useOvertime)
            SgDebug() << '/' << time.MovesLeft(toPlay);
        SgDebug() << " remaining=" << remainingMoves << " timeMove="
                  << timeForMove << '\n';
    }

    return timeForMove;
}

//----------------------------------------------------------------------------

SgObjectWithDefaultTimeControl::~SgObjectWithDefaultTimeControl()
{
}

//----------------------------------------------------------------------------
