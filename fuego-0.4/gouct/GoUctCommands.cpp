//----------------------------------------------------------------------------
/** @file GoUctCommands.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctCommands.h"

#include <fstream>
#include <boost/format.hpp>
#include "GoEyeUtil.h"
#include "GoGtpCommandUtil.h"
#include "GoBoardUtil.h"
#include "GoSafetySolver.h"
#include "GoUctDefaultPriorKnowledge.h"
#include "GoUctDefaultRootFilter.h"
#include "GoUctEstimatorStat.h"
#include "GoUctGlobalSearch.h"
#include "GoUctPatterns.h"
#include "GoUctPlayer.h"
#include "GoUctPlayoutPolicy.h"
#include "GoUctUtil.h"
#include "GoUtil.h"
#include "SgException.h"
#include "SgPointSetUtil.h"
#include "SgRestorer.h"
#include "SgUctTreeUtil.h"
#include "SgWrite.h"

using namespace std;
using boost::format;
using GoGtpCommandUtil::BlackWhiteArg;
using GoGtpCommandUtil::EmptyPointArg;
using GoGtpCommandUtil::PointArg;

typedef GoUctPlayer<GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
                                      GoUctPlayoutPolicyFactory<GoUctBoard> >,
                    GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> > >
                    GoUctPlayerType;


//----------------------------------------------------------------------------

namespace {

GoUctLiveGfx LiveGfxArg(const GtpCommand& cmd, size_t number)
{
    string arg = cmd.ArgToLower(number);
    if (arg == "none")
        return GOUCT_LIVEGFX_NONE;
    if (arg == "counts")
        return GOUCT_LIVEGFX_COUNTS;
    if (arg == "sequence")
        return GOUCT_LIVEGFX_SEQUENCE;
    throw GtpFailure() << "unknown live-gfx argument \"" << arg << '"';
}

string LiveGfxToString(GoUctLiveGfx mode)
{
    switch (mode)
    {
    case GOUCT_LIVEGFX_NONE:
        return "none";
    case GOUCT_LIVEGFX_COUNTS:
        return "counts";
    case GOUCT_LIVEGFX_SEQUENCE:
        return "sequence";
    default:
        SG_ASSERT(false);
        return "?";
    }
}

SgUctMoveSelect MoveSelectArg(const GtpCommand& cmd, size_t number)
{
    string arg = cmd.ArgToLower(number);
    if (arg == "value")
        return SG_UCTMOVESELECT_VALUE;
    if (arg == "count")
        return SG_UCTMOVESELECT_COUNT;
    if (arg == "bound")
        return SG_UCTMOVESELECT_BOUND;
    if (arg == "estimate")
        return SG_UCTMOVESELECT_ESTIMATE;
    throw GtpFailure() << "unknown move select argument \"" << arg << '"';
}

string MoveSelectToString(SgUctMoveSelect moveSelect)
{
    switch (moveSelect)
    {
    case SG_UCTMOVESELECT_VALUE:
        return "value";
    case SG_UCTMOVESELECT_COUNT:
        return "count";
    case SG_UCTMOVESELECT_BOUND:
        return "bound";
    case SG_UCTMOVESELECT_ESTIMATE:
        return "estimate";
    default:
        SG_ASSERT(false);
        return "?";
    }
}

GoUctGlobalSearchMode SearchModeArg(const GtpCommand& cmd, size_t number)
{
    string arg = cmd.ArgToLower(number);
    if (arg == "playout_policy")
        return GOUCT_SEARCHMODE_PLAYOUTPOLICY;
    if (arg == "uct")
        return GOUCT_SEARCHMODE_UCT;
    if (arg == "one_ply")
        return GOUCT_SEARCHMODE_ONEPLY;
    throw GtpFailure() << "unknown search mode argument \"" << arg << '"';
}

string SearchModeToString(GoUctGlobalSearchMode mode)
{
    switch (mode)
    {
    case GOUCT_SEARCHMODE_PLAYOUTPOLICY:
        return "playout_policy";
    case GOUCT_SEARCHMODE_UCT:
        return "uct";
    case GOUCT_SEARCHMODE_ONEPLY:
        return "one_ply";
    default:
        SG_ASSERT(false);
        return "?";
    }
}

string KnowledgeThresholdToString(const std::vector<std::size_t>& t)
{
    if (t.empty())
        return "0";
    std::ostringstream os;
    os << '\"';
    for (std::size_t i = 0; i < t.size(); ++i)
    {
        if (i > 0) 
            os << ' ';
        os << t[i];
    }
    os << '\"';
    return os.str();
}

std::vector<std::size_t> KnowledgeThresholdFromString(const std::string& val)
{
    std::vector<std::size_t> v;
    std::istringstream is(val);
    std::size_t t;
    while (is >> t)
        v.push_back(t);
    if (v.size() == 1 && v[0] == 0)
        v.clear();
    return v;
}

} // namespace

//----------------------------------------------------------------------------

GoUctCommands::GoUctCommands(GoBoard& bd, GoPlayer*& player)
    : m_bd(bd),
      m_player(player)
{
}

void GoUctCommands::AddGoGuiAnalyzeCommands(GtpCommand& cmd)
{
    cmd <<
        "gfx/Uct Bounds/uct_bounds\n"
        "plist/Uct Default Policy/uct_default_policy\n"
        "gfx/Uct Gfx/uct_gfx\n"
        "none/Uct Max Memory/uct_max_memory %s\n"
        "plist/Uct Moves/uct_moves\n"
        "param/Uct Param GlobalSearch/uct_param_globalsearch\n"
        "param/Uct Param Policy/uct_param_policy\n"
        "param/Uct Param Player/uct_param_player\n"
        "param/Uct Param RootFilter/uct_param_rootfilter\n"
        "param/Uct Param Search/uct_param_search\n"
        "plist/Uct Patterns/uct_patterns\n"
        "pstring/Uct Policy Moves/uct_policy_moves\n"
        "gfx/Uct Prior Knowledge/uct_prior_knowledge\n"
        "sboard/Uct Rave Values/uct_rave_values\n"
        "plist/Uct Root Filter/uct_root_filter\n"
        "none/Uct SaveGames/uct_savegames %w\n"
        "none/Uct SaveTree/uct_savetree %w\n"
        "gfx/Uct Sequence/uct_sequence\n"
        "hstring/Uct Stat Player/uct_stat_player\n"
        "none/Uct Stat Player Clear/uct_stat_player_clear\n"
        "hstring/Uct Stat Policy/uct_stat_policy\n"
        "none/Uct Stat Policy Clear/uct_stat_policy_clear\n"
        "hstring/Uct Stat Search/uct_stat_search\n"
        "dboard/Uct Stat Territory/uct_stat_territory\n";
}

/** Show UCT bounds of moves in root node.
    This command is compatible with the GoGui analyze command type "gfx".
    Move bounds are shown as labels on the board, the pass move bound is
    shown as text in the status line.
    @see SgUctSearch::GetBound
*/
void GoUctCommands::CmdBounds(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    const GoUctSearch& search = Search();
    const SgUctTree& tree = search.Tree();
    const SgUctNode& root = tree.Root();
    bool hasPass = false;
    float passBound = 0;
    cmd << "LABEL";
    for (SgUctChildIterator it(tree, root); it; ++it)
    {
        const SgUctNode& child = *it;
        SgPoint move = child.Move();
        float bound = search.GetBound(search.Rave(), root, child);
        if (move == SG_PASS)
        {
            hasPass = true;
            passBound = bound;
        }
        else
            cmd << ' ' << SgWritePoint(move) << ' ' << fixed
                << setprecision(2) << bound;
    }
    cmd << '\n';
    if (hasPass)
        cmd << "TEXT PASS=" << fixed << setprecision(2) << passBound << '\n';
}


