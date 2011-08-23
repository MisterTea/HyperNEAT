//----------------------------------------------------------------------------
/** @file GoGtpExtraCommands.h
*/
//----------------------------------------------------------------------------

#ifndef GO_GTPEXTRACOMMANDS_H
#define GO_GTPEXTRACOMMANDS_H

#include <string>
#include "GtpEngine.h"

class GoBoard;
class GoPlayer;

//----------------------------------------------------------------------------

/** Extra GTP commands to access functionality of the Go library.
    Contains commands that are mainly useful for debugging and testing
    the Go library and that not all Go GTP engines may want to register.
*/
class GoGtpExtraCommands
{
public:
    /** Constructor.
        @param bd The game board.
    */
    GoGtpExtraCommands(GoBoard& bd);

    void AddGoGuiAnalyzeCommands(GtpCommand& cmd);

    /** @page gogtpextracommands GoGtpExtraCommands Commands
        - @link CmdCfgDistance() @c go_distance @endlink
        - @link CmdLadder() @c go_ladder @endlink
        - @link CmdStaticLadder() @c go_static_ladder @endlink
    */
    /** @name Command Callbacks */
    // @{
    // The callback functions are documented in the cpp file
    void CmdCfgDistance(GtpCommand& cmd);
    void CmdLadder(GtpCommand& cmd);
    void CmdStaticLadder(GtpCommand& cmd);
    // @} // @name

    void Register(GtpEngine& engine);

private:
    GoBoard& m_bd;

    void Register(GtpEngine& e, const std::string& command,
                  GtpCallback<GoGtpExtraCommands>::Method method);
};

//----------------------------------------------------------------------------

#endif // GO_GTPEXTRACOMMANDS_H
