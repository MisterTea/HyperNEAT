//----------------------------------------------------------------------------
/** @file GoUctCommands.h
*/
//----------------------------------------------------------------------------

#ifndef GOUCT_COMMANDS_H
#define GOUCT_COMMANDS_H

#include <string>
#include "GtpEngine.h"
#include "GoUctPlayoutPolicy.h"
#include "GoUctGlobalSearch.h"
#include "GoUctPlayer.h"

class GoBoard;
class GoPlayer;
class GoUctBoard;
class GoUctSearch;

//----------------------------------------------------------------------------

/** GTP commands for GoUctPlayer.
    Some of the commands are also usable for other players, as long as they
    use a subclass of GoUctSearch (and implement GoUctObjectWithSearch).
*/
class GoUctCommands
{
public:
    /** Constructor.
        @param bd The game board.
        @param player Reference to pointer to current player, this player can
        be null or a different player, but those commands of this class that
        need a GoUctPlayer will fail, if the current player is not
        GoUctPlayer.
    */
    GoUctCommands(GoBoard& bd, GoPlayer*& player);

    void AddGoGuiAnalyzeCommands(GtpCommand& cmd);

    /** @page gouctgtpcommands GoUctCommands Commands
        - @link CmdFinalScore() @c final_score @endlink
        - @link CmdFinalStatusList() @c final_status_list @endlink
        - @link CmdBounds() @c uct_bounds @endlink
        - @link CmdDefaultPolicy() @c uct_default_policy @endlink
        - @link CmdEstimatorStat() @c uct_estimator_stat @endlink
        - @link CmdGfx() @c uct_gfx @endlink
        - @link CmdMaxMemory() @c uct_max_memory @endlink
        - @link CmdMoves() @c uct_moves @endlink
        - @link CmdParamGlobalSearch() @c uct_param_globalsearch @endlink
        - @link CmdParamPolicy() @c uct_param_policy @endlink
        - @link CmdParamPlayer() @c uct_param_player @endlink
        - @link CmdParamRootFilter() @c uct_param_rootfilter @endlink
        - @link CmdParamSearch() @c uct_param_search @endlink
        - @link CmdPatterns() @c uct_patterns @endlink
        - @link CmdPolicyMoves() @c uct_policy_moves @endlink
        - @link CmdPriorKnowledge() @c uct_prior_knowledge @endlink
        - @link CmdRaveValues() @c uct_rave_values @endlink
        - @link CmdRootFilter() @c uct_root_filter @endlink
        - @link CmdSaveGames() @c uct_savegames @endlink
        - @link CmdSaveTree() @c uct_savetree @endlink
        - @link CmdSequence() @c uct_sequence @endlink
        - @link CmdScore() @c uct_score @endlink
        - @link CmdStatPlayer() @c uct_stat_player @endlink
        - @link CmdStatPlayerClear() @c uct_stat_player_clear @endlink
        - @link CmdStatPolicy() @c uct_stat_policy @endlink
        - @link CmdStatPolicyClear() @c uct_stat_policy_clear @endlink
        - @link CmdStatSearch() @c uct_stat_search @endlink
        - @link CmdStatTerritory() @c uct_stat_territory @endlink
        - @link CmdValue() @c uct_value @endlink
        - @link CmdValueBlack() @c uct_value_black @endlink
    */
    /** @name Command Callbacks */
    // @{
    // The callback functions are documented in the cpp file
    void CmdBounds(GtpCommand& cmd);
    void CmdDefaultPolicy(GtpCommand& cmd);
    void CmdEstimatorStat(GtpCommand& cmd);
    void CmdFinalScore(GtpCommand&);
    void CmdFinalStatusList(GtpCommand&);
    void CmdGfx(GtpCommand& cmd);
    void CmdMaxMemory(GtpCommand& cmd);
    void CmdMoves(GtpCommand& cmd);
    void CmdParamGlobalSearch(GtpCommand& cmd);
    void CmdParamPolicy(GtpCommand& cmd);
    void CmdParamPlayer(GtpCommand& cmd);
    void CmdParamRootFilter(GtpCommand& cmd);
    void CmdParamSearch(GtpCommand& cmd);
    void CmdPatterns(GtpCommand& cmd);
    void CmdPolicyMoves(GtpCommand& cmd);
    void CmdPriorKnowledge(GtpCommand& cmd);
    void CmdRaveValues(GtpCommand& cmd);
    void CmdRootFilter(GtpCommand& cmd);
    void CmdSaveGames(GtpCommand& cmd);
    void CmdSaveTree(GtpCommand& cmd);
    void CmdScore(GtpCommand& cmd);
    void CmdSequence(GtpCommand& cmd);
    void CmdStatPlayer(GtpCommand& cmd);
    void CmdStatPlayerClear(GtpCommand& cmd);
    void CmdStatPolicy(GtpCommand& cmd);
    void CmdStatPolicyClear(GtpCommand& cmd);
    void CmdStatSearch(GtpCommand& cmd);
    void CmdStatTerritory(GtpCommand& cmd);
    void CmdValue(GtpCommand& cmd);
    void CmdValueBlack(GtpCommand& cmd);
    // @} // @name

    void Register(GtpEngine& engine);

private:
    GoBoard& m_bd;

    GoPlayer*& m_player;

    SgPointSet DoFinalStatusSearch();

    GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
                      GoUctPlayoutPolicyFactory<GoUctBoard> >&
        GlobalSearch();

    GoUctPlayer<GoUctGlobalSearch<GoUctPlayoutPolicy<GoUctBoard>,
        GoUctPlayoutPolicyFactory<GoUctBoard> >,
        GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> > >&
        Player();

    GoUctPlayoutPolicy<GoUctBoard>& Policy(std::size_t threadId);

    void Register(GtpEngine& e, const std::string& command,
                  GtpCallback<GoUctCommands>::Method method);

    GoUctSearch& Search();

    GoUctGlobalSearchState<GoUctPlayoutPolicy<GoUctBoard> >&
    ThreadState(std::size_t threadId);
};

//----------------------------------------------------------------------------

#endif // GOUCT_COMMANDS_H