void GoUctCommands::CmdDefaultPolicy(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    GoUctDefaultPriorKnowledge knowledge(m_bd, GoUctPlayoutPolicyParam());
    SgPointSet pattern;
    SgPointSet atari;
    GoPointList empty;
    knowledge.FindGlobalPatternAndAtariMoves(pattern, atari, empty);
    cmd << SgWritePointSet(atari, "", false) << '\n';
}

/** Compute estimator statistics.
    Arguments: trueValueMaxGames maxGames stepSize fileName
    @see GoUctEstimatorStat::Compute()
*/
void GoUctCommands::CmdEstimatorStat(GtpCommand& cmd)
{
    cmd.CheckNuArg(4);
    size_t trueValueMaxGames = cmd.SizeTypeArg(0);
    size_t maxGames = cmd.SizeTypeArg(1);
    size_t stepSize = cmd.SizeTypeArg(2);
    string fileName = cmd.Arg(3);
    GoUctEstimatorStat::Compute(Search(), trueValueMaxGames, maxGames,
                                stepSize, fileName);
}

/** Return final score.
    Does a small search and uses the territory statistics to determine the
    status of blocks.
*/
void GoUctCommands::CmdFinalScore(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    SgPointSet deadStones = DoFinalStatusSearch();
    float score;
    if (! GoBoardUtil::ScorePosition(m_bd, deadStones, score))
        throw GtpFailure("cannot score");
    cmd << GoUtil::ScoreToString(score);
}

/** Return final status of stones.
    Does a small search and uses the territory statistics to determine the
    status of blocks. The engine cannot detect seki, so according to the
    GTP standard, all stones that are not dead are reported as alive, the
    response to seki is an empty list. <br>
    Arguments: alive|seki|dead
*/
void GoUctCommands::CmdFinalStatusList(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    string arg = cmd.Arg(0);
    if (arg == "seki")
        return;
    bool getDead;
    if (arg == "alive")
        getDead = false;
    else if (arg == "dead")
        getDead = true;
    else
        throw GtpFailure("invalid final status argument");
    SgPointSet deadPoints = DoFinalStatusSearch();
    // According to GTP standard the response should be one string(=block)
    // per line
    for (GoBlockIterator it(m_bd); it; ++it)
    {
        if ((getDead && deadPoints.Contains(*it))
            || (! getDead && ! deadPoints.Contains(*it)))
        {
            for (GoBoard::StoneIterator it2(m_bd, *it); it2; ++it2)
                cmd << SgWritePoint(*it2) << ' ';
                cmd << '\n';
        }
    }
}

/** Show move values and sample numbers of last search.
    Arguments: none
    @see GoUctSearch::GoGuiGfx()
*/
void GoUctCommands::CmdGfx(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    const GoUctSearch& s = Search();
    SgBlackWhite toPlay = s.ToPlay();
    GoUctUtil::GfxBestMove(s, toPlay, cmd);
    GoUctUtil::GfxMoveValues(s, toPlay, cmd);
    GoUctUtil::GfxCounts(s.Tree(), cmd);
    GoUctUtil::GfxStatus(s, cmd);
}

