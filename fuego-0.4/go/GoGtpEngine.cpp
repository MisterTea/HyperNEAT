//----------------------------------------------------------------------------
/** @file GoGtpEngine.cpp
    See GoGtpEngine.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoGtpEngine.h"

#include <algorithm>
#include <cmath>
#include <exception>
#include <fstream>
#include <iomanip>
#include <limits>
#include <time.h>
#include <boost/filesystem/operations.hpp>
#include "GoEyeUtil.h"
#include "GoGtpCommandUtil.h"
#include "GoNodeUtil.h"
#include "GoPlayer.h"
#include "GoTimeControl.h"
#include "GoUtil.h"
#include "SgDebug.h"
#include "SgEBWArray.h"
#include "SgException.h"
#include "SgGameReader.h"
#include "SgGameWriter.h"
#include "SgPointSetUtil.h"
#include "SgTime.h"
#include "SgWrite.h"

#if GTPENGINE_PONDER
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>
#endif

using namespace std;
using boost::filesystem::exists;
using boost::filesystem::path;
using boost::filesystem::remove;
using SgPointUtil::Pt;

//----------------------------------------------------------------------------

namespace {

GoRules::KoRule KoRuleArg(GtpCommand& cmd, size_t number)
{
    string arg = cmd.ArgToLower(number);
    if (arg == "simple")
        return GoRules::SIMPLEKO;
    if (arg == "superko")
        return GoRules::SUPERKO;
    if (arg == "pos_superko")
        return GoRules::POS_SUPERKO;
    throw GtpFailure() << "unknown ko rule \"" << arg << '"';
}

string KoRuleToString(GoRules::KoRule rule)
{
    switch (rule)
    {
    case GoRules::SIMPLEKO:
        return "simple";
    case GoRules::SUPERKO:
        return "superko";
    case GoRules::POS_SUPERKO:
        return "pos_superko";
    default:
        SG_ASSERT(false);
        return "?";
    }
}

} // namespace

//----------------------------------------------------------------------------

GoGtpEngine::GoGtpEngine(GtpInputStream& in, GtpOutputStream& out, int fixedBoardSize,
                         const char* programPath, bool noPlayer,
                         bool noHandicap)
    : GtpEngine(in, out),
      m_player(0),
      m_autoBook(0),
      m_noPlayer(noPlayer),
      m_acceptIllegal(false),
      m_autoSave(false),
      m_autoShowBoard(false),
      m_debugToComment(false),
      m_fixedBoardSize(fixedBoardSize),
      m_maxClearBoard(-1),
      m_numberClearBoard(0),
      m_timeLastMove(0),
      m_timeLimit(10),
      m_overhead(0),
      m_board(fixedBoardSize > 0 ? fixedBoardSize : GO_DEFAULT_SIZE),
      m_game(m_board),
      m_sgCommands(*this, programPath),
      m_bookCommands(*this, m_board, m_book),
      m_mpiSynchronizer(SgMpiNullSynchronizer::Create())
{
    Register("all_legal", &GoGtpEngine::CmdAllLegal, this);
    Register("boardsize", &GoGtpEngine::CmdBoardSize, this);
    Register("clear_board", &GoGtpEngine::CmdClearBoard, this);
    Register("get_komi", &GoGtpEngine::CmdGetKomi, this);
    Register("gg-undo", &GoGtpEngine::CmdGGUndo, this);
    Register("go_board", &GoGtpEngine::CmdBoard, this);
    Register("go_param", &GoGtpEngine::CmdParam, this);
    Register("go_param_rules", &GoGtpEngine::CmdParamRules, this);
    Register("go_player_board", &GoGtpEngine::CmdPlayerBoard, this);
    Register("go_point_info", &GoGtpEngine::CmdPointInfo, this);
    Register("go_point_numbers", &GoGtpEngine::CmdPointNumbers, this);
    Register("go_rules", &GoGtpEngine::CmdRules, this);
    Register("go_sentinel_file", &GoGtpEngine::CmdSentinelFile, this);
    Register("go_set_info", &GoGtpEngine::CmdSetInfo, this);
    Register("gogui-analyze_commands", &GoGtpEngine::CmdAnalyzeCommands,
             this);
    Register("gogui-interrupt", &GoGtpEngine::CmdInterrupt, this);
    Register("gogui-play_sequence", &GoGtpEngine::CmdPlaySequence, this);
    Register("gogui-setup", &GoGtpEngine::CmdSetup, this);
    Register("gogui-setup_player", &GoGtpEngine::CmdSetupPlayer, this);
    Register("is_legal", &GoGtpEngine::CmdIsLegal, this);
    Register("kgs-genmove_cleanup", &GoGtpEngine::CmdGenMoveCleanup, this);
    Register("kgs-time_settings", &GoGtpEngine::CmdKgsTimeSettings, this);
    Register("komi", &GoGtpEngine::CmdKomi, this);
    Register("list_stones", &GoGtpEngine::CmdListStones, this);
    Register("loadsgf", &GoGtpEngine::CmdLoadSgf, this);
    Register("play", &GoGtpEngine::CmdPlay, this);
    Register("savesgf", &GoGtpEngine::CmdSaveSgf, this);
    Register("showboard", &GoGtpEngine::CmdShowBoard, this);
    Register("time_left", &GoGtpEngine::CmdTimeLeft, this);
    Register("time_settings", &GoGtpEngine::CmdTimeSettings, this);
    Register("undo", &GoGtpEngine::CmdUndo, this);
    m_sgCommands.Register(*this);
    if (! noPlayer)
    {
        Register("all_move_values", &GoGtpEngine::CmdAllMoveValues, this);
        Register("final_score", &GoGtpEngine::CmdFinalScore, this);
        Register("genmove", &GoGtpEngine::CmdGenMove, this);
        Register("go_clock", &GoGtpEngine::CmdClock, this);
        Register("go_param_timecontrol", &GoGtpEngine::CmdParamTimecontrol,
                 this);
        Register("reg_genmove", &GoGtpEngine::CmdRegGenMove, this);
        Register("reg_genmove_toplay", &GoGtpEngine::CmdRegGenMoveToPlay,
                 this);
        Register("time_lastmove", &GoGtpEngine::CmdTimeLastMove, this);
        m_bookCommands.Register(*this);
    }
    if (! noHandicap)
    {
        Register("fixed_handicap", &GoGtpEngine::CmdFixedHandicap, this);
        Register("place_free_handicap", &GoGtpEngine::CmdPlaceFreeHandicap,
                 this);
        Register("set_free_handicap", &GoGtpEngine::CmdSetFreeHandicap, this);
    }
}

GoGtpEngine::~GoGtpEngine()
{
#ifndef NDEBUG
    m_player = 0;
#endif
}

/** Add player name property to root node. */
void GoGtpEngine::AddPlayerProp(SgBlackWhite color, const string& name,
                                bool overwrite)
{
    SgNode& root = GetGame().Root();
    SgPropID SG_PROP_PLAYER =
        (color == SG_BLACK ? SG_PROP_PLAYER_BLACK : SG_PROP_PLAYER_WHITE);
    if (overwrite || ! root.HasProp(SG_PROP_PLAYER))
        root.SetStringProp(SG_PROP_PLAYER, name);
}

void GoGtpEngine::AddPlayStatistics()
{
    // Default implementation does nothing
}

void GoGtpEngine::AddStatistics(const std::string& key,
                                const std::string& value)
{
    SG_ASSERT(m_statisticsValues.size() == m_statisticsSlots.size());
    if (value.find('\t') != string::npos)
        throw SgException("GoGtpEngine::AddStatistics: value contains tab: '"
                          + value + "'");
    for (size_t i = 0; i < m_statisticsSlots.size(); ++i)
        if (m_statisticsSlots[i] == key)
        {
            m_statisticsValues[i] = value;
            return;
        }
    throw SgException("GoGtpEngine::AddStatistics: invalid key '" + key
                      + "'");
}

