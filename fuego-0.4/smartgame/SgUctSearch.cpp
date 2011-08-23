//----------------------------------------------------------------------------
/** @file SgUctSearch.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgUctSearch.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <boost/format.hpp>
#include <boost/io/ios_state.hpp>
#include <boost/version.hpp>
#include "SgDebug.h"
#include "SgHashTable.h"
#include "SgMath.h"
#include "SgWrite.h"

using namespace std;
using boost::barrier;
using boost::condition;
using boost::format;
using boost::mutex;
using boost::shared_ptr;
using boost::io::ios_all_saver;

#define BOOST_VERSION_MAJOR (BOOST_VERSION / 100000)
#define BOOST_VERSION_MINOR (BOOST_VERSION / 100 % 1000)

//----------------------------------------------------------------------------

namespace {

const bool DEBUG_THREADS = false;

void Notify(mutex& aMutex, condition& aCondition)
{
    mutex::scoped_lock lock(aMutex);
    aCondition.notify_all();
}

} // namespace

//----------------------------------------------------------------------------

void SgUctGameInfo::Clear(std::size_t numberPlayouts)
{
    m_nodes.clear();
    m_inTreeSequence.clear();
    if (numberPlayouts != m_sequence.size())
    {
        m_sequence.resize(numberPlayouts);
        m_skipRaveUpdate.resize(numberPlayouts);
        m_eval.resize(numberPlayouts);
        m_aborted.resize(numberPlayouts);
    }
    for (size_t i = 0; i < numberPlayouts; ++i)
    {
        m_sequence[i].clear();
        m_skipRaveUpdate[i].clear();
    }
}

//----------------------------------------------------------------------------

SgUctThreadState::SgUctThreadState(size_t threadId, int moveRange)
    : m_threadId(threadId),
      m_isSearchInitialized(false),
      m_isTreeOutOfMem(false),
      m_randomizeCounter(0)
{
    if (moveRange > 0)
    {
        m_firstPlay.reset(new size_t[moveRange]);
        m_firstPlayOpp.reset(new size_t[moveRange]);
    }
}

SgUctThreadState::~SgUctThreadState()
{
}

void SgUctThreadState::EndPlayout()
{
    // Default implementation does nothing
}

void SgUctThreadState::GameStart()
{
    // Default implementation does nothing
}

void SgUctThreadState::StartPlayout()
{
    // Default implementation does nothing
}

void SgUctThreadState::StartPlayouts()
{
    // Default implementation does nothing
}

//----------------------------------------------------------------------------

SgUctThreadStateFactory::~SgUctThreadStateFactory()
{
}

//----------------------------------------------------------------------------

SgUctSearch::Thread::Function::Function(Thread& thread)
    : m_thread(thread)
{
}

void SgUctSearch::Thread::Function::operator()()
{
    m_thread();
}

SgUctSearch::Thread::Thread(SgUctSearch& search,
                            auto_ptr<SgUctThreadState> state)
    : m_state(state),
      m_search(search),
      m_quit(false),
      m_threadReady(2),
      m_playFinishedLock(m_playFinishedMutex),
#if BOOST_VERSION_MAJOR == 1 && BOOST_VERSION_MINOR <= 34
      m_globalLock(search.m_globalMutex, false),
#else
      m_globalLock(search.m_globalMutex, boost::defer_lock),
#endif
      m_thread(Function(*this))
{
    m_threadReady.wait();
}

SgUctSearch::Thread::~Thread()
{
    m_quit = true;
    StartPlay();
    m_thread.join();
}

void SgUctSearch::Thread::operator()()
{
    if (DEBUG_THREADS)
        SgDebug() << "SgUctSearch::Thread: starting thread "
                  << m_state->m_threadId << '\n';
    mutex::scoped_lock lock(m_startPlayMutex);
    m_threadReady.wait();
    while (true)
    {
        m_startPlay.wait(lock);
        if (m_quit)
            break;
        m_search.SearchLoop(*m_state, &m_globalLock);
        Notify(m_playFinishedMutex, m_playFinished);
    }
    if (DEBUG_THREADS)
        SgDebug() << "SgUctSearch::Thread: finishing thread "
                  << m_state->m_threadId << '\n';
}

void SgUctSearch::Thread::StartPlay()
{
    Notify(m_startPlayMutex, m_startPlay);
}

void SgUctSearch::Thread::WaitPlayFinished()
{
    m_playFinished.wait(m_playFinishedLock);
}

//----------------------------------------------------------------------------

void SgUctSearchStat::Clear()
{
    m_time = 0;
    m_knowledge = 0;
    m_gamesPerSecond = 0;
    m_gameLength.Clear();
    m_movesInTree.Clear();
    m_aborted.Clear();
}

void SgUctSearchStat::Write(std::ostream& out) const
{
    ios_all_saver saver(out);
    out << SgWriteLabel("Time") << setprecision(2) << m_time << '\n'
        << SgWriteLabel("GameLen") << fixed << setprecision(1);
    m_gameLength.Write(out);
    out << '\n'
        << SgWriteLabel("InTree");
    m_movesInTree.Write(out);
    out << '\n'
        << SgWriteLabel("Aborted")
        << static_cast<int>(100 * m_aborted.Mean()) << "%\n"
        << SgWriteLabel("Games/s") << fixed << setprecision(1)
        << m_gamesPerSecond << '\n';
}

//----------------------------------------------------------------------------

SgUctSearch::SgUctSearch(SgUctThreadStateFactory* threadStateFactory,
                         int moveRange)
    : m_threadStateFactory(threadStateFactory),
      m_logGames(false),
      m_rave(false),
      m_knowledgeThreshold(),
      m_moveSelect(SG_UCTMOVESELECT_COUNT),
      m_raveCheckSame(false),
      m_randomizeRaveFrequency(20),
      m_lockFree(false),
      m_weightRaveUpdates(true),
      m_pruneFullTree(true),
      m_numberThreads(1),
      m_numberPlayouts(1),
      m_maxNodes(4000000),
      m_pruneMinCount(16),
      m_moveRange(moveRange),
      m_maxGameLength(numeric_limits<size_t>::max()),
      m_expandThreshold(1),
      m_biasTermConstant(0.7f),
      m_firstPlayUrgency(10000),
      m_raveWeightInitial(0.9f),
      m_raveWeightFinal(20000),
      m_virtualLoss(false),
      m_logFileName("uctsearch.log"),
      m_fastLog(10),
      m_mpiSynchronizer(SgMpiNullSynchronizer::Create())
{
    // Don't create thread states here, because the factory passes the search
    // (which is not fully constructed here, because the subclass constructors
    // are not called yet) as an argument to the Create() function
}

SgUctSearch::~SgUctSearch()
{
    DeleteThreads();
}

void SgUctSearch::ApplyRootFilter(vector<SgMoveInfo>& moves)
{
    // Filter without changing the order of the unfiltered moves
    vector<SgMoveInfo> filteredMoves;
    for (vector<SgMoveInfo>::const_iterator it = moves.begin();
         it != moves.end(); ++it)
        if (find(m_rootFilter.begin(), m_rootFilter.end(), it->m_move)
            == m_rootFilter.end())
            filteredMoves.push_back(*it);
    moves = filteredMoves;
}

size_t SgUctSearch::GamesPlayed() const
{
    return m_tree.Root().MoveCount() - m_startRootMoveCount;
}

bool SgUctSearch::CheckAbortSearch(SgUctThreadState& state)
{
    size_t gamesPlayed = GamesPlayed();
    if (SgUserAbort())
    {
        Debug(state, "SgUctSearch: abort flag");
        return true;
    }
    const bool isEarlyAbort = CheckEarlyAbort();
    if (gamesPlayed >= m_maxGames)
    {
        Debug(state, "SgUctSearch: max games reached");
        return true;
    }
    if (   isEarlyAbort
        && m_earlyAbort->m_reductionFactor * gamesPlayed >= m_maxGames
       )
    {
        Debug(state, "SgUctSearch: max games reached (early abort)");
        m_wasEarlyAbort = true;
        return true;
    }
    if (m_numberGames % m_checkTimeInterval == 0)
    {
        //JJG: Disable this to ensure reproducable results
#if 0
        double time = m_timer.GetTime();
        if (time > m_maxTime)
        {
            Debug(state, "SgUctSearch: max time reached");
            return true;
        }
        if (isEarlyAbort
            && m_earlyAbort->m_reductionFactor * time > m_maxTime)
        {
            Debug(state, "SgUctSearch: max time reached (early abort)");
            m_wasEarlyAbort = true;
            return true;
        }
        UpdateCheckTimeInterval(time);
#endif
        if (m_moveSelect == SG_UCTMOVESELECT_COUNT)
        {
            double remainingGamesDouble = m_maxGames - gamesPlayed - 1;
            // Use time based count abort, only if time > 1, otherwise
            // m_gamesPerSecond is unreliable
            //JJG: Disable this to ensure reproducable results
#if 0
            if (time > 1.)
            {
                double remainingTime = m_maxTime - time;
                remainingGamesDouble =
                    min(remainingGamesDouble,
                        remainingTime * m_statistics.m_gamesPerSecond);
            }
#endif
            size_t sizeTypeMax = numeric_limits<size_t>::max();
            size_t remainingGames;
            if (remainingGamesDouble > static_cast<double>(sizeTypeMax - 1))
                remainingGames = sizeTypeMax;
            else
                remainingGames = static_cast<size_t>(remainingGamesDouble);
            if (CheckCountAbort(state, remainingGames))
            {
                Debug(state, "SgUctSearch: move cannot change anymore");
                return true;
            }
        }
    }
    return false;
}

bool SgUctSearch::CheckCountAbort(SgUctThreadState& state,
                                  std::size_t remainingGames) const
{
    const SgUctNode& root = m_tree.Root();
    const SgUctNode* bestChild = FindBestChild(root);
    if (bestChild == 0)
        return false;
    size_t bestCount = bestChild->MoveCount();
    vector<SgMove>& excludeMoves = state.m_excludeMoves;
    excludeMoves.clear();
    excludeMoves.push_back(bestChild->Move());
    const SgUctNode* secondBestChild = FindBestChild(root, &excludeMoves);
    if (secondBestChild == 0)
        return false;
    std::size_t secondBestCount = secondBestChild->MoveCount();
    SG_ASSERT(secondBestCount <= bestCount || m_numberThreads > 1);
    return (secondBestCount + remainingGames <= bestCount);
}

bool SgUctSearch::CheckEarlyAbort() const
{
    const SgUctNode& root = m_tree.Root();
    return   m_earlyAbort.get() != 0
          && root.HasMean()
          && root.MoveCount() > m_earlyAbort->m_minGames
          && root.Mean() > m_earlyAbort->m_threshold;
}

void SgUctSearch::CreateThreads()
{
    DeleteThreads();
    for (size_t i = 0; i < m_numberThreads; ++i)
    {
        auto_ptr<SgUctThreadState> state(
                                      m_threadStateFactory->Create(i, *this));
        shared_ptr<Thread> thread(new Thread(*this, state));
        m_threads.push_back(thread);
    }
    m_tree.CreateAllocators(m_numberThreads);
    m_tree.SetMaxNodes(m_maxNodes);

    m_searchLoopFinished.reset(new barrier(m_numberThreads));
}

/** Write a debugging line of text from within a thread.
    Prepends the line with the thread number if number of threads is greater
    than one. Also ensures that the line is written as a single string to
    avoid intermingling of text lines from different threads.
    @param state The state of the thread (only used for state.m_threadId)
    @param textLine The line of text without trailing newline character.
*/
void SgUctSearch::Debug(const SgUctThreadState& state,
                        const std::string& textLine)
{
    if (m_numberThreads > 1)
    {
        // SgDebug() is not necessarily thread-safe
        GlobalLock lock(m_globalMutex);
        SgDebug() << (format("[%1%] %2%\n") % state.m_threadId % textLine);
    }
    else
        SgDebug() << (format("%1%\n") % textLine);
}

