//----------------------------------------------------------------------------
/** @file SgSearch.cpp
    See SgSearch.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgSearch.h"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <sstream>
#include <math.h>
#include "SgDebug.h"
#include "SgHashTable.h"
#include "SgMath.h"
#include "SgNode.h"
#include "SgProbCut.h"
#include "SgSearchControl.h"
#include "SgSearchValue.h"
#include "SgTime.h"
#include "SgVector.h"
#include "SgWrite.h"

using namespace std;

//----------------------------------------------------------------------------

namespace{
    const bool DEBUG_SEARCH = false;
    const bool DEBUG_SEARCH_ITERATIONS = false;
}
//----------------------------------------------------------------------------

void SgKiller::MarkKiller(SgMove killer)
{
    if (killer == m_killer1)
        ++m_count1;
    else if (killer == m_killer2)
    {
        ++m_count2;
        if (m_count1 < m_count2)
        {
            swap(m_killer1, m_killer2);
            swap(m_count1, m_count2);
        }
    }
    else if (m_killer1 == SG_NULLMOVE)
    {
        m_killer1 = killer;
        m_count1 = 1;
    }
    else
    {
        m_killer2 = killer;
        m_count2 = 1;
    }
}

void SgKiller::Clear()
{
    m_killer1 = SG_NULLMOVE;
    m_count1 = 0;
    m_killer2 = SG_NULLMOVE;
    m_count2 = 0;
}

//----------------------------------------------------------------------------

bool SgSearchHashData::IsBetterThan(const SgSearchHashData& data) const
{
    if (m_depth > data.m_depth)
        return true;
    if (m_depth < data.m_depth)
        return false;
    return (! m_isUpperBound && ! m_isLowerBound)
        || (m_isLowerBound && data.m_isLowerBound && m_value > data.m_value)
        || (m_isUpperBound && data.m_isUpperBound && m_value < data.m_value);
}

//----------------------------------------------------------------------------

namespace {

/** copy stack onto sequence, starting with Top() */
void ReverseCopyStack(const SgSearchStack& moveStack, SgVector<SgMove>& sequence)
{
    sequence.Clear();
    for (int i = moveStack.Size() - 1; i >= 0; --i)
    	sequence.PushBack(moveStack[i]);
}

void WriteSgSearchHashData(std::ostream& str, const SgSearch& search, 
						   const SgSearchHashData& data)
{
	str << "move = " << search.MoveString(data.BestMove()) 
    	<< " exact = " << data.IsExactValue()
        << " value = " << data.Value() 
        << '\n';
}


void WriteMoves(const SgSearch& search, const SgVector<SgMove>& sequence)
{
    for (SgVectorIterator<SgMove> it(sequence); it; ++it)
        SgDebug() << ' ' << search.MoveString(*it);
}

void PrintPV(const SgSearch& search, int depth, int value,
             const SgVector<SgMove>& sequence,
             bool isExactValue)
{
    SgDebug() << "Iteration d = " << depth
        << ", value = " << value
        << ", exact = " << isExactValue
        << ", sequence = ";
    WriteMoves(search, sequence);
    SgDebug() << '\n';
}

} // namespace

//----------------------------------------------------------------------------

const int SgSearch::SG_INFINITY = numeric_limits<int>::max();

SgSearch::SgSearch(SgSearchHashTable* hash)
    : m_hash(hash),
      m_tracer(0),
      m_currentDepth(0),
      m_useScout(false),
      m_useKillers(false),
      m_useOpponentBest(0),
      m_useNullMove(0),
      m_nullMoveDepth(2),
      m_aborted(false),
      m_foundNewBest(false),
      m_reachedDepthLimit(false),
      m_stat(),
      m_timerLevel(0),
      m_control(0),
      m_probcut(0),
      m_abortFrequency(1)
{
    InitSearch();
}

SgSearch::~SgSearch()
{
}