void GoGtpEngine::ApplyTimeSettings()
{
    SG_ASSERT(Board().MoveNumber() == 0);
    if (m_timeSettings.NoTimeLimits())
        return;
    GoGame& game = GetGame();
    SgTimeRecord& time = game.Time();
    SgNode& node = *game.CurrentNode();
    int mainTime = m_timeSettings.MainTime();
    time.SetOTPeriod(m_timeSettings.ByoYomiTime());
    time.SetOTNumMoves(m_timeSettings.ByoYomiStones());
    time.SetOverhead(m_overhead);
    time.SetClock(node, SG_BLACK, mainTime);
    time.SetClock(node, SG_WHITE, mainTime);
    SgNode& root = game.Root();
    if (mainTime > 0)
        root.Add(new SgPropTime(SG_PROP_TIME, mainTime));
    root.SetIntProp(SG_PROP_OT_NU_MOVES, m_timeSettings.ByoYomiStones());
    root.Add(new SgPropTime(SG_PROP_OT_PERIOD, m_timeSettings.ByoYomiTime()));
    game.TurnClockOn(true);
}

void GoGtpEngine::AutoSave() const
{
    if (! m_autoSave)
        return;
    try
    {
        SaveGame(m_autoSaveFileName);
    }
    catch (const GtpFailure& failure)
    {
        SgWarning() << failure.Response() << '\n';
    }
}

void GoGtpEngine::BoardChanged()
{
    GoBoard& bd = Board();
    if (m_autoShowBoard)
        SgDebug() << bd;
    AutoSave();
}

void GoGtpEngine::BeforeHandleCommand()
{
    SgSetUserAbort(false);
    SgDebug() << flush;
}

void GoGtpEngine::BeforeWritingResponse()
{
    SgDebug() << flush;
}

void GoGtpEngine::CheckBoardEmpty() const
{
    const GoBoard& bd = Board();
    if (bd.TotalNumStones(SG_BLACK) + bd.TotalNumStones(SG_WHITE) > 0)
        throw GtpFailure("board is not empty");
}

/** Check if move is legal.
    @param message Prefix for error message; move and reason will be appended
    @param color Player of move
    @param move The move
    @param checkOnlyOccupied Only check if point is empty (accepts moves that
    are illegal, because of the ko or suicide rules used)
    @throws GtpFailure if not legal.
*/
void GoGtpEngine::CheckLegal(string message, SgBlackWhite color, SgPoint move,
                             bool checkOnlyOccupied)
{
    GoBoard& bd = Board();
    bool illegal = false;
    string reason = "";
    if (move != SG_PASS)
    {
        if (bd.Occupied(move))
        {
            illegal = true;
            reason = " (occupied)";
        }
        else if (! checkOnlyOccupied)
        {
            bd.Play(move, color);
            GoMoveInfo moveInfo = bd.GetLastMoveInfo();
            bd.Undo();
            if (moveInfo.test(GO_MOVEFLAG_ILLEGAL))
            {
                illegal = true;
                if (moveInfo.test(GO_MOVEFLAG_SUICIDE))
                    reason = " (suicide)";
                else if (moveInfo.test(GO_MOVEFLAG_REPETITION))
                {
                    reason =
                        " (" + KoRuleToString(bd.Rules().GetKoRule()) + ")";
                }
            }
        }
    }
    if (illegal)
    {
        int moveNumber = GetGame().CurrentMoveNumber() + 1;
        throw GtpFailure() << message << moveNumber << ' ' << SgBW(color)
                           << ' ' << SgWritePoint(move) << reason;
    }
}

void GoGtpEngine::CheckMaxClearBoard()
{
    if (m_maxClearBoard >= 0 && m_numberClearBoard > m_maxClearBoard - 1)
        throw GtpFailure() << "maximum number of " << m_maxClearBoard
                           << " reached";
    ++m_numberClearBoard;
}

/** Return all legal move points.
    Compatible with GNU Go's all_legal command.<br>
    Arguments: color
*/
void GoGtpEngine::CmdAllLegal(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    SgBlackWhite color = BlackWhiteArg(cmd, 0);
    SgVector<SgPoint> allLegal;
    for (GoBoard::Iterator p(Board()); p; ++p)
        if (Board().IsLegal(*p, color))
            allLegal.PushBack(*p);
    cmd << SgWritePointList(allLegal, "", false);
}

/** Like GNU Go's all_move_values */
void GoGtpEngine::CmdAllMoveValues(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    GoBoard& bd = Board();
    GoPlayer& player = Player();
    for (GoBoard::Iterator it(bd); it; ++it)
        if (! bd.Occupied(*it))
        {
            int value = player.MoveValue(*it);
            if (value > numeric_limits<int>::min())
                cmd << SgWritePoint(*it) << ' ' << value << '\n';
        }
}

/** Return configuration for GoGui analyze commands.
    See the GoGui documentation http://gogui.sf.net
*/
void GoGtpEngine::CmdAnalyzeCommands(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    cmd <<
        "hpstring/Go Board/go_board\n"
        "param/Go Param/go_param\n"
        "param/Go Param Rules/go_param_rules\n"
        "hpstring/Go Point Info/go_point_info %p\n"
        "sboard/Go Point Numbers/go_point_numbers\n"
        "none/Go Rules/go_rules %s\n"
        "plist/All Legal/all_legal %c\n"
        "string/ShowBoard/showboard\n"
        "string/CpuTime/cputime\n"
        "string/Get Komi/get_komi\n"
        "string/Get Random Seed/get_random_seed\n"
        "plist/List Stones/list_stones %c\n"
        "none/Set Random Seed/set_random_seed %s\n"
        "none/SaveSgf/savesgf %w\n";
    m_sgCommands.AddGoGuiAnalyzeCommands(cmd);
    if (! m_noPlayer)
    {
        m_bookCommands.AddGoGuiAnalyzeCommands(cmd);
        cmd <<
            "pspairs/All Move Values/all_move_values\n"
            "string/Final Score/final_score\n"
            "param/Go Param TimeControl/go_param_timecontrol\n"
            "varc/Reg GenMove/reg_genmove %c\n";
    }
}

/** Print some information about game board.
    See WriteBoardInfo for optional arguments.
*/
void GoGtpEngine::CmdBoard(GtpCommand& cmd)
{
    WriteBoardInfo(cmd, Board());
}

/** Init new game with given board size. */
void GoGtpEngine::CmdBoardSize(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    int size = cmd.IntArg(0, SG_MIN_SIZE, SG_MAX_SIZE);
    if (m_fixedBoardSize > 0 && size != m_fixedBoardSize)
        throw GtpFailure() << "Boardsize " << m_fixedBoardSize << " fixed";
    if (Board().MoveNumber() > 0)
        GameFinished();
    Init(size);
}

/** Init new game.
    @see SetMaxGames()
*/
void GoGtpEngine::CmdClearBoard(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    CheckMaxClearBoard();
    if (! m_sentinelFile.empty() && exists(m_sentinelFile))
        throw GtpFailure() << "Detected sentinel file '"
                           << m_sentinelFile.native() << "'";
    if (Board().MoveNumber() > 0)
        GameFinished();
    Init(Board().Size());
    if (m_player != 0)
        m_player->OnNewGame();
    BoardChanged();
}

/** Show clock info from GoGameRecord::Time() */
void GoGtpEngine::CmdClock(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    cmd << '\n' << GetGame().Time();
}

