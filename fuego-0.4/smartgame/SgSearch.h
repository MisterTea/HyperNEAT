//----------------------------------------------------------------------------
/** @file SgSearch.h
    Search engine.

    SgSearch is the search engine of the Smart Game Board, providing depth-
    first search with iterative deepening and transposition tables.
*/
//----------------------------------------------------------------------------

#ifndef SG_SEARCH_H
#define SG_SEARCH_H

#include "SgBlackWhite.h"
#include "SgHash.h"
#include "SgMove.h"
#include "SgSearchStatistics.h"
#include "SgSearchTracer.h"
#include "SgStack.h"
#include "SgTimer.h"
#include "SgVector.h"

template <class DATA> class SgHashTable;
class SgNode;
class SgProbCut;
class SgSearchControl;

//----------------------------------------------------------------------------

/** Used in class SgSearch to implement killer heuristic.
    Keeps track of two moves that have been successful at a particular level.
    The moves are sorted by frequency.
*/
class SgKiller
{
public:
    SgKiller();

    void MarkKiller(SgMove killer);

    void Clear();

    SgMove GetKiller1() const;

    SgMove GetKiller2() const;

private:
    SgMove m_killer1;

    SgMove m_killer2;

    int m_count1;

    int m_count2;
};

inline SgKiller::SgKiller()
    : m_killer1(SG_NULLMOVE),
      m_killer2(SG_NULLMOVE),
      m_count1(0),
      m_count2(0)
{
}

inline SgMove SgKiller::GetKiller1() const
{
    return m_killer1;
}

inline SgMove SgKiller::GetKiller2() const
{
    return m_killer2;
}

//----------------------------------------------------------------------------

/** Hash data used in class SgSearch. */
class SgSearchHashData
{
public:
    SgSearchHashData();

    SgSearchHashData(int depth, signed value, SgMove bestMove,
                     bool isOnlyUpperBound = false,
                     bool isOnlyLowerBound = false,
                     bool isExactValue = false);

    ~SgSearchHashData();

    int Depth() const;

    int Value() const;

    SgMove BestMove() const;

    bool IsOnlyUpperBound() const;

    bool IsOnlyLowerBound() const;

    void AdjustBounds(int* lower, int* upper);

    bool IsBetterThan(const SgSearchHashData& data) const;

    bool IsValid() const;

    bool IsExactValue() const;

    void Invalidate();

    void AgeData();

private:
    unsigned m_depth : 12;

    unsigned m_isUpperBound : 1;

    unsigned m_isLowerBound : 1;

    unsigned m_isValid : 1;

    unsigned m_isExactValue : 1;

    signed m_value : 16;

    SgMove m_bestMove;
};

typedef SgHashTable<SgSearchHashData> SgSearchHashTable;

inline SgSearchHashData::SgSearchHashData()
    : m_depth(0),
      m_isUpperBound(false),
      m_isLowerBound(false),
      m_isValid(false),
      m_isExactValue(false),
      m_value(-1),
      m_bestMove(SG_NULLMOVE)
{ }

inline SgSearchHashData::SgSearchHashData(int depth, signed value,
                                          SgMove bestMove,
                                          bool isOnlyUpperBound,
                                          bool isOnlyLowerBound,
                                          bool isExactValue)
    : m_depth(depth),
      m_isUpperBound(isOnlyUpperBound),
      m_isLowerBound(isOnlyLowerBound),
      m_isValid(true),
      m_isExactValue(isExactValue),
      m_value(value),
      m_bestMove(bestMove)
{
    // Ensure value fits in 16 bits
    SG_ASSERT(m_value == value);
}

inline SgSearchHashData::~SgSearchHashData()
{
}

inline int SgSearchHashData::Depth() const
{
    return static_cast<int> (m_depth);
}

inline int SgSearchHashData::Value() const
{
    return static_cast<int> (m_value);
}

inline SgMove SgSearchHashData::BestMove() const
{
    return m_bestMove;
}

inline bool SgSearchHashData::IsOnlyUpperBound() const
{
    return m_isUpperBound != 0;
}

