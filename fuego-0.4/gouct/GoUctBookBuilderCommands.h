//----------------------------------------------------------------------------
/** @file GoUctBookBuilderCommands.h
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_BOOKBUILDERCOMMANDS_H
#define GOUCT_BOOKBUILDERCOMMANDS_H

#include <string>
#include <typeinfo>
#include "GtpEngine.h"
#include "GoUctPlayoutPolicy.h"
#include "GoUctGlobalSearch.h"
#include "GoUctPlayer.h"
#include "GoUctBookBuilder.h"

class GoBoard;
class GoPlayer;
class GoUctBoard;
class GoUctSearch;

//----------------------------------------------------------------------------

template<class PLAYER>
class GoUctBookBuilderCommands
{
public:
    /** Constructor.
        @param bd The game board.
        @param player Reference to pointer to current player, this player can
        be null or a different player, but those commands of this class that
        need a GoUctPlayer will fail, if the current player is not a
        PLAYER.
    */
    GoUctBookBuilderCommands(GoBoard& bd, GoPlayer*& player,
                             boost::scoped_ptr<GoAutoBook>& book);

    void AddGoGuiAnalyzeCommands(GtpCommand& cmd);

    /** @page gouctbookbuildergtpcommands GoUctBookBuilderCommands Commands
        - @link CmdOpen() @c autobook_open @endlink
        - @link CmdClose() @c autobook_close @endlink
        - @link CmdSave() @c autobook_save @endlink
        - @link CmdExpand() @c autobook_expand @endlink
        - @link CmdCover() @c autobook_cover @endlink
        - @link CmdRefresh() @c autobook_refresh @endlink
        - @link CmdMerge() @c autobook_merge @endlink
        - @link CmdParam()  @c autobook_param @endlink
        - @link CmdScores() @c autobook_scores @endlink
        - @link CmdStateInfo() @c autobook_state_info @endlink
        - @link CmdCounts() @c autobook_counts @endlink
        - @link CmdPriority() @c autobook_priority @endlink
        - @link CmdLoadDisabled() @c autobook_load_disabled_lines @endlink
        - @link CmdTruncateByDepth() @c autobook_truncate_by_depth @endlink
    */
    /** @name Command Callbacks */
    // @{
    void CmdOpen(GtpCommand& cmd);
    void CmdClose(GtpCommand& cmd);
    void CmdSave(GtpCommand& cmd);
    void CmdExpand(GtpCommand& cmd);
    void CmdCover(GtpCommand& cmd);
    void CmdRefresh(GtpCommand& cmd);
    void CmdMerge(GtpCommand& cmd);
    void CmdParam(GtpCommand& cmd);
    void CmdScores(GtpCommand& cmd);
    void CmdStateInfo(GtpCommand& cmd);
    void CmdCounts(GtpCommand& cmd);
    void CmdPriority(GtpCommand& cmd);
    void CmdLoadDisabled(GtpCommand& cmd);
    void CmdTruncateByDepth(GtpCommand& cmd);
    // @} // @name

    void Register(GtpEngine& engine);

private:
    GoBoard& m_bd;

    GoPlayer*& m_player;

    boost::scoped_ptr<GoAutoBook>& m_book;

    GoUctBookBuilder<PLAYER> m_bookBuilder;

    GoAutoBookParam m_param;
    
    PLAYER& Player();

    void Register(GtpEngine& e, const std::string& command,
                typename GtpCallback<GoUctBookBuilderCommands>::Method method);

    void ShowInfluence(GtpCommand& cmd, GoAutoBookState& state);

    GoAutoBookMoveSelectType MoveSelectArg(const GtpCommand& cmd, 
                                           std::size_t number);

    std::string MoveSelectToString(GoAutoBookMoveSelectType moveSelect);
};

//----------------------------------------------------------------------------

template<class PLAYER>
GoUctBookBuilderCommands<PLAYER>
::GoUctBookBuilderCommands(GoBoard& bd, GoPlayer*& player, 
                           boost::scoped_ptr<GoAutoBook>& book)
