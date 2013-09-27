//----------------------------------------------------------------------------
/** @file GoUctGlobalSearch.h
 */
//----------------------------------------------------------------------------

#ifndef GOUCT_GLOBALSEARCH_H
#define GOUCT_GLOBALSEARCH_H

#include <boost/scoped_ptr.hpp>
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoEyeUtil.h"
#include "GoRegionBoard.h"
#include "GoSafetySolver.h"
#include "GoUctDefaultPriorKnowledge.h"
#include "GoUctSearch.h"
#include "GoUctUtil.h"

//----------------------------------------------------------------------------

/** Enable the usage of the safety solver (currently not functional).
    Compile-time constant to enable the usage of the safety solver. This
    cost some performance and there are no experiments yet showing that it
    improves the playing strength. It is also currently not functional,
    since it has not been updated after code changes.
*/
const bool GOUCT_USE_SAFETY_SOLVER = false;

//----------------------------------------------------------------------------

/** Parameters for GoUctGlobalSearchState */
struct GoUctGlobalSearchStateParam
{
  /** Use the mercy rule.
      Count games early as win or loss, if stone difference on board
      exceeds a threshold of 30% of the total number of points on board.
  */
  bool m_mercyRule;

  /** Compute probabilities of territory in terminal positions. */
  bool m_territoryStatistics;

  /** Modify game result by the length of the simulation.
      This modifies the win/loss result (1/0) by the length of the game
      counted as number of moves from the root position of the search. This
      can help to prefer shorter games (if it is a win) or longer games (if
      it is a loss). It can also have a positive effect on the playing
      strength, because the variance of results is larger for longer games.
      The modification is added for losses and subtracted for wins. The
      value is the length of times the value of the modification parameter.
      The maximum modification is 0.5. The default value of the parameter
      is 0.
  */
  float m_lengthModification;

  /** Modify game result by score.
      This modifies the win/loss result (1/0) by the score of the end
      position. The modification is added for losses and subtracted for
      wins. The modification value is the score divided by the maximum
      score, which can be reached on the board, times the value of the score
      modification parameter. This helps to play moves to maximize the score
      even if the game is already clearly lost or won. Otherwise all moves
      look equal in clearly won or lost positions. It can also reduce the
      typical game length and could even have a positive effect on the
      playing strength. The modification can be disabled by setting the
      parameter to zero. The default value is 0.02.
  */
  float m_scoreModification;

  GoUctGlobalSearchStateParam();
};

//----------------------------------------------------------------------------

/** @page gouctpassmoves Handling of pass-moves in GoUctGlobalSearch

    @section gouctpassplayout Play-out phase

    Pass moves are not played in the play-out phase of the simulations as
    long as there are still moves for which GoUctUtil::GeneratePoint
    returns true, which are mainly moves that don't fill single point
    eyes (see GoUctUtil::GeneratePoint and GoBoardUtil::IsCompletelySurrounded
    for an exact definition). Therefore, it is a requirement on
    GoUctPlayoutPolicy::GenerateMove, not to return pass moves earlier.

    This requirement ensures that all simulated games terminate (as long as
    there is no super-ko cycle, because for speed reasons only simple ko's
    are checked) and that the terminal position after two passes in a row is
    a position that can be quickly evaluated with
    GoBoardUtil::ScoreSimpleEndPosition.

    @section gouctpassintree In-tree phase

    In the in-tree-phase of the game, pass moves are always allowed to avoid
    zugzwang situations, if there is a seki on the board. After two
    passes the game is terminated and scored with
    GoBoardUtil::TrompTaylorScore.
*/