void SgUctSearch::DeleteThreads()
{
    m_threads.clear();
}

/** Expand a node.
    @param state The thread state with state.m_moves already computed.
    @param node The node to expand.
*/
void SgUctSearch::ExpandNode(SgUctThreadState& state, const SgUctNode& node)
{
    size_t threadId = state.m_threadId;
    if (! m_tree.HasCapacity(threadId, state.m_moves.size()))
    {
        Debug(state, str(format("SgUctSearch: maximum tree size %1% reached")
                         % m_tree.MaxNodes()));
        state.m_isTreeOutOfMem = true;
        m_isTreeOutOfMemory = true;
        SgSynchronizeThreadMemory();
        return;
    }
    m_tree.CreateChildren(threadId, node, state.m_moves);
}

const SgUctNode*
SgUctSearch::FindBestChild(const SgUctNode& node,
                           const vector<SgMove>* excludeMoves) const
{
    if (! node.HasChildren())
        return 0;
    const SgUctNode* bestChild = 0;
    float bestValue = 0;
    for (SgUctChildIterator it(m_tree, node); it; ++it)
    {
        const SgUctNode& child = *it;
        if (excludeMoves != 0)
        {
            vector<SgMove>::const_iterator begin = excludeMoves->begin();
            vector<SgMove>::const_iterator end = excludeMoves->end();
            if (find(begin, end, child.Move()) != end)
                continue;
        }
        if (  ! child.HasMean()
           && ! (  (  m_moveSelect == SG_UCTMOVESELECT_BOUND
                   || m_moveSelect == SG_UCTMOVESELECT_ESTIMATE
                   )
                && m_rave
                && child.HasRaveValue()
                )
            )
            continue;
        float moveValue = InverseEval(child.Mean());
        size_t moveCount = child.MoveCount();
        float value;
        switch (m_moveSelect)
        {
        case SG_UCTMOVESELECT_VALUE:
            value = moveValue;
            break;
        case SG_UCTMOVESELECT_COUNT:
            value = moveCount;
            break;
        case SG_UCTMOVESELECT_BOUND:
            value = GetBound(m_rave, node, child);
            break;
        case SG_UCTMOVESELECT_ESTIMATE:
            value = GetValueEstimate(m_rave, child);
            break;
        default:
            SG_ASSERT(false);
            value = SG_UCTMOVESELECT_VALUE;
        }
        if (bestChild == 0 || value > bestValue)
        {
            bestChild = &child;
            bestValue = value;
        }
    }
    return bestChild;
}

