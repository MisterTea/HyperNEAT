//----------------------------------------------------------------------------
/** @file GoBoard.cpp
    See GoBoard.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoUctBoard.h"

#include <boost/static_assert.hpp>
#include <algorithm>
#include "GoBoardUtil.h"
#include "SgNbIterator.h"
#include "SgStack.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

/** Do a consistency check.
    Check some data structures for consistency after and before each play
    (and at some other places).
    This is an expensive check and therefore has to be enabled at compile
    time.
*/
const bool CONSISTENCY = false;

} // namespace

//----------------------------------------------------------------------------

GoUctBoard::GoUctBoard(const GoBoard& bd)
    : m_const(bd.Size())
{
    m_size = -1;
    Init(bd);
}

GoUctBoard::~GoUctBoard()
{
}

void GoUctBoard::CheckConsistency() const
{
    if (! CONSISTENCY)
        return;
    for (SgPoint p = 0; p < SG_MAXPOINT; ++p)
    {
        if (IsBorder(p))
            continue;
        int c = m_color[p];
        SG_ASSERT_EBW(c);
        int n = 0;
        for (SgNb4Iterator it(p); it; ++it)
            if (m_color[*it] == SG_EMPTY)
                ++n;
        SG_ASSERT(n == NumEmptyNeighbors(p));
        n = 0;
        for (SgNb4Iterator it(p); it; ++it)
            if (m_color[*it] == SG_BLACK)
                ++n;
        SG_ASSERT(n == NumNeighbors(p, SG_BLACK));
        n = 0;
        for (SgNb4Iterator it(p); it; ++it)
            if (m_color[*it] == SG_WHITE)
                ++n;
        SG_ASSERT(n == NumNeighbors(p, SG_WHITE));
        if (c == SG_BLACK || c == SG_WHITE)
            CheckConsistencyBlock(p);
        if (c == SG_EMPTY)
            SG_ASSERT(m_block[p] == 0);
    }
}

void GoUctBoard::CheckConsistencyBlock(SgPoint point) const
{
    SG_ASSERT(Occupied(point));
    SgBlackWhite color = GetColor(point);
    GoPointList stones;
    Block::LibertyList liberties;
    SgMarker mark;
    SgStack<SgPoint,SG_MAXPOINT> stack;
    stack.Push(point);
    bool anchorFound = false;
    const Block* block = m_block[point];
    while (! stack.IsEmpty())
    {
        SgPoint p = stack.Pop();
        if (IsBorder(p) || ! mark.NewMark(p))
            continue;
        if (GetColor(p) == color)
        {
            stones.PushBack(p);
            if (p == block->m_anchor)
                anchorFound = true;
            stack.Push(p - SG_NS);
            stack.Push(p - SG_WE);
            stack.Push(p + SG_WE);
            stack.Push(p + SG_NS);
        }
        else if (GetColor(p) == SG_EMPTY)
            liberties.PushBack(p);
    }
    SG_ASSERT(anchorFound);
    SG_ASSERT(color == block->m_color);
    SG_ASSERT(stones.SameElements(block->m_stones));
    SG_ASSERT(liberties.SameElements(block->m_liberties));
    SG_ASSERT(stones.Length() == NumStones(point));
}

void GoUctBoard::AddLibToAdjBlocks(SgPoint p, SgBlackWhite c)
{
    if (NumNeighbors(p, c) == 0)
        return;
    SgReserveMarker reserve(m_marker2);
    m_marker2.Clear();
    Block* b;
    if (m_color[p - SG_NS] == c && (b = m_block[p - SG_NS]) != 0)
    {
        m_marker2.Include(b->m_anchor);
        b->m_liberties.PushBack(p);
    }
    if (m_color[p + SG_NS] == c && (b = m_block[p + SG_NS]) != 0
        && m_marker2.NewMark(b->m_anchor))
        b->m_liberties.PushBack(p);
    if (m_color[p - SG_WE] == c && (b = m_block[p - SG_WE]) != 0
        && m_marker2.NewMark(b->m_anchor))
        b->m_liberties.PushBack(p);
    if (m_color[p + SG_WE] == c && (b = m_block[p + SG_WE]) != 0
        && ! m_marker2.Contains(b->m_anchor))
        b->m_liberties.PushBack(p);
}