/** Computes the maximum number of nodes in search tree given the
    maximum allowed memory for the tree. Assumes two trees. Returns
    current memory usage if no arguments.
    Arguments: max memory for trees
 */
void GoUctCommands::CmdMaxMemory(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(1);
    if (cmd.NuArg() == 0)
        cmd << Search().MaxNodes() * 2 * sizeof(SgUctNode);
    else
    {
        std::size_t memory = cmd.SizeTypeArg(0, 2*sizeof(SgUctNode));
        Search().SetMaxNodes(memory / 2 / sizeof(SgUctNode));
    }
}

/** Return a list of all moves that the search would generate in the current
    position.
    Arguments: none
    @see SgUctSearch::GenerateAllMoves()
*/
void GoUctCommands::CmdMoves(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    vector<SgMoveInfo> moves;
    Search().GenerateAllMoves(moves);
    for (std::size_t i = 0; i < moves.size(); ++i)
        cmd << SgWritePoint(moves[i].m_move) << ' ';
    cmd << '\n';
}

/** Get and set GoUctGlobalSearch parameters.
    This command is compatible with the GoGui analyze command type "param".

    Parameters:
    @arg @c live_gfx See GoUctGlobalSearch::GlobalSearchLiveGfx
    @arg @c mercy_rule See GoUctGlobalSearchStateParam::m_mercyRule
    @arg @c territory_statistics See
        GoUctGlobalSearchStateParam::m_territoryStatistics
    @arg @c length_modification See
        GoUctGlobalSearchStateParam::m_langthModification
    @arg @c score_modification See
        GoUctGlobalSearchStateParam::m_scoreModification
*/
void GoUctCommands::CmdParamGlobalSearch(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
                      GoUctPlayoutPolicyFactory<GoUctBoard> >&
           s = GlobalSearch();
    GoUctGlobalSearchStateParam& p = s.m_param;
    if (cmd.NuArg() == 0)
    {
        // Boolean parameters first for better layout of GoGui parameter
        // dialog, alphabetically otherwise
        cmd << "[bool] live_gfx " << s.GlobalSearchLiveGfx() << '\n'
            << "[bool] mercy_rule " << p.m_mercyRule << '\n'
            << "[bool] territory_statistics " << p.m_territoryStatistics
            << '\n'
            << "[string] length_modification " << p.m_lengthModification
            << '\n'
            << "[string] score_modification " << p.m_scoreModification
            << '\n';
    }
    else if (cmd.NuArg() == 2)
    {
        string name = cmd.Arg(0);
        if (name == "live_gfx")
            s.SetGlobalSearchLiveGfx(cmd.BoolArg(1));
        else if (name == "mercy_rule")
            p.m_mercyRule = cmd.BoolArg(1);
        else if (name == "territory_statistics")
            p.m_territoryStatistics = cmd.BoolArg(1);
        else if (name == "length_modification")
            p.m_lengthModification = cmd.FloatArg(1);
        else if (name == "score_modification")
            p.m_scoreModification = cmd.FloatArg(1);
        else
            throw GtpFailure() << "unknown parameter: " << name;
    }
    else
        throw GtpFailure() << "need 0 or 2 arguments";
}

/** Get and set GoUctPlayer parameters.
    This command is compatible with the GoGui analyze command type "param".

    Parameters:
    @arg @c auto_param See GoUctPlayer::AutoParam
    @arg @c early_pass See GoUctPlayer::EarlyPass
    @arg @c forced_opening_moves See GoUctPlayer::ForcedOpeningMoves
    @arg @c ignore_clock See GoUctPlayer::IgnoreClock
    @arg @c reuse_subtree See GoUctPlayer::ReuseSubtree
    @arg @c use_root_filter See GoUctPlayer::UseRootFilter
    @arg @c max_games See GoUctPlayer::MaxGames
    @arg @c resign_min_games See GoUctPlayer::ResignMinGames
    @arg @c resign_threshold See GoUctPlayer::ResignThreshold
    @arg @c search_mode @c playout|uct|one_ply See GoUctPlayer::SearchMode
*/
void GoUctCommands::CmdParamPlayer(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    GoUctPlayerType& p = Player();
    if (cmd.NuArg() == 0)
    {
        // Boolean parameters first for better layout of GoGui parameter
        // dialog, alphabetically otherwise
        cmd << "[bool] auto_param " << p.AutoParam() << '\n'
            << "[bool] early_pass " << p.EarlyPass() << '\n'
            << "[bool] forced_opening_moves " << p.ForcedOpeningMoves() << '\n'
            << "[bool] ignore_clock " << p.IgnoreClock() << '\n'
            << "[bool] ponder " << p.EnablePonder() << '\n'
            << "[bool] reuse_subtree " << p.ReuseSubtree() << '\n'
            << "[bool] use_root_filter " << p.UseRootFilter() << '\n'
            << "[string] max_games " << p.MaxGames() << '\n'
            << "[string] resign_min_games " << p.ResignMinGames() << '\n'
            << "[string] resign_threshold " << p.ResignThreshold() << '\n'
            << "[list/playout_policy/uct/one_ply] search_mode "
            << SearchModeToString(p.SearchMode()) << '\n';
    }
    else if (cmd.NuArg() >= 1 && cmd.NuArg() <= 2)
    {
        string name = cmd.Arg(0);
        if (name == "auto_param")
            p.SetAutoParam(cmd.BoolArg(1));
        else if (name == "early_pass")
            p.SetEarlyPass(cmd.BoolArg(1));
        else if (name == "forced_opening_moves")
            p.SetForcedOpeningMoves(cmd.BoolArg(1));
        else if (name == "ignore_clock")
            p.SetIgnoreClock(cmd.BoolArg(1));
        else if (name == "ponder")
            p.SetEnablePonder(cmd.BoolArg(1));
        else if (name == "reuse_subtree")
            p.SetReuseSubtree(cmd.BoolArg(1));
        else if (name == "use_root_filter")
            p.SetUseRootFilter(cmd.BoolArg(1));
        else if (name == "max_games")
            p.SetMaxGames(cmd.SizeTypeArg(1, 1));
        else if (name == "resign_min_games")
            p.SetResignMinGames(cmd.SizeTypeArg(1));
        else if (name == "resign_threshold")
            p.SetResignThreshold(cmd.FloatArg(1));
        else if (name == "search_mode")
            p.SetSearchMode(SearchModeArg(cmd, 1));
        else
            throw GtpFailure() << "unknown parameter: " << name;
    }
    else
        throw GtpFailure() << "need 0 or 2 arguments";
}