/** Compute final score.
    Computes a final score only, if Tromp-Taylor rules are used
    (GoRules::CaptureDead() == true and GoRules::JapaneseScoring() == false).
    Otherwise it returns an error. Override this function for players that
    have enough knowledge to do a better scoring.
*/
void GoGtpEngine::CmdFinalScore(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    const GoBoard& bd = Board();
    if (! bd.Rules().CaptureDead() || bd.Rules().JapaneseScoring())
        throw GtpFailure("can only score if Tromp-Taylor rules");
    float komi = bd.Rules().Komi().ToFloat();
    float score = GoBoardUtil::TrompTaylorScore(bd, komi);
    cmd << GoUtil::ScoreToString(score);
}

/** Standard GTP command fixed_handicap. */
void GoGtpEngine::CmdFixedHandicap(GtpCommand& cmd)
{
    int n = cmd.IntArg(0, 2);
    int size = Board().Size();
    SgVector<SgPoint> stones = GoGtpCommandUtil::GetHandicapStones(size, n);
    PlaceHandicap(stones);
}

/** Generate and play a move. */
void GoGtpEngine::CmdGenMove(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    SgBlackWhite color = BlackWhiteArg(cmd, 0);
    GoGame& game = GetGame();
    auto_ptr<SgDebugToString> debugStrToString;
    if (m_debugToComment)
        debugStrToString.reset(new SgDebugToString(true));
    SgPoint move = GenMove(color, false);
    if (move == SG_RESIGN)
    {
        cmd << "resign";
        SgNode* node = game.AddResignNode(color);
        if (debugStrToString.get() != 0)
        {
            node->AddComment("\n\n");
            node->AddComment(debugStrToString->GetString());
        }
        AutoSave();
    }
    else
    {
        SgNode* node = game.AddMove(move, color);
        if (debugStrToString.get() != 0)
            node->AddComment(debugStrToString->GetString());
        game.GoToNode(node);
        BoardChanged();
        cmd << SgWritePoint(move);
    }
    AddPlayerProp(color, Player().Name(), false);
}

/** Generate cleanup move.
    As defined in the kgsGtp interface to the KGS Go server.
    Should not return pass, before all enemy dead stones are captured.<br>
    Arguments: color
    @bug This does not work, if the opponent passes, before he captured all
    of our dead stones, because then we could also pass without capturing all
    of his dead stones (if it is a win accosing to Tromp-Taylor counting),
    but KGS will not use Tromp-Taylor counting in the cleanup phase, but
    send another <tt>final_status_list dead</tt> command. See also bug
    2157832 int the bug tracker.
*/
void GoGtpEngine::CmdGenMoveCleanup(GtpCommand& cmd)
{
    GoRules& rules = Board().Rules();
    bool oldCaptureDead = rules.CaptureDead();
    rules.SetCaptureDead(true);
    RulesChanged();
    try
    {
        CmdGenMove(cmd);
    }
    catch (const GtpFailure& failure)
    {
        rules.SetCaptureDead(oldCaptureDead);
        RulesChanged();
        throw failure;
    }
    rules.SetCaptureDead(oldCaptureDead);
    RulesChanged();
}

/** Get the komi.
    Compatible to GNU Go's get_komi.
*/
void GoGtpEngine::CmdGetKomi(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    cmd << Board().Rules().Komi();
}

/** Undo multiple moves.
    Extension command introduced by GNU Go to undo multiple moves.<br>
    Arguments: optional int<br>
    Fails: if move history is too short<br>
    Returns: nothing
 */
void GoGtpEngine::CmdGGUndo(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(1);
    Undo(cmd.NuArg() == 0 ? 0 : cmd.IntArg(0, 0));
    BoardChanged();
}

/** Sets time settings on kgs.
    Handles the four different kinds of time control on kgs, "none",
    "absolute", "byoyomi" (which is not currently fully supported),
    and "canadian".
 */
void GoGtpEngine::CmdKgsTimeSettings(GtpCommand& cmd)
{
    if (cmd.NuArg() < 1)
        throw GtpFailure("Need at least one argument!");
    if (Board().MoveNumber() > 0)
        throw GtpFailure("cannot change time settings during game");
    std::string type = cmd.Arg(0);
    if (type == "none")
    {
        cmd.CheckNuArg(1);
        // This will make m_timeSetings.NoTimeLimits() to true.
        m_timeSettings = GoGtpTimeSettings(0, 1, 0);
        SG_ASSERT(m_timeSettings.NoTimeLimits());
    }
    else if (type == "absolute")
    {
        cmd.CheckNuArg(2);
        int mainTime = cmd.IntArg(1, 0);
        GoGtpTimeSettings timeSettings(mainTime, 0, 0);
        if (m_timeSettings == timeSettings)
            return;
        m_timeSettings = timeSettings;
        ApplyTimeSettings();
    }
    else if (type == "byoyomi")
    {
        cmd.CheckNuArg(4);
        // FIXME: not fully supported yet!
        int mainTime = cmd.IntArg(1, 0);
        int byoYomiTime = cmd.IntArg(2, 0);
        //int byoYomiPeriods = cmd.IntArg(3, 0);
        GoGtpTimeSettings timeSettings(mainTime, byoYomiTime, 1);
        if (m_timeSettings == timeSettings)
            return;
        m_timeSettings = timeSettings;
        ApplyTimeSettings();
    }    
    else if (type == "canadian")
    {
        cmd.CheckNuArg(4);
        int mainTime = cmd.IntArg(1, 0);
        int byoYomiTime = cmd.IntArg(2, 0);
        int byoYomiStones = cmd.IntArg(3, 0);
        GoGtpTimeSettings timeSettings(mainTime, byoYomiTime, byoYomiStones);
        if (m_timeSettings == timeSettings)
            return;
        m_timeSettings = timeSettings;
        ApplyTimeSettings();
    }
    else
        throw GtpFailure("Unknown type of time control");
}

/** This command indicates that commands can be interrupted using the GoGui
    convention.
    The command does nothing but indicate the ability to handle the
    special comment line <tt># interrupt</tt> used by GoGui.
    It is registered as a handler for @c gogui-interrupt.
*/
void GoGtpEngine::CmdInterrupt(GtpCommand& cmd)
{
    cmd.CheckArgNone();
}

/** Check if move is legal.
    Compatible with GNU Go's is_legal.
    Arguments: color move<br>
    Returns: 0/1
*/
void GoGtpEngine::CmdIsLegal(GtpCommand& cmd)
{
    cmd.CheckNuArg(2);
    SgBlackWhite color = BlackWhiteArg(cmd, 0);
    SgPoint move = MoveArg(cmd, 1);
    cmd << SgWriteBoolAsInt(Board().IsLegal(move, color));
}

/** Set the komi.
    GTP standard command.
*/
void GoGtpEngine::CmdKomi(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    try
    {
        GoKomi komi(cmd.Arg(0));
        GetGame().Root().SetRealProp(SG_PROP_KOMI, komi.ToFloat(), 1);
        m_defaultRules.SetKomi(komi);
        Board().Rules().SetKomi(komi);
        RulesChanged();
    }
    catch (const GoKomi::InvalidKomi& e)
    {
        throw GtpFailure(e.what());
    }
}

/** List stones on board.
    Mainly useful for regression tests to verify the board position.
    For compatibility with GNU Go's list_stones command, the points are
    returned in a single line in the same order that is used by GNU Go 3.6
    (A19, B19, ..., A18, B18, ...)

    Arguments: color<br>
    Returns: List of stones<br>
*/
void GoGtpEngine::CmdListStones(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    SgBlackWhite color = BlackWhiteArg(cmd, 0);
    const GoBoard& bd = Board();
    const SgPointSet& points = bd.All(color);
    bool isFirst = true;
    for (int row = bd.Size(); row >= 1; --row)
        for (int col = 1; col <= bd.Size();++col)
        {
            SgPoint p = Pt(col, row);
            if (points.Contains(p))
            {
                if (! isFirst)
                    cmd << ' ';
                cmd << SgWritePoint(p);
                isFirst = false;
            }
        }
}

