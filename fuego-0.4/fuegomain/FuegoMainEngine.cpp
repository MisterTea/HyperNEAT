//----------------------------------------------------------------------------
/** @file FuegoMainEngine.cpp
    See FuegoMainEngine.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "FuegoMainEngine.h"

#include "FuegoMainUtil.h"
#include "GoGtpCommandUtil.h"
#include "GoUctPlayer.h"

using namespace std;

//----------------------------------------------------------------------------

FuegoMainEngine::FuegoMainEngine(GtpInputStream& in, GtpOutputStream& out,
                                 int fixedBoardSize, const char* programPath,
                                 bool noHandicap)
    : GoGtpEngine(in, out, fixedBoardSize, programPath, false, noHandicap),
      m_uctCommands(Board(), m_player),
      m_autoBookCommands(Board(), m_player, m_autoBook),
      m_safetyCommands(Board())
{
    m_uctCommands.Register(*this);
    m_safetyCommands.Register(*this);
    m_autoBookCommands.Register(*this);
    Register("fuego-license", &FuegoMainEngine::CmdLicense, this);
    SetPlayer(new PlayerType(Board()));
}

FuegoMainEngine::~FuegoMainEngine()
{
}

void FuegoMainEngine::CmdAnalyzeCommands(GtpCommand& cmd)
{
    GoGtpEngine::CmdAnalyzeCommands(cmd);
    m_uctCommands.AddGoGuiAnalyzeCommands(cmd);
    m_safetyCommands.AddGoGuiAnalyzeCommands(cmd);
    m_autoBookCommands.AddGoGuiAnalyzeCommands(cmd);
    cmd << "string/Fuego License/fuego-license\n";
    string response = cmd.Response();
    cmd.SetResponse(GoGtpCommandUtil::SortResponseAnalyzeCommands(response));
}

void FuegoMainEngine::CmdLicense(GtpCommand& cmd)
{
    cmd << "\n" <<
        "Fuego " << FuegoMainUtil::Version() << "\n" <<
        "Copyright (C) 2009 - 10 by the authors of the Fuego project.\n"
        "See http://fuego.sf.net for information about Fuego. Fuego comes\n"
        "with NO WARRANTY to the extent permitted by law. This program is\n"
        "free software; you can redistribute it and/or modify it under the\n"
        "terms of the GNU Lesser General Public License as published by the\n"
        "Free Software Foundation - version 3. For more information about\n"
        "these matters, see the files named COPYING and COPYING.LESSER\n";
}

void FuegoMainEngine::CmdName(GtpCommand& cmd)
{
    cmd << "Fuego";
}

/** Return Fuego version.
    @see FuegoMainUtil::Version()
*/
void FuegoMainEngine::CmdVersion(GtpCommand& cmd)
{
    cmd << FuegoMainUtil::Version();
}

//----------------------------------------------------------------------------
