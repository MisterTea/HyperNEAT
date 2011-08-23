//----------------------------------------------------------------------------
/** @file FuegoTestEngine.cpp
    See FuegoTestEngine.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FuegoTestEngine.h"

#include <boost/preprocessor/stringize.hpp>
#include <boost/algorithm/string.hpp>
#include "GoGtpCommandUtil.h"
#include "GoGtpExtraCommands.h"
#include "SpAveragePlayer.h"
#include "SpCapturePlayer.h"
#include "SpDumbTacticalPlayer.h"
#include "SpGreedyPlayer.h"
#include "SpInfluencePlayer.h"
#include "SpLadderPlayer.h"
#include "SpLibertyPlayer.h"
#include "SpMaxEyePlayer.h"
#include "SpMinLibPlayer.h"
#include "SpRandomPlayer.h"
#include "SpSafePlayer.h"

using namespace std;
using boost::trim_copy;

//----------------------------------------------------------------------------

FuegoTestEngine::FuegoTestEngine(GtpInputStream& in, GtpOutputStream& out,
                                 int fixedBoardSize,
                                 const char* programPath,
                                 const string& player)
    : GoGtpEngine(in, out, fixedBoardSize, programPath),
      m_extraCommands(Board()),
      m_safetyCommands(Board())
{
    Register("fuegotest_param", &FuegoTestEngine::CmdParam, this);
    m_extraCommands.Register(*this);
    m_safetyCommands.Register(*this);
    SetPlayer(player);
}

FuegoTestEngine::~FuegoTestEngine()
{
}

void FuegoTestEngine::CmdAnalyzeCommands(GtpCommand& cmd)
{
    GoGtpEngine::CmdAnalyzeCommands(cmd);
    m_extraCommands.AddGoGuiAnalyzeCommands(cmd);
    m_safetyCommands.AddGoGuiAnalyzeCommands(cmd);
    cmd <<
        "param/FuegoTest Param/fuegotest_param\n";
    string response = cmd.Response();
    cmd.SetResponse(GoGtpCommandUtil::SortResponseAnalyzeCommands(response));
}

void FuegoTestEngine::CmdName(GtpCommand& cmd)
{
    if (m_playerId == "")
        cmd << "FuegoTest";
    else
        GoGtpEngine::CmdName(cmd);
}

/** Player selection.
    This command is compatible with the GoGui analyze command type "param".

    Parameters:
    @arg @c player Player id as in FuegoTestEngine::SetPlayer
*/
void FuegoTestEngine::CmdParam(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    if (cmd.NuArg() == 0)
    {
        cmd <<
            "[list/<none>/average/capture/dumbtactic/greedy/influence/"
            "ladder/liberty/maxeye/minlib/no-search/random/safe] player "
            << (m_playerId == "" ? "<none>" : m_playerId) << '\n';
    }
    else if (cmd.NuArg() >= 1 && cmd.NuArg() <= 2)
    {
        string name = cmd.Arg(0);
        if (name == "player")
        {
            try
            {
                string id = trim_copy(cmd.RemainingLine(0));
                if (id == "<none>")
                    id = "";
                SetPlayer(id);
            }
            catch (const SgException& e)
            {
                throw GtpFailure(e.what());
            }
        }
        else
            throw GtpFailure() << "unknown parameter: " << name;
    }
    else
        throw GtpFailure() << "need 0 or 2 arguments";
}

void FuegoTestEngine::CmdVersion(GtpCommand& cmd)
{
#ifdef VERSION
    cmd << BOOST_PP_STRINGIZE(VERSION);
#else
    cmd << "(" __DATE__ ")";
#endif
#ifndef NDEBUG
    cmd << " (dbg)";
#endif
}

GoPlayer* FuegoTestEngine::CreatePlayer(const string& playerId)
{
    GoBoard& bd = Board();
    if (playerId == "")
        return 0;
    if (playerId == "average")
        return new SpAveragePlayer(bd);
    if (playerId == "capture")
        return new SpCapturePlayer(bd);
    if (playerId == "dumbtactic")
        return new SpDumbTacticalPlayer(bd);
    if (playerId == "greedy")
        return new SpGreedyPlayer(bd);
    if (playerId == "influence")
        return new SpInfluencePlayer(bd);
    if (playerId == "ladder")
        return new SpLadderPlayer(bd);
    if (playerId == "liberty")
        return new SpLibertyPlayer(bd);
    if (playerId == "maxeye")
        return new SpMaxEyePlayer(bd, true);
    if (playerId == "minlib")
        return new SpMinLibPlayer(bd);
    if (playerId == "random")
        return new SpRandomPlayer(bd);
    if (playerId == "safe")
        return new SpSafePlayer(bd);
    throw SgException("unknown player " + playerId);
}

void FuegoTestEngine::SetPlayer(const string& playerId)
{
    GoPlayer* player = CreatePlayer(playerId);
    GoGtpEngine::SetPlayer(player);
    m_playerId = playerId;
}

//----------------------------------------------------------------------------
