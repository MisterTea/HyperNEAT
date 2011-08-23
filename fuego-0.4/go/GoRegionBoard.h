//----------------------------------------------------------------------------
/** @file GoRegionBoard.h
    A GoRegionBoard provides the connected components of a GoBoard.
*/
//----------------------------------------------------------------------------

#ifndef GO_REGIONBOARD_H
#define GO_REGIONBOARD_H

#include "GoBoard.h"
#include "GoRegion.h"

class GoBlock;
class GoChain;

//----------------------------------------------------------------------------

/** GoRegionBoard provides GoRegion, GoBlock and optionally GoChain.
    To keep it updated during search, call OnExecutedMove
    (or OnExecutedUncodedMove) and OnUndoneMove when Executing/Undoing a move.

    A GoRegionBoard depends on a GoBoard (supplied at construction)
    for keeping the low-level board state, Go rules etc.

    There is a certain amount of duplication between the private blocks of a
    GoBoard and the GoBlock's in a GoRegionBoard.

    GoChain's are not updated automatically for performance reasons
    - call GenChains() to update them.
*/
class GoRegionBoard
{
public:

    /** Constructor */
    explicit GoRegionBoard(const GoBoard& board);

    /** release memory. Calls Clear() */
    virtual ~GoRegionBoard();

    /** delete all blocks and regions, set board size */
    void Clear();

    /** For debugging */
    void CheckConsistency() const;

    /** Have blocks and regions been computed for current board position? */
    bool UpToDate() const
    {
        return    ! m_invalid
               && m_boardSize == m_board.Size()
               && m_code == Board().GetHashCode();
    }
    
    bool ComputedHealthy() const
    {
        return m_computedHealthy;
    }

    /** Have chains been computed for current board position? */
    bool ChainsUpToDate() const
    {
        return UpToDate() && m_chainsCode == Board().GetHashCode();
    }

    /** MUST call before playing move on GoBoard */
    void ExecuteMovePrologue();

    /** Called after a move or added stone has been successfully executed.
        The board is guaranteed to be in a legal state.
    */
    void OnExecutedMove(GoPlayerMove playerMove);

    /** Similar to OnExecutedMove, but move is not coded into one int. */
    void OnExecutedUncodedMove(int move, SgBlackWhite moveColor);

    /** Called after a move has been undone.
        The board is guaranteed to be in a legal state.
    */
    void OnUndoneMove();

    /** All GoBlock's of given color */
    SgVectorOf<GoBlock>& AllBlocks(SgBlackWhite color)
    {
        return m_allBlocks[color];
    }

    /** All GoBlock's of given color */
    const SgVectorOf<GoBlock>& AllBlocks(SgBlackWhite color) const
    {
        return m_allBlocks[color];
    }

    /** All GoChain's of given color */
    SgVectorOf<GoChain>& AllChains(SgBlackWhite color)
    {
        return m_allChains[color];
    }

    /** All GoChain's of given color */
    const SgVectorOf<GoChain>& AllChains(SgBlackWhite color) const
    {
        return m_allChains[color];
    }

    /** All GoRegion's of given color */
    SgVectorOf<GoRegion>& AllRegions(SgBlackWhite color)
    {
        return m_allRegions[color];
    }

    /** All GoRegion's of given color */
    const SgVectorOf<GoRegion>& AllRegions(SgBlackWhite color) const
    {
        return m_allRegions[color];
    }

    /** See GoBoard::All */
    const SgPointSet& All(SgBlackWhite color) const
    {
        return Board().All(color);
    }

    /** See GoBoard::AllEmpty */
    const SgPointSet& AllEmpty() const {return Board().AllEmpty();}

    /** See GoBoard::AllPoints */
    const SgPointSet& AllPoints() const {return Board().AllPoints();}

    /** See GoBoard::IsColor */
    bool IsColor(SgPoint p, int c) const {return Board().IsColor(p, c);}

    /** write information on all GoBlock's */
    void WriteBlocks(std::ostream& stream) const;

    /** write information on all GoRegion's */
    void WriteRegions(std::ostream& stream) const;

    /** generate all blocks and regions */
    void GenBlocksRegions();

    /** Compute all GoChain's
        @todo currently this only creates 1 chain for each block.
        The merging is only done in GoSafetySolver::GenBlocksRegions()
        and must be moved here.
    */
    void GenChains();

    /** Clear all flags etc. to recompute regions and blocks */
    void ReInitializeBlocksRegions();

    /** mark all regions that the given attribute has been computed */
    void SetComputedFlagForAll(GoRegionFlag flag);

    const GoBoard& Board() const
    {
        return m_board;
    }

    /** Searches the list of all blocks for the block.
        Boundary must belong to a single block.
    */
    GoBlock* GetBlock(const SgPointSet& boundary,
                         SgBlackWhite color) const;

    /** For incremental update, region where stone was just played */
    GoRegion* PreviousRegionAt(SgPoint p, SgBlackWhite color) const
    {
        SG_ASSERT(Board().Occupied(p));
        SG_ASSERT(m_region[color][p] != 0);
        return m_region[color][p];
    }

