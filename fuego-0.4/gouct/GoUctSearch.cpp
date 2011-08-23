//----------------------------------------------------------------------------
/** @file GoUctSearch.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctSearch.h"

#include <fstream>
#include <iostream>
#include "GoBoardUtil.h"
#include "GoNodeUtil.h"
#include "GoUctUtil.h"
#include "SgDebug.h"
#include "SgGameWriter.h"
#include "SgNode.h"
#include "SgUctTreeUtil.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

//----------------------------------------------------------------------------

const int MOVERANGE = SG_PASS + 1;

SgNode* AppendChild(SgNode* node, const string& comment)
{
    SgNode* child = node->NewRightMostSon();
    child->AddComment(comment);
    return child;
}

SgNode* AppendChild(SgNode* node, SgBlackWhite color, SgPoint move)
{
    SgNode* child = node->NewRightMostSon();
    SgPropID propId =
        (color == SG_BLACK ? SG_PROP_MOVE_BLACK : SG_PROP_MOVE_WHITE);
    child->Add(new SgPropMove(propId, move));
    return child;
}

/** Append game to saved simulations (used if m_keepGames is true) */
void AppendGame(SgNode* node, size_t gameNumber, int threadId,
                SgBlackWhite toPlay, const SgUctGameInfo& info)
{
    SG_ASSERT(node != 0);
    {
        ostringstream comment;
        comment << "Thread " << threadId << '\n'
                << "Game " << gameNumber << '\n';
        node = AppendChild(node, comment.str());
    }
    size_t nuMovesInTree = info.m_inTreeSequence.size();
    for (size_t i = 0; i < nuMovesInTree; ++i)
    {
        node = AppendChild(node, toPlay, info.m_inTreeSequence[i]);
        toPlay = SgOppBW(toPlay);
    }
    SgNode* lastInTreeNode = node;
    SgBlackWhite lastInTreeToPlay = toPlay;
    for (size_t i = 0; i < info.m_eval.size(); ++i)
    {
        node = lastInTreeNode;
        toPlay = lastInTreeToPlay;
        ostringstream comment;
        comment << "Playout " << i << '\n'
                << "Eval " << info.m_eval[i] << '\n'
                << "Aborted " << info.m_aborted[i] << '\n';
        node = AppendChild(node, comment.str());
        for (size_t j = nuMovesInTree; j < info.m_sequence[i].size(); ++j)
        {
            node = AppendChild(node, toPlay, info.m_sequence[i][j]);
            toPlay = SgOppBW(toPlay);
        }
    }
}

//----------------------------------------------------------------------------

} // namespace

//----------------------------------------------------------------------------

GoUctState::AssertionHandler::AssertionHandler(const GoUctState& state)
    : m_state(state)
{
}

void GoUctState::AssertionHandler::Run()
{
    m_state.Dump(SgDebug());
}

//----------------------------------------------------------------------------

GoUctState::GoUctState(std::size_t threadId, const GoBoard& bd)
    : SgUctThreadState(threadId, MOVERANGE),
      m_assertionHandler(*this),
      m_uctBd(bd),
      m_synchronizer(bd)
{
    m_synchronizer.SetSubscriber(m_bd);
    m_isInPlayout = false;
}

void GoUctState::Dump(ostream& out) const
{
    out << "GoUctState[" << m_threadId << "] ";
    if (m_isInPlayout)
        out << "playout board:\n" << m_uctBd;
    else
        out << "board:\n" << m_bd;
}

void GoUctState::Execute(SgMove move)
{
    SG_ASSERT(! m_isInPlayout);
    SG_ASSERT(move == SG_PASS || ! m_bd.Occupied(move));
    // Temporarily switch ko rule to SIMPLEKO to avoid slow full board
    // repetition test in GoBoard::Play()
    GoRestoreKoRule restoreKoRule(m_bd);
    m_bd.Rules().SetKoRule(GoRules::SIMPLEKO);
    m_bd.Play(move);
    SG_ASSERT(! m_bd.LastMoveInfo(GO_MOVEFLAG_ILLEGAL));
    ++m_gameLength;
}

void GoUctState::ExecutePlayout(SgMove move)
{
    SG_ASSERT(m_isInPlayout);
    SG_ASSERT(move == SG_PASS || ! m_uctBd.Occupied(move));
    m_uctBd.Play(move);
    ++m_gameLength;
}

void GoUctState::GameStart()
{
    m_isInPlayout = false;
    m_gameLength = 0;
}

void GoUctState::StartPlayout()
{
    m_uctBd.Init(m_bd);
}

void GoUctState::StartPlayouts()
{
    m_isInPlayout = true;
}

void GoUctState::StartSearch()
{
    m_synchronizer.UpdateSubscriber();
}

void GoUctState::TakeBackInTree(std::size_t nuMoves)
{
    for (size_t i = 0; i < nuMoves; ++i)
        m_bd.Undo();
}

void GoUctState::TakeBackPlayout(std::size_t nuMoves)
{
    m_gameLength -= nuMoves;
}

//----------------------------------------------------------------------------