void GoUctBoard::AddStoneToBlock(SgPoint p, Block* block)
{
    // Stone already placed
    SG_ASSERT(IsColor(p, block->m_color));
    block->m_stones.PushBack(p);
    if (IsEmpty(p - SG_NS) && ! IsAdjacentTo(p - SG_NS, block))
        block->m_liberties.PushBack(p - SG_NS);
    if (IsEmpty(p - SG_WE) && ! IsAdjacentTo(p - SG_WE, block))
        block->m_liberties.PushBack(p - SG_WE);
    if (IsEmpty(p + SG_WE) && ! IsAdjacentTo(p + SG_WE, block))
        block->m_liberties.PushBack(p + SG_WE);
    if (IsEmpty(p + SG_NS) && ! IsAdjacentTo(p + SG_NS, block))
        block->m_liberties.PushBack(p + SG_NS);
    m_block[p] = block;
}

void GoUctBoard::CreateSingleStoneBlock(SgPoint p, SgBlackWhite c)
{
    // Stone already placed
    SG_ASSERT(IsColor(p, c));
    SG_ASSERT(NumNeighbors(p, c) == 0);
    Block& block = m_blockArray[p];
    block.InitSingleStoneBlock(c, p);
    if (IsEmpty(p - SG_NS))
        block.m_liberties.PushBack(p - SG_NS);
    if (IsEmpty(p - SG_WE))
        block.m_liberties.PushBack(p - SG_WE);
    if (IsEmpty(p + SG_WE))
        block.m_liberties.PushBack(p + SG_WE);
    if (IsEmpty(p + SG_NS))
        block.m_liberties.PushBack(p + SG_NS);
    m_block[p] = &block;
}

bool GoUctBoard::IsAdjacentTo(SgPoint p,
                              const GoUctBoard::Block* block) const
{
    return   m_block[p - SG_NS] == block
          || m_block[p - SG_WE] == block
          || m_block[p + SG_WE] == block
          || m_block[p + SG_NS] == block;
}

void GoUctBoard::MergeBlocks(SgPoint p, const SgSList<Block*,4>& adjBlocks)
{
    // Stone already placed
    SG_ASSERT(IsColor(p, adjBlocks[0]->m_color));
    SG_ASSERT(NumNeighbors(p, adjBlocks[0]->m_color) > 1);
    Block* largestBlock = 0;
    int largestBlockStones = 0;
    for (SgSList<Block*,4>::Iterator it(adjBlocks); it; ++it)
    {
        Block* adjBlock = *it;
        int numStones = adjBlock->m_stones.Length();
        if (numStones > largestBlockStones)
        {
            largestBlockStones = numStones;
            largestBlock = adjBlock;
        }
    }
    largestBlock->m_stones.PushBack(p);
    SgReserveMarker reserve(m_marker);
    m_marker.Clear();
    for (Block::LibertyIterator lib(largestBlock->m_liberties); lib; ++lib)
        m_marker.Include(*lib);
    for (SgSList<Block*,4>::Iterator it(adjBlocks); it; ++it)
    {
        Block* adjBlock = *it;
        if (adjBlock == largestBlock)
            continue;
        for (Block::StoneIterator stn(adjBlock->m_stones); stn; ++stn)
        {
            largestBlock->m_stones.PushBack(*stn);
            m_block[*stn] = largestBlock;
        }
        for (Block::LibertyIterator lib(adjBlock->m_liberties); lib; ++lib)
            if (m_marker.NewMark(*lib))
                largestBlock->m_liberties.PushBack(*lib);
    }
    m_block[p] = largestBlock;
    if (IsEmpty(p - SG_NS) && m_marker.NewMark(p - SG_NS))
        largestBlock->m_liberties.PushBack(p - SG_NS);
    if (IsEmpty(p - SG_WE) && m_marker.NewMark(p - SG_WE))
        largestBlock->m_liberties.PushBack(p - SG_WE);
    if (IsEmpty(p + SG_WE) && m_marker.NewMark(p + SG_WE))
        largestBlock->m_liberties.PushBack(p + SG_WE);
    if (IsEmpty(p + SG_NS) && m_marker.NewMark(p + SG_NS))
        largestBlock->m_liberties.PushBack(p + SG_NS);
}