inline bool SgSearchHashData::IsOnlyLowerBound() const
{
    return m_isLowerBound != 0;
}

inline void SgSearchHashData::AdjustBounds(int* lower, int* upper)
{
    if (IsOnlyUpperBound())
        *upper = std::min(*upper, Value());
    else if (IsOnlyLowerBound())
        *lower = std::max(*lower, Value());
    else
    {   // If not just an upper or lower bound, then this is precise.
        *lower = Value();
        *upper = Value();
    }
}

inline bool SgSearchHashData::IsValid() const
{
    return m_isValid;
}

inline bool SgSearchHashData::IsExactValue() const
{
    return m_isExactValue;
}

inline void SgSearchHashData::Invalidate()
{
    m_isValid = false;
}

inline void SgSearchHashData::AgeData()
{
    m_depth = 0;
}

//----------------------------------------------------------------------------
namespace SgSearchLimit {
    static const int MAX_DEPTH = 256;
}

typedef SgStack<SgMove, SgSearchLimit::MAX_DEPTH> SgSearchStack;
//----------------------------------------------------------------------------

/** Alpha-beta search.
    The problem-specific part of the search is isolated in five methods:
    move generation, position evaluation, executing and taking back moves,
    and time control. These need to be overridden for each kind of search.
    The evaluation may employ lookahead or a quiescence search to
    find the value. If so, the sequence from the current position
    to the position where that value is really reached is returned,
    otherwise the empty list is returned.

    @todo Why does AmaSearch::Evaluate need the hash table, shouldn't that be
    done in SgSearch?
    @todo Remove m_depth, pass as argument to Evaluate instead
    @todo Use best-response as move ordering heuristic
*/
class SgSearch
{
public:
    /** One DEPTH_UNIT corresponds to the default search depth for one move,
        in the internal representation of search. Used for fractional ply
        extensions. E.g. extending from atari in Go may count as only
        1/8 ply = DEPTH_UNIT/8. This way forced lines can be searched
        much deeper at a low nominal search depth.
    */
    static const int DEPTH_UNIT = 100;

    /** Remark: could make it 512 if deep ladder search a problem */
    static const int MAX_DEPTH = 256;

   /** Infinity for windowed searches.
        Needs prefix SG_ even if not in global namespace, because there is a
        conflict with a global macro INFINITY.
    */
    static const int SG_INFINITY;

    /** Constructor.
        @param hash Hash table to use or 0, if no hash table should be used.
    */
    SgSearch(SgSearchHashTable* hash);

    virtual ~SgSearch();

    /** Stop search if depth limit was not reached in current iteration.
        Usually this should return true, but it depends on the move generation
        in the subclass. For example, if the move generation prunes some
        moves at lower depths, because the goal cannot be reached at the
        current depth, this function has to return false.
    */
    virtual bool CheckDepthLimitReached() const = 0;

    const SgSearchHashTable* HashTable() const;

    void SetHashTable(SgSearchHashTable* hashtable);

    const SgSearchControl* SearchControl() const;

    /** Search control.
        Set the abort checking function; pass 0 to disable abort checking.
        Caller keeps ownership of control.
    */
    void SetSearchControl(SgSearchControl* control);

    /** ProbCut.
        Set the ProbCut bounds; pass 0 to disable ProbCut.
        Caller keeps ownership of probcut.
    */
    void SetProbCut(SgProbCut* probcut);

    /** Convert move to string (game dependent). */
    virtual std::string MoveString(SgMove move) const = 0;

    virtual void SetToPlay(SgBlackWhite toPlay) = 0;

    /** Hook function called at the beginning of a search.
        Default implementation does nothing.
    */
    virtual void OnStartSearch();

