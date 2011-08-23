//----------------------------------------------------------------------------
/** @file GoBoard.cpp
    See GoBoard.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoBoard.h"

#include <boost/static_assert.hpp>
#include <algorithm>
#include "GoInit.h"
#include "SgNbIterator.h"
#include "SgStack.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

/** Do a consistency check.
    Check some data structures for consistency after and before each play
    and undo (and at some other places).
    This is an expensive check and therefore has to be enabled at compile
    time.
*/
const bool CONSISTENCY = false;

void UpdateChanges(SgPoint p, SgSList<SgPoint,SG_MAXPOINT>& changes,
                   int& nuChanges)
{
    if (! changes.Exclude(p))
        changes.PushBack(p);
    ++nuChanges;
}

} // namespace

//----------------------------------------------------------------------------

GoBoard::GoBoard(int size, const GoSetup& setup, const GoRules& rules)
    : m_snapshot(new Snapshot()),
      m_const(size),
      m_blockList(new SgSList<Block,GO_MAX_NUM_MOVES>()),
      m_moves(new SgSList<StackEntry,GO_MAX_NUM_MOVES>())

{
    GoInitCheck();
    Init(size, rules, setup);
}

GoBoard::~GoBoard()
{
    delete m_blockList;
    m_blockList = 0;
    delete m_moves;
    m_moves = 0;
}

void GoBoard::CheckConsistency() const
{
    if (! CONSISTENCY)
        return;
    int numberBlack = 0;
    int numberWhite = 0;
    for (SgPoint p = 0; p < SG_MAXPOINT; ++p)
    {
        if (IsBorder(p))
            continue;
        int c = m_state.m_color[p];
        SG_ASSERT_EBW(c);
        int n = 0;
        for (SgNb4Iterator it(p); it; ++it)
            if (m_state.m_color[*it] == SG_EMPTY)
                ++n;
        SG_ASSERT(n == NumEmptyNeighbors(p));
        n = 0;
        for (SgNb4Iterator it(p); it; ++it)
            if (m_state.m_color[*it] == SG_BLACK)
                ++n;
        SG_ASSERT(n == NumNeighbors(p, SG_BLACK));
        n = 0;
        for (SgNb4Iterator it(p); it; ++it)
            if (m_state.m_color[*it] == SG_WHITE)
                ++n;
        SG_ASSERT(n == NumNeighbors(p, SG_WHITE));
        if (c == SG_BLACK || c == SG_WHITE)
        {
            SG_ASSERT(m_state.m_all[c].Contains(p));
            CheckConsistencyBlock(p);
        }
        if (c == SG_BLACK)
            ++numberBlack;
        if (c == SG_WHITE)
            ++numberWhite;
        if (c == SG_EMPTY)
            SG_ASSERT(m_state.m_block[p] == 0);
    }
    SG_ASSERT(m_state.m_all[SG_BLACK].Size() == numberBlack);
    SG_ASSERT(m_state.m_all[SG_WHITE].Size() == numberWhite);
}

void GoBoard::CheckConsistencyBlock(SgPoint point) const
{
    SG_ASSERT(Occupied(point));
    SgBlackWhite color = GetColor(point);
    GoPointList stones;
    Block::LibertyList liberties;
    SgMarker mark;
    SgStack<SgPoint,SG_MAXPOINT> stack;
    stack.Push(point);
    while (! stack.IsEmpty())
    {
        SgPoint p = stack.Pop();
        if (IsBorder(p) || ! mark.NewMark(p))
            continue;
        if (GetColor(p) == color)
        {
            stones.PushBack(p);
            stack.Push(p - SG_NS);
            stack.Push(p - SG_WE);
            stack.Push(p + SG_WE);
            stack.Push(p + SG_NS);
        }
        else if (GetColor(p) == SG_EMPTY)
            liberties.PushBack(p);
    }
    const Block* block = m_state.m_block[point];
    SG_DEBUG_ONLY(block);
    SG_ASSERT(stones.Contains(block->Anchor()));
    SG_ASSERT(color == block->Color());
    SG_ASSERT(stones.SameElements(block->Stones()));
    SG_ASSERT(liberties.SameElements(block->Liberties()));
    SG_ASSERT(stones.Length() == NumStones(point));
}