void GoUctBoard::UpdateBlocksAfterAddStone(SgPoint p, SgBlackWhite c,
                                           const SgSList<Block*,4>& adjBlocks)
{
    // Stone already placed
    SG_ASSERT(IsColor(p, c));
    int n = adjBlocks.Length();
    if (n == 0)
        CreateSingleStoneBlock(p, c);
    else
    {
        if (n == 1)
            AddStoneToBlock(p, adjBlocks[0]);
        else
            MergeBlocks(p, adjBlocks);
    }
}

void GoUctBoard::Init(const GoBoard& bd)
{
    if (bd.Size() != m_size)
        InitSize(bd);
    m_prisoners[SG_BLACK] = bd.NumPrisoners(SG_BLACK);
    m_prisoners[SG_WHITE] = bd.NumPrisoners(SG_WHITE);
    m_koPoint = bd.KoPoint();
    m_lastMove = bd.GetLastMove();
    m_secondLastMove = bd.Get2ndLastMove();
    m_toPlay = bd.ToPlay();
    for (GoBoard::Iterator it(bd); it; ++it)
    {
        SgPoint p = *it;
        SgBoardColor c = bd.GetColor(p);
        m_color[p] = c;
        m_nuNeighbors[SG_BLACK][p] = bd.NumNeighbors(p, SG_BLACK);
        m_nuNeighbors[SG_WHITE][p] = bd.NumNeighbors(p, SG_WHITE);
        m_nuNeighborsEmpty[p] = bd.NumEmptyNeighbors(p);
        if (bd.IsEmpty(p))
            m_block[p] = 0;
        else if (bd.Anchor(p) == p)
        {
            SgBoardColor c = m_color[p];
            Block& block = m_blockArray[p];
            block.InitNewBlock(c, p);
            for (GoBoard::StoneIterator it2(bd, p); it2; ++it2)
            {
                block.m_stones.PushBack(*it2);
                m_block[*it2] = &block;
            }
            for (GoBoard::LibertyIterator it2(bd, p); it2; ++it2)
                block.m_liberties.PushBack(*it2);
        }
    }
    CheckConsistency();
}

void GoUctBoard::InitSize(const GoBoard& bd)
{
    m_size = bd.Size();
    m_nuNeighbors[SG_BLACK].Fill(0);
    m_nuNeighbors[SG_WHITE].Fill(0);
    m_nuNeighborsEmpty.Fill(0);
    m_block.Fill(0);
    for (SgPoint p = 0; p < SG_MAXPOINT; ++p)
    {
        if (bd.IsBorder(p))
        {
            m_color[p] = SG_BORDER;
            m_isBorder[p] = true;
        }
        else
            m_isBorder[p] = false;
    }
    m_const.ChangeSize(m_size);
}

