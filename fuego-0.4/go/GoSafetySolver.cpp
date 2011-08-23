//----------------------------------------------------------------------------
/** @file GoSafetySolver.cpp
    See GoSafetySolver.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoSafetySolver.h"

#include "GoBlock.h"
#include "GoChain.h"
#include "GoSafetyUtil.h"
#include "SgConnCompIterator.h"

//----------------------------------------------------------------------------

namespace {

const bool DEBUG_MERGE_CHAINS = false;
const bool DEBUG_SAFETY_SOLVER = false;

bool HaveSharedUnsafe(const SgVectorOf<GoBlock>& list1,
                      const SgVectorOf<GoBlock>& list2)
{
    for (SgVectorIteratorOf<GoBlock> it(list1); it; ++it)
        if (! (*it)->IsSafe() && list2.Contains(*it))
            return true;
    return false;
}

} // namespace

void GoSafetySolver::FindHealthy()
{        
    for (SgBWIterator it; it; ++it)
    {
        SgBlackWhite color(*it);
        for (SgVectorIteratorOf<GoRegion>
             it(Regions()->AllRegions(color)); it; ++it)
            (*it)->ComputeFlag(GO_REGION_STATIC_1VITAL);
    }
   
    // used to just call GoStaticSafetySolver::FindHealthy() here, 
    // but that works with GoBlock's and now we use GoChain's.
    // Code is duplicated though. Can maybe use a template function.
   
    for (SgBWIterator it; it; ++it)
    {
        SgBlackWhite color(*it);
        for (SgVectorIteratorOf<GoRegion>
             it(Regions()->AllRegions(color)); it; ++it)
        {
            GoRegion* r = *it;
            for (SgVectorIteratorOf<GoChain> it2(r->Chains()); it2; ++it2)
            {
                if (RegionHealthyForBlock(*r, **it2)) // virtual call
                    (*it2)->AddHealthy(r);
}   }   }   }

void GoSafetySolver::FindClosure(SgVectorOf<GoBlock>* blocks) const
{
    SgVectorOf<GoBlock> toTest(*blocks);
    while (toTest.NonEmpty())
    {
        const GoBlock* b = toTest.Back();
        toTest.PopBack();
        for (SgVectorIteratorOf<GoRegion> it(b->Healthy()); it; ++it)
        {   GoRegion* r = *it;
            for (SgVectorIteratorOf<GoChain> it(r->Chains()); it; ++it)
            {   GoBlock* b2 = *it;
                if (! blocks->Contains(b2) && b2->ContainsHealthy(r))
                {
                    blocks->PushBack(b2);
                    toTest.PushBack(b2);
}   }   }   }   }

void GoSafetySolver::FindTestSets(SgVectorOf<SgVectorOf<GoBlock> >* sets,
                                  SgBlackWhite color) const
{
    SG_ASSERT(sets->IsEmpty());
    SgVectorOf<GoBlock> doneSoFar;
    for (SgVectorIteratorOf<GoChain>
         it(Regions()->AllChains(color)); it; ++it)
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

bool GoSafetySolver::RegionHealthyForBlock(const GoRegion& r,
                                           const GoBlock& b) const
{
    return    GoStaticSafetySolver::RegionHealthyForBlock(r, b)
           || r.GetFlag(GO_REGION_STATIC_1VITAL);
}


void GoSafetySolver::Test2Vital(GoRegion* r, SgBWSet* safe)
{
    if (r->ComputeAndGetFlag(GO_REGION_STATIC_2V))
        GoSafetyUtil::AddToSafe(Board(), r->Points(), r->Color(),
                  safe, "2-vital:", 0, true);
}

void GoSafetySolver::Find2VitalAreas(SgBWSet* safe)
{
    for (SgBWIterator it; it; ++it)
    {
        SgBlackWhite color(*it);
        for (SgVectorIteratorOf<GoRegion>
             it(Regions()->AllRegions(color)); it; ++it)
            if (  (*it)->Points().Disjoint((*safe)[SG_BLACK]) 
               && (*it)->Points().Disjoint((*safe)[SG_WHITE])
               )
            {
                Test2Vital(*it, safe);
                safe->AssertDisjoint();
            }
    }
}


void GoSafetySolver::FindSurroundedSafeAreas(SgBWSet* safe,
                                             SgBlackWhite color)
{
    // AR keep this always up to date earlier.
    Regions()->SetSafeFlags(*safe); 
        
    // try to find single regions that become safe.
    while (FindSurroundedSingleRegion(safe, color))
    { }
    while (FindSurroundedRegionPair(safe, color))
    	// if found new pair, re-run single region algorithm - it is faster
        while (FindSurroundedSingleRegion(safe, color))
        { }
}

bool GoSafetySolver::FindSafePair(SgBWSet* safe,
                                  SgBlackWhite color,
                                  const SgPointSet& anySafe,
                                  const GoRegion* r1)
{
    for (SgVectorIteratorOf<GoRegion>
         it(Regions()->AllRegions(color)); it; ++it)
    {
        const GoRegion* r2 = *it;
        if (  r2 != r1
           && ! r2->Points().Overlaps(anySafe)
           && HaveSharedUnsafe(r1->Blocks(), r2->Blocks())
           )
        {
            const SgPointSet unionSet(r1->Points() | r2->Points());
            if (GoSafetyUtil::IsTerritory(Board(), unionSet,
                                          (*safe)[color], color))
            {
                GoSafetyUtil::AddToSafe(Board(), unionSet, color, safe,
                          "surr-safe-2", 0, true);
                Regions()->SetSafeFlags(*safe); 
                safe->AssertDisjoint();
                return true;
            }
        }
    }
    return false;
}

bool GoSafetySolver::FindSurroundedRegionPair(SgBWSet* safe,
                                               SgBlackWhite color)
{
    SgPointSet anySafe(safe->Both());
    for (SgVectorIteratorOf<GoRegion>
         it(Regions()->AllRegions(color)); it; ++it)
    {
        GoRegion* r1 = *it;
        if (  ! r1->GetFlag(GO_REGION_SAFE)
           && r1->SomeBlockIsSafe()
           && ! r1->Points().Overlaps(anySafe)
           && FindSafePair(safe, color, anySafe, r1)
           )
            return true;
    }
    return false;
}

bool GoSafetySolver::FindSurroundedSingleRegion(SgBWSet* safe,
                                             SgBlackWhite color)
{
    SgPointSet anySafe(safe->Both());
    for (SgVectorIteratorOf<GoRegion>
         it(Regions()->AllRegions(color)); it; ++it)
    {
        GoRegion* r = *it;
        if (  ! r->GetFlag(GO_REGION_SAFE)
           && r->SomeBlockIsSafe()
           && ! r->Points().Overlaps(anySafe)
           && GoSafetyUtil::ExtendedIsTerritory(Board(), Regions(),
                                   r->PointsPlusInteriorBlocks(),
                                   (*safe)[color],
                                   color)
           )
        {
            GoSafetyUtil::AddToSafe(Board(), r->Points(), color, safe,
                      "surr-safe-1", 0, true);
            Regions()->SetSafeFlags(*safe); 
            return true;
        }
    }
    return false;
}

void GoSafetySolver::FindSafePoints(SgBWSet* safe)
{    
    GoStaticSafetySolver::FindSafePoints(safe);
    safe->AssertDisjoint();
    if (DEBUG_SAFETY_SOLVER)
        GoSafetyUtil::WriteStatistics("Base Solver", Regions(), safe);

    // find areas big enough for two eyes
    Find2VitalAreas(safe);
    safe->AssertDisjoint();
    if (DEBUG_SAFETY_SOLVER)
        GoSafetyUtil::WriteStatistics("2Vital", Regions(), safe);
 
//    GoStaticSafetySolver::FindSafePoints(safe); 
//called again before 030505,
// but now double-counts healthy regions for chains. Must change to
// set a computedHealthy flag
//    safe->AssertDisjoint();

    // find areas small enough that opponent cannot make two eyes
    for (SgBWIterator it; it; ++it)
    {
        FindSurroundedSafeAreas(safe, *it);
        safe->AssertDisjoint();
    }    

    if (DEBUG_SAFETY_SOLVER)
        GoSafetyUtil::WriteStatistics("SurroundedSafe-Final",
                                      Regions(), safe);
}

void GoSafetySolver::Merge(GoChain* c1, GoChain* c2,
                           GoRegion* r, bool bySearch)
{
    SG_ASSERT(! r->GetFlag(GO_REGION_USED_FOR_MERGE));
    r->SetFlag(GO_REGION_USED_FOR_MERGE, true);
    
    GoChainCondition* c = 0;
    if (bySearch)
        c = new GoChainCondition(GO_CHAIN_BY_SEARCH);
    else
    {
        SgPoint lib1, lib2;
        r->Find2FreeLibs(c1, c2, &lib1, &lib2);
        c = new GoChainCondition(GO_CHAIN_TWO_LIBERTIES_IN_REGION,
                                 lib1, lib2);
    }
    
    GoChain* m = new GoChain(c1, c2, c);

    SgBlackWhite color = c1->Color();
    bool found = Regions()->AllChains(color).Exclude(c1);
    SG_ASSERT(found);
    found = Regions()->AllChains(color).Exclude(c2);
    SG_ASSERT(found);
    Regions()->AllChains(color).Include(m);
    SG_ASSERT(Regions()->AllChains(color).UniqueElements());

    for (SgVectorIteratorOf<GoRegion>
         it(Regions()->AllRegions(color)); it; ++it)
    {
        GoRegion* r = *it;
        bool replace1 = r->ReplaceChain(c1, m);
        bool replace2 = r->ReplaceChain(c2, m);
        if (replace1 || replace2)
        {
            r->ReInitialize();
            r->ComputeFlag(GO_REGION_STATIC_1VITAL);
        }
    }

    if (DEBUG_MERGE_CHAINS)
    {
        SgDebug() << "\nmerge:";
        c1->WriteID(SgDebug());
        SgDebug() << " + ";
        c2->WriteID(SgDebug());
        SgDebug() << " = ";
        m->WriteID(SgDebug());
        SgDebug() << '\n';
    }

    delete c1;
    delete c2;
}

void GoSafetySolver::GenBlocksRegions()
{
    if (UpToDate())
        /* */ return; /* */
        
    GoStaticSafetySolver::GenBlocksRegions();
    
    Regions()->GenChains();
    
    // merge blocks adjacent to 1-vital with 2 conn. points
    for (SgBWIterator it; it; ++it)
    {
        SgBlackWhite color(*it);
        for (SgVectorIteratorOf<GoRegion> 
             it(Regions()->AllRegions(color)); it; ++it)
        {
            GoRegion* r = *it;
            r->ComputeFlag(GO_REGION_STATIC_1VITAL);
        }

        bool changed = true;
        while (changed)
        {   changed = false;
            for (SgVectorIteratorOf<GoRegion> 
                 it(Regions()->AllRegions(color)); it; ++it)
            {   GoRegion* r = *it;
                if (  r->GetFlag(GO_REGION_STATIC_1VC)
                   && r->Chains().IsLength(2)
                   && r->Has2Conn() 
                        //  || r->Safe2Cuts(Board()) changed from && to ||
                        // @todo does not work if blocks are already chains???
                        // must explicitly keep chain libs info.
                   )
                // easy case of only 2 chains
                {
                    GoChain* c1 = r->Chains().Front();
                    GoChain* c2 = r->Chains().Back();
                    Merge(c1, c2, r, false); // false = not by search
                    changed = true;
                    break; // to leave iteration
                }
                else if (   r->GetFlag(GO_REGION_STATIC_1VITAL)
                         && r->GetFlag(GO_REGION_CORRIDOR)
                         && ! r->GetFlag(GO_REGION_USED_FOR_MERGE)
                        ) 
                {
                    GoChain* c1 = 0;
                    GoChain* c2 = 0;
                    if (r->Find2Mergable(&c1, &c2))
                    {
                        Merge(c1, c2, r, false);
                        changed = true;
                        break; // to leave iteration
    }   }   }   }   }
    
    m_code = Board().GetHashCode();
}