void SgUctSearch::FindBestSequence(vector<SgMove>& sequence) const
{
    sequence.clear();
    const SgUctNode* current = &m_tree.Root();
    while (true)
    {
        current = FindBestChild(*current);
        if (current == 0)
            break;
        sequence.push_back(current->Move());
        if (! current->HasChildren())
            break;
    }
}

void SgUctSearch::GenerateAllMoves(std::vector<SgMoveInfo>& moves)
{
    if (m_threads.size() == 0)
        CreateThreads();
    moves.clear();
    OnStartSearch();
    SgUctThreadState& state = ThreadState(0);
    state.StartSearch();
    SgProvenNodeType type;
    state.GenerateAllMoves(0, moves, type);
}

float SgUctSearch::GetBound(bool useRave, const SgUctNode& node,
                            const SgUctNode& child) const
{
    size_t posCount = node.PosCount();
    return GetBound(useRave, Log(posCount), child);
}

float SgUctSearch::GetBound(bool useRave, float logPosCount, 
                            const SgUctNode& child) const
{
    float value;
    if (useRave)
        value = GetValueEstimateRave(child);
    else
        value = GetValueEstimate(false, child);
    if (m_biasTermConstant == 0.0)
        return value;
    else
    {
        float moveCount = static_cast<float>(child.MoveCount());
        float bound =
            value + m_biasTermConstant * sqrt(logPosCount / (moveCount + 1));
        return bound;
    }
}

SgUctTree& SgUctSearch::GetTempTree()
{
    m_tempTree.Clear();
    // Use NumberThreads() (not m_tree.NuAllocators()) and MaxNodes() (not
    // m_tree.MaxNodes()), because of the delayed thread (and thereby
    // allocator) creation in SgUctSearch
    if (m_tempTree.NuAllocators() != NumberThreads())
    {
        m_tempTree.CreateAllocators(NumberThreads());
        m_tempTree.SetMaxNodes(MaxNodes());
    }
    else if (m_tempTree.MaxNodes() != MaxNodes())
    {
    	m_tempTree.SetMaxNodes(MaxNodes());
    }
    return m_tempTree;
}

float SgUctSearch::GetValueEstimate(bool useRave, const SgUctNode& child) const
{
    float value = 0.f;
    float weightSum = 0.f;
    bool hasValue = false;
    if (child.HasMean())
    {
        float weight = static_cast<float>(child.MoveCount());
        value += weight * InverseEval(child.Mean());
        weightSum += weight;
        hasValue = true;
    }
    if (useRave && child.HasRaveValue())
    {
        float raveCount = child.RaveCount();
        float weight =
                raveCount
              / (  m_raveWeightParam1
                 + m_raveWeightParam2 * raveCount
                );
        value += weight * child.RaveValue();
        weightSum += weight;
        hasValue = true;
    }
    if (hasValue)
        return value / weightSum;
    else
        return m_firstPlayUrgency;
}

