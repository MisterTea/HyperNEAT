//----------------------------------------------------------------------------
/** @file GoUctBookBuilder.h
 */
//----------------------------------------------------------------------------

#ifndef GOBOOKBUILDER_H
#define GOBOOKBUILDER_H

#include <cmath>
#include <iostream>
#include <fstream>
#include <set>
#include "SgBookBuilder.h"
#include "SgThreadedWorker.h"
#include "GoBoard.h"
#include "GoAutoBook.h"
#include "GoBoardSynchronizer.h"

//----------------------------------------------------------------------------

/** Expands a Book using the given player to evaluate game positions.

    Supports multithreaded evaluation of children.

    @todo Copy settings from passed player to other players.

    @ingroup openingbook
*/
template<class PLAYER>
class GoUctBookBuilder : public SgBookBuilder
{
public:
    GoUctBookBuilder(const GoBoard& brd);
    
    ~GoUctBookBuilder();

    //---------------------------------------------------------------------
    
    void SetPlayer(PLAYER& player);

    /** Sets the state to start work from. */
    void SetState(GoAutoBook& book);

    //---------------------------------------------------------------------    

    float InverseEval(float eval) const;

    bool IsLoss(float eval) const;

    float Value(const SgBookNode& node) const;

    //---------------------------------------------------------------------    

    /** Number of players to use during leaf expansion. Each player
        may use a multi-threaded search. Should speed up the expansion
        of leaf states by a factor of (very close to) NumThreads(). */
    std::size_t NumThreads() const;

    /** See NumThreads() */
    void SetNumThreads(std::size_t num);

    /** Number of games to play when evaluation a state. */
    std::size_t NumGamesPerEvaluation() const;

    /** See NumGamesPerEvaluation. */
    void SetNumGamesPerEvaluation(std::size_t num);

    /** Number of games to play when sorting children. */
    std::size_t NumGamesPerSort() const;

    /** See NumGamesForSort() */
    void SetNumGamesPerSort(std::size_t num);

protected:
    void PrintMessage(std::string msg);

    void PlayMove(SgMove move);

    void UndoMove(SgMove move);

    bool GetNode(SgBookNode& node) const;

    void WriteNode(const SgBookNode& node);

    void FlushBook();

    void EnsureRootExists();

    bool GenerateMoves(std::vector<SgMove>& moves, float& value);

    void GetAllLegalMoves(std::vector<SgMove>& moves);

    void EvaluateChildren(const std::vector<SgMove>& childrenToDo,
                          std::vector<std::pair<SgMove, float> >& scores);
    void Init();

    void StartIteration();

    void EndIteration();

    void BeforeEvaluateChildren();

    void AfterEvaluateChildren();

    void Fini();

    void ClearAllVisited();
    
    void MarkAsVisited();
    
    bool HasBeenVisited();
        
private:
    /** Copyable worker. */
    class Worker
    {
    public:
        Worker(std::size_t id, PLAYER& player);

        float operator()(const SgMove& move);

    private:
        std::size_t m_id;
        
        PLAYER* m_player;
    };

    /** Book this builder is expanding */
    GoAutoBook* m_book;
   
    PLAYER* m_origPlayer;

    GoAutoBookState m_state;

    std::set<SgHashCode> m_visited;

    /** See NumberThreads() */
    std::size_t m_numThreads;

    std::size_t m_numGamesPerEvaluation;

    std::size_t m_numGamesPerSort;

    std::size_t m_num_evals;

    std::size_t m_num_widenings;

    std::size_t m_value_updates;

    std::size_t m_priority_updates;

    std::size_t m_internal_nodes;

    std::size_t m_leaf_nodes;

    std::size_t m_terminal_nodes;

    /** Players for each thread. */
    std::vector<PLAYER*> m_players;

    std::vector<Worker> m_workers;

    SgThreadedWorker<SgMove,float,Worker>* m_threadedWorker;

    void CreateWorkers();

    void DestroyWorkers();
};

//----------------------------------------------------------------------------

template<class PLAYER>
inline std::size_t GoUctBookBuilder<PLAYER>::NumThreads() const
{
    return m_numThreads;
}

template<class PLAYER>
inline void GoUctBookBuilder<PLAYER>::SetNumThreads(std::size_t num)
{
    m_numThreads = num;
}

