//----------------------------------------------------------------------------
/** @file GoRegion.h
    A GoRegion contained in a @see GoRegionBoard.

    A GoRegion is a maximal connected set of points surrounded by stones 
    of the same color.

    Regions compute properties such as 1-vitality,
    2-vitality [Mueller 95, p. 62-63],[Mueller 97b].
    They are used for static and search-based safety solvers.
    They are also used for better move generation, e.g. in semeai.
*/
//----------------------------------------------------------------------------

#ifndef GO_REGION_H
#define GO_REGION_H

#include <bitset>
#include <iosfwd>
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "GoEyeCount.h"
#include "SgVector.h"
#include "SgIncrementalStack.h"
#include "SgMiaiStrategy.h"
#include "SgPointArray.h"

class GoBlock;
class GoChain;
class GoRegionBoard;

//----------------------------------------------------------------------------

/** Information computed about a GoRegion.
    @todo Does not follow naming convention for constants in global
    namespace; rename to all-uppercase and use prefix GO_
*/
enum GoRegionFlag
{
    GO_REGION_SMALL,
    GO_REGION_CORRIDOR,
    GO_REGION_STATIC_1VC,
    GO_REGION_1VC,
    GO_REGION_STATIC_2V,
    GO_REGION_2V,
    GO_REGION_SINGLE_BLOCK_BOUNDARY,
    GO_REGION_OPP_CAN_LIVE_INSIDE,
    GO_REGION_AT_LEAST_SEKI,
    GO_REGION_SAFE,
    GO_REGION_PROTECTED_CUTS,
    GO_REGION_STATIC_1VITAL,
    GO_REGION_1VITAL,
    GO_REGION_USED_FOR_MERGE,
    GO_REGION_VALID,
    GO_REGION_COMPUTED_BLOCKS,
    GO_REGION_COMPUTED_CHAINS,
    GO_REGION_COMPUTED_NAKADE,
    _GO_REGION_FLAG_COUNT
};

/** Set of GoRegionFlag */
typedef std::bitset<_GO_REGION_FLAG_COUNT> GoRegionFlags;

//----------------------------------------------------------------------------

/** GoRegion represents a region surrounded by blocks of one color.

    Each empty point, and each point occupied by white,
    is contained in exactly one black region.
    Similarly each empty point, and each point occupied by black,
    is contained in exactly one white region.

    A region keeps data such as its color, the blocks and chains of its color,
    the eye status, and a strategy to follow to keep the region safe.

    Regions can provide liberties for its boundary blocks.
    They can be "1-vital" or "2-vital". This is used by safety solvers.
    For details see [Mueller 1997]:
    Playing it safe: Recognizing secure territories in computer Go by using
    static rules and search. In H. Matsubara, editor, Game Programming
    Workshop in Japan '97, pages 80-86, Computer Shogi Association, Tokyo, 
    Japan, 1997.

    @todo Avoid cyclic dependency with GoBlock
*/
class GoRegion
{
public:
    /** Construct region on points in color */
    GoRegion(const GoBoard& board, const SgPointSet& points,
             SgBlackWhite color);

    /** Destructor */
    ~GoRegion()
    {
#ifndef NDEBUG
        ++s_free;
#endif
    }

    /** Clear all flags etc. to recompute region */
    void ReInitialize();

    /** For debugging */
    void CheckConsistency() const;

    // Accessors
    /** The points of the region */
    const SgPointSet& Points() const
    {
        return m_points;
    }

    /** The dependency set, outside neighbors of m_points */
    SgPointSet Dep() const
    {
        return m_points.Border(m_bd.Size());
    }

    /** liberties not adjacent to boundary stones */
    SgPointSet AllInsideLibs() const;

    /** Chains of region */
    const SgVectorOf<GoChain>& Chains() const
    {
        return m_chains;
    }

    /** Blocks of region */
    const SgVectorOf<GoBlock>& Blocks() const
    {
        return m_blocks;
    }

    /** Blocks of region.
        Non const version used by GoRegionBoard.
    */
    SgVectorOf<GoBlock>& BlocksNonConst()
    {
        return m_blocks;
    }

    /** Interior blocks of region: all liberties are in m_points */
    SgVectorOf<GoBlock> InteriorBlocks() const;

    /** Is block an interior block of this region? */
    bool IsInteriorBlock(const GoBlock* block) const;

    /** Is block a boundary block of this region? */
    bool IsBoundaryBlock(const GoBlock* block) const
    {
        return ! IsInteriorBlock(block);
    }

    /** all points of region's blocks - even those not contained in Dep() */
    SgPointSet BlocksPoints() const;

    /** area of points plus interior blocks */
    SgPointSet PointsPlusInteriorBlocks() const;

    /** color of region */
    SgBlackWhite Color() const
    {
        return m_color;
    }

    /** minimum number of eyes in region */
    int MinEyes() const {return m_eyes.MinEyes();}