/** Get and set GoUctPlayoutPolicy parameters.
    This command is compatible with the GoGui analyze command type "param".

    Parameters:
    @arg @c statistics_enabled
        See GoUctPlayoutPolicyParam::m_statisticsEnabled
    @arg @c nakade_heuristic
        See GoUctPlayoutPolicyParam::m_useNakadeHeuristic
    @arg @c fillboard_tries
        See GoUctPlayoutPolicyParam::m_fillboardTries
*/
void GoUctCommands::CmdParamPolicy(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    GoUctPlayoutPolicyParam& p = Player().m_playoutPolicyParam;
    if (cmd.NuArg() == 0)
    {
        // Boolean parameters first for better layout of GoGui parameter
        // dialog, alphabetically otherwise
        cmd << "[bool] nakade_heuristic " << p.m_useNakadeHeuristic << '\n'
            << "[bool] statistics_enabled " << p.m_statisticsEnabled << '\n'
            << "fillboard_tries " << p.m_fillboardTries << '\n';
    }
    else if (cmd.NuArg() == 2)
    {
        string name = cmd.Arg(0);
        if (name == "nakade_heuristic")
            p.m_useNakadeHeuristic = cmd.BoolArg(1);
        else if (name == "statistics_enabled")
            p.m_statisticsEnabled = cmd.BoolArg(1);
        else if (name == "fillboard_tries")
            p.m_fillboardTries = cmd.IntArg(1);
        else
            throw GtpFailure() << "unknown parameter: " << name;
    }
    else
        throw GtpFailure() << "need 0 or 2 arguments";
}

/** Get and set GoUctDefaultRootFilter parameters.
    This command is compatible with the GoGui analyze command type "param".

    Parameters:
    @arg @c check_ladders See GoUctDefaultRootFilter::CheckLadders()
*/
void GoUctCommands::CmdParamRootFilter(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    GoUctDefaultRootFilter* f =
        dynamic_cast<GoUctDefaultRootFilter*>(&Player().RootFilter());
    if (f == 0)
        throw GtpFailure("root filter is not GoUctDefaultRootFilter");
    if (cmd.NuArg() == 0)
    {
        // Boolean parameters first for better layout of GoGui parameter
        // dialog, alphabetically otherwise
        cmd << "[bool] check_ladders " << f->CheckLadders() << '\n';
    }
    else if (cmd.NuArg() == 2)
    {
        string name = cmd.Arg(0);
        if (name == "check_ladders")
            f->SetCheckLadders(cmd.BoolArg(1));
        else
            throw GtpFailure() << "unknown parameter: " << name;
    }
    else
        throw GtpFailure() << "need 0 or 2 arguments";
}