/** Load a position from a SGF file. */
void GoGtpEngine::CmdLoadSgf(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    string fileName = cmd.Arg(0);
    int moveNumber = -1;
    if (cmd.NuArg() == 2)
        moveNumber = cmd.IntArg(1, 1);
    ifstream in(fileName.c_str());
    if (! in)
        throw GtpFailure("could not open file");
    SgGameReader reader(in);
    SgNode* root = reader.ReadGame();
    if (root == 0)
        throw GtpFailure("no games in file");
    if (reader.GetWarnings().any())
    {
        SgWarning() << fileName << ":\n";
        reader.PrintWarnings(SgDebug());
    }
    if (Board().MoveNumber() > 0)
        GameFinished();
    GoGame& game = GetGame();
    game.Init(root, true, false);
    if (! GoGameUtil::GotoBeforeMove(&game, moveNumber))
        throw GtpFailure("invalid move number");
    GoRules& rules = Board().Rules();
    rules = m_defaultRules;
    rules.SetKomi(GoNodeUtil::GetKomi(game.CurrentNode()));
    rules.SetHandicap(GoNodeUtil::GetHandicap(game.CurrentNode()));
    RulesChanged();
    if (m_player != 0)
        m_player->OnNewGame();
    BoardChanged();
}

/** Return name of player, if set, GtpEngine::Name otherwise. */
void GoGtpEngine::CmdName(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    if (m_player == 0)
        GtpEngine::CmdName(cmd);
    else
        cmd << m_player->Name();
}

/** Get and set GoGtpEngine parameters.
    Parameters:
    @arg @c auto_save See SetAutoSave()
    @arg @c accept_illegal Accept illegal ko or suicide moves in CmdPlay()
    @arg @c debug_to_comment See SetDebugToComment()
    @arg @c overhead See SgTimeRecord::SetOverhead()
    @arg @c statistics_file See SetStatisticsFile()
    @arg @c timelimit See TimeLimit()
*/
void GoGtpEngine::CmdParam(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    if (cmd.NuArg() == 0)
    {
        cmd << "[bool] accept_illegal " << m_acceptIllegal << '\n'
            << "[bool] debug_to_comment " << m_debugToComment << '\n'
            << "[string] auto_save " << (m_autoSave ? m_autoSavePrefix : "")
            << '\n'
            << "[string] overhead " << m_overhead << '\n'
            << "[string] statistics_file " << m_statisticsFile << '\n'
            << "[string] timelimit " << m_timeLimit << '\n';
    }
    else if (cmd.NuArg() >= 1 && cmd.NuArg() <= 2)
    {
        string name = cmd.Arg(0);
        if (name == "accept_illegal")
            m_acceptIllegal = cmd.BoolArg(1);
        else if (name == "debug_to_comment")
            m_debugToComment = cmd.BoolArg(1);
        else if (name == "auto_save")
        {
            string prefix = cmd.RemainingLine(0);
            if (prefix == "")
                m_autoSave = false;
            else
                SetAutoSave(prefix);
        }
        else if (name == "overhead")
        {
            m_overhead = cmd.FloatArg(1);
            GetGame().Time().SetOverhead(m_overhead);
        }
        else if (name == "statistics_file")
            SetStatisticsFile(cmd.RemainingLine(0));
        else if (name == "timelimit")
            m_timeLimit = cmd.FloatArg(1);
        else
            throw GtpFailure() << "unknown parameter: " << name;
    }
    else
        throw GtpFailure() << "need 0 or 2 arguments";
}

/** Get and set detailed rule parameters.
    Changes the rules in the current game as well as the default rule.

    Parameters:
    @arg @c allow_suicde See GoRules:AllowSuicide()
    @arg @c capture_dead See GoRules:CaptureDead()
    @arg @c extra_handicap_komi See GoRules:ExtraHandicapKomi()
    @arg @c japanese_scoring See GoRules:JapaneseScoring()
    @arg @c two_passes_end_game See GoRules:TwoPassesEndGame()
    @arg @c ko_rule (simple, superko, pos_superko) See GoRules:KoRule()
*/
void GoGtpEngine::CmdParamRules(GtpCommand& cmd)
{
    cmd.CheckNuArgLessEqual(2);
    GoRules& r = Board().Rules();
    if (cmd.NuArg() == 0)
    {
        cmd << "[bool] allow_suicide "
            << SgWriteBoolAsInt(r.AllowSuicide()) << '\n'
            << "[bool] capture_dead "
            << SgWriteBoolAsInt(r.CaptureDead()) << '\n'
            << "[bool] extra_handicap_komi "
            << SgWriteBoolAsInt(r.ExtraHandicapKomi()) << '\n'
            << "[bool] japanese_scoring "
            << SgWriteBoolAsInt(r.JapaneseScoring()) << '\n'
            << "[bool] two_passes_end_game "
            << SgWriteBoolAsInt(r.TwoPassesEndGame()) << '\n'
            << "[list/simple/superko/pos_superko] ko_rule "
            << KoRuleToString(r.GetKoRule()) << '\n';
    }
    else if (cmd.NuArg() == 2)
    {
        string name = cmd.Arg(0);
        if (name == "allow_suicide")
        {
            r.SetAllowSuicide(cmd.BoolArg(1));
            m_defaultRules.SetAllowSuicide(cmd.BoolArg(1));
        }
        else if (name == "capture_dead")
        {
            r.SetCaptureDead(cmd.BoolArg(1));
            m_defaultRules.SetCaptureDead(cmd.BoolArg(1));
        }
        else if (name == "extra_handicap_komi")
        {
            r.SetExtraHandicapKomi(cmd.BoolArg(1));
            m_defaultRules.SetExtraHandicapKomi(cmd.BoolArg(1));
        }
        else if (name == "japanese_scoring")
        {
            r.SetJapaneseScoring(cmd.BoolArg(1));
            m_defaultRules.SetJapaneseScoring(cmd.BoolArg(1));
        }
        else if (name == "two_passes_end_game")
        {
            r.SetTwoPassesEndGame(cmd.BoolArg(1));
            m_defaultRules.SetTwoPassesEndGame(cmd.BoolArg(1));
        }
        else if (name == "ko_rule")
        {
            r.SetKoRule(KoRuleArg(cmd, 1));
            m_defaultRules.SetKoRule(KoRuleArg(cmd, 1));
        }
        else
            throw GtpFailure() << "unknown parameter: " << name;
        RulesChanged();
    }
    else
        throw GtpFailure() << "need 0 or 2 arguments";
}

/** Get and set time control parameters.
    Fails if the current player is not a SgObjectWithDefaultTimeControl
    or the time control is not a GoTimeControl.

    Parameters:
    @arg @c fast_open_factor See SgDefaultTimeControl::FastOpenFactor()
    @arg @c fast_open_moves See SgDefaultTimeControl::FastOpenMoves()
    @arg @c final_space See GoTimeControl::FinalSpace()
    @arg @c remaining_constant See SgDefaultTimeControl::RemainingConstant()
*/
void GoGtpEngine::CmdParamTimecontrol(GtpCommand& cmd)
{
    SgObjectWithDefaultTimeControl* object =
        dynamic_cast<SgObjectWithDefaultTimeControl*>(&Player());
    if (object == 0)
        throw GtpFailure("current player is not a "
                         "SgObjectWithDefaultTimeControl");
    GoTimeControl* c = dynamic_cast<GoTimeControl*>(&object->TimeControl());
    if (c == 0)
        throw GtpFailure("current player does not have a GoTimeControl");
    cmd.CheckNuArgLessEqual(2);
    if (cmd.NuArg() == 0)
    {
        cmd << "fast_open_factor " << c->FastOpenFactor() << '\n'
            << "fast_open_moves " << c->FastOpenMoves() << '\n'
            << "final_space " << c->FinalSpace() << '\n'
            << "remaining_constant " << c->RemainingConstant() << '\n';
    }
    else if (cmd.NuArg() == 2)
    {
        string name = cmd.Arg(0);
        if (name == "fast_open_factor")
            c->SetFastOpenFactor(cmd.FloatArg(1));
        else if (name == "fast_open_moves")
            c->SetFastOpenMoves(cmd.IntArg(1, 0));
        else if (name == "final_space")
            c->SetFinalSpace(max(cmd.FloatArg(1), 0.));
        else if (name == "remaining_constant")
            c->SetRemainingConstant(max(cmd.FloatArg(1), 0.));
        else
            throw GtpFailure() << "unknown parameter: " << name;
    }
    else
        throw GtpFailure() << "need 0 or 2 arguments";
}