    /** maximum number of eyes in region */
    int MaxEyes() const {return m_eyes.MaxEyes();}

    /** minimum number of potential eyes in region */
    int MinPotEyes() const {return m_eyes.MinPotEyes();}

    /** maximum number of potential eyes in region */
    int MaxPotEyes() const {return m_eyes.MaxPotEyes();}

    /** Write all information */
    void Write(std::ostream& out) const;

    /** Write short identifier */
    void WriteID(std::ostream& out) const;

    // Flags
    /** get value of precomputed flag */
    bool GetFlag(GoRegionFlag flag) const;

    /** compute, then get value of precomputed flag */
    bool ComputeAndGetFlag(GoRegionFlag flag);

    /** test if flag has been computed */
    bool ComputedFlag(GoRegionFlag flag) const;

    /** compute the flag, set it to true or false */
    void ComputeFlag(GoRegionFlag flag);

    /** Only blocks are incrementally updated, others must be reset after
        move
    */
    void ResetNonBlockFlags()
    {   m_computedFlags.reset(); // clear all other flags.
        m_computedFlags.set(GO_REGION_COMPUTED_BLOCKS);
        m_eyes.Clear();
        Invalidate();
    }

    /** is region data valid? */
    bool IsValid() const {return m_flags.test(GO_REGION_VALID);}

    /** reset valid flag, region data not current anymore */
    void Invalidate() {m_flags.reset(GO_REGION_VALID);}

    /** Computes GO_REGION_SMALL, GO_REGION_CORRIDOR, 
        GO_REGION_SINGLE_BLOCK_BOUNDARY, 
        GO_REGION_STATIC_1VC, GO_REGION_STATIC_2V
    */
    void ComputeBasicFlags();

    /** compute flag */
    void DoComputeFlag(GoRegionFlag flag);

    /** mark that flag is computed now */
    void SetComputedFlag(GoRegionFlag flag) {m_computedFlags.set(flag);}

    /** set flag to value */
    void SetFlag(GoRegionFlag flag, bool value)
    {   m_computedFlags.set(flag);
        m_flags.set(flag, value);
    }

    /** store search depth used in ExVital1Task for 1vc search */
    void Set1VCDepth(int depth) {m_1vcDepth = depth;}

    // Queries
    /** does region have two free (unused) liberties to connect chains? */
    bool Has2ConnForChains(const GoChain* c1, const GoChain* c2) const;

    /** call Has2ConnForBlocks for region with exactly two chains*/
    bool Has2Conn() const;

    /** is this healthy for a block in list? */
    bool HealthyForSomeBlock(const SgVectorOf<GoBlock>& blocks) const;

    /** Is region completely surrounded by blocks? */
    bool IsSurrounded(const SgVectorOf<GoBlock>& blocks) const;

    /** did ExVital1Task have at least this search depth? */
    bool ComputedVitalForDepth(int depth) const;

    /** is any adjacent block safe? */
    bool SomeBlockIsSafe() const;

    /** are all adjacent block safe? */
    bool AllBlockIsSafe() const;

    /** Is block of anchor adjacent to region? */
    bool AdjacentToBlock(SgPoint anchor) const;

    /** Is one of blocks given by anchors adjacent to region? */
    bool AdjacentToSomeBlock(const SgVector<SgPoint>& anchors) const;

    /** For flat region: cuts not occupied is enough to make region 1-vital */
    bool Safe2Cuts(const GoBoard& board) const;

    /** Test if all empty points are liberties of blocks
        @todo does not test only for boundary block libs, but all libs.
    */
    bool AllEmptyAreLibs() const;

    /** Test if points of region are 2-vital for color.
        Condition 1: all empty points are in liberties of some boundary block
        Condition 2: two intersection points (@see Has2IPs)
        refined version: points need not be liberty, if next to both IP.
        Example: bent-5 in the corner, as in Berlekamp/Wolfe C.11 top left
        corner
    */
    bool Has2SureLibs(SgMiaiStrategy* miaiStrategy) const;

    /** block liberties in this region */
    void InsideLibs(const GoBlock* b, SgVector<SgPoint>* libs) const;

    /** Is a liberty of b in region? */
    bool HasBlockLibs(const GoBlock* b) const;

    /** Can a block get n libs inside region? */
    bool HasLibsForBlock(const GoBlock* b, int n) const;

    /** Does each adjacent block have a liberty in region? */
    bool HasLibForAllBlocks() const;

    /** Does each block have n liberties in region? */
    bool HasLibsForAllBlocks(int n) const;

    /** Can we find two connection paths to each interior empty point? 
        This implements only the simple, non-recursive case.
        See Find2ConnForAllInterior() for a more general solution.
    */
    bool Find2ConnForAll() const;

    /** find 2-connection paths for all interior empty points, using recursive
        extension. Erik v.d.Werf's recursive extension to Find2ConnForAll.
    */
    bool Find2ConnForAllInterior(SgMiaiStrategy* miaiStrategy,
                                 SgVector<SgPoint>& usedLibs) const;