/** Get and set SgUctSearch and GoUctSearch parameters.
    This command is compatible with the GoGui analyze command type "param".

    Parameters:
    @arg @c keep_games See GoUctSearch::KeepGames
    @arg @c lock_free See SgUctSearch::LockFree
    @arg @c log_games See SgUctSearch::LogGames
    @arg @c prune_full_tree See SgUctSearch::PruneFullTree
    @arg @c rave See SgUctSearch::Rave
    @arg @c weight_rave_updates SgUctSearch::WeightRaveUpdates
    @arg @c bias_term_constant See SgUctSearch::BiasTermConstant
    @arg @c expand_threshold See SgUctSearch::ExpandThreshold
    @arg @c first_play_urgency See SgUctSearch::FirstPlayUrgency
    @arg @c knowledge_threshold See SgUctSearch::KnowledgeThreshold
    @arg @c live_gfx @c none|counts|sequence See GoUctSearch::LiveGfx
    @arg @c live_gfx_interval See GoUctSearch::LiveGfxInterval
    @arg @c max_nodes See SgUctSearch::MaxNodes
    @arg @c move_select @c value|count|bound|rave See SgUctSearch::MoveSelect
    @arg @c number_threads See SgUctSearch::NumberThreads
    @arg @c number_playouts See SgUctSearch::NumberPlayouts
    @arg @c prune_min_count See SgUctSearch::PruneMinCount
    @arg @c rave_weight_final See SgUctSearch::RaveWeightFinal
    @arg @c rave_weight_initial See SgUctSearch::RaveWeightInitial
*/
void GoUctCommands::CmdParamSearch(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    GoUctSearch& s = Search();
    if (cmd.NuArg() == 0)
    {
        // Boolean parameters first for better layout of GoGui parameter
        // dialog, alphabetically otherwise
        cmd << "[bool] keep_games " << s.KeepGames() << '\n'
            << "[bool] lock_free " << s.LockFree() << '\n'
            << "[bool] log_games " << s.LogGames() << '\n'
            << "[bool] prune_full_tree " << s.PruneFullTree() << '\n'
            << "[bool] rave " << s.Rave() << '\n'
            << "[bool] virtual_loss " << s.VirtualLoss() << '\n'
            << "[bool] weight_rave_updates " << s.WeightRaveUpdates() << '\n'
            << "[string] bias_term_constant " << s.BiasTermConstant() << '\n'
            << "[string] expand_threshold " << s.ExpandThreshold() << '\n'
            << "[string] first_play_urgency " << s.FirstPlayUrgency() << '\n'
            << "[string] knowledge_threshold "
            << KnowledgeThresholdToString(s.KnowledgeThreshold()) << '\n'
            << "[list/none/counts/sequence] live_gfx "
            << LiveGfxToString(s.LiveGfx()) << '\n'
            << "[string] live_gfx_interval " << s.LiveGfxInterval() << '\n'
            << "[string] max_nodes " << s.MaxNodes() << '\n'
            << "[list/value/count/bound/estimate] move_select "
            << MoveSelectToString(s.MoveSelect()) << '\n'
            << "[string] number_threads " << s.NumberThreads() << '\n'
            << "[string] number_playouts " << s.NumberPlayouts() << '\n'
            << "[string] prune_min_count " << s.PruneMinCount() << '\n'
            << "[string] randomize_rave_frequency " 
            << s.RandomizeRaveFrequency() << '\n'
            << "[string] rave_weight_final " << s.RaveWeightFinal() << '\n'
            << "[string] rave_weight_initial "
            << s.RaveWeightInitial() << '\n';

    }
    else if (cmd.NuArg() == 2)
    {
        string name = cmd.Arg(0);
        if (name == "keep_games")
            s.SetKeepGames(cmd.BoolArg(1));
        else if (name == "knowledge_threshold")
            s.SetKnowledgeThreshold(KnowledgeThresholdFromString(cmd.Arg(1)));
        else if (name == "lock_free")
            s.SetLockFree(cmd.BoolArg(1));
        else if (name == "log_games")
            s.SetLogGames(cmd.BoolArg(1));
        else if (name == "prune_full_tree")
            s.SetPruneFullTree(cmd.BoolArg(1));
        else if (name == "randomize_rave_frequency")
            s.SetRandomizeRaveFrequency(cmd.IntArg(1, 0));
        else if (name == "rave")
            s.SetRave(cmd.BoolArg(1));
        else if (name == "weight_rave_updates")
            s.SetWeightRaveUpdates(cmd.BoolArg(1));
        else if (name == "virtual_loss")
            s.SetVirtualLoss(cmd.BoolArg(1));
        else if (name == "bias_term_constant")
            s.SetBiasTermConstant(cmd.FloatArg(1));
        else if (name == "expand_threshold")
            s.SetExpandThreshold(cmd.SizeTypeArg(1, 1));
        else if (name == "first_play_urgency")
            s.SetFirstPlayUrgency(cmd.FloatArg(1));
        else if (name == "live_gfx")
            s.SetLiveGfx(LiveGfxArg(cmd, 1));
        else if (name == "live_gfx_interval")
            s.SetLiveGfxInterval(cmd.IntArg(1, 1));
        else if (name == "max_nodes")
            s.SetMaxNodes(cmd.SizeTypeArg(1, 1));
        else if (name == "move_select")
            s.SetMoveSelect(MoveSelectArg(cmd, 1));
        else if (name == "number_threads")
            s.SetNumberThreads(cmd.SizeTypeArg(1, 1));
        else if (name == "number_playouts")
            s.SetNumberPlayouts(cmd.IntArg(1, 1));
        else if (name == "prune_min_count")
            s.SetPruneMinCount(cmd.SizeTypeArg(1, 1));
        else if (name == "rave_weight_final")
            s.SetRaveWeightFinal(cmd.FloatArg(1));
        else if (name == "rave_weight_initial")
            s.SetRaveWeightInitial(cmd.FloatArg(1));
        else
            throw GtpFailure() << "unknown parameter: " << name;
    }
    else
        throw GtpFailure() << "need 0 or 2 arguments";
}

/** Show matching patterns.
    Returns: List of points for matching patterns
    @see GoUctPatterns
*/
void GoUctCommands::CmdPatterns(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    GoUctPatterns<GoBoard> patterns(m_bd);
    for (GoBoard::Iterator it(m_bd); it; ++it)
        if (m_bd.IsEmpty(*it) && patterns.MatchAny(*it))
            cmd << SgWritePoint(*it) << ' ';
}

