//----------------------------------------------------------------------------
/** @file SgUctSearch.h
    Class SgUctSearch and helper classes.
*/
//----------------------------------------------------------------------------

#ifndef SG_UCTSEARCH_H
#define SG_UCTSEARCH_H

#include <fstream>
#include <vector>
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/thread/thread.hpp>
#include "SgBlackWhite.h"
#include "SgBWArray.h"
#include "SgTimer.h"
#include "SgUctTree.h"
#include "SgMpiSynchronizer.h"

#define SG_UCTFASTLOG 1
#if SG_UCTFASTLOG
#include "SgFastLog.h"
#endif

//----------------------------------------------------------------------------

/** @defgroup sguctgroup Monte Carlo tree search
    Game-independent Monte Carlo tree search using UCT.

    The main class SgUctSearch keeps a tree with statistics for each node
    visited more than a certain number of times, and then continues with
    random playout (not necessarily uniform random).
    Within the tree, the move with the highest upper confidence bound is
    chosen according to the basic UCT formula:
    @f[ \bar{X}_j + c \sqrt{\frac{\log{n}}{T_j(n)}} @f]
    with:
    - @f$ j @f$ the move index
    - @f$ X_{j,\gamma} @f$ reward for move @f$ j @f$ at sample @f$ \gamma @f$
    - @f$ n @f$ number of times the father node was visited
    - @f$ T_j(n) @f$ number of times the move has been played
    - @f$ c @f$ an appropriate constant

    References:
    - Kocsis, Szepesvari:
      <a href="http://zaphod.aml.sztaki.hu/papers/ecml06.pdf">
      Bandit based Monte-Carlo Planning</a>
    - Auer, Cesa-Bianchi, Fischer:
      <a href="http://homes.dsi.unimi.it/~cesabian/Pubblicazioni/ml-02.pdf">
      Finite-time Analysis of the Multiarmed Bandit Problem</a>
    - Gelly, Wang, Munos, Teytaud:
      <a href="http://hal.inria.fr/docs/00/12/15/16/PDF/RR-6062.pdf">
      Modification of UCT with patterns in Monte-Carlo Go</a>
    - Silver, Gelly:
      <a href=
      "http://imls.engr.oregonstate.edu/www/htdocs/proceedings/icml2007/papers/387.pdf">
      Combining Online and Offline Knowledge in UCT</a>

    @see
    - @ref sguctsearchlockfree
    - @ref sguctsearchweights
    - @ref sguctsearchproven
*/

/** @page sguctsearchlockfree Lock-free mode in SgUctSearch

The basic idea of the lock-free mode in SgUctSearch is to share a tree between
multiple threads without using any locks. Because of specific requirements on
the memory model of the hardware platform, lock-free mode is an optional
feature of the SgUctSearch and needs to be enabled explicitly.

@section sguctsearchlockfreetree Modifying the Tree Structure

The first change to make the lock-free search work is in the handling of
concurrent changes to the structure of the tree. SgUctSearch never deletes
nodes during a search; new nodes are created in a pre-allocated memory array.
In the lock-free algorithm, each thread has its own memory array for creating
new nodes. Only after the nodes are fully created and initialized, are they
linked to the parent node. This can cause some memory overhead, because if
several threads expand the same node only the children created by the last
thread will be used in future simulations. It can also happen that some of the
children that are lost already received value updates; these updates will be
lost.

The child information of a node consists of two variables: a pointer to the
first child in the array, and the number of children. To avoid that another
thread sees an inconsistent state of these variables, all threads assume that
the number of children is valid if the pointer to the first child is not null.
Linking a parent to a new set of children requires first writing the number of
children, then the pointer to the first child. The compiler is prevented from
reordering the writes by declaring these variables as volatile.

@section sguctsearchlockfreevalues Updating Values

The move and RAVE values are stored in the nodes as counts and mean values.
The mean values are updated using an incremental algorithm. Updating them
without protection by a mutex can cause updates of the mean to be lost with or
without increment of the count, as well as updates of the mean occurring
without increment of the count. It could also happen that one thread reads the
count and mean while they are written by another thread, and the first thread
sees an erroneous state that exists only temporarily. In practice, these
faulty updates occur with a low probability and will have only a small effect
on the counts and mean values. They are intentionally ignored.

The only problematic case is if a count is zero, because the mean value is
undefined if the count is zero, and this case has a special meaning at several
places in the search. For example, the computation of the values for the
selection of children in the in-tree phase distinguishes three cases: if the
move count and RAVE count is non-zero, the value will be computed as a
weighted linear combination of both mean values, if the move count is zero,
only the RAVE mean is used, and if both counts are zero, a configurable
constant value, the first play urgency, is used. To avoid this problem, all
threads assume that a mean value is only valid if the corresponding count is
non-zero. Updating a value requires first writing the new mean value, then the
new count. The compiler is prevented from reordering the writes by declaring
the counts and mean values as volatile.

@section sguctsearchlockfreeplatform Platform Requirements

There are some requirements on the memory model of the platform to make the
lock-free search algorithm work. Writes of certain basic types (size_t, int,
float, pointers) must be atomic. Writes by one thread must be seen by other
threads in the same order. The IA-32 and Intel-64 CPU architectures, which are
used in most modern standard computers, guarantee these assumptions. They also
synchronize CPU caches after writes. (See
<a href="http://download.intel.com/design/processor/manuals/253668.pdf">
Intel 64 and IA-32 Architectures Software Developer's Manual</a>, chapter
7.1 Locked Atomic Operations and 7.2 Memory Ordering).
*/