    /** Looks 'depthLimit' moves ahead to find the value of the
        current position and the optimal sequence.
        No values outside the range ['boundLo'..'boundHi'] are expected;
        if values outside that range are encountered, a value <= 'boundLo'
        or >= 'boundHi' will be returned, and the search should be repeated
        with new bounds.
        If node is not 0, then add the whole search tree below '*node'.
        The hash table is seeded with the sequence passed in, so that partial
        results from a previous search can speed up a re-search.
    */
    int DepthFirstSearch(int depthLimit, int boundLo, int boundHi,
                         SgVector<SgMove>* sequence, bool clearHash = true,
                         SgNode* traceNode = 0);

    /** Call DepthFirstSearch with window [-SG_INFINITY,+SG_INFINITY] */
    int DepthFirstSearch(int depthLimit, SgVector<SgMove>* sequence,
                         bool clearHash = true, SgNode* traceNode = 0);

    /** Calls DepthFirstSearch repeatedly with the depth limit starting at
        'depthMin' and increasing with each iteration.
        Stops when the problem is solved, meaning that a result that is
        definitely good for one of the players is reached, or when 'depthMax'
        is reached. The bound parameters are just passed to DepthFirstSearch.
        If node is not 0, then add the whole search tree below '*node'.
        The hash table is seeded with the sequence passed in, so that partial
        results from a previous search can speed up a re-search.
    */
    int IteratedSearch(int depthMin, int depthMax, int boundLo,
                       int boundHi, SgVector<SgMove>* sequence,
                       bool clearHash = true, SgNode* traceNode = 0);


    /** Call IteratedSearch with window [-SG_INFINITY,+SG_INFINITY] */
    int IteratedSearch(int depthMin, int depthMax, SgVector<SgMove>* sequence,
                       bool clearHash = true, SgNode* traceNode = 0);

    /** During IteratedSearch or CombinedSearch, this returns the current
        depth that's being searched to.
    */
    int IteratedSearchDepthLimit() const;

    /** Called at start of each depth level of IteratedSearch.
        Can be overridden to adapt search (or instrumentation) to current
        depth. Must call inherited.
    */
    virtual void StartOfDepth(int depthLimit);

    /** Return whether the search was aborted. */
    bool Aborted() const;

    /** Mark this search as aborted.
        Will terminate next time AbortSearch gets called. Or mark search as
        not having been aborted (e.g. when minimizing area and first search
        succeeds but second one doesn't have enough time to complete).
    */
    void SetAbortSearch(bool fAborted = true);

    void SetScout(bool flag = true);

    void SetKillers(bool flag = true);

    void SetOpponentBest(bool flag = true);

    void SetNullMove(bool flag = true);

    void SetNullMoveDepth(int depth);

    /** Get the current statistics. Can be called during search. */
    void GetStatistics(SgSearchStatistics* stat);

    const SgSearchStatistics& Statistics() const
    {
        return m_stat;
    }
    
    /** Starts the clock and clears the statistics.
        Can be nested; only the outermost call actually does anything.
    */
    void StartTime();

    /** Stops the clock and clears the statistics.
        Can be nested; only the outermost call actually does anything.
    */
    void StopTime();

    /** Generate moves.
        @param moves The returned list is the set of moves to be tried.
        These moves will be tested for legality, so illegal moves can also be
        included if that speeds up move generation. If the empty list
        is returned, the evaluation function will be called with the
        same position right away (thus an evaluation done during move
        generation can be saved and returned from the move evaluation).
        @param depth The remaining depth until a terminal node is reached
        (height > 0). Note that this is a fractional value: each move may be
        counted as less than one full move to look deeper in some variations.
        The value is expressed in DEPTH_UNIT rather than using float to be
        stored compactly in the hash table.
    */
    virtual void Generate(SgVector<SgMove>* moves, int depth) = 0;

    /** The returned value reflects the value of the position, with
        positive values being good for the current player (ToPlay).
        @param isExact Return value, if set, the value is exact, even if it is
        not a terminal positions and Generate would generate moves.
        @param depth See SgSearch::Generate()
        @return The evaluation of the current position.
    */
    virtual int Evaluate(bool* isExact, int depth) = 0;