void SgSearch::CallGenerate(SgVector<SgMove>* moves, int depth)
{
    Generate(moves, depth);
    if (DEBUG_SEARCH)
    {
        SgDebug() << "SgSearch::CallGenerate: d=" << depth;
        WriteMoves(*this, *moves);
        SgDebug() << '\n';
    }
}

void SgSearch::InitSearch(int startDepth)
{
    m_currentDepth = startDepth;
    m_moveStack.Clear();
    m_moveStack.PushBack(SG_NULLMOVE);
    m_moveStack.PushBack(SG_NULLMOVE);
    if (m_useKillers)
    {
        for (int i = 0; i <= MAX_KILLER_DEPTH; ++i)
            m_killers[i].Clear();
    }
}

bool SgSearch::LookupHash(SgSearchHashData& data) const
{
    SG_ASSERT(! data.IsValid());
    if (m_hash == 0 || ! m_hash->Lookup(GetHashCode(), &data))
        return false;
    if (DEBUG_SEARCH)
    {
        SgDebug() << "SgSearch::LookupHash: " << GetHashCode() << ' ';
        WriteSgSearchHashData(SgDebug(), *this, data);
    }
    return true;
}

void SgSearch::OnStartSearch()
{
    // Default implementation does nothing
}

void SgSearch::SetSearchControl(SgSearchControl* control)
{
    m_control = control;
}

void SgSearch::SetProbCut(SgProbCut* probcut)
{
    m_probcut = probcut;
}

void SgSearch::StoreHash(int depth, int value, SgMove move,
                         bool isUpperBound, bool isLowerBound, bool isExact)
{
    SG_ASSERT(m_hash);
    SgSearchHashData data(depth, value, move, isUpperBound, isLowerBound,
                          isExact);
    if (DEBUG_SEARCH)
    {
        SgDebug() << "SgSearch::StoreHash: " << GetHashCode() 
                  << ": ";
        WriteSgSearchHashData(SgDebug(), *this, data);
    }
    m_hash->Store(GetHashCode(), data);
}

bool SgSearch::TraceIsOn() const
{
    return m_tracer && m_tracer->TraceIsOn();
}

bool SgSearch::AbortSearch()
{
    if (! m_aborted)
    {
        // Checking abort is potentially expensive, involves system call.
        // Only check every m_abortFrequency nodes
        if (m_stat.NumNodes() % m_abortFrequency != 0)
            return false;
        m_aborted =
               m_control 
            && m_control->Abort(m_timer.GetTime(), m_stat.NumNodes());
        if (! m_aborted && SgUserAbort())
            m_aborted = true;
        if (m_aborted && TraceIsOn())
            m_tracer->TraceComment("aborted");
    }
    return m_aborted;
}

bool SgSearch::NullMovePrune(int depth, int delta, int beta)
{
    SgSearchStack ignoreStack;
    bool childIsExact = true;
    if (beta >= SG_INFINITY - 1)
        return false;
    if (CallExecute(SG_PASS, &delta, depth))
    {
        float nullvalue = -SearchEngine(depth - delta,
            -beta, -beta + 1, ignoreStack, &childIsExact, true);
        CallTakeBack();
        if (nullvalue >= beta)
        {
            if (TraceIsOn())
                m_tracer->TraceComment("null-move-cut");
            return true;
        }
    }
    return false;
}

void SgSearch::GetStatistics(SgSearchStatistics* stat)
{
    m_stat.SetTimeUsed(m_timer.GetTime());
    *stat = m_stat;
}

void SgSearch::StartTime()
{
    if (++m_timerLevel == 1)
    {
        m_stat.Clear();
        m_timer.Start();
    }
}

void SgSearch::StopTime()
{
    if (--m_timerLevel == 0 && ! m_timer.IsStopped())
        m_timer.Stop();
}

int SgSearch::CallEvaluate(int depth, bool* isExact)
{
    int v = Evaluate(isExact, depth);
    if (DEBUG_SEARCH)
        SgDebug() << "SgSearch::CallEvaluate d = " << depth 
                  << ", v = " << v
                  << '\n';
    return v;
}