bool GoBoard::CheckKo(SgBlackWhite player)
{
    if (! FullBoardRepetition())
        return true;
    m_moveInfo.set(GO_MOVEFLAG_REPETITION);
    if (AnyRepetitionAllowed())
    {
        if (m_koLoser != SG_EMPTY && m_koLoser != player)
            ++m_state.m_koLevel;
        return true;
    }
    if (KoRepetitionAllowed()
        && (m_koLoser != player)
        && (! m_koModifiesHash || (m_state.m_koLevel < MAX_KOLEVEL))
        && (m_koColor != SgOppBW(player)))
    {
        ++m_state.m_koLevel;
        m_koColor = player;
        if (m_koModifiesHash)
            m_state.m_hash.XorWinKo(m_state.m_koLevel, m_koColor);
        return true;
    }
    return false;
}

void GoBoard::AddLibToAdjBlocks(SgPoint p)
{
    if (NumNeighbors(p, SG_BLACK) + NumNeighbors(p, SG_WHITE) == 0)
        return;
    SgSList<Block*,4> blocks = GetAdjacentBlocks(p);
    for (SgSList<Block*,4>::Iterator it(blocks); it; ++it)
    {
        Block* block = *it;
        if (block != 0)
            block->AppendLiberty(p);
    }
}

void GoBoard::AddLibToAdjBlocks(SgPoint p, SgBlackWhite c)
{
    if (NumNeighbors(p, c) == 0)
        return;
    SgSList<Block*,4> blocks = GetAdjacentBlocks(p, c);
    for (SgSList<Block*,4>::Iterator it(blocks); it; ++it)
    {
        Block* block = *it;
        if (block != 0)
            block->AppendLiberty(p);
    }
}

void GoBoard::AddStoneToBlock(SgPoint p, SgBlackWhite c, Block* block,
                              StackEntry& entry)
{
    SG_DEBUG_ONLY(c);
    // Stone already placed
    SG_ASSERT(IsColor(p, c));
    block->AppendStone(p);
    entry.m_newLibs.Clear();
    if (IsEmpty(p - SG_NS) && ! IsAdjacentTo(p - SG_NS, block))
    {
        block->AppendLiberty(p - SG_NS);
        entry.m_newLibs.PushBack(p - SG_NS);
    }
    if (IsEmpty(p - SG_WE) && ! IsAdjacentTo(p - SG_WE, block))
    {
        block->AppendLiberty(p - SG_WE);
        entry.m_newLibs.PushBack(p - SG_WE);
    }
    if (IsEmpty(p + SG_WE) && ! IsAdjacentTo(p + SG_WE, block))
    {
        block->AppendLiberty(p + SG_WE);
        entry.m_newLibs.PushBack(p + SG_WE);
    }
    if (IsEmpty(p + SG_NS) && ! IsAdjacentTo(p + SG_NS, block))
    {
        block->AppendLiberty(p + SG_NS);
        entry.m_newLibs.PushBack(p + SG_NS);
    }
    entry.m_oldAnchor = block->Anchor();
    block->UpdateAnchor(p);
    m_state.m_block[p] = block;
}

GoBoard::Block& GoBoard::CreateNewBlock()
{
    // Reuse without initialization
    m_blockList->Resize(m_blockList->Length() + 1);
    Block& block = m_blockList->Last();
    return block;
}

void GoBoard::CreateSingleStoneBlock(SgPoint p, SgBlackWhite c)
{
    // Stone already placed
    SG_ASSERT(IsColor(p, c));
    SG_ASSERT(NumNeighbors(p, c) == 0);
    Block& block = CreateNewBlock();
    block.Init(c, p);
    if (IsEmpty(p - SG_NS))
        block.AppendLiberty(p - SG_NS);
    if (IsEmpty(p - SG_WE))
        block.AppendLiberty(p - SG_WE);
    if (IsEmpty(p + SG_WE))
        block.AppendLiberty(p + SG_WE);
    if (IsEmpty(p + SG_NS))
        block.AppendLiberty(p + SG_NS);
    m_state.m_block[p] = &block;
}

SgSList<GoBoard::Block*,4> GoBoard::GetAdjacentBlocks(SgPoint p) const
{
    SgSList<Block*,4> result;
    if (NumNeighbors(p, SG_BLACK) > 0 || NumNeighbors(p, SG_WHITE) > 0)
    {
        Block* block;
        if ((block = m_state.m_block[p - SG_NS]) != 0)
            result.PushBack(block);
        if ((block = m_state.m_block[p - SG_WE]) != 0
            && ! result.Contains(block))
            result.PushBack(block);
        if ((block = m_state.m_block[p + SG_WE]) != 0
            && ! result.Contains(block))
            result.PushBack(block);
        if ((block = m_state.m_block[p + SG_NS]) != 0
            && ! result.Contains(block))
            result.PushBack(block);
    }
    return result;
}