/** @page sguctsearchweights Estimator weights in SgUctSearch
    The weights of the estimators (move value, RAVE value) are chosen by
    assuming that the estimators are uncorrelated and modeling the mean
    squared error of estimator @f$ i @f$ by a function that depends on the
    number of samples and parameter constants, which represent the variance
    and bias of the estimator and need to be determined experimentally:

    @f[
    w_i = \frac{1}{Z} \frac{1}{{\rm MSE}_i}
    \qquad Z = \sum_i \frac{1}{{\rm MSE}_i}
    \qquad {\rm MSE}_i = \frac{c_{\rm variance}}{N_i} + c_{\rm bias}^2
    @f]

    Note that this formula is nearly equivalent to the formula suggested
    by David Silver on the Computer Go mailing list for the case of two
    estimators (move value and RAVE value) and used in newer versions of MoGo.
    However, MoGo uses the measured variance of the current RAVE value (for
    both the move weight and RAVE weight) instead variance parameter
    constants.

    The formula is then reformulated to use different constants that describe
    the initial steepness and final asymptotic value of the unnormalized
    weight:

    @f[
    Z w_i =
    \frac{c_{\rm initial}}
         {\frac{1}{N} + \frac{c_{\rm initial}}{c_{\rm final}}}
    @f]

    with:
    - @f$ N @f$ sample count of the estimator
    - @f$ c_{\rm initial} @f$ Initial weight parameter; this is they weight if
      @f$ N = 1 @f$ and @f$ c_{\rm final} \gg c_{\rm initial} @f$
    - @f$ c_{\rm final} @f$ Final weight parameter; this is they weight if
      @f$ N \rightarrow \infty @f$

    For the move value, @f$ c_{\rm bias} = 0 @f$, and the variance can become
    part of the normalization constant, so the weight is simply
    @f$ N_{\rm move} @f$.
    If no estimator has a sample count yet, the first-play-urgency parameter
    is used for the value estimate.
*/

/** @page sguctsearchproven Proven Nodes in SgUctSearch

    When expanding a node (or computing knowledge at a node) it is
    possible the derived class can prove that the state corresponding
    to the node is a proven win or loss. Note this differs from a
    terminal node since a terminal node by definition has no children,
    whereas a proven node can have any number of children.

    Any in-tree phase that encounters a proven node is immediately
    terminated and the result passed back up the tree just as if a
    playout (with the value corresponding to the proven value) had
    been performed.

    The existence of proven nodes in a SgUctTree does not change the
    move selection in any way. For example: a node with both winning
    and losing children will continue to select children without
    regard to their proven value. This means losing children will not
    be avoided and winning children will not be chosen except as a
    result of the playouts recorded as those children are visisted
    (the values of which would now be perfectly accurate).

    Proven values are not currently passed up the tree like in a
    min-max search. It would be possible to pass back winning moves,
    that is, mark the parent of winning moves as winning, but it is
    not possible mark a node as losing if all its moves are
    losing. This is because it is possible progressive widening (or
    other heuristics) are being used by the derived state and in such
    a case it may not be true that all the current children being
    losses implies the parent state is a loss.
*/

//----------------------------------------------------------------------------

typedef SgStatistics<float,std::size_t> SgUctStatistics;

typedef SgStatisticsExt<float,std::size_t> SgUctStatisticsExt;

//----------------------------------------------------------------------------

/** Game result, sequence and nodes of one Monte-Carlo game in SgUctSearch.
    @ingroup sguctgroup
*/
struct SgUctGameInfo
{
    /** The game result of the playout(s).
        The result is from the view of the player at the root.
    */
    std::vector<float> m_eval;

    /** The sequence of the in-tree phase. */
    std::vector<SgMove> m_inTreeSequence;

    /** The sequence of the playout(s).
        For convenient usage, they also include the moves from
        m_inTreeSequence, even if they are the same for each playout.
    */
    std::vector<std::vector<SgMove> > m_sequence;

    /** Was the playout aborted due to maxGameLength (stored for each
        playout).
    */
    std::vector<bool> m_aborted;

    /** Nodes visited in the in-tree phase. */
    std::vector<const SgUctNode*> m_nodes;

    /** Flag to skip RAVE update for moves of the playout(s).
        For convenient usage, the index corresponds to the move number from
        the root position on, even if the flag is currently only used for
        moves in the playout phase, so the flag is false for all moves in the
        in-tree phase.
    */
    std::vector<std::vector<bool> > m_skipRaveUpdate;

    void Clear(std::size_t numberPlayouts);
};

//----------------------------------------------------------------------------

/** Move selection strategy after search is finished.
    @ingroup sguctgroup
*/
enum SgUctMoveSelect
{
    /** Select move with highest mean value. */
    SG_UCTMOVESELECT_VALUE,

    /** Select move with highest count. */
    SG_UCTMOVESELECT_COUNT,

    /** Use UCT bound (or combined bound if RAVE is enabled). */
    SG_UCTMOVESELECT_BOUND,

    /** Use the weighted sum of UCT and RAVE value (but without
        bias terms).
    */
    SG_UCTMOVESELECT_ESTIMATE
};

//----------------------------------------------------------------------------

