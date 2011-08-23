//----------------------------------------------------------------------------
/** @file GoStaticSafetySolver.h
    Recognize safe stones and territories statically.
*/
//----------------------------------------------------------------------------

#ifndef GO_STATICSAFETYSOLVER_H
#define GO_STATICSAFETYSOLVER_H

#include "GoBoard.h"
#include "GoRegion.h"
#include "GoRegionBoard.h"

//----------------------------------------------------------------------------

/** Common algorithm for static safety.
    The algorithm is used to implement both Benson's algorithm for
    unconditional safety and a solver using the extended 1-vitality and
    2-vitality definitions from Martin Mueller's thesis [Mueller 95, p. 62-63]
    and from [Mueller 97b].
*/
class GoStaticSafetySolver
{
public:
    /** Constructor. If regions = 0, allocates own. */
    GoStaticSafetySolver(const GoBoard& board, GoRegionBoard* regions = 0);

    /** Destructor, deallocates if there are own regions */
    virtual ~GoStaticSafetySolver();

    /** @name Accessors */
    // @{

    /** our board */
    const GoBoard& Board() const;

    // @} // @name


    /** @name Forwarding accessors for GoRegionBoard */
    // @{

    /** See GoRegionBoard::UpToDate */
    virtual bool UpToDate() const;

    /** our regions */
    const GoRegionBoard* Regions() const;

    // @} // @name

protected:

    /** our regions */
    GoRegionBoard* Regions();

    /** Main step of Benson's algorithm */
    virtual void FindTestSets(SgVectorOf<SgVectorOf<GoBlock> >* sets,
                              SgBlackWhite color) const;

    /** Compute closure of blocks set for Benson's algorithm.
        Expand set of blocks until all blocks adjacent to all adjacent
        regions are in set.
        see [Benson] for explanation.
    */
    virtual void FindClosure(SgVectorOf<GoBlock>* blocks) const;

    /** Compute all GoBlock's and GoRegion's on board*/
    virtual void GenBlocksRegions();

    /** Is r healthy for b? Implements Benson, override for better tests
        Benson's classic healthyness test: all empty points of region must be
        liberties of the block.
    */
    virtual bool RegionHealthyForBlock(const GoRegion& r,
                                       const GoBlock& b) const;

    /** Main function, compute all safe points on board */
    virtual void FindSafePoints(SgBWSet* safe);


    /** Find healthy regions for block, calls RegionHealthyForBlock */
    virtual void FindHealthy(); //

    /** Test if list of Benson blocks forms a living group.
        Each block must have a sure liberty count of at least 2.
        A region provides one sure liberty if it is healthy and its
        boundary consists only of blocks in the list.
    */
    void TestAlive(SgVectorOf<GoBlock>* blocks, SgBWSet* safe,
                   SgBlackWhite color);

    /** Reduce regions: keep only if completely surrounded by blocks */
    void TestAdjacent(SgVectorOf<GoRegion>* regions,
                      const SgVectorOf<GoBlock>& blocks) const;

private:
    /** The board we are computing on */
    const GoBoard& m_board;

    /** Contains the GoRegion's and GoBlock's we are using */
    GoRegionBoard* m_regions;

    /** Did we allocate the GoRegionBoard or did the user supply it? */
    bool m_allocRegion;

    /** not implemented */
    GoStaticSafetySolver(const GoStaticSafetySolver&);

    /** not implemented */
    GoStaticSafetySolver& operator=(const GoStaticSafetySolver&);
};


inline const GoBoard& GoStaticSafetySolver::Board() const
{
    return m_board;
}

inline GoRegionBoard* GoStaticSafetySolver::Regions()
{
    SG_ASSERT(m_regions);
    return m_regions;
}

inline const GoRegionBoard* GoStaticSafetySolver::Regions() const
{
    SG_ASSERT(m_regions);
    return m_regions;
}

//----------------------------------------------------------------------------

#endif // GO_STATICSAFETYSOLVER_H