bool SgSearch::CallExecute(SgMove move, int* delta, int depth)
{
    const SgBlackWhite toPlay = GetToPlay();
    if (DEBUG_SEARCH)
        SgDebug() << "SgSearch::CallExecute: d = " << depth << ' '
                  << SgBW(toPlay) << ' ' << MoveString(move) << '\n';
    if (Execute(move, delta, depth))
    {
        m_stat.IncNumMoves();
        if (move == SG_PASS)
            m_stat.IncNumPassMoves();
        m_moveStack.PushBack(move);
        ++m_currentDepth;
        if (TraceIsOn())
            m_tracer->AddTraceNode(move, toPlay);
        return true;
    }
    return false;
}

void SgSearch::CallTakeBack()
{
    if (DEBUG_SEARCH)
        SgDebug() << "SgSearch::CallTakeBack\n";
    TakeBack();
    if (TraceIsOn())
        m_tracer->TakeBackTraceNode();
    m_moveStack.PopBack();
    --m_currentDepth;
}

void SgSearch::CreateTracer()
{
    m_tracer = new SgSearchTracer(0);
}

void SgSearch::AddSequenceToHash(const SgVector<SgMove>& sequence, int depth)
{
    if (! m_hash)
        return;
    int numMovesToUndo = 0;
    for (SgVectorIterator<SgMove> iter(sequence); iter; ++iter)
    {
        SgMove move = *iter;
        int delta = DEPTH_UNIT;
        if (CallExecute(move, &delta, depth))
        {
            // Store move with position before the move is played.
            CallTakeBack();

            // Move is the only relevant data for seeding the hash table.
            SgSearchHashData data(0, 0, move);
            SG_ASSERT(move != SG_NULLMOVE);
            m_hash->Store(GetHashCode(), data);
            if (DEBUG_SEARCH)
                SgDebug() << "SgSearch::AddSequenceToHash: "
                          << MoveString(move) << '\n';
            // Execute move again.
            int delta = DEPTH_UNIT;
            if (CallExecute(move, &delta, depth))
                ++numMovesToUndo;
            else // it just worked, should not fail now.
            	SG_ASSERT(false);
        }
        else
            break;
    }

    // Restore the original position.
    while (--numMovesToUndo >= 0)
        CallTakeBack();
}

int SgSearch::DFS(int startDepth, int depthLimit,
                int boundLo, int boundHi,
                SgVector<SgMove>* sequence, bool* isExactValue)
{
    InitSearch(startDepth);
    SG_ASSERT(m_currentDepth == startDepth);
    SG_ASSERT(sequence);
    m_aborted = false;
    m_foundNewBest = false;
    SgSearchStack moveStack;
    int value = SearchEngine(depthLimit * DEPTH_UNIT, 
    						 boundLo, boundHi, moveStack,
                         	 isExactValue);
    ReverseCopyStack(moveStack, *sequence);
    return value;
}


int SgSearch::DepthFirstSearch(int depthLimit, int boundLo, int boundHi,
                               SgVector<SgMove>* sequence, bool clearHash,
                               SgNode* traceNode)
{
    SG_ASSERT(sequence);
    OnStartSearch();
    if (m_tracer && traceNode)
    {
        SG_ASSERT(m_tracer->TraceNode() == 0);
        SG_ASSERT(m_tracer->TraceIsOn());
        m_tracer->InitTracing("DepthFirstSearch");
    }
    StartTime();
    if (clearHash && m_hash)
    {
        m_hash->Clear();
        AddSequenceToHash(*sequence, 0);
    }
    m_depthLimit = 0;
    bool isExactValue = true;
    int value = DFS(0, depthLimit, boundLo, boundHi, sequence, &isExactValue);
    StopTime();
    if (m_tracer && traceNode)
        m_tracer->AppendTrace(traceNode);
    return value;
}