: m_bd(bd),
    m_player(player),
    m_book(book),
    m_bookBuilder(bd),
    m_param()
{
}

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::AddGoGuiAnalyzeCommands(GtpCommand& cmd)
{
    cmd << 
        "none/AutoBook Close/autobook_close\n"
        "none/AutoBook Cover/autobook_cover\n"
        "none/AutoBook Expand/autobook_expand\n"
        "none/AutoBook Open/autobook_open %r\n"
        "none/AutoBook Save/autobook_save\n"
        "none/AutoBook Refresh/autobook_refresh\n"
        "none/AutoBook Merge/autobook_merge %r\n"
        "none/AutoBook Load Disabled Lines/autobook_load_disabled_lines %r\n"
        "none/AutoBook Truncate By Depth/autobook_truncate_by_depth\n"
        "param/AutoBook Param/autobook_param\n"
        "string/AutoBook State Info/autobook_state_info\n"
        "gfx/AutoBook Scores/autobook_scores\n"
        "gfx/AutoBook Counts/autobook_counts\n"
        "gfx/AutoBook Priority/autobook_priority\n";
      
}

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::Register(GtpEngine& e)
{
    Register(e, "autobook_close", &GoUctBookBuilderCommands<PLAYER>::CmdClose);
    Register(e, "autobook_counts", 
             &GoUctBookBuilderCommands<PLAYER>::CmdCounts);
    Register(e, "autobook_cover",
             &GoUctBookBuilderCommands<PLAYER>::CmdCover);             
    Register(e, "autobook_expand", 
             &GoUctBookBuilderCommands<PLAYER>::CmdExpand);
    Register(e, "autobook_open", &GoUctBookBuilderCommands<PLAYER>::CmdOpen);
    Register(e, "autobook_load_disabled_lines",
             &GoUctBookBuilderCommands<PLAYER>::CmdLoadDisabled);
    Register(e, "autobook_merge", 
             &GoUctBookBuilderCommands<PLAYER>::CmdMerge);
    Register(e, "autobook_param", &GoUctBookBuilderCommands<PLAYER>::CmdParam);
    Register(e, "autobook_priority", 
             &GoUctBookBuilderCommands<PLAYER>::CmdPriority);
    Register(e, "autobook_refresh", 
             &GoUctBookBuilderCommands<PLAYER>::CmdRefresh);
    Register(e, "autobook_save",
             &GoUctBookBuilderCommands<PLAYER>::CmdSave);
    Register(e, "autobook_scores", 
             &GoUctBookBuilderCommands<PLAYER>::CmdScores);
    Register(e, "autobook_state_info", 
             &GoUctBookBuilderCommands<PLAYER>::CmdStateInfo);
    Register(e, "autobook_truncate_by_depth", 
             &GoUctBookBuilderCommands<PLAYER>::CmdTruncateByDepth);
}

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::
Register(GtpEngine& engine, const std::string& command,
         typename GtpCallback<GoUctBookBuilderCommands>::Method method)
{
    engine.Register(command, 
                    new GtpCallback<GoUctBookBuilderCommands>(this, method));
}

template<class PLAYER>
PLAYER& GoUctBookBuilderCommands<PLAYER>::Player()
{
    if (m_player == 0)
        throw GtpFailure("player not GoUctPlayer");
    try
    {
        return dynamic_cast<PLAYER&>(*m_player);
    }
    catch (const std::bad_cast&)
    {
        throw GtpFailure("player not of right type!");
    }
}

template<class PLAYER>
GoAutoBookMoveSelectType GoUctBookBuilderCommands<PLAYER>
::MoveSelectArg(const GtpCommand& cmd, std::size_t number)
{
    std::string arg = cmd.ArgToLower(number);
    if (arg == "value")
        return GO_AUTOBOOK_SELECT_VALUE;
    if (arg == "count")
        return GO_AUTOBOOK_SELECT_COUNT;
    throw GtpFailure() << "unknown move select argument \"" << arg << '"';
}

template<class PLAYER>
std::string GoUctBookBuilderCommands<PLAYER>::
MoveSelectToString(GoAutoBookMoveSelectType moveSelect)
{
    switch (moveSelect)
    {
    case GO_AUTOBOOK_SELECT_VALUE:
        return "value";
    case GO_AUTOBOOK_SELECT_COUNT:
        return "count";
    default:
        SG_ASSERT(false);
        return "?";
    }
}

//----------------------------------------------------------------------------

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::ShowInfluence(GtpCommand& cmd,
                                                     GoAutoBookState& state)
{
    cmd << "INFLUENCE ";
    for (GoBoard::Iterator it(m_bd); it; ++it)
    {
        if (m_bd.IsLegal(*it))
        {
            state.Play(*it);
            SgBookNode node;
            if (m_book->Get(state, node))
            {
                float value 
                    = m_bookBuilder.InverseEval(m_bookBuilder.Value(node));
                float scaledValue = (value * 2 - 1);
                if (m_bd.ToPlay() != SG_BLACK)
                    scaledValue *= -1;
                cmd << ' ' << SgWritePoint(*it) << ' ' << scaledValue;
            }
            state.Undo();
        }
    }
}