    /** Return true if the move was executed, false if it was illegal
        and could not be played.
        @param move
        @param delta The amount by which that move shall decrement the current
        depth. Normal is DEPTH_UNIT, but forcing moves may return a delta of
        zero to look deeper into one variation.
        @param depth See SgSearch::Generate() (Execute could need to know the
        depth to reject moves depending on the depth that were originally
        generated, e.g. used in ExCaptureTask::Execute)
    */
    virtual bool Execute(SgMove move, int* delta, int depth) = 0;

    /** Takes back the most recent move successfully executed by Execute. */
    virtual void TakeBack() = 0;

    /** Return the current player. */
    virtual SgBlackWhite GetToPlay() const = 0;

    /** Test whether search should be aborted.
        Return true to abort search. Default implementation checks Abort() of
        the installed search control.
    */
    virtual bool AbortSearch();

    /** Return the hash code for the current position. */
    virtual SgHashCode GetHashCode() const = 0;

    /** The current depth of the search, incremented by 1 for each
        move that's played. Value is 0 at root level of search.
    */
    int CurrentDepth() const;

    /** Indicates which move in the movelist at the previous level was
        executed.
        This may be necessary if the value or moves at a
        position depend on the sequence leading to that position.
    */
    SgMove PrevMove() const;

    /** The move prior to the previous move.
        Knowing both the last two moves is necessary to decide whether the
        current position is a seki, where it's best for both players to pass.
    */
    SgMove PrevMove2() const;

    /** Is the game over? */
    virtual bool EndOfGame() const = 0;

    /** Initialize PrevMove, CurrentDepth and other variables so that they can
        be accessed when move generation/evaluation are called directly,
        not as part of a search.
    */
    void InitSearch(int startDepth = 0);

    /** Is tracing currently active?*/
    bool TraceIsOn() const;

    /** Default creates a SgSearchTracer; override for specific traces */
    virtual void CreateTracer();
    
    /** Set tracer object. Search object assumes ownership */
    void SetTracer(SgSearchTracer* tracer);
    
    SgSearchTracer* Tracer() const;
    
    void SetAbortFrequency(int value);

    /** Core Alpha-beta search. Usually not called directly -
        call DepthFirstSearch or IteratedSearch instead. */
    int SearchEngine(int depth, int alpha, int beta,
                     SgStack<SgMove, SgSearch::MAX_DEPTH>& stack,
                     bool* isExactValue, bool lastNullMove = false);

private:
    /** Hash table */
    SgSearchHashTable* m_hash;

    /** Used to build a trace tree of the search for debugging */
    SgSearchTracer* m_tracer;

    /** The depth of the current position - number of moves from start */
    int m_currentDepth;

    int m_depthLimit;

    /** Stack of all moves executed in search. Used by PrevMove() */
    SgVector<SgMove> m_moveStack;

    bool m_useScout;

    bool m_useKillers;

    /** Move best move from parent to front */
    bool m_useOpponentBest;

    /** Use null move heuristic for forward pruning */
    bool m_useNullMove;

    /** How much less deep to search during null move pruning */
    int m_nullMoveDepth;

    /** True if search is in the process of being aborted. */
    bool m_aborted;

    /** Flag that new best move was found in current iteration. */
    bool m_foundNewBest;

    /** Keeps track of whether the depth limit was reached. */
    bool m_reachedDepthLimit;

    SgSearchStatistics m_stat;
    
    SgTimer m_timer;

    int m_timerLevel;

    /** The search result from the previous iteration */
    int m_prevValue;

    /** The PV from the previous search iteration */
    SgVector<SgMove> m_prevSequence;

    static const int MAX_KILLER_DEPTH = 10;

    /** Killer heuristic. */
    SgArray<SgKiller,MAX_KILLER_DEPTH + 1> m_killers;

    SgSearchControl* m_control;

    SgProbCut* m_probcut;

    int m_abortFrequency;

    /** Depth-first search (see implementation) */
    int DFS(int startDepth, int depthLimit, int boundLo, int boundHi,
            SgVector<SgMove>* sequence, bool* isExactValue);

    /** Try to find current position in m_hash */
    bool LookupHash(SgSearchHashData& data) const;

    bool NullMovePrune(int depth, int delta, int beta);