/** Standard GTP command place_free_handicap.
    The current implementation uses the same locations as for fixed_handicap,
    if defined, and generates additional handicap locations by making the
    player play moves.

    Arguments: number of handicap stones <br>
    Effect: Places handicap stones at chosen locations <br>
    Returns: Handicap stone locations <br>
*/
void GoGtpEngine::CmdPlaceFreeHandicap(GtpCommand& cmd)
{
    CheckBoardEmpty();
    int n = cmd.IntArg(0, 2);
    int size = Board().Size();
    SgVector<SgPoint> stones;
    try
    {
        stones = GoGtpCommandUtil::GetHandicapStones(size, n);
    }
    catch (const GtpFailure&)
    {
    }
    if (stones.Length() < n && m_player != 0)
    {
        // 9 handicap are always defined for odd sizes >= 9
        if (n >= 9 && size % 2 != 0 && size >= 9 && size <= 25)
            stones = GoGtpCommandUtil::GetHandicapStones(size, 9);
        // 4 handicap are always defined for even sizes >= 8 and size 7
        else if (n >= 4 && size % 2 == 0 && (size >= 8 || size == 7)
                 && size <= 25)
            stones = GoGtpCommandUtil::GetHandicapStones(size, 4);
        SgDebug() << "GoGtpEngine: Generating missing handicap\n";
        GoSetup setup;
        for (SgVectorIterator<SgPoint> it(stones); it; ++it)
            setup.AddBlack(*it);
        GoBoard& playerBd = m_player->Board();
        playerBd.Init(playerBd.Size(), setup);
        for (int i = stones.Length(); i < n; ++i)
        {
            SgPoint p = GenMove(SG_BLACK, true);
            SgDebug() << "GoGtpEngine: " << i << ' ' << SgWritePoint(p)
                      << '\n';
            if (p == SG_PASS)
                break;
            playerBd.Play(p, SG_BLACK);
            stones.PushBack(p);
        }
    }
    SG_ASSERT(stones.Length() <= n); // less than n is allowed by GTP standard
    PlaceHandicap(stones);
    cmd << SgWritePointList(stones, "", false);
}

/** Play a move. */
void GoGtpEngine::CmdPlay(GtpCommand& cmd)
{
    cmd.CheckNuArg(2);
    SgBlackWhite color = BlackWhiteArg(cmd, 0);
    SgPoint move = MoveArg(cmd, 1);
    Play(color, move);
    BoardChanged();
}

/** Play a sequence of moves.
    Extension to standard play command used by GoGui.

    This command is registered with the command name @c gogui-play_sequence
    as used in newer versions of GoGui and for a transition period also
    with @c play_sequence as used by older versions of GoGui
*/
void GoGtpEngine::CmdPlaySequence(GtpCommand& cmd)
{
    SgNode* oldCurrentNode = GetGame().CurrentNode();
    try
    {
        for (size_t i = 0; i < cmd.NuArg(); i += 2)
            Play(BlackWhiteArg(cmd, i), MoveArg(cmd, i + 1));
    }
    catch (GtpFailure fail)
    {
        GetGame().GoToNode(oldCurrentNode);
        throw fail;
    }
    BoardChanged();
}

/** Print some information about point. */
void GoGtpEngine::CmdPointInfo(GtpCommand& cmd)
{
    SgPoint p = PointArg(cmd);
    const GoBoard& bd = Board();
    cmd << "Point:\n"
        << SgWriteLabel("Color") << SgEBW(bd.GetColor(p)) << '\n'
        << SgWriteLabel("InCenter") << bd.InCenter(p) << '\n'
        << SgWriteLabel("InCorner") << bd.InCorner(p) << '\n'
        << SgWriteLabel("Line") << bd.Line(p) << '\n'
        << SgWriteLabel("OnEdge") << bd.OnEdge(p) << '\n'
        << SgWriteLabel("EmptyNb") << bd.NumEmptyNeighbors(p) << '\n'
        << SgWriteLabel("EmptyNb8") << bd.Num8EmptyNeighbors(p) << '\n'
        << SgWriteLabel("Pos") << bd.Pos(p) << '\n';
    if (bd.Occupied(p))
    {
        SgVector<SgPoint> adjBlocks;
        GoBoardUtil::AdjacentBlocks(bd, p, SG_MAXPOINT, &adjBlocks);
        cmd << "Block:\n"
            << SgWritePointList(adjBlocks, "AdjBlocks", true)
            << SgWriteLabel("Anchor") << SgWritePoint(bd.Anchor(p)) << '\n'
            << SgWriteLabel("InAtari") << bd.InAtari(p) << '\n'
            << SgWriteLabel("IsSingleStone") << bd.IsSingleStone(p) << '\n'
            << SgWriteLabel("Liberties") << bd.NumLiberties(p) << '\n'
            << SgWriteLabel("Stones") << bd.NumStones(p) << '\n';
    }
    else
        cmd << "EmptyPoint:\n"
            << SgWriteLabel("IsFirst") << bd.IsFirst(p) << '\n'
            << SgWriteLabel("IsLegal/B") << bd.IsLegal(p, SG_BLACK) << '\n'
            << SgWriteLabel("IsLegal/W") << bd.IsLegal(p, SG_WHITE) << '\n'
            << SgWriteLabel("IsSuicide") << bd.IsSuicide(p) << '\n'
            << SgWriteLabel("MakesNakadeShape/B") 
            << GoEyeUtil::MakesNakadeShape(bd, p, SG_BLACK) << '\n'
            << SgWriteLabel("MakesNakadeShape/W") 
            << GoEyeUtil::MakesNakadeShape(bd, p, SG_WHITE) << '\n'
            << SgWriteLabel("IsSimpleEye/B") 
            << GoEyeUtil::IsSimpleEye(bd, p, SG_BLACK) << '\n'
            << SgWriteLabel("IsSimpleEye/W") 
            << GoEyeUtil::IsSimpleEye(bd, p, SG_WHITE) << '\n'
            << SgWriteLabel("IsSinglePointEye/B") 
            << GoEyeUtil::IsSinglePointEye(bd, p, SG_BLACK) << '\n'
            << SgWriteLabel("IsSinglePointEye/W") 
            << GoEyeUtil::IsSinglePointEye(bd, p, SG_WHITE) << '\n'
            << SgWriteLabel("IsPossibleEye/B") 
            << GoEyeUtil::IsPossibleEye(bd, SG_BLACK, p) << '\n'
            << SgWriteLabel("IsPossibleEye/W") 
            << GoEyeUtil::IsPossibleEye(bd, SG_WHITE, p) << '\n'
            ;
}

/** Print some information about player board.
    See WriteBoardInfo for optional arguments.
*/
void GoGtpEngine::CmdPlayerBoard(GtpCommand& cmd)
{
    WriteBoardInfo(cmd, Player().Board());
}

/** Show point numbers used in GoBoard. */
void GoGtpEngine::CmdPointNumbers(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    SgPointArray<int> array(0);
    for (GoBoard::Iterator p(Board()); p; ++p)
        array[*p] = *p;
    cmd << SgWritePointArray<int>(array, Board().Size());
}