//----------------------------------------------------------------------------

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::CmdOpen(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    m_book.reset(new GoAutoBook(cmd.Arg(0), m_param));
}

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::CmdClose(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    m_book.reset(0);
}

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::CmdSave(GtpCommand& cmd)
{
    if (m_book.get() == 0)
        throw GtpFailure() << "No opened autobook!\n";
    cmd.CheckArgNone();
    m_book->Flush();
}

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::CmdStateInfo(GtpCommand& cmd)
{
    if (m_book.get() == 0)
        throw GtpFailure() << "No opened autobook!\n";
    cmd.CheckArgNone();
    GoAutoBookState state(m_bd);
    state.Synchronize();
    SgBookNode node;
    if (!m_book->Get(state, node))
        throw GtpFailure() << "State not in autobook.\n";
    cmd << node;
}

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::CmdExpand(GtpCommand& cmd)
{
    if (m_book.get() == 0)
        throw GtpFailure() << "No opened autobook!\n";
    cmd.CheckNuArg(1);
    int numExpansions = cmd.IntArg(0, 1);
    m_bookBuilder.SetPlayer(Player());
    m_bookBuilder.SetState(*m_book);
    m_bookBuilder.Expand(numExpansions);
}


template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::CmdCover(GtpCommand& cmd)
{
    if (m_book.get() == 0)
        throw GtpFailure() << "No opened autobook!\n";
    cmd.CheckNuArg(2);
    std::vector< std::vector<SgMove> > workList;
    {
        std::string filename = cmd.Arg(0);
        std::ifstream in(filename.c_str());
        if (!in)
            throw GtpFailure() << "Could not open '" << filename << "'\n";
        workList = GoAutoBook::ParseWorkList(in);
        if (workList.empty())
            throw GtpFailure() << "Empty worklist! No action performed";
    }
    int expansionsRequired = cmd.IntArg(1, 1);
    m_bookBuilder.SetPlayer(Player());
    m_bookBuilder.SetState(*m_book);
    m_bookBuilder.Cover(expansionsRequired, workList);
}

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::CmdRefresh(GtpCommand& cmd)
{
    if (m_book.get() == 0)
        throw GtpFailure() << "No opened autobook!\n";
    cmd.CheckArgNone();
    m_bookBuilder.SetPlayer(Player());
    m_bookBuilder.SetState(*m_book);
    m_bookBuilder.Refresh();
}

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::CmdMerge(GtpCommand& cmd)
{
    if (m_book.get() == 0)
        throw GtpFailure() << "No opened autobook!\n";
    cmd.CheckNuArg(1);
    GoAutoBook other(cmd.Arg(0), m_param);
    m_book->Merge(other);
}

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::CmdTruncateByDepth(GtpCommand& cmd)
{
    if (m_book.get() == 0)
        throw GtpFailure() << "No opened autobook!\n";
    cmd.CheckNuArg(2);
    int depth = cmd.IntArg(0, 0);
    GoAutoBook other(cmd.Arg(1), m_param);
    GoAutoBookState state(m_bd);
    state.Synchronize();
    m_book->TruncateByDepth(depth, state, other);
    other.Flush();
}

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::CmdParam(GtpCommand& cmd)
{
    if (cmd.NuArg() == 0)
    {
        cmd << "[bool] use_widening " << m_bookBuilder.UseWidening() << '\n'
            << "[string] alpha " << m_bookBuilder.Alpha() << '\n'
            << "[string] expand_width " << m_bookBuilder.ExpandWidth() << '\n'
            << "[string] expand_threshold " 
            << m_bookBuilder.ExpandThreshold() << '\n'
            << "[string] num_threads " << m_bookBuilder.NumThreads() << '\n'
            << "[string] num_games_per_evaluation " 
            << m_bookBuilder.NumGamesPerEvaluation() << '\n'
            << "[string] num_games_per_sort "
            << m_bookBuilder.NumGamesPerSort() << '\n'
            << "[string] usage_count " 
            << m_param.m_usageCountThreshold << '\n'
            << "[list/value/count] move_select "
            << MoveSelectToString(m_param.m_selectType) << '\n';
    }
    else if (cmd.NuArg() == 2)
    {
        std::string name = cmd.Arg(0);
        if (name == "num_threads")
            m_bookBuilder.SetNumThreads(cmd.IntArg(1, 1));
        else if (name == "num_games_per_evaluation")
            m_bookBuilder.SetNumGamesPerEvaluation(cmd.SizeTypeArg(1, 1));
        else if (name == "num_games_per_sort")
            m_bookBuilder.SetNumGamesPerSort(cmd.SizeTypeArg(1, 1));
        else if (name == "use_widening")
            m_bookBuilder.SetUseWidening(cmd.BoolArg(1));
        else if (name == "expand_width")
            m_bookBuilder.SetExpandWidth(cmd.IntArg(1, 1));
        else if (name == "expand_threshold")
            m_bookBuilder.SetExpandThreshold(cmd.IntArg(1, 1));
        else if (name == "usage_count")
            m_param.m_usageCountThreshold = cmd.SizeTypeArg(1, 0);
        else if (name == "move_select")
            m_param.m_selectType = MoveSelectArg(cmd, 1);
        else if (name == "alpha")
        {
            float alpha = cmd.FloatArg(1);
            if (alpha < 0)
                throw GtpFailure("Alpha must be greater than 0!");
            m_bookBuilder.SetAlpha(alpha);
        }
    }
    else
        throw GtpFailure() << "Expected 0 or 2 arguments!\n";
}