/** Global UCT-Search for Go.
    - @ref gouctpassmoves
    @tparam POLICY The playout policy
*/
template<class POLICY>
class GoUctGlobalSearchState
: public GoUctState
{
 public:
  const SgBWSet& m_safe;

  const SgPointArray<bool>& m_allSafe;

  /** Probabilities that a point belongs to Black in a terminal position.
      Only computed if GoUctGlobalSearchStateParam::m_territoryStatistics.
  */
  SgPointArray<SgUctStatistics> m_territoryStatistics;

  /** Constructor.
      @param threadId The number of the thread. Needed for passing to
      constructor of SgUctThreadState.
      @param bd The board
      @param policy The random policy (takes ownership). It is possible to
      set the policy to null at construction time to allowed a multi-step
      construction; but then a policy has to be set with SetPolicy(), before
      the search is used.
      @param param Parameters. Stores a reference to the argument.
      @param policy_param. Stores a reference to the argument.
      @param safe Safety information. Stores a reference to the argument.
      @param allSafe Safety information. Stores a reference to the argument.
  */
  GoUctGlobalSearchState(std::size_t threadId, const GoBoard& bd,
                         POLICY* policy,
                         const GoUctGlobalSearchStateParam& param,
                         const GoUctPlayoutPolicyParam& policyParam,
                         const SgBWSet& safe,
                         const SgPointArray<bool>& allSafe);

  ~GoUctGlobalSearchState();

  float Evaluate();

  bool GenerateAllMoves(std::size_t count, std::vector<SgMoveInfo>& moves,
                        SgProvenNodeType& provenType);

  SgMove GeneratePlayoutMove(bool& skipRaveUpdate);

  void ExecutePlayout(SgMove move);

  void GameStart();

  void EndPlayout();

  void StartPlayout();

  void StartPlayouts();

  void StartSearch();

  POLICY* Policy();

  /** Set random policy.
      Sets a new random policy and deletes the old one, if it existed.
  */
  void SetPolicy(POLICY* policy);

  void ClearTerritoryStatistics();

 private:
  const GoUctGlobalSearchStateParam& m_param;

  const GoUctPlayoutPolicyParam& m_policyParam;

  /** See SetMercyRule() */
  bool m_mercyRuleTriggered;

  /** Number of pass moves played in a row in the playout phase. */
  int m_passMovesPlayoutPhase;

  /** See SetMercyRule() */
  int m_mercyRuleThreshold;

  /** Difference of stones on board.
      Black counts positive.
  */
  int m_stoneDiff;

  /** Board move number at root node of search. */
  int m_initialMoveNumber;

  /** The area in which moves should be generated. */
  GoPointList m_area;

  /** See SetMercyRule() */
  float m_mercyRuleResult;

  /** Inverse of maximum score one can reach on a board of the current
      size.
  */
  float m_invMaxScore;

  SgRandom m_random;

  GoUctDefaultPriorKnowledge m_priorKnowledge;

  boost::scoped_ptr<POLICY> m_policy;

  /** Not implemented */
  GoUctGlobalSearchState(const GoUctGlobalSearchState& search);

  /** Not implemented */
  GoUctGlobalSearchState& operator=(const GoUctGlobalSearchState& search);

  bool CheckMercyRule();

  template<class BOARD>
    float EvaluateBoard(const BOARD& bd, float komi);

  /** Generates all legal moves with no knowledge values. */
  void GenerateLegalMoves(std::vector<SgMoveInfo>& moves);

  float GetKomi() const;
};

template<class POLICY>
GoUctGlobalSearchState<POLICY>::GoUctGlobalSearchState(std::size_t threadId,
                                                       const GoBoard& bd, POLICY* policy,
                                                       const GoUctGlobalSearchStateParam& param,
                                                       const GoUctPlayoutPolicyParam& policyParam,
                                                       const SgBWSet& safe, const SgPointArray<bool>& allSafe)
: GoUctState(threadId, bd),
  m_safe(safe),
  m_allSafe(allSafe),
  m_param(param),
  m_policyParam(policyParam),
  m_priorKnowledge(Board(), m_policyParam),
  m_policy(policy)
{
  ClearTerritoryStatistics();
}

template<class POLICY>
GoUctGlobalSearchState<POLICY>::~GoUctGlobalSearchState()
{
}

/** See SetMercyRule() */
template<class POLICY>
bool GoUctGlobalSearchState<POLICY>::CheckMercyRule()
{
  SG_ASSERT(m_param.m_mercyRule);
  // Only used in playout; m_stoneDiff only defined in playout
  SG_ASSERT(IsInPlayout());
  if (m_stoneDiff >= m_mercyRuleThreshold)
    {
      m_mercyRuleTriggered = true;
      m_mercyRuleResult = (UctBoard().ToPlay() == SG_BLACK ? 1 : 0);
    }
  else if (m_stoneDiff <= -m_mercyRuleThreshold)
    {
      m_mercyRuleTriggered = true;
      m_mercyRuleResult = (UctBoard().ToPlay() == SG_WHITE ? 1 : 0);
    }
  else
    SG_ASSERT(! m_mercyRuleTriggered);
  return m_mercyRuleTriggered;
}

template<class POLICY>
void GoUctGlobalSearchState<POLICY>::ClearTerritoryStatistics()
{
  for (SgPointArray<SgUctStatistics>::NonConstIterator
         it(m_territoryStatistics); it; ++it)
    (*it).Clear();
}