int SgSearch::IteratedSearch(int depthMin, int depthMax, int boundLo,
                             int boundHi, SgVector<SgMove>* sequence,
                             bool clearHash, SgNode* traceNode)
{
    SG_ASSERT(sequence);
    OnStartSearch();
    if (m_tracer && traceNode)
    {
        SG_ASSERT(m_tracer->TraceNode() == 0);
        SG_ASSERT(m_tracer->TraceIsOn());
        m_tracer->InitTracing("IteratedSearch");
    }
    StartTime();
    if (clearHash && m_hash)
    {
        m_hash->Clear();
        AddSequenceToHash(*sequence, 0);
    }

    int value = 0;
    m_depthLimit = depthMin;
    // done in DFS, but that's too late, is tested after StartOfDepth
    m_aborted = false;

    // Keep track of value and sequence of previous iteration in case search
    // gets aborted (since value and sequence are then ill-defined).
    m_prevValue = 0;
    m_prevSequence.Clear();
    bool isExactValue = true;

    do
    {
        if (m_control != 0
            && ! m_control->StartNextIteration(m_depthLimit,
                                               m_timer.GetTime(), 
                                               m_stat.NumNodes()))
            SetAbortSearch();
        if (m_aborted)
            break;
        StartOfDepth(m_depthLimit);

        // Record depth limit of depths where we actually do some search.
        m_stat.SetDepthReached(m_depthLimit);

        // Remember whether we actually reach the depth limit. If not, no
        // sense in increasing the depth limit, won't find anything new.
        m_reachedDepthLimit = false;
        isExactValue = true;
        m_foundNewBest = false;

        // Fixed-depth search.
        value = DFS(0, m_depthLimit, boundLo, boundHi, sequence,
                    &isExactValue);

        // Restore result of previous iteration if aborted.
        if (m_aborted)
        {
            if (m_prevSequence.NonEmpty() && ! m_foundNewBest)
            {
                value = m_prevValue;
                *sequence = m_prevSequence;
            }
            break;
        }
        else
        {
            if (DEBUG_SEARCH_ITERATIONS)
                PrintPV(*this, m_depthLimit, value, *sequence, isExactValue);
            m_prevValue = value;
            m_prevSequence = *sequence;
        }

        // Stop iteration as soon as exact result or a bounding value found.
        if (isExactValue || value <= boundLo || boundHi <= value)
            break;

        ++m_depthLimit;

    } while (  m_depthLimit <= depthMax
            && ! isExactValue
            && ! m_aborted
            && (! CheckDepthLimitReached() || m_reachedDepthLimit)
            );

    StopTime();
    if (m_tracer && traceNode)
        m_tracer->AppendTrace(traceNode);
    return value;
}

bool SgSearch::TryMove(SgMove move, const SgVector<SgMove>& specialMoves,
                       const int depth,
                       const int alpha, const int beta,
                       int& loValue, int& hiValue,
                       SgSearchStack& stack,
                       bool& allExact,
                       bool& isCutoff)
{
	if (specialMoves.Contains(move)) // already tried move before
    	return false;

    int delta = DEPTH_UNIT;
    if (! CallExecute(move, &delta, depth))
    	return false;

    bool childIsExact = true;
    SgSearchStack newStack;
    int merit = -SearchEngine(depth - delta, -hiValue,
                              -max(loValue, alpha), newStack,
                              &childIsExact);
    if (loValue < merit && ! m_aborted) // new best move
    {
        loValue = merit;
        if (m_useScout)
        {
            // If getting a move that's better than what we have
            // so far, not good enough to cause a cutoff, was
            // searched with a narrow window, and doesn't
            // immediately lead to a terminal node, then search
            // again with a wide window to get a more precise
            // value.
            if (  alpha < merit
               && merit < beta
               && delta < depth
               )
            {
                childIsExact = true;
                loValue = -SearchEngine(depth-delta, -beta,
                                        -merit, newStack,
                                        &childIsExact);
            }
            hiValue = max(loValue, alpha) + 1;
        }
        stack.CopyFrom(newStack);
        stack.Push(move);
        SG_ASSERT(move != SG_NULLMOVE);
        if (m_currentDepth == 1 && ! m_aborted)
            m_foundNewBest = true;
    }
    if (! childIsExact)
        allExact = false;
    CallTakeBack();
    if (loValue >= beta)
    {
        // Move generated a cutoff: add this move to the list of
        // killers.
        if (m_useKillers && m_currentDepth <= MAX_KILLER_DEPTH)
            m_killers[m_currentDepth].MarkKiller(move);
        if (TraceIsOn())
            m_tracer->TraceComment("b-cut");
        isCutoff = true;
    }
    return true;
}