template<class PLAYER>
inline std::size_t GoUctBookBuilder<PLAYER>
::NumGamesPerEvaluation() const
{
    return m_numGamesPerEvaluation;
}

template<class PLAYER>
inline void GoUctBookBuilder<PLAYER>
::SetNumGamesPerEvaluation(std::size_t num)
{
    m_numGamesPerEvaluation = num;
}

template<class PLAYER>
inline std::size_t GoUctBookBuilder<PLAYER>::NumGamesPerSort() const
{
    return m_numGamesPerSort;
}

template<class PLAYER>
inline void GoUctBookBuilder<PLAYER>::SetNumGamesPerSort(std::size_t num)
{
    m_numGamesPerSort = num;
}

//----------------------------------------------------------------------------

template<class PLAYER>
GoUctBookBuilder<PLAYER>::GoUctBookBuilder(const GoBoard& bd)
    : SgBookBuilder(), 
      m_book(0),
      m_origPlayer(0),
      m_state(bd),
      m_numThreads(1),
      m_numGamesPerEvaluation(10000),
      m_numGamesPerSort(10000)
{
    SetAlpha(30.0);
    SetExpandWidth(8);
}

template<class PLAYER>
GoUctBookBuilder<PLAYER>::~GoUctBookBuilder()
{
}

//----------------------------------------------------------------------------

/** Copies the player and board and creates the threads. */
template<class PLAYER>
void GoUctBookBuilder<PLAYER>::CreateWorkers()
{
    PrintMessage("GoUctBookBuilder::CreateWorkers()\n");
    for (std::size_t i = 0; i < m_numThreads; ++i)
    {
        PLAYER* newPlayer = new PLAYER(m_state.Board());

        // TODO: COPY SETTINGS SOMEHOW
        //newPlayer->CopySettingsFrom(m_origPlayer);
        
        // Always search, don't use forced moves
        newPlayer->SetForcedOpeningMoves(false);
        // Ensure all games are played; ie, do not use early count abort.
        newPlayer->Search().SetMoveSelect(SG_UCTMOVESELECT_ESTIMATE);
        // Should be enough for a 100k search. Needs 10GB 8 threaded.
        newPlayer->Search().SetMaxNodes(8500000);
        newPlayer->SetWriteDebugOutput(false);

        m_players.push_back(newPlayer);
        m_workers.push_back(Worker(i, *m_players[i]));
    }
    m_threadedWorker 
        = new SgThreadedWorker<SgMove,float,Worker>(m_workers);
}

