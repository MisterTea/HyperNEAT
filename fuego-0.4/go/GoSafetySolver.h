//----------------------------------------------------------------------------
/** @file GoSafetySolver.h
    Recognize safe territories using 1-vital and 2-vital definitions
    under alternating play
    from Martin Mueller's thesis [Mueller 95, p. 62-63] 
    and from [Mueller 97b].
*/
//----------------------------------------------------------------------------

#ifndef GO_SAFETYSOLVER_H
#define GO_SAFETYSOLVER_H

#include "GoStaticSafetySolver.h"

//----------------------------------------------------------------------------

/** Improved static detection of safe blocks and regions
    Uses static rules to find 1-vital, 2-vital and safely surrounded areas.
*/
class GoSafetySolver : public GoStaticSafetySolver
{
public:
    /** Constructor, @see GoStaticSafetySolver */
    explicit GoSafetySolver(GoBoard& board, GoRegionBoard* regions = 0) 
             : GoStaticSafetySolver(board, regions)
    {
        m_code.Invalidate();
    }

    /** Main function, compute safe points */
    void FindSafePoints(SgBWSet* safe);

    /** Find areas surrounded by safe blocks where the opponent cannot live */
    virtual void FindSurroundedSafeAreas(SgBWSet* safe, SgBlackWhite color);

    /** call virtual RegionHealthyForBlock */
    virtual void FindHealthy();

    /** Check if internal state matches board */
    virtual bool UpToDate() const
    {
        return    GoStaticSafetySolver::UpToDate() 
               && m_code == Board().GetHashCode();
    }

protected:  

    /** See GoStaticSafetySolver::RegionHealthyForBlock.
        This implementation uses 1-vital information
    */
    virtual bool RegionHealthyForBlock(const GoRegion& r,
                                       const GoBlock& b) const;

    /** Compute all GoBlock's, GoChain's and GoRegion's on m_board */
    virtual void GenBlocksRegions();

    /** Like @see GoStaticSafetySolver::FindClosure, but uses GoChain's */
    virtual void FindClosure(SgVectorOf<GoBlock>* blocks) const;

    /** Like @see GoStaticSafetySolver::FindTestSets, but uses GoChain's */
    virtual void FindTestSets(SgVectorOf<SgVectorOf<GoBlock> >* sets,
                              SgBlackWhite color) const;

    /** static test if r is 2-vital. If yes add to safe set */
    virtual void Test2Vital(GoRegion* r, SgBWSet* safe);

    /** call Test2Vital for regions, add  2-vital ones to safe set */
    void Find2VitalAreas(SgBWSet* safe);

    /** try to find a safe pair of regions r1 + other, call AddToSafe() */
    bool FindSafePair(SgBWSet* safe,
                      SgBlackWhite color,
                      const SgPointSet& anySafe,
                      const GoRegion* r1);

     /** create new chain representing both chains connected in this region */
    void Merge(GoChain* c1, GoChain* c2, GoRegion* r, bool bySearch);

private:
    /** find pairs of regions that are safe together. */
    bool FindSurroundedRegionPair(SgBWSet* safe, SgBlackWhite color);

    /** Find new safe region */
    bool FindSurroundedSingleRegion(SgBWSet* safe, SgBlackWhite color);
    
    /** for chain info */
    SgHashCode m_code;
};

//----------------------------------------------------------------------------

#endif // GO_SAFETYSOLVER_H