SgSList<GoBoard::Block*,4> GoBoard::GetAdjacentBlocks(SgPoint p,
                                                      SgBlackWhite c) const
{
    SgSList<Block*,4> result;
    if (NumNeighbors(p, c) > 0)
    {
        Block* block;
        if (IsColor(p - SG_NS, c))
            result.PushBack(m_state.m_block[p - SG_NS]);
        if (IsColor(p - SG_WE, c)
            && ! result.Contains((block = m_state.m_block[p - SG_WE])))
            result.PushBack(block);
        if (IsColor(p + SG_WE, c)
            && ! result.Contains((block = m_state.m_block[p + SG_WE])))
            result.PushBack(block);
        if (IsColor(p + SG_NS, c)
            && ! result.Contains((block = m_state.m_block[p + SG_NS])))
            result.PushBack(block);
    }
    return result;
}

bool GoBoard::IsAdjacentTo(SgPoint p, const GoBoard::Block* block) const
{
    return   m_state.m_block[p - SG_NS] == block
          || m_state.m_block[p - SG_WE] == block
          || m_state.m_block[p + SG_WE] == block
          || m_state.m_block[p + SG_NS] == block;
}

void GoBoard::MergeBlocks(SgPoint p, SgBlackWhite c,
                          const SgSList<Block*,4>& adjBlocks)
{
    // Stone already placed
    SG_ASSERT(IsColor(p, c));
    SG_ASSERT(NumNeighbors(p, c) > 1);
    Block& block = CreateNewBlock();
    block.Init(c, p);
    SgReserveMarker reserve(m_marker);
    SG_UNUSED(reserve);
    m_marker.Clear();
    for (SgSList<Block*,4>::Iterator it(adjBlocks); it; ++it)
    {
        Block* adjBlock = *it;
        for (Block::StoneIterator stn(adjBlock->Stones()); stn; ++stn)
        {
            block.AppendStone(*stn);
            m_state.m_block[*stn] = &block;
        }
        for (Block::LibertyIterator lib(adjBlock->Liberties()); lib; ++lib)
            if (m_marker.NewMark(*lib))
                block.AppendLiberty(*lib);
        block.UpdateAnchor(adjBlock->Anchor());
    }
    m_state.m_block[p] = &block;
    if (IsEmpty(p - SG_NS) && m_marker.NewMark(p - SG_NS))
        block.AppendLiberty(p - SG_NS);
    if (IsEmpty(p - SG_WE) && m_marker.NewMark(p - SG_WE))
        block.AppendLiberty(p - SG_WE);
    if (IsEmpty(p + SG_WE) && m_marker.NewMark(p + SG_WE))
        block.AppendLiberty(p + SG_WE);
    if (IsEmpty(p + SG_NS) && m_marker.NewMark(p + SG_NS))
        block.AppendLiberty(p + SG_NS);
}

void GoBoard::RemoveLibFromAdjBlocks(SgPoint p, SgBlackWhite c)
{
    if (NumNeighbors(p, c) == 0)
        return;
    SgSList<Block*,4> blocks = GetAdjacentBlocks(p, c);
    for (SgSList<Block*,4>::Iterator it(blocks); it; ++it)
        (*it)->ExcludeLiberty(p);
}

void GoBoard::RestoreKill(Block* block, SgBlackWhite c)
{
    SgBlackWhite opp = SgOppBW(c);
    for (Block::StoneIterator it(block->Stones()); it; ++it)
    {
        SgPoint stn = *it;
        AddStone(stn, c);
        m_state.m_block[stn] = block;
        RemoveLibFromAdjBlocks(stn, opp);
    }
    int nuStones = block->Stones().Length();
    m_state.m_numStones[c] += nuStones;
    m_state.m_prisoners[c] -= nuStones;
    SG_ASSERT(m_state.m_prisoners[c] >= 0);
}