/** Base class for the thread state.
    Subclasses must be thread-safe, it must be possible to use different
    instances of this class in different threads (after construction, the
    constructor does not need to be thread safe). Beware not to use classes
    that are not thread-safe, because they use global variables
    (e.g. SgRandom::Global())
    @note Technically it is possible to use a non-thread safe implementation
    of subclasses, as long as the search is run with only one thread.
    @ingroup sguctgroup
*/
class SgUctThreadState
{
public:
    /** Number of the thread between 0 and SgUctSearch::NumberThreads() - 1 */
    const std::size_t m_threadId;

    bool m_isSearchInitialized;

    /** Flag indicating the a node could not be expanded, because the
        maximum tree size was reached.
    */
    bool m_isTreeOutOfMem;

    SgUctGameInfo m_gameInfo;

    /** Local variable for SgUctSearch::UpdateRaveValues().
        Reused for efficiency. Stores the first time a move was played
        by the color to play at the root position (move is used as an index,
        do m_moveRange must be > 0); numeric_limits<size_t>::max(), if the
        move was not played.
    */
    boost::scoped_array<std::size_t> m_firstPlay;

    /** Local variable for SgUctSearch::UpdateRaveValues().
        Like m_firstPlayToPlay, but for opponent color.
    */
    boost::scoped_array<std::size_t> m_firstPlayOpp;

    /** Local variable for SgUctSearch::PlayInTree().
        Reused for efficiency.
    */
    std::vector<SgMoveInfo> m_moves;

    /** Local variable for SgUctSearch::CheckCountAbort().
        Reused for efficiency.
    */
    std::vector<SgMove> m_excludeMoves;

    /** Thread's counter for Randomized Rave in SgUctSearch::SelectChild().
     */
    int m_randomizeCounter;

    SgUctThreadState(size_t threadId, int moveRange = 0);

    virtual ~SgUctThreadState();

    /** @name Pure virtual functions */
    // @{

    /** Evaluate end-of-game position.
        Will only be called if GenerateAllMoves() or GeneratePlayoutMove()
        returns no moves. Should return larger values if position is better
        for the player to move.
    */
    virtual float Evaluate() = 0;

    /** Execute a move.
        @param move The move
     */
    virtual void Execute(SgMove move) = 0;

    /** Execute a move in the playout phase.
        For optimization if the subclass uses uses a different game state
        representation in the playout phase. Otherwise the function can be
        implemented in the subclass by simply calling Execute().
        @param move The move
     */
    virtual void ExecutePlayout(SgMove move) = 0;

    /** Generate moves.
        Moves will be explored in the order of the returned list.
        If return is true, trees under children will be deleted.
        @param count Number of times node has been visited. For knowledge-
        based computations.
        @param[out] moves The generated moves or empty list at end of game
    */
    virtual bool GenerateAllMoves(std::size_t count, 
                                  std::vector<SgMoveInfo>& moves,
                                  SgProvenNodeType& provenType) = 0;

    /** Generate random move.
        Generate a random move in the play-out phase (outside the UCT tree).
        @param[out] skipRaveUpdate This value should be set to true, if the
        move should be excluded from RAVE updates. Otherwise it can be
        ignored.
        @return The move or SG_NULLMOVE at the end of the game.
    */
    virtual SgMove GeneratePlayoutMove(bool& skipRaveUpdate) = 0;

    /** Start search.
        This function should do any necessary preparations for playing games
        in the thread, like initializing the thread's copy of the game state
        from the global game state. The function does not have to be
        thread-safe.
    */
    virtual void StartSearch() = 0;

    /** Take back moves played in the in-tree phase. */
    virtual void TakeBackInTree(std::size_t nuMoves) = 0;

    /** Take back moves played in the playout phase.
        The search engine does not assume that the moves are really taken back
        after this function is called. If the subclass implements the playout
        in s separate state, which is initialized in StartPlayout() and does
        not support undo, the implementation of this function can be left
        empty in the subclass.
    */
    virtual void TakeBackPlayout(std::size_t nuMoves) = 0;

    // @} // name


    /** @name Virtual functions */
    // @{

    /** Function that will be called by PlayGame() before the game.
        Default implementation does nothing.
    */
    virtual void GameStart();

    /** Function that will be called at the beginning of the playout phase.
        Will be called only once (not once per playout!). Can be used for
        example to save some state of the current position for more efficient
        implementation of TakeBackPlayout().
        Default implementation does nothing.
    */
    virtual void StartPlayouts();

    /** Function that will be called at the beginning of each playout.
        Default implementation does nothing.
    */
    virtual void StartPlayout();

    /** Function that will be called after each playout.
        Default implementation does nothing.
    */
    virtual void EndPlayout();

    // @} // name
};

//----------------------------------------------------------------------------

class SgUctSearch;

/** Create game specific thread state.
    @see SgUctThreadState
    @ingroup sguctgroup
*/
class SgUctThreadStateFactory
{
public:
    virtual ~SgUctThreadStateFactory();

    virtual SgUctThreadState* Create(std::size_t threadId,
                                     const SgUctSearch& search) = 0;
};

//----------------------------------------------------------------------------

/** Statistics of the last search performed by SgUctSearch. */
struct SgUctSearchStat
{
    double m_time;

    /** Number of nodes for which the knowledge threshold was exceeded. */ 
    std::size_t m_knowledge;

    /** Games per second.
        Useful values only if search time is higher than resolution of
        SgTime::Get().
    */
    double m_gamesPerSecond;

    SgUctStatisticsExt m_gameLength;

    SgUctStatisticsExt m_movesInTree;

