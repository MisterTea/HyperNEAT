//----------------------------------------------------------------------------
/** @file GoBlock.h
    A block contained in a GoRegionBoard.
*/
//----------------------------------------------------------------------------

#ifndef GO_BLOCK_H
#define GO_BLOCK_H

#include <iosfwd>
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "SgBlackWhite.h"
#include "SgDebug.h"
#include "SgPoint.h"
#include "SgVector.h"

class GoRegion;

//----------------------------------------------------------------------------

/** A block augmented by a list of its healthy regions.
    Used together with GoRegion in GoRegionBoard.
    @todo Avoid cyclic dependency with GoBlock
*/
class GoBlock
{
public:
    /** GoBlock Constructor
        Parameters:
        color: color of block
        anchor: stone identifying block
        board: the board we are on
    */
    GoBlock(SgBlackWhite color, SgPoint anchor, const GoBoard& board)
        : m_has1Eye(false),
          m_bd(board),
          m_color(color),
          m_anchor(anchor),
          m_isSafe(false)
    {
        ++s_alloc;
        for (GoBoard::StoneIterator it(board, anchor); it; ++it)
            m_stones.Include(*it);
    }

    /** Destructor */
    virtual ~GoBlock(){ ++s_free;}

    /** For debugging */
    void CheckConsistency() const;

    /** Write data for block */
    virtual void Write(std::ostream& out) const;

    /** Write short identifier for block */
    virtual void WriteID(std::ostream& out) const;

    // Accessors
    SgBlackWhite Color() const
    {
        return m_color;
    }

    /** The unique stone with smallest number identifying a block */
    SgPoint Anchor() const
    {
        return m_anchor;
    }

    /** set of all stones */
    const SgPointSet& Stones() const
    {
        return m_stones;
    }

    /** number of liberties */
    int NuLiberties() const
    {
        return m_bd.NumLiberties(m_anchor);
    }

    /** set of all liberties
        @todo slow
    */
    SgPointSet Liberties() const
    {
        return m_stones.Border(m_bd.Size()) & m_bd.AllEmpty();
    }

    /** is block proven safe? */
    bool IsSafe() const
    {
        return m_isSafe;
    }

    /** Are all empty points in area our liberties? */
    virtual bool AllEmptyAreLiberties(const SgPointSet& area) const;

    /** is lib our liberty? */
    bool HasLiberty(SgPoint lib) const
    {
        return m_bd.IsLibertyOfBlock(lib, m_anchor);
    }

    /** Does block have a simple eye? */
    bool Has1Eye() const
    {
        return m_has1Eye;
    }

    /** Mark block as proven safe */
    void SetToSafe()
    {
        m_isSafe = true;
    }

    /** compute if block has 1 clear eye, if yes set the m_has1Eye flag */
    void TestFor1Eye(const GoRegion* r);

    /** Clear previous computation */
    virtual void ReInitialize()
    {
        m_isSafe = false;
        m_has1Eye = false;
        m_healthy.Clear();
    }

    /** For incremental computation: add stone to block */
    void AddStone(SgPoint stone);

    /** For undo: remove added stone */
    void RemoveStone(SgPoint stone);

    /** r is healthy for this */
    void AddHealthy(GoRegion* r)
    {
        if (! m_healthy.Contains(r))
            m_healthy.PushBack(r);
#ifndef NDEBUG
        else
            // @todo debug me! see case 1540-1550 in uct_move.tst
            // seems to happen after same position recomputed with second
            // color.
            SgDebug() << "DOUBLE ADD " << r << '\n';
#endif
    }

    /** For incremental computation: r is no longer a neighbor region */
    void RemoveRegion(GoRegion* r)
    {
        m_healthy.Exclude(r);
    }
    // @todo keep all regions stored.

    /** is r in our healthy list? */
    bool ContainsHealthy(const GoRegion* r) const
    {
        return m_healthy.Contains(r);
    }

    /** list of healthy regions */
    const SgVectorOf<GoRegion>& Healthy() const
    {
        return m_healthy;
    }

    /** class finalization */
    static void Fini();

protected:
    /** list of healthy regions */
    SgVectorOf<GoRegion> m_healthy;

    /** does block have one eye? */
    bool m_has1Eye;

    /** board */
    const GoBoard& m_bd;

    /** This constructor used only in GoChain constructor
        Stones is set to the union of the merged chains.
    */
    GoBlock(const GoBlock* b, const SgPointSet& stones,
           const SgVectorOf<GoRegion>& healthy)
        : m_healthy(healthy),
          m_has1Eye(false),
          m_bd(b->m_bd),
          m_color(b->Color()),
          m_stones(stones),
          m_anchor(SG_NULLPOINT),
          m_isSafe(false)

    {
        ++s_alloc;
    }

private:
    /** Color of block */
    SgBlackWhite m_color;

    /** Stones of block */
    SgPointSet m_stones;

    /** Anchor*/
    SgPoint m_anchor;

    /** Is block marked as safe? */
    bool m_isSafe;

    /** Bookkeeping for debugging */
    static int s_alloc, s_free;
    // AR add m_healthy2 for 2 sure liberty regions?
};

std::ostream& operator<<(std::ostream& stream, const GoBlock& b);

//----------------------------------------------------------------------------

#endif // GO_BLOCK_H