bool SgSearch::TrySpecialMove(SgMove move, SgVector<SgMove>& specialMoves,
                       const int depth,
                       const int alpha, const int beta,
                       int& loValue, int& hiValue,
                       SgSearchStack& stack,
                       bool& allExact,
                       bool& isCutoff)

{
	if (specialMoves.Contains(move))
    	return false;
    bool executed = TryMove(move, specialMoves,
                             depth, alpha, beta,
                       		 loValue, hiValue, stack,
                             allExact, isCutoff);
    specialMoves.PushBack(move);
    return executed;
}

int SgSearch::SearchEngine(const int depth, int alpha, int beta,
                           SgSearchStack& stack, bool* isExactValue,
                           bool lastNullMove)
{
    SG_ASSERT(stack.IsEmpty() || stack.Top() != SG_NULLMOVE);
    SG_ASSERT(alpha < beta);

    // Only place we check whether the search has been newly aborted. In all
    // other places, just check whether search was aborted before.
    // AR: what to return here?
    // if - (SG_INFINITY-1), then will be positive on next level?
    if (AbortSearch())
    {
        *isExactValue = false;
        return alpha;
    }

    // Null move pruning
    if (  m_useNullMove
       && depth > 0
       && ! lastNullMove
       && NullMovePrune(depth, DEPTH_UNIT * (1 + m_nullMoveDepth), beta)
       )
    {
        *isExactValue = false;
        return beta;
    }

    // ProbCut
    if (m_probcut && m_probcut->IsEnabled())
    {
        int probCutValue;
        if (m_probcut->ProbCut(*this, depth, alpha, beta, stack, 
                               isExactValue, &probCutValue)
           )
            return probCutValue;
    }

    m_stat.IncNumNodes();
    bool hasMove = false; // true if a move has been executed at this level
    int loValue = -(SG_INFINITY - 1);
    m_reachedDepthLimit = m_reachedDepthLimit || (depth <= 0);

    // check whether position is solved from hash table.
    SgSearchHashData data; // initialized to ! data.IsValid()
    if (LookupHash(data))
    {
        if (data.IsExactValue()) // exact value: stop search
        {
            *isExactValue = true;
            stack.Clear();
            if (data.BestMove() != SG_NULLMOVE)
                stack.Push(data.BestMove());
            if (TraceIsOn())
                m_tracer->TraceValue(data.Value(), GetToPlay(),
                                     "exact-hash", true);
            return data.Value();
        }
    }

    bool allExact = true; // Do all moves have exact evaluation?
    if (depth > 0 && ! EndOfGame())
    {
        // Check whether current position has already been encountered.
        SgMove tryFirst = SG_NULLMOVE;
        SgMove opponentBest = SG_NULLMOVE;
        if (data.IsValid())
        {
            if (data.Depth() > 0)
            {
                tryFirst = data.BestMove();
                SG_ASSERT(tryFirst != SG_NULLMOVE);
            }

            // If data returned from hash table is based on equal or deeper 
            // search than what we plan to do right now, just use that data. 
            // The hash table may have deeper data for the current position
            // since the same number of moves may result in more 'depth'
            // left if the 'delta' for the moves has been smaller, which
            // will happen when most moves came from the cache.
            if (depth <= data.Depth())
            {
                // Rely on value returned from hash table to be for the
                // current position. In Go, it can happen that the move is
                // not legal (ko recapture)
                int delta = DEPTH_UNIT;
                bool canExecute = CallExecute(tryFirst, &delta, depth);
                if (canExecute)
                    CallTakeBack();
                else
                    tryFirst = SG_NULLMOVE;
                if (tryFirst != SG_NULLMOVE || data.IsExactValue())
                {
                    // getting a deep enough hash hit or an exact value
                    // is as good as reaching the depth limit by search.
                    m_reachedDepthLimit = true;
                    // Update bounds with data from cache.
                    data.AdjustBounds(&alpha, &beta);

                    if (alpha >= beta)
                    {
                        *isExactValue = data.IsExactValue();
                        stack.Clear();
                        if (tryFirst != SG_NULLMOVE)
                            stack.Push(tryFirst);
                        if (TraceIsOn())
                            m_tracer->TraceValue(data.Value(), GetToPlay(),
                                           "Hash hit", *isExactValue);
                        return data.Value();
                    }
                }
            }

            int delta = DEPTH_UNIT;
            if (  tryFirst != SG_NULLMOVE
               && CallExecute(tryFirst, &delta, depth)
               )
            {
                bool childIsExact = true;
                loValue = -SearchEngine(depth-delta, -beta, -alpha, stack,
                                        &childIsExact);
                if (TraceIsOn())
                    m_tracer->TraceComment("tryFirst");
                CallTakeBack();
                hasMove = true;
                if (m_aborted)
                {
                    if (TraceIsOn())
                        m_tracer->TraceComment("aborted");
                    *isExactValue = false;
                    return (1 < m_currentDepth) ? alpha : loValue;
                }
                if (stack.NonEmpty())
                {
                    opponentBest = stack.Top();
                    SG_ASSERT(opponentBest != SG_NULLMOVE);
                }
                stack.Push(tryFirst);
                if (! childIsExact)
                   allExact = false;
                if (loValue >= beta)
                {
                    if (TraceIsOn())
                        m_tracer->TraceValue(loValue, GetToPlay());
                    // store in hash table. Known to be exact only if
                    // solved for one player.
                    bool isExact = SgSearchValue::IsSolved(loValue);
                    StoreHash(depth, loValue, tryFirst,
                              false /*isUpperBound*/,
                              true /*isLowerBound*/, isExact);
                    *isExactValue = isExact;
                    if (TraceIsOn())
                        m_tracer->TraceValue(loValue, GetToPlay(),
                                             "b-cut", isExact);
                    return loValue;
                }
            }
        }

        // 'hiValue' is equal to 'beta' for alpha-beta algorithm, and gets set
        // to alpha+1 for Scout, except for the first move.
        int hiValue =
            (hasMove && m_useScout) ? max(loValue, alpha) + 1 : beta;
        bool foundCutoff = false;
        SgVector<SgMove> specialMoves;
         // Don't execute 'tryFirst' again.
        if (tryFirst != SG_NULLMOVE)
            specialMoves.PushBack(tryFirst);
 
        // Heuristic: "a good move for my opponent is a good move for me"
        if (  ! foundCutoff
           && m_useOpponentBest
           && opponentBest != SG_NULLMOVE
           && TrySpecialMove(opponentBest, specialMoves,
                       		 depth, alpha, beta,
                       		 loValue, hiValue, stack,
                             allExact, foundCutoff)
           )
            hasMove = true;
           
        if (  ! foundCutoff 
           && m_useKillers
           && m_currentDepth <= MAX_KILLER_DEPTH
           )
        {
            SgMove killer1 = m_killers[m_currentDepth].GetKiller1();
            if (  killer1 != SG_NULLMOVE
               && TrySpecialMove(killer1, specialMoves,
                       		 depth, alpha, beta,
                       		 loValue, hiValue, stack,
                             allExact, foundCutoff)
               )
                hasMove = true;
            SgMove killer2 = m_killers[m_currentDepth].GetKiller2();
            if (  ! foundCutoff 
               && killer2 != SG_NULLMOVE
               && TrySpecialMove(killer2, specialMoves,
                       		 depth, alpha, beta,
                       		 loValue, hiValue, stack,
                             allExact, foundCutoff)
               )
                hasMove = true;
        }

        // Generate the moves for this position.
        SgVector<SgMove> moves;
        if (! foundCutoff && ! m_aborted)
        {
            CallGenerate(&moves, depth);
            // Iterate through all the moves to find the best move and
            // correct value for this position.
            for (SgVectorIterator<SgMove> it(moves); it && ! foundCutoff; ++it)
            {
                if (TryMove(*it, specialMoves,
                       		 depth, alpha, beta,
                       		 loValue, hiValue, stack,
                             allExact, foundCutoff)
                   )
                     hasMove = true;
                if (! foundCutoff && m_aborted)
                {
                    if (TraceIsOn())
                        m_tracer->TraceComment("ABORTED");
                    *isExactValue = false;
                    return (1 < m_currentDepth) ? alpha : loValue;
                }
            }
        }

        // Make sure the move added to the hash table really got generated.
#ifndef NDEBUG
        if (hasMove && stack.NonEmpty() && ! m_aborted)
        {
            SgMove bestMove = stack.Top();
            SG_ASSERT(bestMove != SG_NULLMOVE);
            SG_ASSERT(  specialMoves.Contains(bestMove)
                     || moves.Contains(bestMove)
                     );
        }
#endif
    }

    bool isSolved = ! m_aborted;
    if (! m_aborted)
    {
        // Evaluate position if terminal node (either no moves generated, or
        // none of the generated moves were legal).
        bool solvedByEval = false;
        if (! hasMove)
        {
            m_stat.IncNumEvals();
            stack.Clear();
            loValue = CallEvaluate(depth, &solvedByEval);
        }

        // Save data about current position in the hash table.
        isSolved = solvedByEval
                || SgSearchValue::IsSolved(loValue)
                || (hasMove && allExact);
        // || EndOfGame(); bug: cannot store exact score after two passes.
        if (  m_hash
           && ! m_aborted
           && (isSolved || stack.NonEmpty())
           )
        {
            SgMove bestMove = SG_NULLMOVE;
            if (stack.NonEmpty())
            {
                bestMove = stack.Top();
                SG_ASSERT(bestMove != SG_NULLMOVE);
            }
            SG_ASSERT(alpha <= beta);
            StoreHash(depth, loValue, bestMove,
                      (loValue <= alpha) /* upper */,
                      (beta <= loValue) /* lower*/, isSolved);
        }
    }

    // If aborted search and didn't find any values, just return alpha.
    // Can't return best found so far, since may not have tried the optimal
    // counter-move yet. However, return best value found so far on top
    // level, since assuming hash move will have been tried first.
    if (m_aborted && (1 < m_currentDepth || loValue < alpha))
        loValue = alpha;

    *isExactValue = isSolved;
    if (TraceIsOn())
        m_tracer->TraceValue(loValue, GetToPlay(), 0, isSolved);
    SG_ASSERT(stack.IsEmpty() || stack.Top() != SG_NULLMOVE);
    return loValue;
}

void SgSearch::StartOfDepth(int depth)
{
    if (DEBUG_SEARCH)
        SgDebug() << "SgSearch::StartOfDepth: " << depth << '\n';
    // add another separate search tree. We are either at the root of the
    // previous depth tree or, if depth==0, one level higher at the
    // linking node.
    if (m_tracer && ! m_aborted)
        m_tracer->StartOfDepth(depth);
}

//----------------------------------------------------------------------------