void GoGtpEngine::CmdQuit(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    if (Board().MoveNumber() > 0)
        GameFinished();
    GtpEngine::CmdQuit(cmd);
}

/** Generate a move, but do not play it.
    Like in GNU Go, if there was a random seed set, it is initialized before
    each reg_genmove to avoid a dependency of the random numbers on previous
    move generations.
*/
void GoGtpEngine::CmdRegGenMove(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    SgRandom::SetSeed(SgRandom::Seed());
    SgPoint move = GenMove(BlackWhiteArg(cmd, 0), true);
    if (move == SG_RESIGN)
        cmd << "resign";
    else
        cmd << SgWritePoint(move);
}

/** Version of CmdRegGenMove() without color argument.
    This is a non-standard version of reg_genmove without color argument.
    It generates a move for the color to play.

*/
void GoGtpEngine::CmdRegGenMoveToPlay(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    SgRandom::SetSeed(SgRandom::Seed());
    SgPoint move = GenMove(Board().ToPlay(), true);
    cmd << SgWritePoint(move);
}

/** Set named rules.
    @see GoGtpEngine::SetNamedRules()
*/
void GoGtpEngine::CmdRules(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    string arg = cmd.Arg(0);
    try
    {
        SetNamedRules(arg);
    }
    catch (const SgException&)
    {
        throw GtpFailure() << "unknown rules: " << arg;
    }
}

/** Save current game to file.
    Saves the complete game tree, including any trees from searches
    if storing searches is enabled with global flags.<br>
    Argument: filename
*/
void GoGtpEngine::CmdSaveSgf(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    SaveGame(cmd.Arg(0));
}

/** Define a file that makes future clear_board commands fail.
    Defining a sentinel file can be used, for example, to abort playing on
    KGS, because <a href="http://www.gokgs.com/download.xhtml">kgsGtp.jar</a>
    quits, if a clear_board command fails. This command will remove the
    sentinel file, if it currently exists. Future invocations of clear_board
    will fail, if the sentinel file exists at that time. <br>
    Argument: filename
*/
void GoGtpEngine::CmdSentinelFile(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    path sentinelFile = path(cmd.Arg(0));
    if (! sentinelFile.empty())
        try
        {
            remove(sentinelFile);
        }
	catch (const std::exception& e)
        {
            throw GtpFailure() << "could not remove sentinel file: "
                               << e.what();
        }
    m_sentinelFile = sentinelFile;
}

/** Standard GTP command for explicit placement of handicap stones.
    Arguments: list of points
 */
void GoGtpEngine::CmdSetFreeHandicap(GtpCommand& cmd)
{
    SgVector<SgPoint> stones = PointListArg(cmd);
    if (stones.RemoveDuplicates())
        throw GtpFailure("duplicate handicap stones not allowed");
    PlaceHandicap(stones);
}

/** Set game info property in root node of internal SGF tree.
    Arguments: info value (value is remaining line after gameinfo) <br>
    Supported infos:
    - game_name
    - player_black
    - player_white
    - result
*/
void GoGtpEngine::CmdSetInfo(GtpCommand& cmd)
{
    string key = cmd.Arg(0);
    string value = cmd.RemainingLine(0);
    SgNode& root = GetGame().Root();
    if (key == "game_name")
        root.SetStringProp(SG_PROP_GAME_NAME, value);
    else if (key == "player_black")
        AddPlayerProp(SG_BLACK, value, true);
    else if (key == "player_white")
        AddPlayerProp(SG_WHITE, value, true);
    else if (key == "result")
        root.SetStringProp(SG_PROP_RESULT, value);
    AutoSave();
}

/** Place setup stones.
    Command will be used by future versions of GoGui. <br>
    Argument: color point [color point ...] <br>
    With color: b, black, w, white
*/
void GoGtpEngine::CmdSetup(GtpCommand& cmd)
{
    const GoBoard& bd = Board();
    if (bd.MoveNumber() > 0)
        throw GtpFailure("setup only allowed on empty board");
    if (cmd.NuArg() % 2 != 0)
        throw GtpFailure("need even number of arguments");
    SgBWArray<SgPointSet> points;
    for (size_t i = 0; i < cmd.NuArg(); i += 2)
    {
        SgBlackWhite c = BlackWhiteArg(cmd, i);
        SgPoint p = PointArg(cmd, i + 1);
        for (SgBWIterator it; it; ++it)
            points[*it].Exclude(p);
        points[c].Include(p);
    }
    SgPropAddStone* addBlack = new SgPropAddStone(SG_PROP_ADD_BLACK);
    SgPropAddStone* addWhite = new SgPropAddStone(SG_PROP_ADD_WHITE);
    for (SgSetIterator it(points[SG_BLACK]); it; ++it)
        if (bd.GetColor(*it) != SG_BLACK)
            addBlack->PushBack(*it);
    for (SgSetIterator it(points[SG_WHITE]); it; ++it)
        if (bd.GetColor(*it) != SG_WHITE)
            addWhite->PushBack(*it);
    GoGame& game = GetGame();
    SgNode* node = game.CurrentNode()->NewRightMostSon();
    node->Add(addBlack);
    node->Add(addWhite);
    game.GoToNode(node);
    BoardChanged();
}

/** Set color to play.
    Command will be used by future versions of GoGui. <br>
    Argument: color <br>
*/
void GoGtpEngine::CmdSetupPlayer(GtpCommand& cmd)
{
    cmd.CheckNuArg(1);
    SgBlackWhite toPlay = BlackWhiteArg(cmd, 0);
    SgNode* node = GetGame().CurrentNode();
    node->Props().RemoveProp(SG_PROP_PLAYER);
    node->Add(new SgPropPlayer(SG_PROP_PLAYER, toPlay));
    Board().SetToPlay(toPlay);
    if (m_player != 0)
        m_player->UpdateSubscriber();
    BoardChanged();
}

/** Show current position. */
void GoGtpEngine::CmdShowBoard(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    cmd << '\n' << Board();
}

/** Time of last ganmove command. */
void GoGtpEngine::CmdTimeLastMove(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    cmd << setprecision(2) << m_timeLastMove;
}

/** Standard GTP command. */
void GoGtpEngine::CmdTimeLeft(GtpCommand& cmd)
{
    cmd.CheckNuArg(3);
    SgBlackWhite color = BlackWhiteArg(cmd, 0);
    // GTP draft 2 standard does not say if time left can be negative,
    // CGOS server sends negative time, but we replace a negative time by
    // zero (not sure, if SgTimeRecord::SetTimeLeft can handle negative times)
    int timeLeft = max(0, cmd.IntArg(1));
    int movesLeft = cmd.IntArg(2, 0);
    SgTimeRecord& time = GetGame().Time();
    time.SetTimeLeft(color, timeLeft);
    time.SetMovesLeft(color, movesLeft);
}

/** Standard GTP command. */
void GoGtpEngine::CmdTimeSettings(GtpCommand& cmd)
{
    cmd.CheckNuArg(3);
    int mainTime = cmd.IntArg(0, 0);
    int byoYomiTime = cmd.IntArg(1, 0);
    int byoYomiStones = cmd.IntArg(2, 0);
    GoGtpTimeSettings timeSettings(mainTime, byoYomiTime, byoYomiStones);
    if (m_timeSettings == timeSettings)
        return;
    if (Board().MoveNumber() > 0)
        throw GtpFailure("cannot change time settings during game");
    m_timeSettings = timeSettings;
    ApplyTimeSettings();
}

/** Undo a move. */
void GoGtpEngine::CmdUndo(GtpCommand& cmd)
{
    cmd.CheckArgNone();
    Undo(1);
    BoardChanged();
}