    /** Store current position in hash table */
    void StoreHash(int depth, int value, SgMove move, bool isUpperBound,
                   bool isLowerBound, bool isExact);

    /** Seed the hash table with the given sequence. */
    void AddSequenceToHash(const SgVector<SgMove>& sequence, int depth);

    /** Evaluate current position; possibly write debug output */
    int CallEvaluate(int depth, bool* isExact);

    /** Execute move; update m_moveStack, m_currentDepth and statistics */
    bool CallExecute(SgMove move, int* delta, int depth);

    /** Generate moves; possibly write debug output */
    void CallGenerate(SgVector<SgMove>* moves, int depth);

    /** Take back move; update m_moveStack and m_currentDepth */
    void CallTakeBack();

    bool TryMove(SgMove move, const SgVector<SgMove>& specialMoves,
                       const int depth,
                       const int alpha, const int beta,
                       int& loValue, int& hiValue,
                       SgSearchStack& stack,
                       bool& allExact,
                       bool& isCutoff);

    bool TrySpecialMove(SgMove move, SgVector<SgMove>& specialMoves,
                       const int depth,
                       const int alpha, const int beta,
                       int& loValue, int& hiValue,
                       SgSearchStack& stack,
                       bool& allExact,
                       bool& isCutoff);

    /** Not implemented */
    SgSearch(const SgSearch&);

    /** Not implemented */
    SgSearch& operator=(const SgSearch&);
};

inline bool SgSearch::Aborted() const
{
    return m_aborted;
}

inline int SgSearch::CurrentDepth() const
{
    return m_currentDepth;
}

inline int SgSearch::DepthFirstSearch(int depthLimit,
                                      SgVector<SgMove>* sequence,
                                      bool clearHash, SgNode* traceNode)
{
    return DepthFirstSearch(depthLimit, -SG_INFINITY, SG_INFINITY, sequence,
                            clearHash, traceNode);
}

inline const SgSearchHashTable* SgSearch::HashTable() const
{
    return m_hash;
}

inline void SgSearch::SetHashTable(SgSearchHashTable* hashtable)
{
    m_hash = hashtable;
}

inline int SgSearch::IteratedSearchDepthLimit() const
{
    return m_depthLimit;
}

inline int SgSearch::IteratedSearch(int depthMin, int depthMax,
                                    SgVector<SgMove>* sequence, 
                                    bool clearHash,
                                    SgNode* traceNode)
{
    return IteratedSearch(depthMin, depthMax, -SG_INFINITY, SG_INFINITY,
                          sequence, clearHash, traceNode);
}

inline SgMove SgSearch::PrevMove() const
{
    return m_moveStack.Back();
}

inline SgMove SgSearch::PrevMove2() const
{
    return m_moveStack.TopNth(2);
}

inline const SgSearchControl* SgSearch::SearchControl() const
{
    return m_control;
}

inline void SgSearch::SetAbortFrequency(int value)
{
    m_abortFrequency = value;
}

inline void SgSearch::SetAbortSearch(bool fAborted)
{
    m_aborted = fAborted;
}

inline void SgSearch::SetKillers(bool flag)
{
    m_useKillers = flag;
}

inline void SgSearch::SetNullMove(bool flag)
{
    m_useNullMove = flag;
}

inline void SgSearch::SetNullMoveDepth(int depth)
{
    m_nullMoveDepth = depth;
}

inline void SgSearch::SetOpponentBest(bool flag)
{
    m_useOpponentBest = flag;
}

inline void SgSearch::SetScout(bool flag)
{
    m_useScout = flag;
}

inline void SgSearch::SetTracer(SgSearchTracer* tracer)
{
    // check that an existing tracer is not overwritten.
    // This can potentially be allowed in the future if needed. 
    SG_ASSERT(! m_tracer || ! tracer);
    m_tracer = tracer;
}

inline SgSearchTracer* SgSearch::Tracer() const
{
    return m_tracer;
}
    
//----------------------------------------------------------------------------

#endif // SG_SEARCH_H