/** Optimized version of GetValueEstimate() if RAVE and not other
    estimators are used.
    Previously there were more estimators than move value and RAVE value,
    and in the future there may be again. GetValueEstimate() is easier to
    extend, this function is more optimized for the special case.
*/
float SgUctSearch::GetValueEstimateRave(const SgUctNode& child) const
{
    SG_ASSERT(m_rave);
    bool hasRave = child.HasRaveValue();
    float value;
    if (child.HasMean())
    {
        float moveValue = InverseEval(child.Mean());
        if (hasRave)
        {
            float moveCount = child.MoveCount();
            float raveCount = child.RaveCount();
            float weight =
                raveCount
                / (moveCount
                   * (m_raveWeightParam1 + m_raveWeightParam2 * raveCount)
                   + raveCount);
            value = weight * child.RaveValue() + (1.f - weight) * moveValue;
        }
        else
        {
            // This can happen only in lock-free multi-threading. Normally,
            // each move played in a position should also cause a RAVE value
            // to be added. But in lock-free multi-threading it can happen
            // that the move value was already updated but the RAVE value not
            SG_ASSERT(m_numberThreads > 1 && m_lockFree);
            value = moveValue;
        }
    }
    else if (hasRave)
        value = child.RaveValue();
    else
        value = m_firstPlayUrgency;
    SG_ASSERT(m_numberThreads > 1
              || fabs(value - GetValueEstimate(m_rave, child)) < 1e-3/*epsilon*/);
    return value;
}

string SgUctSearch::LastGameSummaryLine() const
{
    return SummaryLine(LastGameInfo());
}

float SgUctSearch::Log(float x) const
{
    // TODO: can we speed up the computation of the logarithm by taking
    // advantage of the fact that the argument is an integer type?
    // Maybe round result to integer (then it is simple the position of the
    // highest bit
#if SG_UCTFASTLOG
    return m_fastLog.Log(x);
#else
    return log(x);
#endif
}

/** Creates the children with the given moves and merges with existing
    children in the tree. */
void SgUctSearch::CreateChildren(SgUctThreadState& state, 
                                 const SgUctNode& node,
                                 bool deleteChildTrees)
{
    size_t threadId = state.m_threadId;
    if (! m_tree.HasCapacity(threadId, state.m_moves.size()))
    {
        Debug(state, str(format("SgUctSearch: maximum tree size %1% reached")
                         % m_tree.MaxNodes()));
        state.m_isTreeOutOfMem = true;
        m_isTreeOutOfMemory = true;
        SgSynchronizeThreadMemory();
        return;
    }
    m_tree.MergeChildren(threadId, node, state.m_moves, deleteChildTrees);
}

bool SgUctSearch::NeedToComputeKnowledge(const SgUctNode* current)
{
    if (m_knowledgeThreshold.empty())
        return false;
    for (std::size_t i = 0; i < m_knowledgeThreshold.size(); ++i)
    {
        const std::size_t threshold = m_knowledgeThreshold[i];
        if (current->KnowledgeCount() < threshold)
        {
            if (current->MoveCount() >= threshold)
            {
                // Mark knowledge computed immediately so other
                // threads fall through and do not waste time
                // re-computing this knowledge.
                SgUctNode* node = const_cast<SgUctNode*>(current);
                node->SetKnowledgeCount(threshold);
                SG_ASSERT(current->MoveCount());
                return true;
            }
            return false;
        }
    }
    return false;
}

void SgUctSearch::OnStartSearch()
{
    m_mpiSynchronizer->OnStartSearch(*this);
}

void SgUctSearch::OnEndSearch()
{
    m_mpiSynchronizer->OnEndSearch(*this);
}

/** Print time, mean, nodes searched, and PV */
void SgUctSearch::PrintSearchProgress(double currTime) const
{
    const int MAX_SEQ_PRINT_LENGTH = 15;
    const size_t MIN_MOVE_COUNT = 10;
    size_t rootMoveCount = m_tree.Root().MoveCount();
    float rootMean = m_tree.Root().Mean();
    ostringstream out;
    const SgUctNode* current = &m_tree.Root();
    out << fixed << setprecision(0)
        << SgTime::Format(currTime, true) << " | "
        << fixed << setprecision(3) << rootMean << " "
        << "| " << rootMoveCount << " ";
    for(int i = 0; i <= MAX_SEQ_PRINT_LENGTH && current->HasChildren(); ++i)
    {
        current = FindBestChild(*current);
        if (current == 0 || current->MoveCount() < MIN_MOVE_COUNT)
            break;
        if (i == 0)
            out << "|";
        if (i < MAX_SEQ_PRINT_LENGTH)
            out << " " << SgWritePoint(current->Move());
        else
            out << " *";
    }
    SgDebug() << out.str() << endl;
}

void SgUctSearch::OnSearchIteration(std::size_t gameNumber, int threadId,
                                    const SgUctGameInfo& info)
{
    const int DISPLAY_INTERVAL = 5;

    m_mpiSynchronizer->OnSearchIteration(*this, gameNumber, threadId, info);
    //JJG: Remove to create reproducable results
#if 0
    double currTime = m_timer.GetTime();

    if (threadId == 0 && currTime - m_lastScoreDisplayTime > DISPLAY_INTERVAL)
    {
        PrintSearchProgress(currTime);
        m_lastScoreDisplayTime = currTime;
    }
#endif
}

