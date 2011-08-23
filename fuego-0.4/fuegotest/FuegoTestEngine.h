//----------------------------------------------------------------------------
/** @file FuegoTestEngine.h
*/
//----------------------------------------------------------------------------

#ifndef FUEGOTEST_ENGINE_H
#define FUEGOTEST_ENGINE_H

#include "GoGtpEngine.h"
#include "GoGtpExtraCommands.h"
#include "GoSafetyCommands.h"

//----------------------------------------------------------------------------

/** GTP interface with commands for testing functionality of the Fuego
    libraries.
    @see @ref fuegotestoverview
*/
class FuegoTestEngine
    : public GoGtpEngine
{
public:
    /** Constructor.
        @param in
        @param out
        @param fixedBoardSize
        @param programPath
        @param player Player ID as in CreatePlayer()
    */
    FuegoTestEngine(GtpInputStream& in, GtpOutputStream& out,
                    int fixedBoardSize, const char* programPath = 0,
                    const std::string& player = "");

    ~FuegoTestEngine();

    /** @page fuegotestenginecommands FuegoTestEngine Commands
        - @link CmdParam() @c fuegotest_param @endlink
    */
    void CmdAnalyzeCommands(GtpCommand& cmd);
    void CmdName(GtpCommand& cmd);
    void CmdParam(GtpCommand& cmd);
    void CmdVersion(GtpCommand& cmd);

private:
    GoGtpExtraCommands m_extraCommands;

    GoSafetyCommands m_safetyCommands;

    /** Player ID as in CreatePlayer() */
    std::string m_playerId;

    GoPlayer* CreatePlayer(const std::string& name);

    void SetPlayer(const std::string& playerId);
};

//----------------------------------------------------------------------------

#endif // FUEGOTEST_ENGINE_H

