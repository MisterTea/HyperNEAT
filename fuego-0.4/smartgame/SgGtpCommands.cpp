//----------------------------------------------------------------------------
/** @file SgGtpCommands.cpp
    See SgGtpCommands.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgGtpCommands.h"

#include <iomanip>
#include <iostream>
//#include <unistd.h>
#include "SgDebug.h"
#include "SgRandom.h"
#include "SgTime.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

string ParseCpuTimeId(const GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(1);
    if (cmd.NuArg() > 0)
        return cmd.Arg(0);
    return "total";
}

SgTimeMode TimeModeArg(const GtpCommand& cmd, size_t number)
{
    string arg = cmd.ArgToLower(number);
    if (arg == "cpu")
        return SG_TIME_CPU;
    if (arg == "real")
        return SG_TIME_REAL;
    throw GtpFailure() << "unknown time mode argument \"" << arg << '"';
}

string TimeModeToString(SgTimeMode mode)
{
    switch (mode)
    {
    case SG_TIME_CPU:
        return "cpu";
    case SG_TIME_REAL:
        return "real";
    default:
        SG_ASSERT(false);
        return "?";
    }
}

} // namespace

//----------------------------------------------------------------------------

SgGtpCommands::SgGtpCommands(GtpEngine& engine, const char* programPath)
    : m_programPath(programPath),
      m_engine(engine)
{
}

SgGtpCommands::~SgGtpCommands()
{
}

void SgGtpCommands::AddGoGuiAnalyzeCommands(GtpCommand& cmd)
{
    cmd <<
        "param/SmartGame Param/sg_param\n";
}

/** Run another GTP command and compare its response against a float value.
    Arguments: float command [arg...] <br>
    Returns: -1 if response is smaller than float; 1 otherwise.
*/
void SgGtpCommands::CmdCompareFloat(GtpCommand& cmd)
{
    double value = cmd.FloatArg(0);
    string response = m_engine.ExecuteCommand(cmd.RemainingLine(0));
    istringstream in(response);
    double responseValue;
    in >> responseValue;
    if (! in)
        throw GtpFailure() << "response '" << response << "' is not a float";
    cmd << (responseValue < value ? "-1" : "1");
}

/** Run another GTP command and compare its response against an integer value.
    Arguments: int command [arg...] <br>
    Returns: -1 if response is smaller than int; 0 if it is equal; 1 if it is
    greater
*/
void SgGtpCommands::CmdCompareInt(GtpCommand& cmd)
{
    int value = cmd.IntArg(0);
    string response = m_engine.ExecuteCommand(cmd.RemainingLine(0));
    istringstream in(response);
    int responseValue;
    in >> responseValue;
    if (! in)
        throw GtpFailure() << "response '" << response
                           << "' is not an integer";
    if (responseValue == value)
        cmd << "0";
    else if (responseValue < value)
        cmd << "-1";
    else
        cmd << "1";
}

/** Return process time.
    An optional string argument can be used as an ID for a timer.
    Default ID is "total".
    Other IDs are only allowed if cputime_reset was called with this ID
    before.
*/
void SgGtpCommands::CmdCpuTime(GtpCommand& cmd)
{

    string id = ParseCpuTimeId(cmd);
    double timeNow = SgTime::Get(SG_TIME_CPU);
    double timeDiff = timeNow;
    if (m_cpuTimes.find(id) == m_cpuTimes.end())
    {
        if (id != "total")
            throw GtpFailure() << "unknown cputime id " << id;
    }
    else
        timeDiff -= m_cpuTimes[id];
    cmd << fixed << setprecision(3) << timeDiff;
}

/** Reset process time.
    An optional string argument can be used as an ID for a timer.
    Default ID is "total".
*/
void SgGtpCommands::CmdCpuTimeReset(GtpCommand& cmd)
{
    string id = ParseCpuTimeId(cmd);
    double timeNow = SgTime::Get(SG_TIME_CPU);
    m_cpuTimes[id] = timeNow;
}

/** Run a debugger and attach it to the current program.
    Arguments: debugger-type <br>
    Currently implemented debugger types:
    - gdb_kde GDB in KDE terminal
    - gdb_gnome GDB in GNOME terminal
*/
void SgGtpCommands::CmdDebugger(GtpCommand& cmd)
{
#ifdef _MSC_VER
    throw GtpFailure("Disabled in msvc build");
#else
    cmd.CheckNuArg(1);
    string type = cmd.Arg(0);
    const char* path = m_programPath;
    if (path == 0)
        throw GtpFailure("location of executable unknown");
    pid_t pid = getpid();
    ostringstream s;
    if (type == "gdb_kde")
        s << "konsole -e gdb " << path << ' ' << pid << " &";
    else if (type == "gdb_gnome")
        s << "gnome-terminal -e 'gdb " << path << ' ' << pid << "' &";
    else
        throw GtpFailure() << "unknown debugger: " << type;
    SgDebug() << "Executing: " << s.str() << '\n';
    int retval = system(s.str().c_str());
    if (retval != 0)
        throw GtpFailure() << "command returned " << retval;
#endif
}