void SgUctSearch::PlayGame(SgUctThreadState& state, GlobalLock* lock)
{
    state.m_isTreeOutOfMem = false;
    state.GameStart();
    SgUctGameInfo& info = state.m_gameInfo;
    info.Clear(m_numberPlayouts);
    bool isTerminal;
    bool abortInTree = ! PlayInTree(state, isTerminal);

    // add a virtual loss to all nodes in path
    if (m_virtualLoss)
        m_tree.AddVirtualLoss(info.m_nodes);

    // The playout phase is always unlocked
    if (lock != 0)
        lock->unlock();

    size_t nuMovesInTree = info.m_inTreeSequence.size();

    // Play some "fake" playouts if node is a proven node
    if (! info.m_nodes.empty() && info.m_nodes.back()->IsProven())
    {
        for (size_t i = 0; i < m_numberPlayouts; ++i)
        {
            info.m_sequence[i] = info.m_inTreeSequence;
            info.m_skipRaveUpdate[i].assign(nuMovesInTree, false);
            float eval = info.m_nodes.back()->IsProvenWin() ? 1.0 : 0.0;
            size_t nuMoves = info.m_sequence[i].size();
            if (nuMoves % 2 != 0)
                eval = InverseEval(eval);
            info.m_aborted[i] = abortInTree || state.m_isTreeOutOfMem;
            info.m_eval[i] = eval;
        }
    }
    else 
    {
        state.StartPlayouts();
        for (size_t i = 0; i < m_numberPlayouts; ++i)
        {
            state.StartPlayout();
            info.m_sequence[i] = info.m_inTreeSequence;
            // skipRaveUpdate only used in playout phase
            info.m_skipRaveUpdate[i].assign(nuMovesInTree, false);
            bool abort = abortInTree || state.m_isTreeOutOfMem;
            if (! abort && ! isTerminal)
                abort = ! PlayoutGame(state, i);
            float eval;
            if (abort)
                eval = UnknownEval();
            else
                eval = state.Evaluate();
            size_t nuMoves = info.m_sequence[i].size();
            if (nuMoves % 2 != 0)
                eval = InverseEval(eval);
            info.m_aborted[i] = abort;
            info.m_eval[i] = eval;
            state.EndPlayout();
            state.TakeBackPlayout(nuMoves - nuMovesInTree);
        }
    }
    state.TakeBackInTree(nuMovesInTree);

    // End of unlocked part if ! m_lockFree
    if (lock != 0)
        lock->lock();

    // Remove the virtual loss
    if (m_virtualLoss)
        m_tree.RemoveVirtualLoss(info.m_nodes);

    UpdateTree(info);
    if (m_rave)
        UpdateRaveValues(state);
    UpdateStatistics(info);
}

/** Play game until it leaves the tree.
    @param state
    @param[out] isTerminal Was the sequence terminated because of a real
    terminal position (GenerateAllMoves() returned an empty list)?
    @return @c false, if game was aborted due to maximum length
 */
bool SgUctSearch::PlayInTree(SgUctThreadState& state, bool& isTerminal)
{
    vector<SgMove>& sequence = state.m_gameInfo.m_inTreeSequence;
    vector<const SgUctNode*>& nodes = state.m_gameInfo.m_nodes;
    const SgUctNode* root = &m_tree.Root();
    const SgUctNode* current = root;
    nodes.push_back(current);
    bool breakAfterSelect = false;
    isTerminal = false;
    bool deepenTree = false;
    while (true)
    {
        if (sequence.size() == m_maxGameLength)
            return false;
        if (current->IsProven())
            break;
        if (! current->HasChildren())
        {
            state.m_moves.clear();
            SgProvenNodeType provenType = SG_NOT_PROVEN;
            state.GenerateAllMoves(0, state.m_moves, provenType);
            if (current == root)
                ApplyRootFilter(state.m_moves);
            if (provenType != SG_NOT_PROVEN)
            {
                SgUctNode* node = const_cast<SgUctNode*>(current);
                node->SetProvenNodeType(provenType);
                break;
            }
            if (state.m_moves.empty())
            {
                isTerminal = true;
                break;
            }
            if (  deepenTree
               || current->MoveCount() >= m_expandThreshold
               )
            {
                deepenTree = false;
                ExpandNode(state, *current);
                if (state.m_isTreeOutOfMem)
                    return true;
                if (! deepenTree)
                    breakAfterSelect = true;
            }
            else
                break;
        }
        else if (NeedToComputeKnowledge(current))
        {
            m_statistics.m_knowledge++;
            deepenTree = false;
            SgProvenNodeType provenType = SG_NOT_PROVEN;
            bool truncate = state.GenerateAllMoves(current->KnowledgeCount(), 
                                                   state.m_moves,
                                                   provenType);
            if (current == root)
                ApplyRootFilter(state.m_moves);
            CreateChildren(state, *current, truncate);
            if (provenType != SG_NOT_PROVEN)
            {
                SgUctNode* node = const_cast<SgUctNode*>(current);
                node->SetProvenNodeType(provenType);
                break;
            }
            if (state.m_moves.empty())
            {
                isTerminal = true;
                break;
            }
            if (state.m_isTreeOutOfMem)
                return true;
            if (! deepenTree)
                breakAfterSelect = true;
        }
        current = &SelectChild(state.m_randomizeCounter, *current);
        nodes.push_back(current);
        SgMove move = current->Move();
        state.Execute(move);
        sequence.push_back(move);
        if (breakAfterSelect)
            break;
    }
    return true;
}

/** Finish the game using GeneratePlayoutMove().
    @param state The thread state.
    @param playout The number of the playout.
    @return @c false if game was aborted
*/
bool SgUctSearch::PlayoutGame(SgUctThreadState& state, std::size_t playout)
{
    SgUctGameInfo& info = state.m_gameInfo;
    vector<SgMove>& sequence = info.m_sequence[playout];
    vector<bool>& skipRaveUpdate = info.m_skipRaveUpdate[playout];
    while (true)
    {
        if (sequence.size() == m_maxGameLength)
            return false;
        bool skipRave = false;
        SgMove move = state.GeneratePlayoutMove(skipRave);
        if (move == SG_NULLMOVE)
            break;
        state.ExecutePlayout(move);
        sequence.push_back(move);
        skipRaveUpdate.push_back(skipRave);
    }
    return true;
}