    /** An IP divides a region into two eyes AND connects all surrounding
        blocks, this one defines that ip has to be adjancent to all interior
        points.
    */
    bool Has2IPs(const SgVector<SgPoint>& interiorEmpty, SgMiaiPair* ips)
        const;

    /** whether there are 2 intersection points, doesn't have to be adjacent
        to all interior points.
    */
    bool Has2IntersectionPoints(const SgVector<SgPoint>& usedLibs) const;

    /** Get all intersections points inside region */
    void GetIPs(SgVector<SgPoint>* ips) const;

    /** Get all SgMiaiPairs that can divide the region. A dividing 
        SgMiaiPair has two adjacent points that are libs from the 
        same boundary block, and they are split points for region.
        This is a loose condition for SgMiaiPairs. 
    */
    void GetDivideMiaiPairs(SgVector<SgMiaiPair>& pairs) const;

    /** Compute joint liberties of all m_blocks. Since we need at least 2
        joint libs, we stop computing if we find that this is impossible.
    */
    void JointLibs(SgVector<SgPoint>* libs) const;

    /** See ExEye::IsCorridor() */
    bool IsCorridor() const;

    /** update region if affected: old was merged into newChain */
    bool ReplaceChain(const GoChain* old, const GoChain* newChain); 

    /** chains are mergable if two connections are found */
    bool Find2Mergable(GoChain** c1, GoChain** c2) const;

    /** Find two so far unused liberties for connecting c1 and c2 
        @todo In future, must respect all other chain conditions 
              active in this region.
    */
    void Find2FreeLibs(const GoChain* c1, const GoChain* c2, 
                        SgPoint* lib1, SgPoint* lib2) const;

    /** Get blocks of color in area */
    void FindBlocks(const GoRegionBoard& ra);

    /** Set blocks for this region from pre-found blocks list */
    void SetBlocks(const SgVectorOf<GoBlock>& blocks);

    /** Get chain of color in area.
            @todo There must be faster ways to do this.
    */
    void FindChains(const GoRegionBoard& ra);

    /** Set safe flag for region */
    void SetToSafe() {SetFlag(GO_REGION_SAFE, true);}

    // Execute move helpers
    /** For incremental update - block no longer adjacent */
    void RemoveBlock(const GoBlock* b);

    /** Stone was played: remove from m_points */
    void OnAddStone(SgPoint p);

    /** Stone removed: add to m_points */
    void OnRemoveStone(SgPoint p);

    /** class finalization */
    static void Fini();

private:

    /** The board */
    const GoBoard& m_bd;

    /** Flags describing attributes of region */
    GoRegionFlags m_flags;

    /** Which flags have been computed? */
    GoRegionFlags m_computedFlags;

    /** Points of region */
    SgPointSet m_points;

    /** Color of region = color of surrounding stones */
    SgBlackWhite m_color;

    /** Blocks of m_color adjacent to region. Some may be on inside */
    SgVectorOf<GoBlock> m_blocks;

    /** Chains of region */
    SgVectorOf<GoChain> m_chains;  

    /** Number of eyes in region */
    GoEyeCount m_eyes;

    /** Point to change eye status. Can be SG_NULLPOINT */
    SgPoint m_vitalPoint;

    /** search depth used in ExVital1Task for 1vc search */
    int m_1vcDepth;

    /** Miai strategy to keep region safe. */
    SgMiaiStrategy m_miaiStrategy;

    /** A simple test if all cuts between blocks are protected. 
        Works only for corridors (@see IsCorridor):
        If corridor, then opp. can get at most 2 libs (Opp2L).
        If Opp2L && cut point empty then opp cut move is self atari,
        therefore cut is protected (and remains protected if opponent
        is captured and plays inside again)
    */
    bool ProtectedCuts(const GoBoard& board) const;

    /** Static test if region is 1-vital.
        @todo explain the algorithm.
    */
    bool ComputeIs1Vital() const;

    /** Static test if region is 1-vital and all blocks are connected. */
    bool StaticIs1VitalAndConnected() const;

    /** Compute eye status for some standard small regions */
    void ComputeNakade();

    /** Compute eye space for region having only 1 block */
    void ComputeSingleBlockEyeSpace();

    /** Compute eye space for region having more than 1 block */
    void ComputeMultipleBlockEyeSpace();

    /** Compute eye space for region */
    void ComputeEyeSpace();

    /** compute at most maxNu empty points in the interior
        @todo does not test only for boundary block libs, but all libs.
    */
    void InteriorEmpty(SgVector<SgPoint>* interiorEmpty, int maxNu) const;

#ifndef NDEBUG
    /** debugging bookkeeping. */
    static int s_alloc, s_free;
#endif
};

std::ostream& operator<<(std::ostream& stream, const GoRegion& r);

//----------------------------------------------------------------------------

#endif // GO_REGION_H