void GoBoard::UpdateBlocksAfterAddStone(SgPoint p, SgBlackWhite c,
                                        StackEntry& entry)
{
    // Stone already placed
    SG_ASSERT(IsColor(p, c));
    if (NumNeighbors(p, c) == 0)
    {
        entry.m_stoneAddedTo = 0;
        CreateSingleStoneBlock(p, c);
        entry.m_merged.Clear();
    }
    else
    {
        SgSList<Block*,4> adjBlocks = GetAdjacentBlocks(p, c);
        if (adjBlocks.Length() == 1)
        {
            Block* block = adjBlocks[0];
            AddStoneToBlock(p, c, block, entry);
            entry.m_stoneAddedTo = block;
        }
        else
        {
            entry.m_stoneAddedTo = 0;
            MergeBlocks(p, c, adjBlocks);
            entry.m_merged = adjBlocks;
        }
    }
}

void GoBoard::UpdateBlocksAfterUndo(const StackEntry& entry)
{
    SgPoint p = entry.m_point;
    if (IsPass(p))
        return;
    SgBlackWhite c = entry.m_color;
    Block* block = entry.m_suicide;
    if (block != 0)
        RestoreKill(block, c);
    RemoveStone(p);
    --m_state.m_numStones[c];
    m_state.m_block[p] = 0;
    Block* stoneAddedTo = entry.m_stoneAddedTo;
    if (stoneAddedTo != 0)
    {
        stoneAddedTo->PopStone();
        for (SgSList<SgPoint,4>::Iterator it(entry.m_newLibs); it; ++it)
            stoneAddedTo->ExcludeLiberty(*it);
        stoneAddedTo->SetAnchor(entry.m_oldAnchor);
    }
    else
    {
        for (SgSList<Block*,4>::Iterator it(entry.m_merged); it; ++it)
        {
            Block* block = *it;
            for (Block::StoneIterator stn(block->Stones()); stn; ++stn)
                m_state.m_block[*stn] = block;
        }
        m_blockList->PopBack();
    }
    for (SgSList<Block*,4>::Iterator it(entry.m_killed); it; ++it)
        RestoreKill(*it, SgOppBW(entry.m_color));
    AddLibToAdjBlocks(p);
}

void GoBoard::Init(int size, const GoRules& rules, const GoSetup& setup)
{
    m_rules = rules;
    m_size = size;
    SG_ASSERTRANGE(m_size, SG_MIN_SIZE, SG_MAX_SIZE);
    m_state.m_hash.Clear();
    m_moves->Clear();
    m_state.m_prisoners[SG_BLACK] = 0;
    m_state.m_prisoners[SG_WHITE] = 0;
    m_state.m_numStones[SG_BLACK] = 0;
    m_state.m_numStones[SG_WHITE] = 0;
    m_countPlay = 0;
    m_state.m_koPoint = SG_NULLPOINT;
    m_allowAnyRepetition = false;
    m_allowKoRepetition = false;
    m_koColor = SG_EMPTY;
    m_koLoser = SG_EMPTY;
    m_state.m_koLevel = 0;
    m_koModifiesHash = true;
    m_const.ChangeSize(m_size);
    for (SgPoint p = 0; p < SG_MAXPOINT; ++p)
    {
        m_state.m_color[p] = SG_BORDER;
        m_isBorder[p] = true;
    }
    m_state.m_isFirst.Fill(true);
    m_state.m_isNewPosition = true;
    for (SgGrid row = 1; row <= m_size; ++row)
        for (SgGrid col = 1; col <= m_size; ++col)
        {
            SgPoint p = SgPointUtil::Pt(col, row);
            m_state.m_color[p] = SG_EMPTY;
            m_isBorder[p] = false;
        }
    m_state.m_all.Clear();
    m_state.m_empty.Clear();
    for (SgPoint p = 0; p < SG_MAXPOINT; ++p)
    {
        m_state.m_nuNeighbors[SG_BLACK][p] = 0;
        m_state.m_nuNeighbors[SG_WHITE][p] = 0;
        if (IsBorder(p))
            m_state.m_nuNeighborsEmpty[p] = 4;
        else
        {
            m_state.m_empty.Include(p);
            m_state.m_nuNeighborsEmpty[p] = 0;
            for (SgNb4Iterator it(p); it; ++it)
                if (IsEmpty(*it))
                    ++m_state.m_nuNeighborsEmpty[p];
        }
    }
    m_setup = setup;
    for (SgBWIterator c; c; ++c)
        for (SgSetIterator it(setup.m_stones[*c]); it; ++it)
        {
            SgPoint p = *it;
            SG_ASSERT(IsValidPoint(p));
            SG_ASSERT(IsEmpty(p));
            AddStone(p, *c);
            ++m_state.m_numStones[*c];
            m_state.m_hash.XorStone(p, *c);
            m_state.m_isFirst[p] = false;
        }
    m_state.m_toPlay = setup.m_player;
    m_blockList->Clear();
    m_state.m_block.Fill(0);
    for (GoBoard::Iterator it(*this); it; ++it)
    {
        SgBoardColor c = m_state.m_color[*it];
        if (c != SG_EMPTY && m_state.m_block[*it] == 0)
        {
            GoBoard::Block& block = CreateNewBlock();
            InitBlock(block, c, *it);
        }
    }
    m_snapshot->m_moveNumber = -1;
    CheckConsistency();
}