/** Destroys copied players, boards, and threads. */
template<class PLAYER>
void GoUctBookBuilder<PLAYER>::DestroyWorkers()
{
    PrintMessage("GoUctBookBuilder::DestroyWorkers()\n");
    for (std::size_t i = 0; i < m_numThreads; ++i)
        delete m_players[i];
    delete m_threadedWorker;
    m_workers.clear();
    m_players.clear();
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::Init()
{
    CreateWorkers();
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::Fini()
{
    DestroyWorkers();
}

//----------------------------------------------------------------------------

template<class PLAYER>
GoUctBookBuilder<PLAYER>::Worker::Worker(std::size_t id, PLAYER& player)

    : m_id(id), 
      m_player(&player)
{
}

template<class PLAYER>
float GoUctBookBuilder<PLAYER>::Worker::operator()(const SgMove& move)
{
    m_player->UpdateSubscriber();
    if (move >= 0)
        m_player->Board().Play(move);
    m_player->GenMove(SgTimeRecord(true, 9999), m_player->Board().ToPlay());
    GoUctSearch& search 
        = dynamic_cast<GoUctSearch&>(m_player->Search());
    float score = search.Tree().Root().Mean();
    return score;
}

//----------------------------------------------------------------------------

template<class PLAYER>
inline void GoUctBookBuilder<PLAYER>::SetPlayer(PLAYER& player)
{
    m_origPlayer = &player;
}

template<class PLAYER>
inline void GoUctBookBuilder<PLAYER>::SetState(GoAutoBook& book)
{
    m_book = &book;
    m_state.Synchronize();
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::PrintMessage(std::string msg)
{
    SgDebug() << msg;
}

template<class PLAYER>
inline float GoUctBookBuilder<PLAYER>::InverseEval(float eval) const
{
    return 1.0 - eval;
}

template<class PLAYER>
inline bool GoUctBookBuilder<PLAYER>::IsLoss(float eval) const
{
    return eval < -100;
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::PlayMove(SgMove move)
{
    m_state.Play(move);
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::UndoMove(SgMove move)
{
    SG_UNUSED(move);
    m_state.Undo();
}

template<class PLAYER>
bool GoUctBookBuilder<PLAYER>::GetNode(SgBookNode& node) const
{
    return m_book->Get(m_state, node);
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::WriteNode(const SgBookNode& node)
{
    m_book->Put(m_state, node);
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::FlushBook()
{
    SgDebug() << "Flushing DB...\n";
    m_book->Flush();
}

template<class PLAYER>
float GoUctBookBuilder<PLAYER>::Value(const SgBookNode& node) const
{
    return node.m_value;
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::GetAllLegalMoves(std::vector<SgMove>& moves)
{
    for (GoBoard::Iterator it(m_state.Board()); it; ++it)
        if (m_state.Board().IsLegal(*it))
            moves.push_back(*it);
}

/** Creates root node if necessary. */
template<class PLAYER>
void GoUctBookBuilder<PLAYER>::EnsureRootExists()
{
    SgBookNode root;
    if (!GetNode(root))
    {
        BeforeEvaluateChildren();
        PrintMessage("Creating root node...\n");
        float value = m_workers[0](SG_NULLMOVE);
        WriteNode(SgBookNode(value));
    }
}

/** Computes an ordered set of moves to consider. */
template<class PLAYER>
bool GoUctBookBuilder<PLAYER>::GenerateMoves(std::vector<SgMove>& moves,
                                             float& value)
{
    SG_UNUSED(value);

    // Search for a few seconds.
    SgDebug() << m_state.Board() << '\n';
    m_players[0]->SetMaxGames(m_numGamesPerSort);
    m_workers[0](SG_NULLMOVE);
    std::vector<std::pair<int, SgMove> > ordered;
    // Store counts for each move in vector.
    {
        const SgUctTree& tree = m_players[0]->Search().Tree();
        const SgUctNode& root = tree.Root();
        for (GoBoard::Iterator it(m_state.Board()); it; ++it)
            if (m_state.Board().IsLegal(*it))
            {
                SgMove move = *it;
                const SgUctNode* node = 
                    SgUctTreeUtil::FindChildWithMove(tree, root, move);
                if (node && node->PosCount() > 0)
                {
                    ordered.push_back(std::make_pair(-node->PosCount(), move));
                }
            }
    }
    // Sort moves based on count of this search. 
    std::stable_sort(ordered.begin(), ordered.end());
    for (std::size_t i = 0; i < ordered.size(); ++i)
        moves.push_back(ordered[i].second);
    SgDebug() << '\n';
    return false;
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::BeforeEvaluateChildren()
{
    for (std::size_t i = 0; i < m_numThreads; ++i)
        m_players[i]->SetMaxGames(m_numGamesPerEvaluation);
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>
::EvaluateChildren(const std::vector<SgMove>& childrenToDo,
                   std::vector<std::pair<SgMove, float> >& scores)
{
    SgDebug() << "Evaluating children:";
    for (std::size_t i = 0; i < childrenToDo.size(); ++i)
        SgDebug() << ' ' << SgWritePoint(childrenToDo[i]);
    SgDebug() << '\n';
    m_threadedWorker->DoWork(childrenToDo, scores);
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::AfterEvaluateChildren()
{
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::StartIteration()
{
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::EndIteration()
{
}

template<class PLAYER>
void GoUctBookBuilder<PLAYER>::ClearAllVisited()
{
    m_visited.clear();
}
    
template<class PLAYER>
void GoUctBookBuilder<PLAYER>::MarkAsVisited()
{
    m_visited.insert(m_state.GetHashCode());
}
    
template<class PLAYER>
bool GoUctBookBuilder<PLAYER>::HasBeenVisited()
{
    return m_visited.count(m_state.GetHashCode()) == 1;
}

//----------------------------------------------------------------------------

#endif // GOBOOKBUILDER_HPP
