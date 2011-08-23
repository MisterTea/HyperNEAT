//----------------------------------------------------------------------------
/** @file SgTimeRecord.cpp
    See SgTimeRecord.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgTimeRecord.h"

#include <iomanip>
#include <limits>
#include <sstream>
#include "SgDebug.h"
#include "SgNode.h"
#include "SgProp.h"
#include "SgTime.h"
#include "SgWrite.h"

using namespace std;

//----------------------------------------------------------------------------

SgTimeRecord::SgTimeRecord(int numMoves, double period, double overhead,
                           bool loseOnTime)
    : m_overtimeNumMoves(numMoves),
      m_overtimePeriod(period),
      m_overhead(overhead),
      m_loseOnTime(loseOnTime),
      m_player(SG_BLACK),
      m_clockIsOn(false),
      m_suspended(false),
      m_atNode(0),
      m_timeLeft(0),
      m_movesLeft(0),
      m_timeOfLastUpdate(0)
{
}

SgTimeRecord::SgTimeRecord(bool oneMoveOnly, double timeForMove)
    : m_overtimeNumMoves(1),
      m_overtimePeriod(timeForMove),
      m_overhead(0),
      m_loseOnTime(false),
      m_player(SG_BLACK),
      m_clockIsOn(false),
      m_suspended(false),
      m_atNode(0),
      m_timeLeft(timeForMove),
      m_movesLeft(1),
      m_timeOfLastUpdate(0)
{
    SG_DEBUG_ONLY(oneMoveOnly);
    SG_ASSERT(oneMoveOnly);
}

SgBWArray<double> SgTimeRecord::GetTimeFromTree(SgNode& node)
{
    SgBWArray<double> time;
    time[SG_BLACK] =
        node.TopProp(SG_PROP_TIME_BLACK)->GetRealProp(SG_PROP_TIME_BLACK);
    time[SG_WHITE] =
        node.TopProp(SG_PROP_TIME_WHITE)->GetRealProp(SG_PROP_TIME_WHITE);
    return time;
}

SgBWArray<int> SgTimeRecord::GetOTMovesFromTree(SgNode& node)
{
    SgBWArray<int> numMoves;
    numMoves[SG_BLACK] =
        node.TopProp(SG_PROP_OT_BLACK)->GetIntProp(SG_PROP_OT_BLACK);
    numMoves[SG_WHITE] =
        node.TopProp(SG_PROP_OT_WHITE)->GetIntProp(SG_PROP_OT_WHITE);
    return numMoves;
}

SgClockState SgTimeRecord::GetClockState() const
{
    if (m_clockIsOn)
        return SG_CLOCK_RUNNING;
    else if (m_suspended)
        return SG_CLOCK_SUSPENDED;
    else
        return SG_CLOCK_OFF;
}

double SgTimeRecord::TimeLeft(SgBlackWhite player) const
{
    SG_ASSERT_BW(player);
    return m_timeLeft[player];
}

void SgTimeRecord::SetTimeInTree(SgNode& node, SgBWArray<double> time)
{
    node.SetRealProp(SG_PROP_TIME_BLACK, time[SG_BLACK]);
    node.SetRealProp(SG_PROP_TIME_WHITE, time[SG_WHITE]);
}

void SgTimeRecord::TurnClockOn(bool turnOn)
{
    if (m_suspended || (turnOn != m_clockIsOn))
    {
        if (turnOn)
            m_timeOfLastUpdate = SgTime::Get();
        else
            UpdateTimeLeft();
        m_clockIsOn = turnOn;
        m_suspended = false;
    }
}

void SgTimeRecord::SuspendClock()
{
    if (m_clockIsOn && ! m_suspended)
    {
        m_clockIsOn = false;
        m_suspended = true;
    }
}

void SgTimeRecord::SetClock(SgNode& node, SgBlackWhite player, double time)
{
    node.SetRealProp(SgProp::PlayerProp(SG_PROP_TIME_BLACK, player), time);
    m_timeLeft[player] = time;
    if (player == m_player)
        m_timeOfLastUpdate = SgTime::Get();
    if (m_timeLeft[player] <= 0.0001)
    {
        m_timeLeft[player] = OTPeriod();
        m_movesLeft[player] = OTNumMoves();
    }
}

void SgTimeRecord::UpdateTimeLeft()
{
    double now = SgTime::Get();
    if (m_clockIsOn && (now > m_timeOfLastUpdate))
    {
        bool outOfTime = (m_timeLeft[m_player] < now - m_timeOfLastUpdate);
        m_timeLeft[m_player] -= now - m_timeOfLastUpdate;
        if (outOfTime)
        {
            SgDebug() << "SgTimeRecord: outOfTime\n";
            if (UseOvertime() && m_movesLeft[m_player] <= 0)
            {
                SgDebug() << "SgTimeRecond: reseting overtime\n";
                m_timeLeft[m_player] += OTPeriod();
                if (m_timeLeft[m_player] > OTPeriod())
                    m_timeLeft[m_player] = OTPeriod();
                m_movesLeft[m_player] = OTNumMoves();
            }
            else if (LoseOnTime())
            {
                SgDebug() << "SgTimeRecord: lost on time\n";
                SuspendClock();
            }
        }
    }
    m_timeOfLastUpdate = now;
}

void SgTimeRecord::EnterNode(SgNode& node, SgBlackWhite player)
{
    if (m_clockIsOn && &node != m_atNode)
        SuspendClock();
    m_player = player;
    if (! m_clockIsOn)
    {
        m_atNode = &node;
        m_timeLeft = GetTimeFromTree(node);
        m_movesLeft = GetOTMovesFromTree(node);
        // AR: commented out in Modula-2: fTimeOfLastUpdate := Ticks();
    }
}

void SgTimeRecord::PlayedMove(SgNode& node, SgBlackWhite player)
{
    m_atNode = &node;
    if (m_suspended && ! node.HasSon())
        TurnClockOn(true);
    else if (m_clockIsOn)
    {
        UpdateTimeLeft();
        if (UseOvertime())
        {
            if (m_movesLeft[player] > 0)
            {
                m_movesLeft[player]--;
                if (m_movesLeft[player] == 0)
                {
                    SgDebug() << "SgTimeRecond: reseting overtime\n";
                    m_movesLeft[player] = OTNumMoves();
                    m_timeLeft[player] = OTPeriod();
                }
            }
            node.SetIntProp(SgProp::PlayerProp(SG_PROP_OT_BLACK, player),
                            m_movesLeft[player]);
        }
        m_timeLeft[player] -= Overhead();
        node.SetRealProp(SgProp::PlayerProp(SG_PROP_TIME_BLACK, player),
                         m_timeLeft[player]);
    }
}

//----------------------------------------------------------------------------

ostream& operator<<(ostream& out, const SgTimeRecord& time)
{
    out << SgWriteLabel("Overhead") << time.Overhead() << '\n'
        << SgWriteLabel("UseOvertime") << time.UseOvertime() << '\n'
        << SgWriteLabel("OTPeriod") << time.OTPeriod() << '\n'
        << SgWriteLabel("OTNumMoves") << time.OTNumMoves() << '\n'
        << SgWriteLabel("LoseOnTime") << time.LoseOnTime() << '\n'
        << SgWriteLabel("ClockIsRunning") << time.ClockIsRunning() << '\n'
        << SgWriteLabel("ClockState") << time.GetClockState() << '\n'
        << SgWriteLabel("TimeLeft/B") << time.TimeLeft(SG_BLACK) << '\n'
        << SgWriteLabel("TimeLeft/W") << time.TimeLeft(SG_WHITE) << '\n'
        << SgWriteLabel("MovesLeft/B") << time.MovesLeft(SG_BLACK) << '\n'
        << SgWriteLabel("MovesLeft/W") << time.MovesLeft(SG_WHITE) << '\n';
    return out;
}

ostream& operator<<(ostream& out, SgClockState clockState)
{
    switch (clockState)
    {
    case SG_CLOCK_OFF:
        out << "CLOCK_OFF";
        break;
    case SG_CLOCK_RUNNING:
        out << "CLOCK_RUNNING";
        break;
    case SG_CLOCK_SUSPENDED:
        out << "CLOCK_SUSPENDED";
        break;
    default:
        SG_ASSERT(false);
        out << "?";
    }
    return out;
}

//----------------------------------------------------------------------------