    SgUctStatistics m_aborted;

    void Clear();

    void Write(std::ostream& out) const;
};

//----------------------------------------------------------------------------

/** Optional parameters to SgUctSearch::Search() to allow early aborts.
    If early abort is used, the search will be aborted after a fraction of the
    resources (max time, max nodes) are spent, if the value is a clear win
    (above a threshold).
*/
struct SgUctEarlyAbortParam
{
    /** The threshold to define what a clear win is. */
    float m_threshold;

    /** The minimum number of games to allow an early abort.
        For a very low number of simulations, the value can be very
        unreliable.
    */
    std::size_t m_minGames;

    /** The inverse fraction of the total resources (max time, max nodes),
        after which the early abort check is performed.
    */
    int m_reductionFactor;
};

//----------------------------------------------------------------------------

/** Monte Carlo tree search using UCT.
    The evaluation function is assumed to be in <code>[0..1]</code> and
    inverted with <code>1 - eval</code>.
    @ingroup sguctgroup
*/
class SgUctSearch
{
public:
    static float InverseEval(float eval);

    /** Constructor.
        @param threadStateFactory The tread state factory (takes ownership).
        Can be null and set later (before using the search) with
        SetThreadStateFactory to allow multi-step construction.
        @param moveRange Upper integer limit (exclusive) used for move
        representation. Certain enhancements of SgUctSearch (e.g. Rave())
        need to store data in arrays using the move as an index for
        efficient implementation. If the game does not use a small integer
        range for its move representation, this parameter should be 0.
        Then, enhancements that require a small move range cannot be enabled.
    */
    SgUctSearch(SgUctThreadStateFactory* threadStateFactory,
                int moveRange = 0);

    virtual ~SgUctSearch();

    void SetThreadStateFactory(SgUctThreadStateFactory* factory);

    /** @name Pure virtual functions */
    // @{

    /** Convert move to string (game dependent).
        This function needs to be thread-safe.
    */
    virtual std::string MoveString(SgMove move) const = 0;

    /** Evaluation value to use if evaluation is unknown.
        This value will be used, if games are aborted, because they exceed
        the maximum game length.
        This function needs to be thread-safe.
    */
    virtual float UnknownEval() const = 0;

    // @} // name


    /** @name Virtual functions */
    // @{

    /** Hook function that will be called by Search() after each game.
        Default implementation does nothing.
        This function does not need to be thread-safe.
        @param gameNumber The number of this iteration.
        @param threadId
        @param info The game info of the thread which played this iteration.
        @warning If LockFree() is enabled, this function will be called from
        multiple threads without locking. The subclass should handle this
        case appropriately by using its own lock or disabling functionality
        that will not work without locking.
    */
    virtual void OnSearchIteration(std::size_t gameNumber, int threadId,
                                   const SgUctGameInfo& info);

    /** Hook function that will be called by StartSearch().
        Default implementation calls m_mpiSynchronizer.StartSearch().
        This function does not need to be thread-safe.
    */
    virtual void OnStartSearch();

    /** Hook function that will be called when search
    completes.  Default implementation calls
    m_mpiSynchronizer.EndSearch().  This function
    does not need to be thread-safe.
    */
    virtual void OnEndSearch();

    virtual void OnThreadStartSearch(SgUctThreadState& state);

    virtual void OnThreadEndSearch(SgUctThreadState& state);

    virtual std::size_t GamesPlayed() const;

    // @} // name


    /** @name Search functions */
    // @{

    /** Get a list of all generated moves.
        Sets up thread state 0 for a seach and calls GenerateAllMoves
        of the thread state.
    */
    void GenerateAllMoves(std::vector<SgMoveInfo>& moves);

    /** Play a single game.
        Plays a single game using the thread state of the first thread.
        Call StartSearch() before calling this function.
    */
    void PlayGame();

    /** Start search.
        Initializes search for current position and clears statistics.
        @param rootFilter Moves to filter at the root node
        @param initTree The tree to initialize the search with. 0 for no
        initialization. The trees are actually swapped, not copied.
    */
    void StartSearch(const std::vector<SgMove>& rootFilter
                     = std::vector<SgMove>(),
                     SgUctTree* initTree = 0);

    void EndSearch();

    /** Calls StartSearch() and then maxGames times PlayGame().
        @param maxGames The maximum number of games (greater or equal one).
        @param maxTime The maximum time in seconds.
        @param[out] sequence The move sequence with the best value.
        @param rootFilter Moves to filter at the root node
        @param initTree The tree to initialize the search with. 0 for no
        initialization. The trees are actually swapped, not copied.
        @param earlyAbort See SgUctEarlyAbortParam. Null means not to do an
        early abort.
        @return The value of the root position.
    */
    float Search(std::size_t maxGames, double maxTime,
                 std::vector<SgMove>& sequence,
                 const std::vector<SgMove>& rootFilter
                 = std::vector<SgMove>(),
                 SgUctTree* initTree = 0,
                 SgUctEarlyAbortParam* earlyAbort = 0);

    /** Do a one-ply Monte-Carlo search instead of the UCT search.
        @param maxGames
        @param maxTime
        @param[out] value The value of the best move
    */
    SgPoint SearchOnePly(size_t maxGames, double maxTime, float& value);

    /** Find child node with best move.
        @param node The father node.
        @param excludeMoves Optional list of moves to ignore in the children
        nodes.
        @return The best child or 0 if no child nodes exists.
    */
    const SgUctNode*
    FindBestChild(const SgUctNode& node,
                  const std::vector<SgMove>* excludeMoves = 0) const;