/** Echo command argument line as response.
    This command is compatible with GNU Go's 'echo' command.
*/
void SgGtpCommands::CmdEcho(GtpCommand& cmd)
{
    cmd << cmd.ArgLine();
}

/** Echo command argument line to std::cerr.
    This command is compatible with GNU Go's 'echo_err' command.
*/
void SgGtpCommands::CmdEchoErr(GtpCommand& cmd)
{
    string line = cmd.ArgLine();
    cerr << line << endl;
    cmd << line;
}

/** Execute GTP commands from a file.
    Argument: filename <br>
    Aborts on the first command that fails. Responses to the commands in the
    file are written to SgDebug()
    @see GtpEngine::ExecuteFile
*/
void SgGtpCommands::CmdExec(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    m_engine.ExecuteFile(cmd.Arg(0), SgDebug());
}

/** Return the current random seed.
    See SgRandom::SetSeed(int) for the special meaning of zero and negative
    values.
*/
void SgGtpCommands::CmdGetRandomSeed(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    cmd << SgRandom::Seed();
}

/** Set global parameters used in module SmartGame.
    Parameters:
    @arg @c time_mode cpu|real See SgTime
*/
void SgGtpCommands::CmdParam(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    if (cmd.NuArg() == 0)
    {
        // Boolean parameters first for better layout of GoGui parameter
        // dialog, alphabetically otherwise
        cmd << "[list/cpu/real] time_mode "
            << TimeModeToString(SgTime::DefaultMode()) << '\n';
    }
    else if (cmd.NuArg() >= 1 && cmd.NuArg() <= 2)
    {
        string name = cmd.Arg(0);
        if (name == "time_mode")
            SgTime::SetDefaultMode(TimeModeArg(cmd, 1));
        else
            throw GtpFailure() << "unknown parameter: " << name;
    }
    else
        throw GtpFailure() << "need 0 or 2 arguments";
}

#ifdef _MSC_VER
#include "windows.h"
#else
#include <unistd.h>
#endif

/** Return the process ID. */
void SgGtpCommands::CmdPid(GtpCommand& cmd)
{
    cmd.CheckArgNone();
#ifdef _MSC_VER
	cmd << GetCurrentProcessId();
#else
    cmd << getpid();
#endif
}

/** Set and store random seed.
    Arguments: seed <br>
    See SgRandom::SetSeed(int) for the special meaning of zero and negative
    values.
*/
void SgGtpCommands::CmdSetRandomSeed(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    SgRandom::SetSeed(cmd.IntArg(0));
}

/** Switch debug logging on/off. */
void SgGtpCommands::CmdQuiet(GtpCommand& cmd)
{
    if (cmd.BoolArg(0))
        SgDebugToNull();
    else
        SgSwapDebugStr(&cerr);
}

void SgGtpCommands::Register(GtpEngine& engine)
{
    engine.Register("cputime", &SgGtpCommands::CmdCpuTime, this);
    engine.Register("cputime_reset", &SgGtpCommands::CmdCpuTimeReset, this);
    engine.Register("echo", &SgGtpCommands::CmdEcho, this);
    engine.Register("echo_err", &SgGtpCommands::CmdEchoErr, this);
    engine.Register("get_random_seed", &SgGtpCommands::CmdGetRandomSeed, this);
    engine.Register("pid", &SgGtpCommands::CmdPid, this);
    engine.Register("set_random_seed", &SgGtpCommands::CmdSetRandomSeed, this);
    engine.Register("sg_debugger", &SgGtpCommands::CmdDebugger, this);
    engine.Register("sg_compare_float", &SgGtpCommands::CmdCompareFloat, this);
    engine.Register("sg_compare_int", &SgGtpCommands::CmdCompareInt, this);
    engine.Register("sg_exec", &SgGtpCommands::CmdExec, this);
    engine.Register("sg_param", &SgGtpCommands::CmdParam, this);
    engine.Register("quiet", &SgGtpCommands::CmdQuiet, this);
}

//----------------------------------------------------------------------------

