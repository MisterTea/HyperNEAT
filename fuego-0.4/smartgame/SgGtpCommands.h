//----------------------------------------------------------------------------
/** @file SgGtpCommands.h
    General utility GTP commands.
*/
//----------------------------------------------------------------------------

#ifndef SG_GTPCOMMANDS_H
#define SG_GTPCOMMANDS_H

#include "GtpEngine.h"

//----------------------------------------------------------------------------

/** General utility GTP commands.
    Implements generally useful command, which do not require a board or
    player. Uses functionality from module SmartGo.
    This class can be used in GTP engines.
    @note
    You should make it a member of your engine to ensure that its lifetime
    is as long as the engine's lifetime, otherwise the pointers in GtpCallback
    will point to freed memory.
    @see @ref sggtpcommandscommands
*/
class SgGtpCommands
{
public:
    /** Constructor.
        @param engine The GTP engine (needed for wrapper commands that invoke
        arbitrary other engine commands like sg_compare_float)
        @param programPath The path to the executable for the engine for
        sg_debug, 0 if unknown
    */
    SgGtpCommands(GtpEngine& engine, const char* programPath = 0);

    virtual ~SgGtpCommands();

    /** @page sggtpcommandscommands SgGtpCommands Commands
        - @link CmdCpuTime() @c cputime @endlink
        - @link CmdCpuTimeReset() @c cputime_reset @endlink
        - @link CmdEcho() @c echo @endlink
        - @link CmdEchoErr() @c echo_err @endlink
        - @link CmdGetRandomSeed() @c get_random_seed @endlink
        - @link CmdPid() @c pid @endlink
        - @link CmdSetRandomSeed() @c set_random_seed @endlink
        - @link CmdCompareFloat() @c sg_compare_float @endlink
        - @link CmdCompareInt() @c sg_compare_int @endlink
        - @link CmdDebugger() @c sg_debugger @endlink
        - @link CmdExec() @c sg_exec @endlink
        - @link CmdParam() @c sg_param @endlink
        - @link CmdQuiet() @c quiet @endlink
    */
    /** @name Command Callbacks */
    // @{
    virtual void CmdCompareFloat(GtpCommand&);
    virtual void CmdCompareInt(GtpCommand&);
    virtual void CmdCpuTime(GtpCommand&);
    virtual void CmdCpuTimeReset(GtpCommand&);
    virtual void CmdDebugger(GtpCommand&);
    virtual void CmdEcho(GtpCommand&);
    virtual void CmdEchoErr(GtpCommand&);
    virtual void CmdExec(GtpCommand&);
    virtual void CmdGetRandomSeed(GtpCommand&);
    virtual void CmdParam(GtpCommand&);
    virtual void CmdPid(GtpCommand&);
    virtual void CmdSetRandomSeed(GtpCommand&);
    virtual void CmdQuiet(GtpCommand&);
    // @} // @name

    void AddGoGuiAnalyzeCommands(GtpCommand& cmd);

    /** Register commands at engine.
        Make sure that this object lives as long as the GtpEngine,
        for example by making it a member of the engine.
    */
    void Register(GtpEngine& engine);

private:
    const char* m_programPath;

    GtpEngine& m_engine;

    /** Cputimes for cputime and cputime_reset commands. */
    std::map<std::string,double> m_cpuTimes;
};

//----------------------------------------------------------------------------

#endif // SG_GTPCOMMANDS_H
