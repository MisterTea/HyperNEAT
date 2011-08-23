//----------------------------------------------------------------------------
/** @file GoSafetyCommands.cpp
    See GoSafetyCommands.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoSafetyCommands.h"

#include "GoBensonSolver.h"
#include "GoBoard.h"
#include "GoGtpCommandUtil.h"
#include "GoModBoard.h"
#include "GoSafetyUtil.h"
#include "GoSafetySolver.h"
#include "SgPointSet.h"

using namespace std;
using GoGtpCommandUtil::BlackWhiteArg;

//----------------------------------------------------------------------------

GoSafetyCommands::GoSafetyCommands(const GoBoard& bd)
    : m_bd(bd)
{
}

void GoSafetyCommands::AddGoGuiAnalyzeCommands(GtpCommand& cmd)
{
    cmd <<
        "gfx/Go Safe Benson/go_safe_gfx benson\n"
        "gfx/Go Safe Static/go_safe_gfx static\n"
        "plist/Go Safe Dame Static/go_safe_dame_static\n";
}

/** Return dame points after running static safety algorithm. */
void GoSafetyCommands::CmdDameStatic(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    GoModBoard modBoard(m_bd);
    GoBoard& bd = modBoard.Board();
    GoRegionBoard regionAttachment(bd);
    GoSafetySolver solver(bd, &regionAttachment);
    SgBWSet safe;
    solver.FindSafePoints(&safe);
    SgPointSet dame = GoSafetyUtil::FindDamePoints(bd, m_bd.AllEmpty(), safe);
    cmd << SgWritePointSet(dame, "", false);
}

/** Information about safe points optimized for graphical display in GoGui.
    This command is compatible with GoGui's analyze command type "gfx".
    Arguments: benson|static <br>
    Returns: GoGui gfx commands to display safe points and additional
    information in the status line
    - black and white territory: safe points
    - Color Magenta (#980098): dame points
    - Color Red: safe for black and white (should not happen)
    - Circle: unsurroundable
    - Status line: point counts, percentage of safe points
*/
void GoSafetyCommands::CmdGfx(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    string type = cmd.Arg(0);
    int totalRegions = 0;
    SgBWSet safe = GetSafe(totalRegions, type);
    SgPointSet dame;
    SgPointSet unsurroundable;
    GoSafetyUtil::FindDameAndUnsurroundablePoints(m_bd, m_bd.AllEmpty(), safe,
                                                  &dame, &unsurroundable);
    cmd << "BLACK";
    for (SgSetIterator it(safe[SG_BLACK]); it; ++it)
        cmd << ' ' << SgWritePoint(*it);
    cmd << '\n';
    cmd << "WHITE";
    for (SgSetIterator it(safe[SG_WHITE]); it; ++it)
        cmd << ' ' << SgWritePoint(*it);
    cmd << '\n';
    cmd << "COLOR #980098";
    for (SgSetIterator it(dame); it; ++it)
        cmd << ' ' << SgWritePoint(*it);
    cmd << '\n';
    cmd << "CIRCLE";
    for (SgSetIterator it(unsurroundable); it; ++it)
        cmd << ' ' << SgWritePoint(*it);
    cmd << '\n';
    SgPointSet blackAndWhite = safe[SG_WHITE] & safe[SG_BLACK];
    if (blackAndWhite.Size() > 0)
    {
        // Shouldn't happen
        cmd << "COLOR red ";
        for (SgSetIterator it(blackAndWhite); it; ++it)
            cmd << ' ' << SgWritePoint(*it);
        cmd << '\n';
    }
    int nuBlack = safe[SG_BLACK].Size();
    int nuWhite = safe[SG_WHITE].Size();
    int nuPoints = m_bd.AllPoints().Size();
    cmd << "TEXT Solver: " << cmd.Arg(0)
        << "  B: " << nuBlack << " (" << (100 * nuBlack / nuPoints) << " %)"
        << "  W: " << nuWhite << " (" << (100 * nuWhite / nuPoints) << " %)"
        << "  Both: " << (nuBlack + nuWhite)
        << " (" << (100 * (nuBlack + nuWhite) / nuPoints) << " %)"
        << "  Regions: " << totalRegions;
}

/** List of safe points.
    If no color is given, safe points of both colors are listed.
    Arguments: benson|static [black|white]<br>
    Returns: number of point followed bu list of points in one line.
*/
void GoSafetyCommands::CmdSafe(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    string type = cmd.Arg(0);
    int totalRegions = 0;
    SgBWSet safe = GetSafe(totalRegions, type);
    SgPointSet set =
        (cmd.NuArg() == 2 ? safe[BlackWhiteArg(cmd, 1)] : safe.Both());
    cmd << set.Size();
    for (SgSetIterator it(set); it; ++it)
        cmd << ' ' << SgWritePoint(*it);
}

SgBWSet GoSafetyCommands::GetSafe(int& totalRegions, const string& type)
{
    GoModBoard modBoard(m_bd);
    GoBoard& bd = modBoard.Board();
    GoRegionBoard regionAttachment(bd);
    SgBWSet safe;
    if (type == "benson")
    {
        GoBensonSolver solver(bd, &regionAttachment);
        solver.FindSafePoints(&safe);
    }
    else if (type == "static")
    {
        GoSafetySolver solver(bd, &regionAttachment);
        solver.FindSafePoints(&safe);
    }
    else
        throw GtpFailure() << "invalid safety solver: " << type;
    SgPointSet proved = safe.Both();
    for (SgBWIterator it; it; ++it)
    {
        SgBlackWhite c = *it;
        for (SgVectorIteratorOf<GoRegion> it(regionAttachment.AllRegions(c));
             it; ++it)
            if ((*it)->Points().SubsetOf(proved))
                ++totalRegions;
    }
    return safe;
}

void GoSafetyCommands::Register(GtpEngine& e)
{
    Register(e, "go_safe", &GoSafetyCommands::CmdSafe);
    Register(e, "go_safe_gfx", &GoSafetyCommands::CmdGfx);
    Register(e, "go_safe_dame_static", &GoSafetyCommands::CmdDameStatic);
}

void GoSafetyCommands::Register(GtpEngine& engine,
                                const std::string& command,
                                GtpCallback<GoSafetyCommands>::Method method)
{
    engine.Register(command,
                    new GtpCallback<GoSafetyCommands>(this, method));
}

//----------------------------------------------------------------------------