void GoBoard::InitBlock(GoBoard::Block& block, SgBlackWhite c, SgPoint anchor)
{
    SG_ASSERT_BW(c);
    Block::LibertyList liberties;
    GoPointList stones;
    SgReserveMarker reserve(m_marker);
    m_marker.Clear();
    SgStack<SgPoint,SG_MAX_ONBOARD> stack;
    stack.Push(anchor);
    m_marker.NewMark(anchor);
    while (! stack.IsEmpty())
    {
        SgPoint p = stack.Pop();
        if (m_state.m_color[p] == SG_EMPTY)
        {
            if (! liberties.Contains(p))
                liberties.PushBack(p);
        }
        else if (m_state.m_color[p] == c)
        {
            stones.PushBack(p);
            m_state.m_block[p] = &block;
            for (SgNb4Iterator it(p); it; ++it)
                if (! m_isBorder[*it] && m_marker.NewMark(*it))
                    stack.Push(*it);
        }
    }
    block.Init(c, anchor, stones, liberties);
}

GoPlayerMove GoBoard::Move(int i) const
{
    const StackEntry& entry = (*m_moves)[i];
    SgPoint p = entry.m_point;
    SgBlackWhite c = entry.m_color;
    return GoPlayerMove(c, p);
}

bool GoBoard::StackOverflowLikely() const
{
    return (MoveNumber() > GO_MAX_NUM_MOVES - 50);
}

int GoBoard::AdjacentBlocks(SgPoint point, int maxLib, SgPoint anchors[],
                            int maxAnchors) const
{
    SG_DEBUG_ONLY(maxAnchors);
    SG_ASSERT(Occupied(point));
    const SgBlackWhite other = SgOppBW(GetStone(point));
    int n = 0;
    SgReserveMarker reserve(m_marker);
    SG_UNUSED(reserve);
    m_marker.Clear();
    for (GoBoard::StoneIterator it(*this, point); it; ++it)
    {
        if (NumNeighbors(*it, other) > 0)
        {
            SgPoint p = *it;
            if (IsColor(p - SG_NS, other)
                && m_marker.NewMark(Anchor(p - SG_NS))
                && AtMostNumLibs(p - SG_NS, maxLib))
                anchors[n++] = Anchor(p - SG_NS);
            if (IsColor(p - SG_WE, other)
                && m_marker.NewMark(Anchor(p - SG_WE))
                && AtMostNumLibs(p - SG_WE, maxLib))
                anchors[n++] = Anchor(p - SG_WE);
            if (IsColor(p + SG_WE, other)
                && m_marker.NewMark(Anchor(p + SG_WE))
                && AtMostNumLibs(p + SG_WE, maxLib))
                anchors[n++] = Anchor(p + SG_WE);
            if (IsColor(p + SG_NS, other)
                && m_marker.NewMark(Anchor(p + SG_NS))
                && AtMostNumLibs(p + SG_NS, maxLib))
                anchors[n++] = Anchor(p + SG_NS);
        }
    };
    // Detect array overflow.
    SG_ASSERT(n < maxAnchors);
    anchors[n] = SG_ENDPOINT;
    return n;
}

void GoBoard::NeighborBlocks(SgPoint p, SgBlackWhite c,
                             SgPoint anchors[]) const
{
    SG_ASSERT(IsEmpty(p));
    SgReserveMarker reserve(m_marker);
    SG_UNUSED(reserve);
    m_marker.Clear();
    int i = 0;
    if (NumNeighbors(p, c) > 0)
    {
        if (IsColor(p - SG_NS, c) && m_marker.NewMark(Anchor(p - SG_NS)))
            anchors[i++] = Anchor(p - SG_NS);
        if (IsColor(p - SG_WE, c) && m_marker.NewMark(Anchor(p - SG_WE)))
            anchors[i++] = Anchor(p - SG_WE);
        if (IsColor(p + SG_WE, c) && m_marker.NewMark(Anchor(p + SG_WE)))
            anchors[i++] = Anchor(p + SG_WE);
        if (IsColor(p + SG_NS, c) && m_marker.NewMark(Anchor(p + SG_NS)))
            anchors[i++] = Anchor(p + SG_NS);
    }
    anchors[i] = SG_ENDPOINT;
}