template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::CmdLoadDisabled(GtpCommand& cmd)
{
    if (m_book.get() == 0)
        throw GtpFailure() << "No opened autobook!\n";
    cmd.CheckNuArg(1);
    std::vector< std::vector<SgMove> > workList;
    {
        std::string filename = cmd.Arg(0);
        std::ifstream in(filename.c_str());
        if (!in)
            throw GtpFailure() << "Could not open '" << filename << "'\n";
        workList = GoAutoBook::ParseWorkList(in);
    }
    GoBoard brd(m_bd.Size());
    GoAutoBookState state(brd);
    state.Synchronize();
    std::set<SgHashCode> disabled;
    for (std::size_t i = 0; i < workList.size(); ++i)
    {
        for (std::size_t j = 0; j < workList[i].size(); ++j)
            state.Play(workList[i][j]);
        disabled.insert(state.GetHashCode());
        for (std::size_t j = 0; j < workList[i].size(); ++j)
            state.Undo();
    }
    m_book->AddDisabledLines(disabled);
}

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::CmdScores(GtpCommand& cmd)
{
    if (m_book.get() == 0)
        throw GtpFailure() << "No opened autobook!\n";
    cmd.CheckArgNone();
    GoAutoBookState state(m_bd);
    state.Synchronize();
    ShowInfluence(cmd, state);
    cmd << "\nLABEL ";
    for (GoBoard::Iterator it(m_bd); it; ++it)
    {
        if (m_bd.IsLegal(*it))
        {
            state.Play(*it);
            SgBookNode node;
            if (m_book->Get(state, node))
            {
                float value 
                    = m_bookBuilder.InverseEval(m_bookBuilder.Value(node));
                cmd << ' ' << SgWritePoint(*it) 
                    << ' ' << std::fixed << std::setprecision(3) << value;
            }
            state.Undo();
        }
    }
    cmd << '\n';
}

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::CmdCounts(GtpCommand& cmd)
{
    if (m_book.get() == 0)
        throw GtpFailure() << "No opened autobook!\n";
    cmd.CheckArgNone();
    GoAutoBookState state(m_bd);
    state.Synchronize();
    ShowInfluence(cmd, state);
    cmd << "\nLABEL ";
    for (GoBoard::Iterator it(m_bd); it; ++it)
    {
        if (m_bd.IsLegal(*it))
        {
            state.Play(*it);
            SgBookNode node;
            if (m_book->Get(state, node))
                cmd << ' ' << SgWritePoint(*it) << ' ' << node.m_count;
            state.Undo();
        }
    }
    cmd << '\n';
}

template<class PLAYER>
void GoUctBookBuilderCommands<PLAYER>::CmdPriority(GtpCommand& cmd)
{
    if (m_book.get() == 0)
        throw GtpFailure() << "No opened autobook!\n";
    cmd.CheckArgNone();
    GoAutoBookState state(m_bd);
    state.Synchronize();
    SgBookNode parent;
    if (!m_book->Get(state, parent))
        throw GtpFailure("Current state not in book!");
    ShowInfluence(cmd, state);
    cmd << "\nLABEL ";
    for (GoBoard::Iterator it(m_bd); it; ++it)
    {
        if (m_bd.IsLegal(*it))
        {
            state.Play(*it);
            SgBookNode child;
            if (m_book->Get(state, child))
            {
                float priority = m_bookBuilder.ComputePriority
                    (parent, child.m_value, child.m_priority);
                cmd << ' ' << SgWritePoint(*it) << ' ' 
                    << std::fixed << std::setprecision(1) << priority;
            }
            state.Undo();
        }
    }
    cmd << '\n';
}

//----------------------------------------------------------------------------

#endif // GOUCT_BOOKBUILDERCOMMANDS_H