    /** Extract sequence of best moves from root node.
        @param[out] sequence The resulting sequence.
    */
    void FindBestSequence(std::vector<SgMove>& sequence) const;

    /** Return the bound of a move.
        This is the bound that was used for move selection. It can be the
        pure UCT bound or the combined bound if RAVE is used.
        @param useRave Whether rave should be used or not.
        @param node The node corresponding to the position
        @param child The node corresponding to the move
    */
    float GetBound(bool useRave, const SgUctNode& node, 
                   const SgUctNode& child) const;

    // @} // name


    /** @name Search data */
    // @{

    /** Info for last game.
        Returns the last game info of the first thread.
        This function is not thread-safe.
    */
    const SgUctGameInfo& LastGameInfo() const;

    /** One-line summary text for last game.
        Contains: move, count and mean for all nodes; result
        Returns the last game info of the first thread.
        This function is not thread-safe.
    */
    std::string LastGameSummaryLine() const;

    /** See parameter earlyAbort in Search() */
    bool WasEarlyAbort() const;

    const SgUctTree& Tree() const;

    /** Get temporary tree.
        Returns a tree that is compatible in size and number of allocators
        to the tree of the search. This tree is used by the search itself as
        a temporary tree for certain operations during the search, but can be
        used by other code while the search is not running.
    */
    SgUctTree& GetTempTree();

    // @} // name


    /** @name Parameters */
    // @{

    /** Constant c in the bias term.
        This constant corresponds to 2 C_p in the original UCT paper.
        The default value is 0.7, which works well in 9x9 Go.
    */
    float BiasTermConstant() const;

    /** See BiasTermConstant() */
    void SetBiasTermConstant(float biasTermConstant);

    /** Points at which to recompute children.  
        Specifies the number of visits at which GenerateAllMoves() is
        called again at the node. This is to allow multiple knowledge
        computations to occur as a node becomes more
        important. Returned move info will be merged with info in the
        tree. This is can be used prune, add children, give a bonus to
        a move, etc.
    */
    std::vector<std::size_t> KnowledgeThreshold() const;

    /** See KnowledgeThreshold() */
    void SetKnowledgeThreshold(const std::vector<std::size_t>& counts);

    /** Maximum number of nodes in the tree.
        @note The search owns two trees, one of which is used as a temporary
        tree for some operations (see GetTempTree()). This functions sets
        the maximum number of nodes per tree.
    */
    std::size_t MaxNodes() const;

    /** See MaxNodes()
        @param maxNodes Maximum number of nodes (>= 1)
    */
    void SetMaxNodes(std::size_t maxNodes);

    /** The number of threads to use during the search. */
    std::size_t NumberThreads() const;

    /** See SetNumberThreads() */
    void SetNumberThreads(std::size_t n);

    /** Lock-free usage of multi-threaded search.
        @ref sguctsearchlockfree
    */
    bool LockFree() const;

    /** See LockFree() */
    void SetLockFree(bool enable);

    /** See SetRandomizeRaveFrequency() */
    int RandomizeRaveFrequency() const;

    /** Randomly turns off rave with given frequency -
        once in every frequency child selections.
        Helps in rare cases where rave ignores the best move. 
        Set frequency to 0 to switch it off.
    */
    void SetRandomizeRaveFrequency(int frequency);

    /** Don't update RAVE value if opponent played the same move first.
        Default is false (since it depends on the game and move
        representation, if it should be used).
    */
    bool RaveCheckSame() const;

    /** See RaveCheckSame() */
    void SetRaveCheckSame(bool enable);

    /** First play urgency.
        The value for unexplored moves. According to UCT they should
        always be preferred to moves, that have been played at least once.
        According to the MoGo tech report, it can be useful to use smaller
        values (as low as 1) to encorouge early exploitation.
        Default value is 10000.
        @see @ref sguctsearchweights
    */
    float FirstPlayUrgency() const;

    /** See FirstPlayUrgency() */
    void SetFirstPlayUrgency(float firstPlayUrgency);

    /** Log one-line summary for each game during Search() to a file.
        @todo File name still is hard-coded to "uctsearch.log"
    */
    bool LogGames() const;

    /** See LogGames() */
    void SetLogGames(bool enable);

    /** Maximum game length.
        If the number of moves in a game exceed this length, it will be
        counted as a loss.
        The default is @c numeric_limits<size_t>::max()
    */
    std::size_t MaxGameLength() const;

    /** See MaxGameLength() */
    void SetMaxGameLength(std::size_t maxGameLength);

    /** Required number of simulations to expand a node in the tree.
        The default is 2, which means a node will be expanded on the second
        visit.
    */
    std::size_t ExpandThreshold() const;

    /** See ExpandThreshold() */
    void SetExpandThreshold(std::size_t expandThreshold);

    /** The number of playouts per simulated game.
        Useful for multi-threading to increase the workload of the threads.
        Default is 1.
    */
    std::size_t NumberPlayouts() const;

    void SetNumberPlayouts(std::size_t n);

    /** Use the RAVE algorithm (Rapid Action Value Estimation).
        See Gelly, Silver 2007 in the references in the class description.
        In difference to the original description of the RAVE algorithm,
        no "RAVE bias term" is used. The estimated value of a move is the
        weighted mean of the move value and the RAVE value and then a
        single UCT-like bias term is added.
        @see RaveWeightFunc
    */
    bool Rave() const;