template<class POLICY>
void GoUctGlobalSearchState<POLICY>::EndPlayout()
{
  GoUctState::EndPlayout();
  m_policy->EndPlayout();
}

template<class POLICY>
float GoUctGlobalSearchState<POLICY>::Evaluate()
{
  float komi = GetKomi();
  if (IsInPlayout())
    return EvaluateBoard(UctBoard(), komi);
  else
    return EvaluateBoard(Board(), komi);
}

template<class POLICY>
template<class BOARD>
float GoUctGlobalSearchState<POLICY>::EvaluateBoard(const BOARD& bd,
                                                    float komi)
{
  float score;
  SgPointArray<SgEmptyBlackWhite> scoreBoard;
  SgPointArray<SgEmptyBlackWhite>* scoreBoardPtr;
  if (m_param.m_territoryStatistics)
    scoreBoardPtr = &scoreBoard;
  else
    scoreBoardPtr = 0;
  if (m_passMovesPlayoutPhase < 2)
    // Two passes not in playout phase, see comment in GenerateAllMoves()
    score = GoBoardUtil::TrompTaylorScore(bd, komi, scoreBoardPtr);
  else
    {
      if (m_param.m_mercyRule && m_mercyRuleTriggered)
        return m_mercyRuleResult;
      score = GoBoardUtil::ScoreSimpleEndPosition(bd, komi, m_safe,
                                                  false, scoreBoardPtr);
    }
  if (m_param.m_territoryStatistics)
    for (typename BOARD::Iterator it(bd); it; ++it)
      switch (scoreBoard[*it])
        {
        case SG_BLACK:
          m_territoryStatistics[*it].Add(1);
          break;
        case SG_WHITE:
          m_territoryStatistics[*it].Add(0);
          break;
        case SG_EMPTY:
          m_territoryStatistics[*it].Add(0.5);
          break;
        }
  if (bd.ToPlay() != SG_BLACK)
    score *= -1;
  float lengthMod = std::min(GameLength() * m_param.m_lengthModification, 0.5f);
  if (score > std::numeric_limits<float>::epsilon())
    return
      (1 - m_param.m_scoreModification)
      + m_param.m_scoreModification * score * m_invMaxScore
      - lengthMod;
  else if (score < -std::numeric_limits<float>::epsilon())
    return
      m_param.m_scoreModification
      + m_param.m_scoreModification * score * m_invMaxScore
      + lengthMod;
  else
    // Draw. Can happen if komi is an integer
    return 0;
}

template<class POLICY>
void GoUctGlobalSearchState<POLICY>::ExecutePlayout(SgMove move)
{
  GoUctState::ExecutePlayout(move);
  const GoUctBoard& bd = UctBoard();
  if (bd.ToPlay() == SG_BLACK)
    m_stoneDiff -= bd.NuCapturedStones();
  else
    m_stoneDiff += bd.NuCapturedStones();
  m_policy->OnPlay();
}

template<class POLICY>
void GoUctGlobalSearchState<POLICY>::GameStart()
{
  GoUctState::GameStart();
}

template<class POLICY>
void GoUctGlobalSearchState<POLICY>::GenerateLegalMoves(
                                                        std::vector<SgMoveInfo>& moves)
{
  //SG_ASSERT(! IsInPlayout());
  const GoBoard& bd = Board();
  SG_ASSERT(! bd.Rules().AllowSuicide());

  if (GoBoardUtil::TwoPasses(bd))
    // Evaluate with Tromp-Taylor (we have no other evaluation that can
    // score arbitrary positions). However, if the rules don't require
    // CaptureDead(), the two passes need to be played in the search
    // sequence. This avoids cases, in which playing a pass (after the
    // opponent's last move in the real game was a pass) is only good
    // under Tromp-Taylor scoring (see
    // regression/sgf/pass/tromp-taylor-pass.sgf).
    // Both won't work in Japanese rules, but it is not easy to define
    // what a terminal position is in Japanese rules.
    if (bd.Rules().CaptureDead()
        || bd.MoveNumber() - m_initialMoveNumber >= 2)
      return;

  SgBlackWhite toPlay = bd.ToPlay();
  for (GoBoard::Iterator it(bd); it; ++it)
    {
      SgPoint p = *it;
      if (bd.IsEmpty(p)
          && ! GoEyeUtil::IsSimpleEye(bd, p, toPlay)
          && ! m_allSafe[p]
          && bd.IsLegal(p, toPlay))
        moves.push_back(SgMoveInfo(p));
    }

  // Full randomization is too expensive and in most cases not necessary,
  // if prior knowledge is available for initialization or RAVE values are
  // available after playing the first move. However we put a random move
  // to the front, because the first move in a Go board iteration is often
  // a bad corner move
  if (moves.size() > 1)
    std::swap(moves[0], moves[m_random.Int(moves.size())]);
  moves.push_back(SgMoveInfo(SG_PASS));
}