float SgUctSearch::Search(std::size_t maxGames, double maxTime,
                          vector<SgMove>& sequence,
                          const vector<SgMove>& rootFilter,
                          SgUctTree* initTree,
                          SgUctEarlyAbortParam* earlyAbort)
{
    //JJG: Remove to create reproducable results
#if 0
    m_timer.Start();
#endif
    m_rootFilter = rootFilter;
    if (m_logGames)
    {
    	m_log.open(m_mpiSynchronizer->ToNodeFilename(m_logFileName).c_str());
        m_log << "StartSearch maxGames=" << maxGames << '\n';
    }
    m_maxGames = maxGames;
    m_maxTime = maxTime;
    m_earlyAbort.reset(0);
    //JJG: DISABLE EARLY ABORT
    //if (earlyAbort != 0)
        //m_earlyAbort.reset(new SgUctEarlyAbortParam(*earlyAbort));

    for (size_t i = 0; i < m_threads.size(); ++i)
    {
        m_threads[i]->m_state->m_isSearchInitialized = false;
    }
    StartSearch(rootFilter, initTree);
    size_t pruneMinCount = m_pruneMinCount;
    while (true)
    {
        m_isTreeOutOfMemory = false;
        SgSynchronizeThreadMemory();
        for (size_t i = 0; i < m_threads.size(); ++i)
            m_threads[i]->StartPlay();
        for (size_t i = 0; i < m_threads.size(); ++i)
            m_threads[i]->WaitPlayFinished();
        if (m_aborted || ! m_pruneFullTree)
            break;
        else
        {
            //JJG: Remove to create reproducable results
#if 0
            double startPruneTime = m_timer.GetTime();
            SgDebug() << "SgUctSearch: pruning nodes with count < "
                  << pruneMinCount << " (at time " << fixed << setprecision(1)
                  << startPruneTime << ")\n";
#endif
            SgUctTree& tempTree = GetTempTree();
            m_tree.CopyPruneLowCount(tempTree, pruneMinCount, true);
            int prunedSizePercentage =
                static_cast<int>(tempTree.NuNodes() * 100 / m_tree.NuNodes());
            //JJG: Remove to create reproducable results
#if 0
            SgDebug() << "SgUctSearch: pruned size: " << tempTree.NuNodes()
                      << " (" << prunedSizePercentage << "%) time: "
                      << (m_timer.GetTime() - startPruneTime) << "\n";
#endif
            if (prunedSizePercentage > 50)
                pruneMinCount *= 2;
            else
             	pruneMinCount = m_pruneMinCount; 
            m_tree.Swap(tempTree);
        }
    }
    EndSearch();
    //JJG: Remove to create reproducable results
#if 0
    m_statistics.m_time = m_timer.GetTime();
    if (m_statistics.m_time > numeric_limits<double>::epsilon())
        m_statistics.m_gamesPerSecond = GamesPlayed() / m_statistics.m_time;
#endif
    if (m_logGames)
        m_log.close();
    FindBestSequence(sequence);
    return (m_tree.Root().MoveCount() > 0) ? m_tree.Root().Mean() : 0.5;
}

/** Loop invoked by each thread for playing games. */
void SgUctSearch::SearchLoop(SgUctThreadState& state, GlobalLock* lock)
{
    if (! state.m_isSearchInitialized)
    {
        OnThreadStartSearch(state);
        state.m_isSearchInitialized = true;
    }

    if (NumberThreads() == 1 || m_lockFree)
        lock = 0;
    if (lock != 0)
        lock->lock();
    state.m_isTreeOutOfMem = false;
    while (! state.m_isTreeOutOfMem)
    {
        PlayGame(state, lock);
        OnSearchIteration(m_numberGames + 1, state.m_threadId,
                          state.m_gameInfo);
        if (m_logGames)
            m_log << SummaryLine(state.m_gameInfo) << '\n';
        ++m_numberGames;
        if (m_isTreeOutOfMemory)
            break;
        if (m_aborted || CheckAbortSearch(state))
        {
            m_aborted = true;
        	SgSynchronizeThreadMemory();
            break;
        }
    }
    if (lock != 0)
        lock->unlock();

    m_searchLoopFinished->wait();
    if (m_aborted || ! m_pruneFullTree)
        OnThreadEndSearch(state);
}

void SgUctSearch::OnThreadStartSearch(SgUctThreadState& state)
{
    m_mpiSynchronizer->OnThreadStartSearch(*this, state);
}

void SgUctSearch::OnThreadEndSearch(SgUctThreadState& state)
{
    m_mpiSynchronizer->OnThreadEndSearch(*this, state);
}

SgPoint SgUctSearch::SearchOnePly(size_t maxGames, double maxTime,
                                  float& value)
{
    if (m_threads.size() == 0)
        CreateThreads();
    OnStartSearch();
    // SearchOnePly is not multi-threaded.
    // It uses the state of the first thread.
    SgUctThreadState& state = ThreadState(0);
    state.StartSearch();
    vector<SgMoveInfo> moves;
    SgProvenNodeType provenType;
    state.GameStart();
    state.GenerateAllMoves(0, moves, provenType);
    vector<SgUctStatisticsBase> statistics(moves.size());
    size_t games = 0;
    //JJG: Remove to create reproducable results
#if 0
    m_timer.Start();
#endif
    SgUctGameInfo& info = state.m_gameInfo;
    //JJG: Remove to create reproducable results
#if 0
    while (games < maxGames && m_timer.GetTime() < maxTime && ! SgUserAbort())
#else
    while (games < maxGames && ! SgUserAbort())
#endif
    {
        for (size_t i = 0; i < moves.size(); ++i)
        {
            state.GameStart();
            info.Clear(1);
            SgMove move = moves[i].m_move;
            state.Execute(move);
            info.m_inTreeSequence.push_back(move);
            info.m_sequence[0].push_back(move);
            info.m_skipRaveUpdate[0].push_back(false);
            state.StartPlayouts();
            state.StartPlayout();
            bool abortGame = ! PlayoutGame(state, 0);
            float eval;
            if (abortGame)
                eval = UnknownEval();
            else
                eval = state.Evaluate();
            state.EndPlayout();
            state.TakeBackPlayout(info.m_sequence[0].size() - 1);
            state.TakeBackInTree(1);
            statistics[i].Add(info.m_sequence[0].size() % 2 == 0 ?
                              eval : InverseEval(eval));
            OnSearchIteration(games + 1, 0, info);
            ++games;
        }
    }
    SgMove bestMove = SG_NULLMOVE;
    for (size_t i = 0; i < moves.size(); ++i)
    {
        SgDebug() << SgWritePoint(moves[i].m_move) 
                  << ' ' << statistics[i].Mean() << '\n';
        if (bestMove == SG_NULLMOVE || statistics[i].Mean() > value)
        {
            bestMove = moves[i].m_move;
            value = statistics[i].Mean();
        }
    }
    return bestMove;
}