/** Return equivalent best moves in playout policy.
    See GoUctPlayoutPolicy::GetEquivalentBestMoves() <br>
    Arguments: none <br>
    Returns: Move type string followed by move list on a single line.
*/
void GoUctCommands::CmdPolicyMoves(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    GoUctPlayoutPolicy<GoBoard> policy(m_bd, Player().m_playoutPolicyParam);
    policy.StartPlayout();
    policy.GenerateMove();
    cmd << GoUctPlayoutPolicyTypeStr(policy.MoveType());
    GoPointList moves = policy.GetEquivalentBestMoves();
    // Sort for deterministic response
    // (GoUctPlayoutPolicy::GetEquivalentBestMoves() does not return
    // a deterministic list, because GoUctUtil::SelectRandom() may modify
    // the list in a non-deterministic way)
    moves.Sort();
    for (int i = 0; i < moves.Length(); ++i)
        cmd << ' ' << SgWritePoint(moves[i]);
}

/** Show prior knowledge.
    The response is compatible to the GoGui analyze command type @c
    gfx and shows the prior knowledge values as influence and the
    counts as labels.
*/
void GoUctCommands::CmdPriorKnowledge(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(1);
    size_t count = 0;
    if (cmd.NuArg() == 1)
        count = cmd.SizeTypeArg(0, 0);
    GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> >& state
        = ThreadState(0);
    state.StartSearch(); // Updates thread state board
    vector<SgMoveInfo> moves;
    SgProvenNodeType provenType;
    state.GenerateAllMoves(count, moves, provenType);

    cmd << "INFLUENCE ";
    for (size_t i = 0; i < moves.size(); ++i) 
    {
        SgMove move = moves[i].m_move;
        float value = SgUctSearch::InverseEval(moves[i].m_value);
        //float value = moves[i].m_value;
        size_t count = moves[i].m_count;
        if (count > 0)
        {
            float scaledValue = (value * 2 - 1);
            if (m_bd.ToPlay() != SG_BLACK)
                scaledValue *= -1;
            cmd << ' ' << SgWritePoint(move) << ' ' << scaledValue;
        }
    }
    cmd << "\nLABEL ";
    for (size_t i = 0; i < moves.size(); ++i)
    {
        SgMove move = moves[i].m_move;
        size_t count = moves[i].m_count;
        if (count > 0)
            cmd << ' ' << SgWritePoint(move) << ' ' << count;
    }
    cmd << '\n';
}

/** Show RAVE values of last search at root position.
    This command is compatible to the GoGui analyze command type @c sboard.
    The values are scaled to [-1,+1] from Black's point of view.
    @see SgUctSearch::Rave
*/
void GoUctCommands::CmdRaveValues(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    const GoUctSearch& search = Search();
    if (! search.Rave())
        throw GtpFailure("RAVE not enabled");
    SgPointArray<string> array("\"\"");
    const SgUctTree& tree = search.Tree();
    for (SgUctChildIterator it(tree, tree.Root()); it; ++it)
    {
        const SgUctNode& child = *it;
        SgPoint p = child.Move();
        if (p == SG_PASS || ! child.HasRaveValue())
            continue;
        ostringstream out;
        out << fixed << setprecision(2) << child.RaveValue();
        array[p] = out.str();
    }
    cmd << '\n'
        << SgWritePointArray<string>(array, m_bd.Size());
}

/** Return filtered root moves.
    @see GoUctRootFilter::Get()
*/
void GoUctCommands::CmdRootFilter(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    cmd << SgWritePointList(Player().RootFilter().Get(), "", false);
}

/** Save the UCT tree in SGF format.
    Arguments: filename [max_depth] <br>
    max_depth is an optional argument to cut the tree at a certain depth
    (the root node has depth 0). If it is not used, the full tree will be
    saved.
    @see GoUctSearch::SaveTree()
*/
void GoUctCommands::CmdSaveTree(GtpCommand& cmd)
{
    if (Search().MpiSynchronizer()->IsRootProcess())
    {
    cmd.CheckNuArgLessEqual(2);
    string fileName = cmd.Arg(0);
    int maxDepth = -1;
    if (cmd.NuArg() == 2)
        maxDepth = cmd.IntArg(1, 0);
    ofstream out(fileName.c_str());
    if (! out)
        throw GtpFailure() << "Could not open " << fileName;
    Search().SaveTree(out, maxDepth);
    }
}

/** Save all random games.
    Arguments: filename
    @see GoUctSearch::SaveGames()
*/
void GoUctCommands::CmdSaveGames(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    string fileName = cmd.Arg(0);
    try
    {
        Search().SaveGames(fileName);
    }
    catch (const SgException& e)
    {
        throw GtpFailure(e.what());
    }
}

/** Count the score using the scoring function of UCT.
    Arguments: none <br>
    Returns: Score (Win/Loss)
    @see GoBoardUtil::ScoreSimpleEndPosition()
*/
void GoUctCommands::CmdScore(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    try
    {
        float komi = m_bd.Rules().Komi().ToFloat();
        cmd << GoBoardUtil::ScoreSimpleEndPosition(m_bd, komi);
    }
    catch (const SgException& e)
    {
        throw GtpFailure(e.what());
    }
}

/** Show the best sequence from last search.
    This command is compatible with the GoGui analyze command type "gfx"
    (There is no "var" command type supported in GoGui 1.1, which allows
    to specify the first color to move within the response, and this command
    returns the sequence of the last search, which is unrelated to the current
    color to play on the board.) <br>
    Arguments: none
*/
void GoUctCommands::CmdSequence(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    GoUctUtil::GfxSequence(Search(), Search().ToPlay(), cmd);
}

/** Write statistics of GoUctPlayer.
    Arguments: none
    @see GoUctPlayer::Statistics
*/
void GoUctCommands::CmdStatPlayer(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    Player().GetStatistics().Write(cmd);
}

