//----------------------------------------------------------------------------
/** @file GoSafetyCommands.h */
//----------------------------------------------------------------------------

#ifndef GO_SAFETYCOMMANDS_H
#define GO_SAFETYCOMMANDS_H

#include <string>
#include "GtpEngine.h"
#include "SgBWSet.h"

class GoBoard;

//----------------------------------------------------------------------------

/** GTP commands related to safety solvers. */
class GoSafetyCommands
{
public:
    /** Constructor.
        @param bd The game board.
    */
    GoSafetyCommands(const GoBoard& bd);

    void AddGoGuiAnalyzeCommands(GtpCommand& cmd);

    /** @page gosafetycommands GoSafetyCommands Commands
        - @link CmdDameStatic() @c go_safe_dame_static @endlink
        - @link CmdSafe() @c go_safe @endlink
        - @link CmdGfx() @c go_safe_gfx @endlink
    */
    /** @name Command Callbacks */
    // @{
    // The callback functions are documented in the cpp file
    void CmdDameStatic(GtpCommand& cmd);
    void CmdGfx(GtpCommand& cmd);
    void CmdSafe(GtpCommand& cmd);
    // @} // @name

    void Register(GtpEngine& engine);

private:
    const GoBoard& m_bd;

    SgBWSet GetSafe(int& totalRegions, const std::string& type);

    void Register(GtpEngine& e, const std::string& command,
                  GtpCallback<GoSafetyCommands>::Method method);
};

//----------------------------------------------------------------------------

#endif // GO_SAFETYCOMMANDS_H
