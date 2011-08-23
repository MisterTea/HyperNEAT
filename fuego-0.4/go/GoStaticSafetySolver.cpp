//----------------------------------------------------------------------------
/** @file GoStaticSafetySolver.cpp
    See GoStaticSafetySolver.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoStaticSafetySolver.h"

#include "GoBlock.h"
#include "GoChain.h"
#include "GoSafetyUtil.h"
#include "SgDebug.h"

using GoSafetyUtil::AddToSafe;

//----------------------------------------------------------------------------

GoStaticSafetySolver::GoStaticSafetySolver(const GoBoard& board,
                                           GoRegionBoard* regions)
    : m_board(board),
      m_allocRegion(! regions)
{
    if (regions)
        m_regions = regions;
    else
        m_regions = new GoRegionBoard(board);
}

GoStaticSafetySolver::~GoStaticSafetySolver()
{
    if (m_allocRegion)
        delete m_regions;
    // else m_regions belongs to the user, so leave it.
}

bool GoStaticSafetySolver::RegionHealthyForBlock(const GoRegion& r,
                                          const GoBlock& b) const
{
    return b.AllEmptyAreLiberties(r.Points());
}

bool GoStaticSafetySolver::UpToDate() const
{
    return Regions()->UpToDate();
}

void GoStaticSafetySolver::GenBlocksRegions()
{
    if (UpToDate())
        Regions()->ReInitializeBlocksRegions();
    else
    {
        Regions()->GenBlocksRegions();
    }
}


void GoStaticSafetySolver::FindHealthy()
{
    if (Regions()->ComputedHealthy())
        return;
    for (SgBWIterator it; it; ++it)
    {
        SgBlackWhite color(*it);
        for (SgVectorIteratorOf<GoRegion>
             it(Regions()->AllRegions(color)); it; ++it)
        {
            GoRegion* r = *it;
            for (SgVectorIteratorOf<GoBlock> it2(r->Blocks()); it2; ++it2)
            {
                if (RegionHealthyForBlock(*r, **it2)) // virtual call
                {
                    (*it2)->AddHealthy(r);
                }
            }
        }
    }
    Regions()->SetComputedHealthy();
}

void GoStaticSafetySolver::TestAdjacent(SgVectorOf<GoRegion>* regions,
                               const SgVectorOf<GoBlock>& blocks) const
{
    SgVectorOf<GoRegion> newregions;
    for (SgVectorIteratorOf<GoRegion> it(*regions); it; ++it)
        if ((*it)->IsSurrounded(blocks))
            newregions.PushBack(*it);
    *regions = newregions;
}

void GoStaticSafetySolver::TestAlive(SgVectorOf<GoBlock>* blocks,
                              SgBWSet* safe,
                              SgBlackWhite color)
{
    SgVectorOf<GoRegion> regions(Regions()->AllRegions(color));
    SgVectorOf<GoBlock> toDelete;

    bool changed = true;
    while (changed)
    {
        TestAdjacent(&regions, *blocks);
        toDelete.Clear();
        for (SgVectorIteratorOf<GoBlock> it(*blocks); it; ++it)
        {
            const SgVectorOf<GoRegion>& br = (*it)->Healthy();

            bool has2 = br.MinLength(2);
            if (has2)
            {
                int nuRegions = 0;
                for (SgVectorIteratorOf<GoRegion> it(br); it; ++it)
                {
                    if (regions.Contains(*it))
                    {
                        ++nuRegions;
                        if (nuRegions >= 2)
                            break;
                    }
                }
                has2 = (nuRegions >= 2);
            }
            if (! has2)
                toDelete.PushBack(*it);
        }

        changed = toDelete.NonEmpty();
        if (changed)
        {
            for (SgVectorIteratorOf<GoBlock> it(toDelete); it; ++it)
            {
                bool found = blocks->Exclude(*it);
                SG_DEBUG_ONLY(found);
                SG_ASSERT(found);
            }
        }
    }

    if (blocks->NonEmpty()) // found safe blocks
    {
        SgPointSet blockPoints;
        for (SgVectorIteratorOf<GoBlock> it(*blocks); it; ++it)
        {
            blockPoints |= (*it)->Stones();
            (*it)->SetToSafe();
        }

        color = blocks->Front()->Color();
        AddToSafe(m_board, blockPoints, color, safe,
                  "TestAlive-Blocks", 0, false);

        for (SgVectorIteratorOf<GoRegion> it(regions); it; ++it)
            if ((*it)->HealthyForSomeBlock(*blocks))
            {
                (*it)->SetToSafe();
                AddToSafe(m_board, (*it)->Points(), color, safe,
                          "TestAlive-Region", 0, false);
            }
    }
}

void GoStaticSafetySolver::FindClosure(SgVectorOf<GoBlock>* blocks) const
{
    SgVectorOf<GoBlock> toTest(*blocks);
    while (toTest.NonEmpty())
    {
        const GoBlock* b = toTest.Back();
        toTest.PopBack();
        for (SgVectorIteratorOf<GoRegion> it(b->Healthy()); it; ++it)
        {
            const GoRegion* r = *it;
            for (SgVectorIteratorOf<GoBlock> it(r->Blocks()); it; ++it)
            {
                const GoBlock* b2 = *it;
                if (! blocks->Contains(b2) && b2->ContainsHealthy(r))
                {
                    blocks->PushBack(b2);
                    toTest.PushBack(b2);
                }
            }
        }
    }
}

void GoStaticSafetySolver::FindTestSets(
                                     SgVectorOf<SgVectorOf<GoBlock> >* sets,
                                     SgBlackWhite color) const
{
    SG_ASSERT(sets->IsEmpty());
    SgVectorOf<GoBlock> doneSoFar;
    for (SgVectorIteratorOf<GoBlock>
         it(Regions()->AllBlocks(color)); it; ++it)
    {
        GoBlock* block = *it;
        if (! doneSoFar.Contains(block))
        {
            SgVectorOf<GoBlock>* blocks = new SgVectorOf<GoBlock>;
            blocks->PushBack(block);

            FindClosure(blocks);
            doneSoFar.PushBackList(*blocks);
            sets->PushBack(blocks);
        }
    }
}

void GoStaticSafetySolver::FindSafePoints(SgBWSet* safe)
{
    GenBlocksRegions(); // find all blocks and regions on whole board
    FindHealthy(); // find healthy regions of blocks

    for (SgBWIterator it; it; ++it)
    {
        SgBlackWhite color(*it);
        SgVectorOf<SgVectorOf<GoBlock> > sets;
        // find maximal sets for aliveness testing
        FindTestSets(&sets, color);

        for (SgVectorIteratorOf<SgVectorOf<GoBlock> > it(sets); it; ++it)
        {   TestAlive(*it, safe, color);
            // find safe subset within each maximal set
            delete *it;
        }
    }

    Regions()->SetComputedFlagForAll(GO_REGION_SAFE);
}