template<class POLICY>
bool GoUctGlobalSearchState<POLICY>::GenerateAllMoves(std::size_t count,
                                                      std::vector<SgMoveInfo>& moves,
                                                      SgProvenNodeType& provenType)
{
  provenType = SG_NOT_PROVEN;
  moves.clear();  // FIXME: needed?
  GenerateLegalMoves(moves);
  if (! moves.empty())
    {
      if (count == 0)
        m_priorKnowledge.ProcessPosition(moves);
    }
  return false;
}

template<class POLICY>
SgMove GoUctGlobalSearchState<POLICY>::GeneratePlayoutMove(
                                                           bool& skipRaveUpdate)
{
  SG_ASSERT(IsInPlayout());
  if (m_param.m_mercyRule && CheckMercyRule())
    return SG_NULLMOVE;
  SgPoint move = m_policy->GenerateMove();
  SG_ASSERT(move != SG_NULLMOVE);
#ifndef NDEBUG
  // Check that policy generates pass only if no points are left for which
  // GeneratePoint() returns true. See GoUctPlayoutPolicy::GenerateMove()
  if (move == SG_PASS)
    {
      const GoUctBoard& bd = UctBoard();
      SgBalancer balancer(100); // FIXME: Could be quite slow!!
      for (GoUctBoard::Iterator it(bd); it; ++it)
        SG_ASSERT(  bd.Occupied(*it)
                    || m_safe.OneContains(*it)
                    || GoBoardUtil::SelfAtari(bd, *it)
                    || ! GoUctUtil::GeneratePoint(bd, balancer,
                                                  *it, bd.ToPlay())
                    );
    }
  else
    SG_ASSERT(! m_safe.OneContains(move));
#endif
  // The position guaranteed to be a terminal position, which can be
  // evaluated with GoBoardUtil::ScoreSimpleEndPosition(), only after two
  // passes in a row, both of them generated by GeneratePlayoutMove() in
  // the playout phase
  if (move == SG_PASS)
    {
      skipRaveUpdate = true; // Don't update RAVE values for pass moves
      if (m_passMovesPlayoutPhase < 2)
        ++m_passMovesPlayoutPhase;
      else
        return SG_NULLMOVE;
    }
  else
    m_passMovesPlayoutPhase = 0;
  return move;
}

/** Get komi including extra handicap komi points, if used by the rules. */
template<class POLICY>
float GoUctGlobalSearchState<POLICY>::GetKomi() const
{
  const GoRules& rules = Board().Rules();
  float komi = rules.Komi().ToFloat();
  if (rules.ExtraHandicapKomi())
    komi += rules.Handicap();
  return komi;
}

template<class POLICY>
inline POLICY* GoUctGlobalSearchState<POLICY>::Policy()
{
  return m_policy.get();
}

template<class POLICY>
void GoUctGlobalSearchState<POLICY>::SetPolicy(POLICY* policy)
{
  m_policy.reset(policy);
}

template<class POLICY>
void GoUctGlobalSearchState<POLICY>::StartPlayout()
{
  GoUctState::StartPlayout();
  m_passMovesPlayoutPhase = 0;
  m_mercyRuleTriggered = false;
  const GoBoard& bd = Board();
  m_stoneDiff = bd.All(SG_BLACK).Size() - bd.All(SG_WHITE).Size();
  m_policy->StartPlayout();
}

template<class POLICY>
void GoUctGlobalSearchState<POLICY>::StartPlayouts()
{
  GoUctState::StartPlayouts();
}

template<class POLICY>
void GoUctGlobalSearchState<POLICY>::StartSearch()
{
  GoUctState::StartSearch();
  const GoBoard& bd = Board();
  int size = bd.Size();
  float maxScore = size * size + GetKomi();
  m_invMaxScore = 1 / maxScore;
  m_initialMoveNumber = bd.MoveNumber();
  m_mercyRuleThreshold = static_cast<int>(0.3 * size * size);
  ClearTerritoryStatistics();
}

//----------------------------------------------------------------------------