    /** Region of color at point p */
    GoRegion* RegionAt(SgPoint p, SgBlackWhite color) const
    {
        SG_ASSERT(UpToDate());
        SG_ASSERT(! Board().IsColor(p, color));
        SG_ASSERT(m_region[color][p] != 0);
        return m_region[color][p];
    }

    /** Region of color in area */
    void RegionsAt(const SgPointSet& area, SgBlackWhite color,
                            SgVectorOf<GoRegion>* regions) const;

    /** Region of color adjacent to points */
    void AdjacentRegions(const SgVector<SgPoint>& points, SgBlackWhite color,
                            SgVectorOf<GoRegion>* regions) const;

    /** Return GoBlock's just captured on last move, before update.
        For incremental update.
        Can be called for any empty point.
        returns 0 if no previous block there.
    */
    void PreviousBlocksAt(const SgVector<SgPoint>& area, SgBlackWhite color,
                            SgVectorOf<GoBlock>* captures) const;

    /** GoBlock at point p*/
    GoBlock* BlockAt(SgPoint p) const
    {
        SG_ASSERT(m_block[p] != 0);
        return m_block[p];
    }

    /** GoChain at point p*/
    GoChain* ChainAt(SgPoint p) const;

    /** Is block at point p marked as safe?*/
    bool IsSafeBlock(SgPoint p) const;

    /** Set safe flag for block at p*/
    void SetToSafe(SgPoint p) const;

    /** Set safe flags for all blocks in safe*/
    void SetSafeFlags(const SgBWSet& safe);

    /** Set m_computedHealthy flag to true */
    void SetComputedHealthy();
    
    /** class initialization */
    static bool Init();

    /** class finalization */
    static void Fini();

private:

    // Compute from scratch helpers
    /** Generate blocks */
    void GenBlocks();

    /** Set the m_has1Eye flag for all blocks with 1 eye */
    void FindBlocksWithEye();

    // Execute move helpers
    /** Generate the block with given anchor */
    GoBlock* GenBlock(SgPoint anchor, SgBlackWhite color);

    /** Generate a region of color in area */
    GoRegion* GenRegion(const SgPointSet& area, SgBlackWhite color);

    /** incremental update of block after move */
    void UpdateBlock(int move, SgBlackWhite moveColor);

    /** Sets m_region elements to point to r */
    void SetRegionArrays(GoRegion* r);

    /** add block to GoRegionBoard */
    void AddBlock(GoBlock* b, bool isExecute = true);

    /** remove block from GoRegionBoard */
    void RemoveBlock(GoBlock* b, bool isExecute, bool removeFromRegions);

    /** add region to GoRegionBoard */
    void AddRegion(GoRegion* r, bool isExecute = true);

    /** remove region from GoRegionBoard */
    void RemoveRegion(GoRegion* r, bool isExecute = true);

    /** For all captured stones: merge its adjacent previous regions into one.
        then for all captured stones: add their area to its single adjacent
        region.
    */
    void MergeAdjacentAndAddBlock(SgPoint move, SgBlackWhite capturedColor);

    /** Merge all regions and the captured area into new large region */
    GoRegion* MergeAll(const SgVectorOf<GoRegion>& regions,
                      const SgPointSet& captured, SgBlackWhite color);

    // Undo move helpers
    /** stores incremental state changes for execute/undo moves */
    SgIncrementalStack m_stack;

    /** push on m_stack */
    void PushRegion(int type, GoRegion* r);

    /** push on m_stack */
    void PushStone(GoRegion* r, SgPoint move);

    /** push on m_stack */
    void PushBlock(int type, GoBlock* b);

    /** add stome to b and push information on m_stack */
    void AppendStone(GoBlock* b, SgPoint move);

    // data members

    const GoBoard& m_board;

    /** pointer to region, defined only at anchor of region */
    SgBWArray<SgPointArray<GoRegion*> > m_region;

    /** pointer from stone to block, 0 if empty point */
    SgPointArray<GoBlock*> m_block;

    /** All blocks on board */
    SgBWArray<SgVectorOf<GoBlock> > m_allBlocks;

    /** All chains on board */
    SgBWArray<SgVectorOf<GoChain> > m_allChains;

    /** All regions on board */
    SgBWArray<SgVectorOf<GoRegion> > m_allRegions;

    /** Code for last time block and region information was computed */
    SgHashCode m_code;

    /** Code for last time chain information was computed */
    SgHashCode m_chainsCode;

    /** does block and region data match current board? */
    bool m_invalid;
    
    /** has healthy count been computed for all blocks?
        @todo in fully incremental mode, this should be determined locally
        for each block, not globally.
    */
    bool m_computedHealthy;
    
    /** Boardsize is needed to avoid problems with resizing an empty board */
    int m_boardSize;

    /** debugging bookkeeping. @todo do it in debug only */
    static int s_alloc, s_free;

};

//----------------------------------------------------------------------------

#endif // GO_REGIONBOARD_H