void GoBoard::NeighborBlocks(SgPoint p, SgBlackWhite c, int maxLib,
                             SgPoint anchors[]) const
{
    SG_ASSERT(IsEmpty(p));
    SgReserveMarker reserve(m_marker);
    SG_UNUSED(reserve);
    m_marker.Clear();
    int i = 0;
    if (NumNeighbors(p, c) > 0)
    {
        if (IsColor(p - SG_NS, c) && m_marker.NewMark(Anchor(p - SG_NS))
            && AtMostNumLibs(p - SG_NS, maxLib))
            anchors[i++] = Anchor(p - SG_NS);
        if (IsColor(p - SG_WE, c) && m_marker.NewMark(Anchor(p - SG_WE))
            && AtMostNumLibs(p - SG_WE, maxLib))
            anchors[i++] = Anchor(p - SG_WE);
        if (IsColor(p + SG_WE, c) && m_marker.NewMark(Anchor(p + SG_WE))
            && AtMostNumLibs(p + SG_WE, maxLib))
            anchors[i++] = Anchor(p + SG_WE);
        if (IsColor(p + SG_NS, c) && m_marker.NewMark(Anchor(p + SG_NS))
            && AtMostNumLibs(p + SG_NS, maxLib))
            anchors[i++] = Anchor(p + SG_NS);
    }
    anchors[i] = SG_ENDPOINT;
}

void GoBoard::AddStone(SgPoint p, SgBlackWhite c)
{
    SG_ASSERT(IsEmpty(p));
    SG_ASSERT_BW(c);
    m_state.m_color[p] = c;
    m_state.m_empty.Exclude(p);
    m_state.m_all[c].Include(p);
    --m_state.m_nuNeighborsEmpty[p - SG_NS];
    --m_state.m_nuNeighborsEmpty[p - SG_WE];
    --m_state.m_nuNeighborsEmpty[p + SG_WE];
    --m_state.m_nuNeighborsEmpty[p + SG_NS];
    SgArray<int,SG_MAXPOINT>& nuNeighbors = m_state.m_nuNeighbors[c];
    ++nuNeighbors[p - SG_NS];
    ++nuNeighbors[p - SG_WE];
    ++nuNeighbors[p + SG_WE];
    ++nuNeighbors[p + SG_NS];
}

void GoBoard::RemoveStone(SgPoint p)
{
    SgBlackWhite c = GetStone(p);
    SG_ASSERT_BW(c);
    m_state.m_color[p] = SG_EMPTY;
    m_state.m_empty.Include(p);
    m_state.m_all[c].Exclude(p);
    ++m_state.m_nuNeighborsEmpty[p - SG_NS];
    ++m_state.m_nuNeighborsEmpty[p - SG_WE];
    ++m_state.m_nuNeighborsEmpty[p + SG_WE];
    ++m_state.m_nuNeighborsEmpty[p + SG_NS];
    SgArray<int,SG_MAXPOINT>& nuNeighbors = m_state.m_nuNeighbors[c];
    --nuNeighbors[p - SG_NS];
    --nuNeighbors[p - SG_WE];
    --nuNeighbors[p + SG_WE];
    --nuNeighbors[p + SG_NS];
}

void GoBoard::AddStoneForUndo(SgPoint p, SgBlackWhite c)
{
    SG_ASSERT_BW(c);
    SG_ASSERT_BOARDRANGE(p);
    m_state.m_isFirst[p] = false;
    // If this is the first time a point is played here,
    // then repetition is impossible until the next capture
    m_state.m_isNewPosition = m_state.m_isNewPosition || m_state.m_isFirst[p];
    m_state.m_hash.XorStone(p, c);
    AddStone(p, c);
}

void GoBoard::RemoveStoneForUndo(SgPoint p)
{
    SG_ASSERT_BOARDRANGE(p);
    m_state.m_hash.XorStone(p, GetStone(p));
    RemoveStone(p);
}