const SgUctNode& SgUctSearch::SelectChild(int& randomizeCounter, 
                                          const SgUctNode& node)
{
    bool useRave = m_rave;
    if (m_randomizeRaveFrequency > 0)
    {
        ++randomizeCounter;
        if (randomizeCounter % m_randomizeRaveFrequency == 0)
            useRave = false;
    }
    SG_ASSERT(node.HasChildren());
    size_t posCount = node.PosCount();
    if (posCount == 0)
        // If position count is zero, return first child
        return *SgUctChildIterator(m_tree, node);
    float logPosCount = Log(posCount);
    const SgUctNode* bestChild = 0;
    float bestUpperBound = 0;
    for (SgUctChildIterator it(m_tree, node); it; ++it)
    {
        const SgUctNode& child = *it;
        float bound = GetBound(useRave, logPosCount, child);
        if (bestChild == 0 || bound > bestUpperBound)
        {
            bestChild = &child;
            bestUpperBound = bound;
        }
    }
    SG_ASSERT(bestChild != 0);
    return *bestChild;
}

void SgUctSearch::SetNumberThreads(std::size_t n)
{
    SG_ASSERT(n >= 1);
    if (m_numberThreads == n)
        return;
    m_numberThreads = n;
    CreateThreads();
}

void SgUctSearch::SetRave(bool enable)
{
    if (enable && m_moveRange <= 0)
        throw SgException("RAVE not supported for this game");
    m_rave = enable;
}

void SgUctSearch::SetThreadStateFactory(SgUctThreadStateFactory* factory)
{
    SG_ASSERT(m_threadStateFactory.get() == 0);
    m_threadStateFactory.reset(factory);
    DeleteThreads();
    // Don't create states here, because this function could be called in the
    // constructor of the subclass, and the factory passes the search (which
    // is not fully constructed) as an argument to the Create() function
}

void SgUctSearch::StartSearch(const vector<SgMove>& rootFilter,
                              SgUctTree* initTree)
{
    if (m_threads.size() == 0)
        CreateThreads();
    if (m_numberThreads > 1 && SgTime::DefaultMode() == SG_TIME_CPU)
        // Using CPU time with multiple threads makes the measured time
        // and games/sec not very meaningful; the total cputime is not equal
        // to the total real time, even if there is no other load on the
        // machine, because the time, while threads are waiting for a lock
        // does not contribute to the cputime.
        SgWarning() << "SgUctSearch: using cpu time with multiple threads\n";
    m_raveWeightParam1 = 1.f / m_raveWeightInitial;
    m_raveWeightParam2 = 1.f / m_raveWeightFinal;
    if (initTree == 0)
        m_tree.Clear();
    else
    {
        m_tree.Swap(*initTree);
        if (m_tree.HasCapacity(0, m_tree.Root().NuChildren()))
            m_tree.ApplyFilter(0, m_tree.Root(), rootFilter);
        else
            SgWarning() <<
                "SgUctSearch: "
                "root filter not applied (tree reached maximum size)\n";
    }
    m_statistics.Clear();
    m_aborted = false;
    m_wasEarlyAbort = false;
    m_checkTimeInterval = 1;
    m_numberGames = 0;
    //JJG: Remove to create reproducable results
#if 0
    m_lastScoreDisplayTime = m_timer.GetTime();
#endif
    OnStartSearch();
    m_startRootMoveCount = m_tree.Root().MoveCount();
    for (size_t i = 0; i < m_threads.size(); ++i)
        ThreadState(i).StartSearch();
}

void SgUctSearch::EndSearch()
{
    OnEndSearch();
}

string SgUctSearch::SummaryLine(const SgUctGameInfo& info) const
{
    ostringstream buffer;
    const vector<const SgUctNode*>& nodes = info.m_nodes;
    for (size_t i = 1; i < nodes.size(); ++i)
    {
        const SgUctNode* node = nodes[i];
        SgMove move = node->Move();
        buffer << ' ' << MoveString(move) << " (" << fixed << setprecision(2)
               << node->Mean() << ',' << node->MoveCount() << ')';
    }
    for (size_t i = 0; i < info.m_eval.size(); ++i)
        buffer << ' ' << fixed << setprecision(2) << info.m_eval[i];
    return buffer.str();
}

void SgUctSearch::UpdateCheckTimeInterval(double time)
{
    if (time < numeric_limits<double>::epsilon())
        return;
    // Dynamically update m_checkTimeInterval (see comment at definition of
    // m_checkTimeInterval)
    float wantedTimeDiff = (m_maxTime > 1 ? 0.1 : 0.1 * m_maxTime);
    if (time < wantedTimeDiff / 10)
    {
        // Computing games per second might be unreliable for small times
        m_checkTimeInterval *= 2;
        return;
    }
    m_statistics.m_gamesPerSecond = GamesPlayed() / time;
    double gamesPerSecondPerThread =
        m_statistics.m_gamesPerSecond / m_numberThreads;
    m_checkTimeInterval =
        static_cast<size_t>(wantedTimeDiff * gamesPerSecondPerThread);
    if (m_checkTimeInterval == 0)
        m_checkTimeInterval = 1;
}