void GoGtpEngine::CheckMoveStackOverflow() const
{
    const int RESERVE = 50;
    if (Board().MoveNumber() >= GO_MAX_NUM_MOVES - RESERVE)
        throw GtpFailure("too many moves");
    if (Board().StackOverflowLikely())
        throw GtpFailure("move stack overflow");
}

std::vector<std::string> GoGtpEngine::CreateStatisticsSlots()
{
    return vector<string>();
}

SgBlackWhite GoGtpEngine::BlackWhiteArg(const GtpCommand& cmd,
                                        std::size_t number) const
{
    return GoGtpCommandUtil::BlackWhiteArg(cmd, number);
}

void GoGtpEngine::CreateAutoSaveFileName()
{
    time_t timeValue = time(0);
    struct tm* timeStruct = localtime(&timeValue);
    char timeBuffer[128];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y%m%d%H%M%S", timeStruct);
    ostringstream fileName;
    fileName << m_autoSavePrefix << timeBuffer << ".sgf";
    m_autoSaveFileName = fileName.str();
}

void GoGtpEngine::DumpState(ostream& out) const
{
    out << "GoGtpEngine board:\n";
    GoBoardUtil::DumpBoard(Board(), out);
    if (m_player != 0)
    {
        out << "GoPlayer board:\n";
        GoBoardUtil::DumpBoard(m_player->Board(), out);
    }
}

SgEmptyBlackWhite GoGtpEngine::EmptyBlackWhiteArg(const GtpCommand& cmd,
                                                  std::size_t number) const
{
    return GoGtpCommandUtil::EmptyBlackWhiteArg(cmd, number);
}

SgPoint GoGtpEngine::EmptyPointArg(const GtpCommand& cmd,
                                   std::size_t number) const
{
    return GoGtpCommandUtil::EmptyPointArg(cmd, number, Board());
}

/** Do what is necessary when a game is finished.
    Note that since GTP allows arbitrary state changes, it is not always
    clearly defined, if a game is played and when it is finished, but
    this function should at least be ensured to be called at the end of a
    game in the use case of playing a game or a series of games.
*/
void GoGtpEngine::GameFinished()
{
    if (m_player != 0)
        m_player->OnGameFinished();
}

SgPoint GoGtpEngine::GenMove(SgBlackWhite color, bool ignoreClock)
{
    SG_ASSERT_BW(color);
    CheckMoveStackOverflow();
    StartStatistics();
    GoPlayer& player = Player();
    GoBoard& bd = Board();
    bd.SetToPlay(color);
    double startTime = SgTime::Get();
    SgTimeRecord time;
    if (ignoreClock || m_timeSettings.NoTimeLimits())
        time = SgTimeRecord(true, m_timeLimit);
    else
        time = GetGame().Time();
    AddStatistics("GAME", m_autoSaveFileName);
    AddStatistics("MOVE", GetGame().CurrentMoveNumber() + 1);
    SgPoint move = SG_NULLMOVE;
    if (m_autoBook.get() != 0)
    {
        SgDebug() << "GoGtpEngine: Checking AutoBook instead of book\n";
        move = m_autoBook->LookupMove(bd);
    }
    else 
        move = m_book.LookupMove(bd);
    m_mpiSynchronizer->SynchronizeMove(move);
    if (move != SG_NULLMOVE)
    {
        SgDebug() << "GoGtpEngine: Using move from opening book\n";
        AddStatistics("BOOK", 1);
    }
    else
        AddStatistics("BOOK", 0);
    if (move == SG_NULLMOVE)
        move = player.GenMove(time, color);
    m_mpiSynchronizer->SynchronizeMove(move);
    m_timeLastMove = SgTime::Get() - startTime;
    AddStatistics("TIME", m_timeLastMove);
    if (move == SG_NULLMOVE)
        throw GtpFailure() << player.Name() << " generated NULLMOVE";
    if (move != SG_RESIGN)
        CheckLegal(player.Name() + " generated illegal move: ", color, move,
                   false);
    AddPlayStatistics();
    SaveStatistics();
    return move;
}

void GoGtpEngine::Init(int size)
{
    m_game.Init(size, m_defaultRules);
    time_t timeValue = time(0);
    struct tm* timeStruct = localtime(&timeValue);
    char dateBuffer[128];
    strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", timeStruct);
    m_game.Root().SetStringProp(SG_PROP_DATE, dateBuffer);
    ApplyTimeSettings();
    CreateAutoSaveFileName();
}

void GoGtpEngine::InitStatistics()
{
    m_statisticsSlots.clear();
    m_statisticsSlots.push_back("GAME");
    m_statisticsSlots.push_back("MOVE");
    m_statisticsSlots.push_back("TIME");
    m_statisticsSlots.push_back("BOOK");
    vector<string> slots = CreateStatisticsSlots();
    for (vector<string>::const_iterator i = slots.begin(); i != slots.end();
         ++i)
    {
        if (i->find('\t') != string::npos)
            throw SgException("GoGtpEngine::InitStatistics: statistics slot"
                              " contains tab: '" + (*i) + "'");
        if (find(m_statisticsSlots.begin(), m_statisticsSlots.end(), *i)
            != m_statisticsSlots.end())
            throw SgException("GoGtpEngine::InitStatistics: duplicate"
                              " statistics slot '" + (*i) + "'");
        m_statisticsSlots.push_back(*i);
    }
    if (MpiSynchronizer()->IsRootProcess())
    {
    ofstream out(m_statisticsFile.c_str(), ios::app);
    // TODO: What to do with an existing file? We want a single file, if
    // twogtp or Go server experiments are interrupted and restarted, but if
    // the file is from different player, the format is not compatible.
    // For now, we simple append to the file.
    out << '#'; // Start header line with a comment character
    for (size_t i = 0; i < m_statisticsSlots.size(); ++i)
    {
        out << m_statisticsSlots[i];
        if (i < m_statisticsSlots.size() - 1)
        out << '\t';
        else
        out << '\n';
    }
    }
}

SgPoint GoGtpEngine::MoveArg(const GtpCommand& cmd, std::size_t number) const
{
    return GoGtpCommandUtil::MoveArg(cmd, number, Board());
}

void GoGtpEngine::PlaceHandicap(const SgVector<SgPoint>& stones)
{
    CheckBoardEmpty();
    GoBoard& bd = Board();
    GoGame& game = GetGame();
    SgNode* node = game.CurrentNode();
    if (node->HasSon())
        node = game.CurrentNode()->NewRightMostSon();
    SgPropAddStone* addBlack = new SgPropAddStone(SG_PROP_ADD_BLACK);
    for (SgVectorIterator<SgPoint> it(stones); it; ++it)
        addBlack->PushBack(*it);
    node->Add(addBlack);
    SgPropInt* handicap = new SgPropInt(SG_PROP_HANDICAP, stones.Length());
    node->Add(handicap);
    bd.Rules().SetHandicap(stones.Length());
    RulesChanged();
    game.GoToNode(node);
    BoardChanged();
}

void GoGtpEngine::Play(SgBlackWhite color, SgPoint move)
{
    CheckMoveStackOverflow();
    // Play "resign" is not allowed by the GTP standard (draft version 2),
    // but there could be controllers that send it (e.g. CGOS did in a
    // previous version ), so we just ignore it.
    if (move == SG_RESIGN)
        return;
    CheckLegal("illegal move: ", color, move, m_acceptIllegal);
    SgNode* node = GetGame().AddMove(move, color);
    GetGame().GoToNode(node);
}

GoPlayer& GoGtpEngine::Player() const
{
    if (m_player == 0)
        throw GtpFailure("no player set");
    return *m_player;
}

SgPoint GoGtpEngine::PointArg(const GtpCommand& cmd) const
{
    return GoGtpCommandUtil::PointArg(cmd, Board());
}