void GoBoard::KillBlock(const Block* block)
{
    SgBlackWhite c = block->Color();
    SgBlackWhite opp = SgOppBW(c);
    for (Block::StoneIterator it(block->Stones()); it; ++it)
    {
        SgPoint stn = *it;
        AddLibToAdjBlocks(stn, opp);
        RemoveStoneForUndo(stn);
        m_capturedStones.PushBack(stn);
        m_state.m_block[stn] = 0;
    }
    int nuStones = block->Stones().Length();
    m_state.m_numStones[c] -= nuStones;
    m_state.m_prisoners[c] += nuStones;
    if (nuStones == 1)
        // Remember that single stone was captured, check conditions on
        // capturing block later
        m_state.m_koPoint = block->Anchor();
}

bool GoBoard::FullBoardRepetition() const
{
    GoRules::KoRule koRule = Rules().GetKoRule();
    if (koRule == GoRules::SIMPLEKO)
    {
        int nuMoves = MoveNumber();
        if (nuMoves == 0)
            return false;
        const StackEntry& entry = (*m_moves)[nuMoves - 1];
        return (entry.m_point == entry.m_koPoint);
    }
    SgBWArray<SgSList<SgPoint,SG_MAXPOINT> > changes;
    int nuChanges = 0;
    int moveNumber = m_moves->Length() - 1;
    bool requireSameToPlay = (koRule == GoRules::SUPERKO);
    while (moveNumber >= 0)
    {
        const StackEntry& entry = (*m_moves)[moveNumber];
        SgPoint p = entry.m_point;
        if (! IsPass(p) && entry.m_color != SG_EMPTY
            && entry.m_color == GetColor(p)
            && entry.m_isFirst)
            // In this case we know that all erlier positions are
            // empty at p, therefore different to current position
            return false;

        if (! IsPass(entry.m_point))
        {
            UpdateChanges(entry.m_point, changes[entry.m_color], nuChanges);
            for (SgSList<Block*,4>::Iterator it(entry.m_killed); it; ++it)
            {
                Block* killed = *it;
                for (GoPointList::Iterator stn(killed->Stones()); stn; ++stn)
                    UpdateChanges(*stn, changes[killed->Color()], nuChanges);
            }
            Block* suicide = entry.m_suicide;
            if (suicide != 0)
                for (GoPointList::Iterator stn(suicide->Stones()); stn;
                     ++stn)
                    UpdateChanges(*stn, changes[suicide->Color()], nuChanges);
        }

        if (nuChanges > 0
            && (! requireSameToPlay || entry.m_toPlay == m_state.m_toPlay)
            && changes[SG_BLACK].IsEmpty() && changes[SG_WHITE].IsEmpty())
            return true;
        --moveNumber;
    }
    return false;
}

bool GoBoard::CheckSuicide(SgPoint p, StackEntry& entry)
{
    if (! HasLiberties(p))
    {
        entry.m_suicide = m_state.m_block[p];
        KillBlock(entry.m_suicide);
        m_moveInfo.set(GO_MOVEFLAG_SUICIDE);
        return m_rules.AllowSuicide();
    }
    return true;
}

void GoBoard::Play(SgPoint p, SgBlackWhite player)
{
    SG_ASSERT(p != SG_NULLMOVE);
    SG_ASSERT_BW(player);
    SG_ASSERT(IsPass(p) || (IsValidPoint(p) && IsEmpty(p)));
    CheckConsistency();
    ++m_countPlay;
    // Reuse stack entry without initialization
    m_moves->Resize(m_moves->Length() + 1);
    StackEntry& entry = m_moves->Last();
    entry.m_point = p;
    entry.m_color = player;
    SaveState(entry);
    m_state.m_koPoint = SG_NULLPOINT;
    m_capturedStones.Clear();
    m_moveInfo.reset();
    SgBlackWhite opp = SgOppBW(player);
    if (IsPass(p))
    {
        m_state.m_toPlay = opp;
        return;
    }
    bool isLegal = true;
    // Place the stone there tentatively. Remember whether this is the
    // first time a stone gets played at that point to speed up check
    // for full-board repetition below.
    bool wasFirstStone = IsFirst(p);
    AddStoneForUndo(p, player);
    ++m_state.m_numStones[player];
    RemoveLibAndKill(p, opp, entry);
    if (! entry.m_killed.IsEmpty())
    {
        m_moveInfo.set(GO_MOVEFLAG_CAPTURING);
        // Repetition now possible, unless its the first play here
        m_state.m_isNewPosition = m_state.m_isNewPosition && wasFirstStone;
    }
    UpdateBlocksAfterAddStone(p, player, entry);
    entry.m_suicide = 0;
    if (m_state.m_koPoint != SG_NULLPOINT)
        if (NumStones(p) > 1 || NumLiberties(p) > 1)
            m_state.m_koPoint = SG_NULLPOINT;
    isLegal = CheckSuicide(p, entry);
    m_state.m_toPlay = opp;
    if (! wasFirstStone && ! IsNewPosition() && ! CheckKo(player))
        isLegal = false;
    if (! isLegal)
        m_moveInfo.set(GO_MOVEFLAG_ILLEGAL);
    // @see @ref sgboardhashhistory
    if (! m_capturedStones.IsEmpty() && m_koModifiesHash)
    {
        // This assumes that in the exact same position, the stones will
        // be captured in the same sequence. Currently holds due to the
        // way KillBlockIfNoLiberty is implemented; may be fragile.
        SgPoint firstCapturedStone = m_capturedStones[0];
        m_state.m_hash.XorCaptured(MoveNumber(), firstCapturedStone);
    }
    CheckConsistency();
}