/** Clear statistics of GoUctPlayer.
    Arguments: none
    @see GoUctPlayer::Statistics
*/
void GoUctCommands::CmdStatPlayerClear(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    Player().ClearStatistics();
}

/** Write statistics of playout policy.
    Arguments: none <br>
    Needs enabling the statistics with
    <code>uct_param_policy statistics_enabled</code>
    Only the statistics of the first thread's policy used.
    @see GoUctPlayoutPolicyStat
*/
void GoUctCommands::CmdStatPolicy(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    if (! Player().m_playoutPolicyParam.m_statisticsEnabled)
        SgWarning() << "statistics not enabled in policy parameters\n";
    cmd << "Black Statistics:\n";
    Policy(0).Statistics(SG_BLACK).Write(cmd);
    cmd << "\nWhite Statistics:\n";
    Policy(0).Statistics(SG_WHITE).Write(cmd);
}

/** Clear statistics of GoUctPlayoutPolicy
    Arguments: none <br>
    Only the statistics of the first thread's policy used.
    @see GoUctPlayoutPolicyStat
*/
void GoUctCommands::CmdStatPolicyClear(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    Policy(0).ClearStatistics();
}

/** Write statistics of search and tree.
    Arguments: none
    @see SgUctSearch::WriteStatistics()
*/
void GoUctCommands::CmdStatSearch(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    const GoUctSearch& search = Search();
    SgUctTreeStatistics treeStatistics;
    treeStatistics.Compute(search.Tree());
    cmd << "SearchStatistics:\n";
    search.WriteStatistics(cmd);
    cmd << "TreeStatistics:\n"
        << treeStatistics;
}

/** Write average point status.
    This command is compatible with the GoGui analyze type @c dboard. <br>
    Statistics are only collected, if enabled with
    <code>uct_param_global_search territory_statistics 1</code>. <br>
    Arguments: none
    @see GoUctGlobalSearchState::m_territoryStatistics
*/
void GoUctCommands::CmdStatTerritory(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    SgPointArray<SgUctStatistics> territoryStatistics
        = ThreadState(0).m_territoryStatistics;
    SgPointArray<float> array;
    for (GoBoard::Iterator it(m_bd); it; ++it)
    {
        if (territoryStatistics[*it].Count() == 0)
            throw GtpFailure("no statistics available");
        array[*it] = territoryStatistics[*it].Mean() * 2 - 1;
    }
    cmd << '\n'
        << SgWritePointArrayFloat<float>(array, m_bd.Size(), true, 3);
}

/** Return value of root node from last search.
    Arguments: none
*/
void GoUctCommands::CmdValue(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    cmd << Search().Tree().Root().Mean();
}

/** Return value of root node from last search, from Black's point of view.
    Arguments: none
*/
void GoUctCommands::CmdValueBlack(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    float value = Search().Tree().Root().Mean();
    if (Search().ToPlay() == SG_WHITE)
        value = SgUctSearch::InverseEval(value);
    cmd << value;
}

/** Do a small search with territory statistics enabled to determine what
    blocks are dead for the final_status_list and final_score commands.
    @return Point set containing dead stones.
*/
SgPointSet GoUctCommands::DoFinalStatusSearch()
{
    SgPointSet deadStones;
    if (GoBoardUtil::TwoPasses(m_bd) && m_bd.Rules().CaptureDead())
        // Everything is alive if end position and Tromp-Taylor rules
        return deadStones;

    const size_t MAX_GAMES = 10000;
    SgDebug() << "GoUctCommands::DoFinalStatusSearch: doing a search with "
              << MAX_GAMES << " games to determine final status\n";
    GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
                      GoUctPlayoutPolicyFactory<GoUctBoard> >&
        search = GlobalSearch();
    SgRestorer<bool> restorer(&search.m_param.m_territoryStatistics);
    search.m_param.m_territoryStatistics = true;
    // Undo passes, because UCT search always scores with Tromp-Taylor after
    // two passes in-tree
    int nuUndoPass = 0;
    SgBlackWhite toPlay = m_bd.ToPlay();
    while (m_bd.GetLastMove() == SG_PASS)
    {
        m_bd.Undo();
        toPlay = SgOppBW(toPlay);
        ++nuUndoPass;
    }
    m_player->UpdateSubscriber();
    if (nuUndoPass > 0)
        SgDebug() << "Undoing " << nuUndoPass << " passes\n";
    vector<SgMove> sequence;
    search.Search(MAX_GAMES, numeric_limits<double>::max(), sequence);
    SgDebug() << SgWriteLabel("Sequence")
              << SgWritePointList(sequence, "", false);
    for (int i = 0; i < nuUndoPass; ++i)
    {
        m_bd.Play(SG_PASS, toPlay);
        toPlay = SgOppBW(toPlay);
    }
    m_player->UpdateSubscriber();

    SgPointArray<SgUctStatistics> territoryStatistics =
        ThreadState(0).m_territoryStatistics;
    GoSafetySolver safetySolver(m_bd);
    SgBWSet safe;
    safetySolver.FindSafePoints(&safe);
    for (GoBlockIterator it(m_bd); it; ++it)
    {
        SgBlackWhite c = m_bd.GetStone(*it);
        bool isDead = safe[SgOppBW(c)].Contains(*it);
        if (! isDead && ! safe[c].Contains(*it))
        {
            SgStatistics<float,int> averageStatus;
            for (GoBoard::StoneIterator it2(m_bd, *it); it2; ++it2)
            {
                if (territoryStatistics[*it2].Count() == 0)
                    // No statistics, maybe all simulations aborted due to
                    // max length or mercy rule.
                    return deadStones;
                averageStatus.Add(territoryStatistics[*it2].Mean());
            }
            const float threshold = 0.3;
            isDead =
                ((c == SG_BLACK && averageStatus.Mean() < threshold)
                 || (c == SG_WHITE && averageStatus.Mean() > 1 - threshold));
        }
        if (isDead)
            for (GoBoard::StoneIterator it2(m_bd, *it); it2; ++it2)
                deadStones.Include(*it2);
    }
    return deadStones;
}

GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
                      GoUctPlayoutPolicyFactory<GoUctBoard> >&
GoUctCommands::GlobalSearch()
{
    return Player().GlobalSearch();
}

GoUctPlayerType& GoUctCommands::Player()
{
    if (m_player == 0)
        throw GtpFailure("player not GoUctPlayer");
    try
    {
        return dynamic_cast<GoUctPlayerType&>(*m_player);
    }
    catch (const bad_cast&)
    {
        throw GtpFailure("player not GoUctPlayer");
    }
}

GoUctPlayoutPolicy<GoUctBoard>&
GoUctCommands::Policy(std::size_t threadId)
{
    GoUctPlayoutPolicy<GoUctBoard>* policy =
        dynamic_cast<GoUctPlayoutPolicy<GoUctBoard>*>(
                                              ThreadState(threadId).Policy());
    if (policy == 0)
        throw GtpFailure("player has no GoUctPlayoutPolicy");
    return *policy;
}

void GoUctCommands::Register(GtpEngine& e)
{
    Register(e, "final_score", &GoUctCommands::CmdFinalScore);
    Register(e, "final_status_list", &GoUctCommands::CmdFinalStatusList);
    Register(e, "uct_bounds", &GoUctCommands::CmdBounds);
    Register(e, "uct_default_policy", &GoUctCommands::CmdDefaultPolicy);
    Register(e, "uct_estimator_stat", &GoUctCommands::CmdEstimatorStat);
    Register(e, "uct_gfx", &GoUctCommands::CmdGfx);
    Register(e, "uct_max_memory", &GoUctCommands::CmdMaxMemory);
    Register(e, "uct_moves", &GoUctCommands::CmdMoves);
    Register(e, "uct_param_globalsearch",
             &GoUctCommands::CmdParamGlobalSearch);
    Register(e, "uct_param_policy", &GoUctCommands::CmdParamPolicy);
    Register(e, "uct_param_player", &GoUctCommands::CmdParamPlayer);
    Register(e, "uct_param_rootfilter", &GoUctCommands::CmdParamRootFilter);
    Register(e, "uct_param_search", &GoUctCommands::CmdParamSearch);
    Register(e, "uct_patterns", &GoUctCommands::CmdPatterns);
    Register(e, "uct_policy_moves", &GoUctCommands::CmdPolicyMoves);
    Register(e, "uct_prior_knowledge", &GoUctCommands::CmdPriorKnowledge);
    Register(e, "uct_rave_values", &GoUctCommands::CmdRaveValues);
    Register(e, "uct_root_filter", &GoUctCommands::CmdRootFilter);
    Register(e, "uct_savegames", &GoUctCommands::CmdSaveGames);
    Register(e, "uct_savetree", &GoUctCommands::CmdSaveTree);
    Register(e, "uct_sequence", &GoUctCommands::CmdSequence);
    Register(e, "uct_score", &GoUctCommands::CmdScore);
    Register(e, "uct_stat_player", &GoUctCommands::CmdStatPlayer);
    Register(e, "uct_stat_player_clear", &GoUctCommands::CmdStatPlayerClear);
    Register(e, "uct_stat_policy", &GoUctCommands::CmdStatPolicy);
    Register(e, "uct_stat_policy_clear", &GoUctCommands::CmdStatPolicyClear);
    Register(e, "uct_stat_search", &GoUctCommands::CmdStatSearch);
    Register(e, "uct_stat_territory", &GoUctCommands::CmdStatTerritory);
    Register(e, "uct_value", &GoUctCommands::CmdValue);
    Register(e, "uct_value_black", &GoUctCommands::CmdValueBlack);
}

void GoUctCommands::Register(GtpEngine& engine, const std::string& command,
                             GtpCallback<GoUctCommands>::Method method)
{
    engine.Register(command, new GtpCallback<GoUctCommands>(this, method));
}

GoUctSearch& GoUctCommands::Search()
{
    try
    {
        GoUctObjectWithSearch& object =
            dynamic_cast<GoUctObjectWithSearch&>(*m_player);
        return object.Search();
    }
    catch (const bad_cast&)
    {
        throw GtpFailure("player is not a GoUctObjectWithSearch");
    }
}

/** Return state of first thread, if search is of type GoUctGlobalSearch.
    @throws GtpFailure, if search is a different subclass or threads are not
    yet created.
*/
GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> >&
GoUctCommands::ThreadState(std::size_t threadId)
{
    GoUctSearch& search = Search();
    if (! search.ThreadsCreated())
        search.CreateThreads();
    try
    {
        return dynamic_cast<
             GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> >&>(
                                                search.ThreadState(threadId));
    }
    catch (const bad_cast&)
    {
        throw GtpFailure("player has no GoUctGlobalSearchState");
    }
}

//----------------------------------------------------------------------------