GoUctSearch::GoUctSearch(GoBoard& bd, SgUctThreadStateFactory* factory)
    : SgUctSearch(factory, MOVERANGE),
      m_keepGames(false),
      m_liveGfxInterval(5000),
      m_toPlay(SG_BLACK),
      m_bd(bd),
      m_root(0),
      m_liveGfx(GOUCT_LIVEGFX_NONE)
{
    SetRaveCheckSame(true);
}

GoUctSearch::~GoUctSearch()
{
    if (m_root != 0)
        m_root->DeleteTree();
    m_root = 0;
}

std::string GoUctSearch::MoveString(SgMove move) const
{
    return SgPointUtil::PointToString(move);
}

void GoUctSearch::OnSearchIteration(std::size_t gameNumber, int threadId,
                                    const SgUctGameInfo& info)
{
    SgUctSearch::OnSearchIteration(gameNumber, threadId, info);

    if (m_liveGfx != GOUCT_LIVEGFX_NONE && threadId == 0
        && gameNumber % m_liveGfxInterval == 0)
    {
        SgDebug() << "gogui-gfx:\n";
        switch (m_liveGfx)
        {
        case GOUCT_LIVEGFX_COUNTS:
            GoUctUtil::GfxBestMove(*this, m_toPlay, SgDebug());
            GoUctUtil::GfxMoveValues(*this, m_toPlay, SgDebug());
            GoUctUtil::GfxCounts(Tree(), SgDebug());
            GoUctUtil::GfxStatus(*this, SgDebug());
            break;
        case GOUCT_LIVEGFX_SEQUENCE:
            GoUctUtil::GfxSequence(*this, m_toPlay, SgDebug());
            GoUctUtil::GfxStatus(*this, SgDebug());
            break;
        case GOUCT_LIVEGFX_NONE:
            SG_ASSERT(false); // Already checked above
            break;
        }
        SgDebug() << '\n';
    }
    if (! LockFree() && m_root != 0)
        AppendGame(m_root, gameNumber, threadId, m_toPlay, info);
}

void GoUctSearch::OnStartSearch()
{
    SgUctSearch::OnStartSearch();

    if (m_root != 0)
    {
        m_root->DeleteTree();
        m_root = 0;
    }
    if (m_keepGames)
    {
        m_root = GoNodeUtil::CreateRoot(m_bd);
        if (LockFree())
            SgWarning() <<
                "GoUctSearch: keep games will be ignored"
                " in lock free search\n";
    }
    m_toPlay = m_bd.ToPlay(); // Not needed if SetToPlay() was called
    for (SgBWIterator it; it; ++it)
        m_stones[*it] = m_bd.All(*it);
    int size = m_bd.Size();
    // Limit to avoid very long games if m_simpleKo
    int maxGameLength = min(3 * size * size,
                            GO_MAX_NUM_MOVES - m_bd.MoveNumber());
    SetMaxGameLength(maxGameLength);
    m_boardHistory.SetFromBoard(m_bd);
}

void GoUctSearch::SaveGames(const string& fileName) const
{
    if (MpiSynchronizer()->IsRootProcess())
    {
    if (m_root == 0)
        throw SgException("No games to save");
    ofstream out(fileName.c_str());
    SgGameWriter writer(out);
    writer.WriteGame(*m_root, true, 0, "", 1, 19);
    }
}

void GoUctSearch::SaveTree(std::ostream& out, int maxDepth) const
{
    GoUctUtil::SaveTree(Tree(), m_bd.Size(), m_stones, m_toPlay, out,
                        maxDepth);
}

SgBlackWhite GoUctSearch::ToPlay() const
{
    return m_toPlay;
}

//----------------------------------------------------------------------------

SgPoint GoUctSearchUtil::TrompTaylorPassCheck(SgPoint move,
                                              const GoUctSearch& search)
{
    const GoBoard& bd = search.Board();
    bool isFirstPass = (bd.GetLastMove() != SG_PASS);
    bool isTrompTaylorRules = bd.Rules().CaptureDead();
    if (move != SG_PASS || ! isTrompTaylorRules || ! isFirstPass)
        return move;
    float komi = bd.Rules().Komi().ToFloat();
    float trompTaylorScore = GoBoardUtil::TrompTaylorScore(bd, komi);
    if (search.ToPlay() != SG_BLACK)
        trompTaylorScore *= -1;
    const SgUctTree& tree = search.Tree();
    const SgUctNode& root = tree.Root();
    float value = root.Mean();
    float trompTaylorWinValue = (trompTaylorScore > 0 ? 1 : 0);
    if (value < trompTaylorWinValue)
        return move;
    SgDebug() << "GoUctSearchUtil::TrompTaylorPassCheck: bad pass move value="
              << value << " trompTaylorScore=" << trompTaylorScore << '\n';
    vector<SgMove> excludeMoves;
    excludeMoves.push_back(SG_PASS);
    const SgUctNode* bestChild = search.FindBestChild(root, &excludeMoves);
    if (bestChild == 0)
    {
        SgDebug() <<
            "GoUctSearchUtil::TrompTaylorPassCheck: "
            "(no second best move found)\n";
        return move;
    }
    return bestChild->Move();
}

//----------------------------------------------------------------------------