    /** See Rave() */
    void SetRave(bool enable);

    /** See SgUctMoveSelect */
    SgUctMoveSelect MoveSelect() const;

    /** See SgUctMoveSelect */
    void SetMoveSelect(SgUctMoveSelect moveSelect);

    /** See @ref sguctsearchweights. */
    float RaveWeightInitial() const;

    /** See @ref sguctsearchweights. */
    void SetRaveWeightInitial(float value);

    /** See @ref sguctsearchweights. */
    float RaveWeightFinal() const;

    /** See @ref sguctsearchweights. */
    void SetRaveWeightFinal(float value);

    /** Weight RAVE updates.
        Gives more weight to moves that are closer to the position for which
        the RAVE statistics are stored. The weighting function is linearly
        decreasing from 2 to 0 with the move number from the position for
        which the RAVE statistics are stored to the end of the simulated game.
    */
    bool WeightRaveUpdates() const;

    /** See WeightRaveUpdates() */
    void SetWeightRaveUpdates(bool enable);

    /** Whether search uses virtual loss. */
    bool VirtualLoss() const;

    /** See VirtualLoss() */
    void SetVirtualLoss(bool enable);

    /** Prune nodes with low counts if tree is full.
        This will prune nodes below a minimum count, if the tree gets full
        during a search. The minimum count is PruneMinCount() at the beginning
        of the search and is doubled every time a pruning operation does not
        reduce the tree by at least a factor of 2. The creation of the pruned
        tree uses the temporary tree (see GetTempTree()).
    */
    bool PruneFullTree() const;

    /** See PruneFullTree() */
    void SetPruneFullTree(bool enable);

    /** See PruneFullTree() */
    std::size_t PruneMinCount() const;

    /** See PruneFullTree() */
    void SetPruneMinCount(std::size_t n);


    void SetMpiSynchronizer(const SgMpiSynchronizerHandle &synchronizerHandle);

    SgMpiSynchronizerHandle MpiSynchronizer();

    const SgMpiSynchronizerHandle MpiSynchronizer() const;

    // @} // name


    /** @name Statistics */
    // @{

    const SgUctSearchStat& Statistics() const;

    void WriteStatistics(std::ostream& out) const;

    // @} // name

    /** Get state of one of the threads.
        Requires: ThreadsCreated()
    */
    SgUctThreadState& ThreadState(int i) const;

    /** Check if threads are already created.
        The threads are created at the beginning of the first search
        (to allow multi-step construction with setting the policy after
        the constructor call).
    */
    bool ThreadsCreated() const;

    /** Create threads.
        The threads are created at the beginning of the first search
        (to allow multi-step construction with setting the policy after
        the constructor call). This function needs to be called explicitely
        only, if a thread state is going to be used before the first search.
    */
    void CreateThreads();

private:
    typedef boost::recursive_mutex::scoped_lock GlobalLock;

    friend class Thread;

    class Thread
    {
    public:
        std::auto_ptr<SgUctThreadState> m_state;

        Thread(SgUctSearch& search, std::auto_ptr<SgUctThreadState> state);

        ~Thread();

        void StartPlay();

        void WaitPlayFinished();

    private:
        /** Copyable function object that invokes Thread::operator().
            Needed because the the constructor of boost::thread copies the
            function object argument.
        */
        class Function
        {
        public:
            Function(Thread& thread);

            void operator()();

        private:
            Thread& m_thread;
        };

        friend class Thread::Function;

        SgUctSearch& m_search;

        bool m_quit;

        boost::barrier m_threadReady;

        boost::mutex m_startPlayMutex;

        boost::mutex m_playFinishedMutex;

        boost::condition m_startPlay;

        boost::condition m_playFinished;

        boost::mutex::scoped_lock m_playFinishedLock;

        GlobalLock m_globalLock;

        /** The thread.
            Order dependency: must be constructed as the last member, because
            the constructor starts the thread.
        */
        boost::thread m_thread;

        void operator()();

        void PlayGames();
    };

    std::auto_ptr<SgUctThreadStateFactory> m_threadStateFactory;

    /** See LogGames() */
    bool m_logGames;

    /** See Rave() */
    bool m_rave;
   
    /** See KnowledgeThreshold() */
    std::vector<std::size_t> m_knowledgeThreshold;

    /** Flag indicating that the search was terminated because the maximum
        time or number of games was reached.
    */
    volatile bool m_aborted;
    
    volatile bool m_isTreeOutOfMemory;

    std::auto_ptr<boost::barrier> m_searchLoopFinished;

    /** See SgUctEarlyAbortParam. */
    bool m_wasEarlyAbort;

    /** See SgUctEarlyAbortParam.
        The auto pointer is empty, if no early abort is used.
    */
    std::auto_ptr<SgUctEarlyAbortParam> m_earlyAbort;

    /** See SgUctMoveSelect */
    SgUctMoveSelect m_moveSelect;

    /** See RaveCheckSame() */
    bool m_raveCheckSame;

    /** See SetRandomizeRaveFrequency() */
    int m_randomizeRaveFrequency;

    /** See LockFree() */
    bool m_lockFree;

    /** See WeightRaveUpdates() */
    bool m_weightRaveUpdates;

    /** See PruneFullTree() */
    bool m_pruneFullTree;

    /** See NumberThreads() */
    std::size_t m_numberThreads;