/** Factory for creating a GoUctGlobalSearchState.
    @tparam POLICY The playout policy
    @tparam FACTORY The factory for the playout policy
*/
template<class POLICY, class FACTORY>
  class GoUctGlobalSearchStateFactory
  : public SgUctThreadStateFactory
{
 public:
  /** Constructor.
      @param bd
      @param playoutPolicyFactory Factory for playout policies.
      Stores a reference. Lifetime of parameter must exceed the lifetime of
      this instance.
      @param safe
      @param allSafe
  */
  GoUctGlobalSearchStateFactory(GoBoard& bd,
                                FACTORY& playoutPolicyFactory,
                                const GoUctPlayoutPolicyParam& policyParam,
                                const SgBWSet& safe,
                                const SgPointArray<bool>& allSafe);

  SgUctThreadState* Create(std::size_t threadId, const SgUctSearch& search);

 private:
  GoBoard& m_bd;

  FACTORY& m_playoutPolicyFactory;

  const GoUctPlayoutPolicyParam& m_policyParam;

  const SgBWSet& m_safe;

  const SgPointArray<bool>& m_allSafe;
};

template<class POLICY, class FACTORY>
  GoUctGlobalSearchStateFactory<POLICY,FACTORY>
  ::GoUctGlobalSearchStateFactory(GoBoard& bd,
                                  FACTORY& playoutPolicyFactory,
                                  const GoUctPlayoutPolicyParam& policyParam,
                                  const SgBWSet& safe,
                                  const SgPointArray<bool>& allSafe)
  : m_bd(bd),
  m_playoutPolicyFactory(playoutPolicyFactory),
  m_policyParam(policyParam),
  m_safe(safe),
  m_allSafe(allSafe)
  {
  }

//----------------------------------------------------------------------------

/** Full board Monte-Carlo tree search.
    @tparam POLICY The playout policy
    @tparam FACTORY The factory for the playout policy
*/
template<class POLICY, class FACTORY>
  class GoUctGlobalSearch
  : public GoUctSearch
{
 public:
  GoUctGlobalSearchStateParam m_param;

  /** Constructor.
      @param bd
      @param playoutPolicyFactory Creates multiple instances of the playout
      policies. Takes ownership. playoutPolicyFactory should not have
      SetSafe() and SetAllSafe() already set, because the search will call
      these functions using its own safety information.
  */
  GoUctGlobalSearch(GoBoard& bd,
                    FACTORY* playoutPolicyFactory,
                    const GoUctPlayoutPolicyParam& policyParam);

  /** @name Pure virtual functions of SgUctSearch */
  // @{

  float UnknownEval() const;

  // @} // @name


  /** @name Virtual functions of SgUctSearch */
  // @{

  void OnStartSearch();

  void OnSearchIteration(std::size_t gameNumber, int threadId,
                         const SgUctGameInfo& info);

  // @} // @name


  /** Set default search parameters optimized for a board size. */
  void SetDefaultParameters(int boardSize);

  /** Output live graphics commands for GoGui.
      Similar to the GOUCT_LIVEGFX_COUNTS mode in GoUctSearch, but the
      influence data shows the terriroy statistics (which must be enabled)
      instead of the move values. No move counts are shown.
      The live graphics interval from GoUctSearch will be used.
      @see GoUctSearch::LiveGfxInterval()
  */
  bool GlobalSearchLiveGfx() const;

  /** See GlobalSearchLiveGfx() */
  void SetGlobalSearchLiveGfx(bool enable);

 private:
  SgBWSet m_safe;

  SgPointArray<bool> m_allSafe;

  boost::scoped_ptr<FACTORY> m_playoutPolicyFactory;

  GoRegionBoard m_regions;

  /** See GlobalSearchLiveGfx() */
  bool m_globalSearchLiveGfx;
};

template<class POLICY, class FACTORY>
  GoUctGlobalSearch<POLICY,FACTORY>::GoUctGlobalSearch(GoBoard& bd,
                                                       FACTORY* playoutFactory,
                                                       const GoUctPlayoutPolicyParam& policyParam)
  : GoUctSearch(bd, 0),
  m_playoutPolicyFactory(playoutFactory),
  m_regions(bd),
  m_globalSearchLiveGfx(GOUCT_LIVEGFX_NONE)
  {
    SgUctThreadStateFactory* stateFactory =
      new GoUctGlobalSearchStateFactory<POLICY,FACTORY>(bd,
                                                        *playoutFactory,
                                                        policyParam,
                                                        m_safe, m_allSafe);
    SetThreadStateFactory(stateFactory);
    SetDefaultParameters(bd.Size());
  }

