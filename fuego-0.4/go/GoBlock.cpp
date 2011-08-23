//----------------------------------------------------------------------------
/** @file GoBlock.cpp
    See GoBlock.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoBlock.h"

#include <iostream>
#include "GoRegion.h"
#include "SgPointSetUtil.h"
#include "SgWrite.h"

//----------------------------------------------------------------------------

static const bool CHECK = SG_CHECK && true;
static const bool HEAVYCHECK = SG_HEAVYCHECK && CHECK && false;

//----------------------------------------------------------------------------

int GoBlock::s_alloc = 0;
int GoBlock::s_free = 0;

void GoBlock::AddStone(SgPoint stone)
{
    if (HEAVYCHECK)
        SG_ASSERT(! m_stones.Contains(stone));
    m_stones.Include(stone);
    if (stone < m_anchor)
    {
        if (HEAVYCHECK)
            SG_ASSERT(stone == m_bd.Anchor(m_anchor));
        m_anchor = stone;
    }
}

void GoBlock::RemoveStone(SgPoint stone)
{
    if (HEAVYCHECK)
        SG_ASSERT(m_stones.Contains(stone));
    m_stones.Exclude(stone);
    if (stone == m_anchor)
        m_anchor = m_bd.Anchor(m_stones.PointOf());
}

bool GoBlock::AllEmptyAreLiberties(const SgPointSet& area) const
{
    const SgPoint anchor = Anchor();
    for (SgSetIterator it(area); it; ++it)
        if (  m_bd.IsEmpty(*it)
           && ! m_bd.IsLibertyOfBlock(*it, anchor)
           )
            return false;
    return true;
}

void GoBlock::TestFor1Eye(const GoRegion* r)
{
    if (r->GetFlag(GO_REGION_SMALL) && r->Blocks().IsLength(1))
    // @todo what if more than one block?
    {
        m_has1Eye = true;
    }
}

void GoBlock::CheckConsistency() const
{
    SG_ASSERT(Stones().SubsetOf(m_bd.All(Color())));
    SgPoint anchor = Anchor();
    SG_ASSERT(m_bd.Anchor(anchor) == Anchor());
    SgPointSet stones;
    for (GoBoard::StoneIterator it(m_bd, anchor); it; ++it)
        stones.Include(*it);
    SG_ASSERT(Stones() == stones);
}

void GoBlock::Fini()
{
    SG_ASSERT(s_alloc == s_free);
}

void GoBlock::Write(std::ostream& stream) const
{
    WriteID(stream);
    stream  <<  '\n'
            << SgWritePointSet(Stones(), "Stones: ")
            << "\nhealthy: " << Healthy().Length()
            << "\nisSafe: " << SgWriteBoolean(IsSafe())
            << "\nhas1Eye: " << SgWriteBoolean(Has1Eye())
            << "\n";
}

void GoBlock::WriteID(std::ostream& stream) const
{
    stream << ' ' << SgBW(Color())
           << " Block " << SgWritePoint(Anchor());
}

//----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const GoBlock& b)
{
    b.Write(stream);
    return stream;
}

//----------------------------------------------------------------------------