    /** See NumberPlayouts() */
    std::size_t m_numberPlayouts;

    /** See MaxNodes() */
    std::size_t m_maxNodes;

    /** See PruneMinCount() */
    std::size_t m_pruneMinCount;

    /** See parameter moveRange in constructor */
    const int m_moveRange;

    /** See MaxGameLength() */
    std::size_t m_maxGameLength;

    /** See ExpandThreshold() */
    std::size_t m_expandThreshold;

    /** Number of games limit for the current search. */
    std::size_t m_maxGames;

    /** Number of games played in the current search. */
    std::size_t m_numberGames;

    std::size_t m_startRootMoveCount;

    /** Interval in number of games in which to check time abort.
        Avoids that the potentially expensive SgTime::Get() is called after
        every game. The interval is updated dynamically according to the
        current games/sec, such that it is called ten times per second
        (if the total search time is at least one second, otherwise ten times
        per total maximum search time)
    */
    std::size_t m_checkTimeInterval;

    double m_lastScoreDisplayTime;

    /** See BiasTermConstant() */
    float m_biasTermConstant;

    /** See FirstPlayUrgency() */
    float m_firstPlayUrgency;

    /** See @ref sguctsearchweights. */
    float m_raveWeightInitial;

    /** See @ref sguctsearchweights. */
    float m_raveWeightFinal;

    /** 1 / m_raveWeightInitial precomputed for efficiency */
    double m_raveWeightParam1;

    /** m_raveWeightInitial / m_raveWeightFinal precomputed for efficiency */
    double m_raveWeightParam2;

    /** Time limit for current search. */
    double m_maxTime;

    /** See VirtualLoss() */
    bool m_virtualLoss;

    std::string m_logFileName;

    //JJG: Remove to create reproducable results
#if 0
    SgTimer m_timer;
#endif

    SgUctTree m_tree;

    /** See GetTempTree() */
    SgUctTree m_tempTree;

    /** See parameter rootFilter in function Search() */
    std::vector<SgMove> m_rootFilter;

    std::ofstream m_log;

    /** Mutex for protecting global variables during multi-threading.
        Currently, only the play-out phase of games is thread safe, therefore
        this lock is always locked elsewhere (in-tree phase, updating of
        values and statistics, etc.)
    */
    boost::recursive_mutex m_globalMutex;

    SgUctSearchStat m_statistics;

    /** List of threads.
        The elements are owned by the vector (shared_ptr is only used because
        auto_ptr should not be used with standard containers)
    */
    std::vector<boost::shared_ptr<Thread> > m_threads;

#if SG_UCTFASTLOG
    SgFastLog m_fastLog;
#endif

    boost::shared_ptr<SgMpiSynchronizer> m_mpiSynchronizer;


    void ApplyRootFilter(std::vector<SgMoveInfo>& moves);

    bool CheckAbortSearch(SgUctThreadState& state);

    bool CheckEarlyAbort() const;

    bool CheckCountAbort(SgUctThreadState& state,
                         std::size_t remainingGames) const;

    void Debug(const SgUctThreadState& state, const std::string& textLine);

    void DeleteThreads();

    void ExpandNode(SgUctThreadState& state, const SgUctNode& node);

    void CreateChildren(SgUctThreadState& state, const SgUctNode& node,
                        bool deleteChildTrees);

    float GetBound(bool useRave, float logPosCount, 
                   const SgUctNode& child) const;

    float GetValueEstimate(bool useRave, const SgUctNode& child) const;

    float GetValueEstimateRave(const SgUctNode& child) const;

    float Log(float x) const;

    bool NeedToComputeKnowledge(const SgUctNode* current);

    void PlayGame(SgUctThreadState& state, GlobalLock* lock);

    bool PlayInTree(SgUctThreadState& state, bool& isTerminal);

    bool PlayoutGame(SgUctThreadState& state, std::size_t playout);

	void PrintSearchProgress(double currTime) const;
    
    void SearchLoop(SgUctThreadState& state, GlobalLock* lock);

    const SgUctNode& SelectChild(int& randomizeCounter, const SgUctNode& node);

    std::string SummaryLine(const SgUctGameInfo& info) const;

    void UpdateCheckTimeInterval(double time);

    void UpdateDynRaveBias();

    void UpdateRaveValues(SgUctThreadState& state);

    void UpdateRaveValues(SgUctThreadState& state, std::size_t playout);

    void UpdateRaveValues(SgUctThreadState& state, std::size_t playout,
                          float eval, std::size_t i,
                          const std::size_t firstPlay[],
                          const std::size_t firstPlayOpp[]);

    void UpdateStatistics(const SgUctGameInfo& info);

    void UpdateTree(const SgUctGameInfo& info);
};

inline float SgUctSearch::BiasTermConstant() const
{
    return m_biasTermConstant;
}

inline std::size_t SgUctSearch::ExpandThreshold() const
{
    return m_expandThreshold;
}

inline float SgUctSearch::FirstPlayUrgency() const
{
    return m_firstPlayUrgency;
}

inline float SgUctSearch::InverseEval(float eval)
{
    return (1 - eval);
}

inline bool SgUctSearch::LockFree() const
{
    return m_lockFree;
}

inline const SgUctGameInfo& SgUctSearch::LastGameInfo() const
{
    return ThreadState(0).m_gameInfo;
}

inline bool SgUctSearch::LogGames() const
{
    return m_logGames;
}