/** Update the RAVE values in the tree for both players after a game was
    played.
    @see SgUctSearch::Rave()
*/
void SgUctSearch::UpdateRaveValues(SgUctThreadState& state)
{
    for (size_t i = 0; i < m_numberPlayouts; ++i)
        UpdateRaveValues(state, i);
}

void SgUctSearch::UpdateRaveValues(SgUctThreadState& state,
                                   std::size_t playout)
{
    SgUctGameInfo& info = state.m_gameInfo;
    const vector<SgMove>& sequence = info.m_sequence[playout];
    if (sequence.size() == 0)
        return;
    SG_ASSERT(m_moveRange > 0);
    size_t* firstPlay = state.m_firstPlay.get();
    size_t* firstPlayOpp = state.m_firstPlayOpp.get();
    fill_n(firstPlay, m_moveRange, numeric_limits<size_t>::max());
    fill_n(firstPlayOpp, m_moveRange, numeric_limits<size_t>::max());
    const vector<const SgUctNode*>& nodes = info.m_nodes;
    const vector<bool>& skipRaveUpdate = info.m_skipRaveUpdate[playout];
    float eval = info.m_eval[playout];
    float invEval = InverseEval(eval);
    size_t nuNodes = nodes.size();
    size_t i = sequence.size() - 1;
    bool opp = (i % 2 != 0);

    // Update firstPlay, firstPlayOpp arrays using playout moves
    for ( ; i >= nuNodes; --i)
    {
        SG_ASSERT(i < skipRaveUpdate.size());
        SG_ASSERT(i < sequence.size());
        if (! skipRaveUpdate[i])
        {
            SgMove mv = sequence[i];
            size_t& first = (opp ? firstPlayOpp[mv] : firstPlay[mv]);
            if (i < first)
                first = i;
        }
        opp = ! opp;
    }

    while (true)
    {
        SG_ASSERT(i < skipRaveUpdate.size());
        SG_ASSERT(i < sequence.size());
        // skipRaveUpdate currently not used in in-tree phase
        SG_ASSERT(i >= info.m_inTreeSequence.size() || ! skipRaveUpdate[i]);
        if (! skipRaveUpdate[i])
        {
            SgMove mv = sequence[i];
            size_t& first = (opp ? firstPlayOpp[mv] : firstPlay[mv]);
            if (i < first)
                first = i;
            if (opp)
                UpdateRaveValues(state, playout, invEval, i,
                                 firstPlayOpp, firstPlay);
            else
                UpdateRaveValues(state, playout, eval, i,
                                 firstPlay, firstPlayOpp);
        }
        if (i == 0)
            break;
        --i;
        opp = ! opp;
    }
}

void SgUctSearch::UpdateRaveValues(SgUctThreadState& state,
                                   std::size_t playout, float eval,
                                   std::size_t i,
                                   const std::size_t firstPlay[],
                                   const std::size_t firstPlayOpp[])
{
    SG_ASSERT(i < state.m_gameInfo.m_nodes.size());
    const SgUctNode* node = state.m_gameInfo.m_nodes[i];
    if (! node->HasChildren())
        return;
    std::size_t len = state.m_gameInfo.m_sequence[playout].size();
    for (SgUctChildIterator it(m_tree, *node); it; ++it)
    {
        const SgUctNode& child = *it;
        SgMove mv = child.Move();
        size_t first = firstPlay[mv];
        SG_ASSERT(first >= i);
        if (first == numeric_limits<size_t>::max())
            continue;
        if  (m_raveCheckSame && SgUtil::InRange(firstPlayOpp[mv], i, first))
            continue;
        float weight;
        if (m_weightRaveUpdates)
            weight = 2.f - static_cast<float>(first - i) / (len - i);
        else
            weight = 1.f;
        m_tree.AddRaveValue(child, eval, weight);
    }
}

void SgUctSearch::UpdateStatistics(const SgUctGameInfo& info)
{
    m_statistics.m_movesInTree.Add(
                            static_cast<float>(info.m_inTreeSequence.size()));
    for (size_t i = 0; i < m_numberPlayouts; ++i)
    {
        m_statistics.m_gameLength.Add(
                               static_cast<float>(info.m_sequence[i].size()));
        m_statistics.m_aborted.Add(info.m_aborted[i] ? 1.f : 0.f);
    }
}

void SgUctSearch::UpdateTree(const SgUctGameInfo& info)
{
    float eval = 0;
    for (size_t i = 0; i < m_numberPlayouts; ++i)
        eval += info.m_eval[i];
    eval /= m_numberPlayouts;
    float inverseEval = InverseEval(eval);
    const vector<const SgUctNode*>& nodes = info.m_nodes;

    for (size_t i = 0; i < nodes.size(); ++i)
    {
        const SgUctNode& node = *nodes[i];
        const SgUctNode* father = (i > 0 ? nodes[i - 1] : 0);
        m_tree.AddGameResults(node, father, i % 2 == 0 ? eval : inverseEval, m_numberPlayouts);
    }
}

void SgUctSearch::WriteStatistics(ostream& out) const
{
    out << SgWriteLabel("Count") << m_tree.Root().MoveCount() << '\n'
        << SgWriteLabel("Nodes") << m_tree.NuNodes() << '\n';
    if (!m_knowledgeThreshold.empty())
        out << SgWriteLabel("Knowledge") 
            << m_statistics.m_knowledge << " (" << fixed << setprecision(1) 
            << m_statistics.m_knowledge * 100.0 / m_tree.Root().MoveCount()
            << "%)\n";
    m_statistics.Write(out);
    m_mpiSynchronizer->WriteStatistics(out);
}

//----------------------------------------------------------------------------