SgPoint GoGtpEngine::PointArg(const GtpCommand& cmd, std::size_t number) const
{
    return GoGtpCommandUtil::PointArg(cmd, number, Board());
}

SgVector<SgPoint> GoGtpEngine::PointListArg(const GtpCommand& cmd,
                                          std::size_t number) const
{
    return GoGtpCommandUtil::PointListArg(cmd, number, Board());
}

SgVector<SgPoint> GoGtpEngine::PointListArg(const GtpCommand& cmd) const
{
    return GoGtpCommandUtil::PointListArg(cmd, Board());
}

void GoGtpEngine::RespondNumberArray(GtpCommand& cmd,
                                     const SgPointArray<int>& array,
                                     int scale)
{
    GoGtpCommandUtil::RespondNumberArray(cmd, array, scale, Board());
}

void GoGtpEngine::RulesChanged()
{
    if (m_player != 0)
        m_player->UpdateSubscriber();
}

void GoGtpEngine::SaveGame(const std::string& fileName) const
{
    if (MpiSynchronizer()->IsRootProcess())
    {
    try
    {
        ofstream out(fileName.c_str());
        SgGameWriter writer(out);
        writer.WriteGame(GetGame().Root(), true, 0, "", 1, 19);
    }
    catch (const SgException& e)
    {
        throw GtpFailure(e.what());
    }
    }
}

void GoGtpEngine::SaveStatistics()
{
    if (MpiSynchronizer()->IsRootProcess())
    {
    if (m_statisticsFile == "")
        return;
    SG_ASSERT(m_statisticsValues.size() == m_statisticsSlots.size());
    ofstream out(m_statisticsFile.c_str(), ios::app);
    for (size_t i = 0; i < m_statisticsSlots.size(); ++i)
    {
        out << m_statisticsValues[i];
        if (i < m_statisticsSlots.size() - 1)
        out << '\t';
        else
        out << '\n';
    }
    }
}

void GoGtpEngine::SetAutoSave(const std::string& prefix)
{
    m_autoSave = true;
    m_autoSavePrefix = prefix;
    CreateAutoSaveFileName();
}

void GoGtpEngine::SetAutoShowBoard(bool showBoard)
{
    m_autoShowBoard = showBoard;
    if (m_autoShowBoard)
        SgDebug() << Board();
}

inline void GoGtpEngine::SetStatisticsFile(const std::string& fileName)
{
    m_statisticsFile = fileName;
    InitStatistics();
}

void GoGtpEngine::SetPlayer(GoPlayer* player)
{
    m_player = player;
    GetGame().SetPlayer(SG_BLACK, player);
    GetGame().SetPlayer(SG_WHITE, player);
    if (m_player != 0)
        m_player->OnNewGame();
    InitStatistics();
}

void GoGtpEngine::SetNamedRules(const string& namedRules)
{
    Board().Rules().SetNamedRules(namedRules);
    m_defaultRules.SetNamedRules(namedRules);
    RulesChanged();
}

void GoGtpEngine::StartStatistics()
{
    m_statisticsValues.clear();
    m_statisticsValues.resize(m_statisticsSlots.size(), "-");
}

SgPoint GoGtpEngine::StoneArg(const GtpCommand& cmd, std::size_t number) const
{
    return GoGtpCommandUtil::StoneArg(cmd, number, Board());
}

void GoGtpEngine::Undo(int n)
{
    SG_ASSERT(n >= 0);
    GoGame& game = GetGame();
    SgNode* node = game.CurrentNode();
    for (int i = 0; i < n; ++i)
    {
        if (! node->HasNodeMove() || ! node->HasFather())
            throw GtpFailure() << "cannot undo " << n << " move(s)";
        node = node->Father();
    }
    game.GoToNode(node);
}

/** Write board info.
    Optional arguments:
    - countplay
*/
void GoGtpEngine::WriteBoardInfo(GtpCommand& cmd, const GoBoard& bd)
{
    cmd.CheckNuArgLessEqual(1);
    if (cmd.NuArg() == 1)
    {
        string arg = cmd.Arg(0);
        if (arg == "countplay")
            cmd << bd.CountPlay();
        else
            throw GtpFailure() << "unknown argument " << arg;
        return;
    }
    cmd << "Board:\n"
        << SgWriteLabel("Hash") << bd.GetHashCode() << '\n'
        << SgWriteLabel("HashToPlay") << bd.GetHashCodeInclToPlay() << '\n'
        << SgWriteLabel("KoColor") << SgEBW(bd.KoColor()) << '\n'
        << SgWriteLabel("MoveNumber") << bd.MoveNumber() << '\n'
        << SgWriteLabel("NumStones[B]") << bd.TotalNumStones(SG_BLACK) << '\n'
        << SgWriteLabel("NumStones[W]") << bd.TotalNumStones(SG_WHITE) << '\n'
        << SgWriteLabel("NumEmpty") << bd.TotalNumEmpty() << '\n'
        << SgWriteLabel("ToPlay") << SgBW(bd.ToPlay()) << '\n'
        << SgWriteLabel("CountPlay") << bd.CountPlay() << '\n'
        << "Sets:\n"
        << SgWritePointSet(bd.AllPoints(), "AllPoints")
        << SgWritePointSet(bd.All(SG_BLACK), "AllBlack")
        << SgWritePointSet(bd.All(SG_WHITE), "AllWhite")
        << SgWritePointSet(bd.AllEmpty(), "AllEmpty")
        << SgWritePointSet(bd.Corners(), "Corners")
        << SgWritePointSet(bd.Edges(), "Edges")
        << SgWritePointSet(bd.Centers(), "Centers")
        << SgWritePointSet(bd.SideExtensions(), "SideExtensions")
        << SgWritePointSet(bd.Occupied(), "Occupied");
}

#if GTPENGINE_PONDER

void GoGtpEngine::Ponder()
{
    if (m_player == 0)
        return;
    // Call GoPlayer::Ponder() after 0.2 seconds delay to avoid calls in very
    // short intervals between received commands
    boost::xtime time;
    boost::xtime_get(&time, boost::TIME_UTC_);
    bool aborted = false;
    for (int i = 0; i < 200; ++i)
    {
        if (SgUserAbort())
        {
            aborted = true;
                break;
        }
        time.nsec += 1000000; // 1 msec
        boost::thread::sleep(time);
    }
    m_mpiSynchronizer->SynchronizeUserAbort(aborted);
    if (! aborted)
    {
        m_mpiSynchronizer->OnStartPonder();
        m_player->Ponder();
        m_mpiSynchronizer->OnEndPonder();
    }
}

void GoGtpEngine::StopPonder()
{
    SgSetUserAbort(true);
}

void GoGtpEngine::InitPonder()
{
    SgSetUserAbort(false);
}

#endif // GTPENGINE_PONDER

#if GTPENGINE_INTERRUPT

void GoGtpEngine::Interrupt()
{
    SgSetUserAbort(true);
}

#endif // GTPENGINE_INTERRUPT

void GoGtpEngine::SetMpiSynchronizer(const SgMpiSynchronizerHandle &handle)
{
    m_mpiSynchronizer = SgMpiSynchronizerHandle(handle);
}

SgMpiSynchronizerHandle GoGtpEngine::MpiSynchronizer()
{
    return SgMpiSynchronizerHandle(m_mpiSynchronizer);
}

const SgMpiSynchronizerHandle GoGtpEngine::MpiSynchronizer() const
{
    return SgMpiSynchronizerHandle(m_mpiSynchronizer);
}


//----------------------------------------------------------------------------

GoGtpAssertionHandler::GoGtpAssertionHandler(const GoGtpEngine& engine)
    : m_engine(engine)
{
}

void GoGtpAssertionHandler::Run()
{
    m_engine.DumpState(SgDebug());
    SgDebug() << flush;
}

//----------------------------------------------------------------------------