void GoBoard::Undo()
{
    CheckConsistency();
    const StackEntry& entry = m_moves->Last();
    RestoreState(entry);
    UpdateBlocksAfterUndo(entry);
    m_moves->PopBack();
    CheckConsistency();
}

/** Remove liberty from adjacent blocks and kill opponent blocks without
    liberties.
*/
void GoBoard::RemoveLibAndKill(SgPoint p, SgBlackWhite opp,
                               StackEntry& entry)
{
    entry.m_killed.Clear();
    if (NumNeighbors(p, SG_BLACK) == 0 && NumNeighbors(p, SG_WHITE) == 0)
        return;
    SgSList<Block*,4> blocks = GetAdjacentBlocks(p);
    for (SgSList<Block*,4>::Iterator it(blocks); it; ++it)
    {
        Block* b = *it;
        b->ExcludeLiberty(p);
        if (b->Color() == opp && b->NumLiberties() == 0)
        {
            entry.m_killed.PushBack(b);
            KillBlock(b);
        }
    }
}

void GoBoard::RestoreState(const StackEntry& entry)
{
    m_state.m_hash = entry.m_hash;
    m_state.m_koPoint = entry.m_koPoint;
    if (! IsPass(entry.m_point))
    {
        m_state.m_isFirst[entry.m_point] = entry.m_isFirst;
        m_state.m_isNewPosition = entry.m_isNewPosition;
    }
    m_state.m_toPlay = entry.m_toPlay;
    m_state.m_koLevel = entry.m_koLevel;
    m_koColor = entry.m_koColor;
    m_koLoser = entry.m_koLoser;
    m_koModifiesHash = entry.m_koModifiesHash;
}

void GoBoard::SaveState(StackEntry& entry)
{
    entry.m_hash = m_state.m_hash;
    if (! IsPass(entry.m_point))
    {
        entry.m_isFirst = m_state.m_isFirst[entry.m_point];
        entry.m_isNewPosition = m_state.m_isNewPosition;
    }
    entry.m_toPlay = m_state.m_toPlay;
    entry.m_koPoint = m_state.m_koPoint;
    entry.m_koLevel = m_state.m_koLevel;
    entry.m_koColor = m_koColor;
    entry.m_koLoser = m_koLoser;
    entry.m_koModifiesHash = m_koModifiesHash;
}

void GoBoard::TakeSnapshot()
{
    m_snapshot->m_moveNumber = MoveNumber();
    m_snapshot->m_blockListSize = m_blockList->Length();
    m_snapshot->m_state = m_state;
    for (GoBoard::Iterator it(*this); it; ++it)
    {
        SgPoint p = *it;
        if (m_state.m_block[p] != 0)
            m_snapshot->m_blockArray[p] = *m_state.m_block[p];
    }
}

void GoBoard::RestoreSnapshot()
{
    SG_ASSERT(m_snapshot->m_moveNumber >= 0);
    SG_ASSERT(m_snapshot->m_moveNumber <= MoveNumber());
    if (m_snapshot->m_moveNumber == MoveNumber())
        return;
    m_blockList->Resize(m_snapshot->m_blockListSize);
    m_moves->Resize(m_snapshot->m_moveNumber);
    m_state = m_snapshot->m_state;
    for (GoBoard::Iterator it(*this); it; ++it)
    {
        SgPoint p = *it;
        if (m_state.m_block[p] != 0)
            *m_state.m_block[p] = m_snapshot->m_blockArray[p];
    }
    CheckConsistency();
}

//----------------------------------------------------------------------------