inline std::size_t SgUctSearch::MaxGameLength() const
{
    return m_maxGameLength;
}

inline std::size_t SgUctSearch::MaxNodes() const
{
    return m_maxNodes;
}

inline SgUctMoveSelect SgUctSearch::MoveSelect() const
{
    return m_moveSelect;
}

inline std::size_t SgUctSearch::NumberThreads() const
{
    return m_numberThreads;
}

inline std::size_t SgUctSearch::NumberPlayouts() const
{
    return m_numberPlayouts;
}

inline void SgUctSearch::PlayGame()
{
    PlayGame(ThreadState(0), 0);
}

inline bool SgUctSearch::PruneFullTree() const
{
    return m_pruneFullTree;
}

inline std::size_t SgUctSearch::PruneMinCount() const
{
    return m_pruneMinCount;
}

inline bool SgUctSearch::Rave() const
{
    return m_rave;
}

inline bool SgUctSearch::RaveCheckSame() const
{
    return m_raveCheckSame;
}

inline float SgUctSearch::RaveWeightInitial() const
{
    return m_raveWeightInitial;
}

inline float SgUctSearch::RaveWeightFinal() const
{
    return m_raveWeightFinal;
}

inline void SgUctSearch::SetBiasTermConstant(float biasTermConstant)
{
    m_biasTermConstant = biasTermConstant;
}

inline void SgUctSearch::SetExpandThreshold(std::size_t expandThreshold)
{
    SG_ASSERT(expandThreshold >= 1);
    m_expandThreshold = expandThreshold;
}

inline void SgUctSearch::SetFirstPlayUrgency(float firstPlayUrgency)
{
    m_firstPlayUrgency = firstPlayUrgency;
}

inline void SgUctSearch::SetLockFree(bool enable)
{
    m_lockFree = enable;
}

inline void SgUctSearch::SetLogGames(bool enable)
{
    m_logGames = enable;
}

inline void SgUctSearch::SetMaxGameLength(std::size_t maxGameLength)
{
    m_maxGameLength = maxGameLength;
}

inline void SgUctSearch::SetMaxNodes(std::size_t maxNodes)
{
    m_maxNodes = maxNodes;
    if (m_threads.size() > 0) // Threads already created
        m_tree.SetMaxNodes(m_maxNodes);
}

inline void SgUctSearch::SetMoveSelect(SgUctMoveSelect moveSelect)
{
    m_moveSelect = moveSelect;
}

inline std::vector<std::size_t> SgUctSearch::KnowledgeThreshold() const
{
    return m_knowledgeThreshold;
}

inline void 
SgUctSearch::SetKnowledgeThreshold(const std::vector<std::size_t>& t)
{
    m_knowledgeThreshold = t;
}

inline void SgUctSearch::SetNumberPlayouts(std::size_t n)
{
    SG_ASSERT(n >= 1);
    m_numberPlayouts = n;
}

inline void SgUctSearch::SetPruneFullTree(bool enable)
{
    m_pruneFullTree = enable;
}

inline void SgUctSearch::SetPruneMinCount(std::size_t n)
{
    m_pruneMinCount = n;
}

inline void SgUctSearch::SetMpiSynchronizer(const SgMpiSynchronizerHandle 
                                            &synchronizerHandle)
{
    m_mpiSynchronizer = SgMpiSynchronizerHandle(synchronizerHandle);
}

inline SgMpiSynchronizerHandle SgUctSearch::MpiSynchronizer()
{
    return SgMpiSynchronizerHandle(m_mpiSynchronizer);
}

inline const SgMpiSynchronizerHandle SgUctSearch::MpiSynchronizer() const
{
    return SgMpiSynchronizerHandle(m_mpiSynchronizer);
}

inline int SgUctSearch::RandomizeRaveFrequency() const
{
    return m_randomizeRaveFrequency;
}

inline void SgUctSearch::SetRandomizeRaveFrequency(int frequency)
{
    m_randomizeRaveFrequency = frequency;    
}

inline void SgUctSearch::SetRaveCheckSame(bool enable)
{
    m_raveCheckSame = enable;
}

inline void SgUctSearch::SetRaveWeightFinal(float value)
{
    m_raveWeightFinal = value;
}

inline void SgUctSearch::SetRaveWeightInitial(float value)
{
    m_raveWeightInitial = value;
}

inline void SgUctSearch::SetWeightRaveUpdates(bool enable)
{
    m_weightRaveUpdates = enable;
}

inline bool SgUctSearch::VirtualLoss() const
{
    return m_virtualLoss;
}

inline void SgUctSearch::SetVirtualLoss(bool enable)
{
    m_virtualLoss = enable;
}

inline const SgUctSearchStat& SgUctSearch::Statistics() const
{
    return m_statistics;
}

inline bool SgUctSearch::ThreadsCreated() const
{
    return (m_threads.size() > 0);
}

inline SgUctThreadState& SgUctSearch::ThreadState(int i) const
{
    SG_ASSERT(static_cast<std::size_t>(i) < m_threads.size());
    return *m_threads[i]->m_state;
}

inline const SgUctTree& SgUctSearch::Tree() const
{
    return m_tree;
}

inline bool SgUctSearch::WasEarlyAbort() const
{
    return m_wasEarlyAbort;
}

inline bool SgUctSearch::WeightRaveUpdates() const
{
    return m_weightRaveUpdates;
}

//----------------------------------------------------------------------------

#endif // SG_UCTSEARCH_H