template<class POLICY, class FACTORY>
  inline bool GoUctGlobalSearch<POLICY,FACTORY>::GlobalSearchLiveGfx() const
{
  return m_globalSearchLiveGfx;
}

template<class POLICY, class FACTORY>
  void GoUctGlobalSearch<POLICY,FACTORY>::OnSearchIteration(
                                                            std::size_t gameNumber,
                                                            int threadId,
                                                            const SgUctGameInfo& info)
{
  GoUctSearch::OnSearchIteration(gameNumber, threadId, info);
  if (m_globalSearchLiveGfx && threadId == 0
      && gameNumber % LiveGfxInterval() == 0)
    {
      const GoUctGlobalSearchState<POLICY>& state =
        dynamic_cast<GoUctGlobalSearchState<POLICY>&>(ThreadState(0));
      SgDebug() << "gogui-gfx:\n";
      GoUctUtil::GfxBestMove(*this, ToPlay(), SgDebug());
      GoUctUtil::GfxTerritoryStatistics(state.m_territoryStatistics,
                                        Board(), SgDebug());
      GoUctUtil::GfxStatus(*this, SgDebug());
      SgDebug() << '\n';
    }
}

template<class POLICY, class FACTORY>
  void GoUctGlobalSearch<POLICY,FACTORY>::OnStartSearch()
{
  GoUctSearch::OnStartSearch();
  m_safe.Clear();
  m_allSafe.Fill(false);
  if (GOUCT_USE_SAFETY_SOLVER)
    {
      GoBoard& bd = Board();
      GoSafetySolver solver(bd, &m_regions);
      solver.FindSafePoints(&m_safe);
      for (GoBoard::Iterator it(bd); it; ++it)
        m_allSafe[*it] = m_safe.OneContains(*it);
    }
  if (m_globalSearchLiveGfx && ! m_param.m_territoryStatistics)
    SgWarning() <<
      "GoUctGlobalSearch: "
      "live graphics need territory statistics enabled\n";
}

template<class POLICY, class FACTORY>
  void GoUctGlobalSearch<POLICY,FACTORY>::SetDefaultParameters(int boardSize)
{
  SetFirstPlayUrgency(1);
  SetMoveSelect(SG_UCTMOVESELECT_COUNT);
  SetRave(true);
  SetExpandThreshold(1);
  SetVirtualLoss(true);
  SetBiasTermConstant(0.0);
  if (boardSize < 15)
    {
      // These parameters were mainly tested on 9x9
      // using GoUctPlayoutPolicy and GoUctDefaultPriorKnowledge
      SetRaveWeightInitial(1.0);
      SetRaveWeightFinal(5000);
      m_param.m_lengthModification = 0;
    }
  else
    {
      // These parameters were mainly tested on 19x19
      // using GoUctPlayoutPolicy and GoUctDefaultPriorKnowledge
      SetRaveWeightInitial(0.9);
      SetRaveWeightFinal(5000);
      m_param.m_lengthModification = 0.00028;
    }
}

template<class POLICY, class FACTORY>
  inline void GoUctGlobalSearch<POLICY,FACTORY>::SetGlobalSearchLiveGfx(
                                                                        bool enable)
{
  m_globalSearchLiveGfx = enable;
}

template<class POLICY, class FACTORY>
  float GoUctGlobalSearch<POLICY,FACTORY>::UnknownEval() const
{
  // Note: 0.5 is not a possible value for a Bernoulli variable, better
  // use 0?
  return 0.5;
}

//----------------------------------------------------------------------------

template<class POLICY, class FACTORY>
  SgUctThreadState* GoUctGlobalSearchStateFactory<POLICY,FACTORY>::Create(
                                                                          std::size_t threadId, const SgUctSearch& search)
{
  const GoUctGlobalSearch<POLICY,FACTORY>& globalSearch =
    dynamic_cast<const GoUctGlobalSearch<POLICY,FACTORY>&>(search);
  GoUctGlobalSearchState<POLICY>* state =
    new GoUctGlobalSearchState<POLICY>(threadId, globalSearch.Board(), 0,
                                       globalSearch.m_param,
                                       m_policyParam,
                                       m_safe, m_allSafe);
  POLICY* policy = m_playoutPolicyFactory.Create(state->UctBoard());
  state->SetPolicy(policy);
  return state;
}

//----------------------------------------------------------------------------

#endif // GOUCT_GLOBALSEARCH_H