void GoUctBoard::NeighborBlocks(SgPoint p, SgBlackWhite c,
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

void GoUctBoard::AddStone(SgPoint p, SgBlackWhite c)
{
    SG_ASSERT(IsEmpty(p));
    SG_ASSERT_BW(c);
    m_color[p] = c;
    --m_nuNeighborsEmpty[p - SG_NS];
    --m_nuNeighborsEmpty[p - SG_WE];
    --m_nuNeighborsEmpty[p + SG_WE];
    --m_nuNeighborsEmpty[p + SG_NS];
    SgArray<int,SG_MAXPOINT>& nuNeighbors = m_nuNeighbors[c];
    ++nuNeighbors[p - SG_NS];
    ++nuNeighbors[p - SG_WE];
    ++nuNeighbors[p + SG_WE];
    ++nuNeighbors[p + SG_NS];
}

/** Remove liberty from adjacent blocks and kill opponent blocks without
    liberties.
    As a side effect, computes adjacent blocks of own color to avoid a
    second call to GetAdjacentBlocks() in UpdateBlocksAfterAddStone().
*/
void GoUctBoard::RemoveLibAndKill(SgPoint p, SgBlackWhite opp,
                                  SgSList<Block*,4>& ownAdjBlocks)
{
    SgReserveMarker reserve(m_marker);
    m_marker.Clear();
    Block* b;
    if ((b = m_block[p - SG_NS]) != 0)
    {
        m_marker.Include(b->m_anchor);
        b->m_liberties.Exclude(p);
        if (b->m_color == opp)
        {
            if (b->m_liberties.Length() == 0)
                KillBlock(b);
        }
        else
            ownAdjBlocks.PushBack(b);
    }
    if ((b = m_block[p - SG_WE]) != 0 && m_marker.NewMark(b->m_anchor))
    {
        b->m_liberties.Exclude(p);
        if (b->m_color == opp)
        {
            if (b->m_liberties.Length() == 0)
                KillBlock(b);
        }
        else
            ownAdjBlocks.PushBack(b);
    }
    if ((b = m_block[p + SG_WE]) != 0 && m_marker.NewMark(b->m_anchor))
    {
        b->m_liberties.Exclude(p);
        if (b->m_color == opp)
        {
            if (b->m_liberties.Length() == 0)
                KillBlock(b);
        }
        else
            ownAdjBlocks.PushBack(b);
    }
    if ((b = m_block[p + SG_NS]) != 0 && ! m_marker.Contains(b->m_anchor))
    {
        b->m_liberties.Exclude(p);
        if (b->m_color == opp)
        {
            if (b->m_liberties.Length() == 0)
                KillBlock(b);
        }
        else
            ownAdjBlocks.PushBack(b);
    }
}

void GoUctBoard::KillBlock(const Block* block)
{
    SgBlackWhite c = block->m_color;
    SgBlackWhite opp = SgOppBW(c);
    SgArray<int,SG_MAXPOINT>& nuNeighbors = m_nuNeighbors[c];
    for (Block::StoneIterator it(block->m_stones); it; ++it)
    {
        SgPoint p = *it;
        AddLibToAdjBlocks(p, opp);
        m_color[p] = SG_EMPTY;
        ++m_nuNeighborsEmpty[p - SG_NS];
        ++m_nuNeighborsEmpty[p - SG_WE];
        ++m_nuNeighborsEmpty[p + SG_WE];
        ++m_nuNeighborsEmpty[p + SG_NS];
        --nuNeighbors[p - SG_NS];
        --nuNeighbors[p - SG_WE];
        --nuNeighbors[p + SG_WE];
        --nuNeighbors[p + SG_NS];
        m_capturedStones.PushBack(p);
        m_block[p] = 0;
    }
    int nuStones = block->m_stones.Length();
    m_prisoners[c] += nuStones;
    if (nuStones == 1)
        // Remember that single stone was captured, check conditions on
        // capturing block later
        m_koPoint = block->m_anchor;
}

void GoUctBoard::Play(SgPoint p)
{
    SG_ASSERT(p >= 0); // No special move, see SgMove
    SG_ASSERT(p == SG_PASS || (IsValidPoint(p) && IsEmpty(p)));
    CheckConsistency();
    m_koPoint = SG_NULLPOINT;
    m_capturedStones.Clear();
    SgBlackWhite opp = SgOppBW(m_toPlay);
    if (p != SG_PASS)
    {
        AddStone(p, m_toPlay);
        SgSList<Block*,4> adjBlocks;
        if (NumNeighbors(p, SG_BLACK) > 0 || NumNeighbors(p, SG_WHITE) > 0)
            RemoveLibAndKill(p, opp, adjBlocks);
        UpdateBlocksAfterAddStone(p, m_toPlay, adjBlocks);
        if (m_koPoint != SG_NULLPOINT)
            if (NumStones(p) > 1 || NumLiberties(p) > 1)
                m_koPoint = SG_NULLPOINT;
        SG_ASSERT(HasLiberties(p)); // Suicide not supported by GoUctBoard
    }
    m_secondLastMove = m_lastMove;
    m_lastMove = p;
    m_toPlay = opp;
    CheckConsistency();
}

//----------------------------------------------------------------------------
