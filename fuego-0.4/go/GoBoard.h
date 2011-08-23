//----------------------------------------------------------------------------
/** @file GoBoard.h
    Go board with basic board and blocks.

    GoBoard defines a Go board that implements the rules of Go and provides
    a lot of helper functions to get blocks, liberties, adjacent blocks,
    and so on.
*/
//----------------------------------------------------------------------------

#ifndef GO_BOARD_H
#define GO_BOARD_H

#include <bitset>
#include <cstring>
#include <boost/cstdint.hpp>
#include <boost/static_assert.hpp>
#include "GoPlayerMove.h"
#include "GoRules.h"
#include "GoSetup.h"
#include "SgArray.h"
#include "SgBoardConst.h"
#include "SgBoardColor.h"
#include "SgMarker.h"
#include "SgBWArray.h"
#include "SgBWSet.h"
#include "SgHash.h"
#include "SgNbIterator.h"
#include "SgPoint.h"
#include "SgPointArray.h"
#include "SgPointIterator.h"
#include "SgPointSet.h"
#include "SgSList.h"
using namespace boost;

//----------------------------------------------------------------------------

/** Board size to choose at startup. */
const int GO_DEFAULT_SIZE = (SG_MAX_SIZE >= 19 ? 19 : SG_MAX_SIZE);

/** Maximum number of moves in game.
    HEURISTIC: Longest possible game.
*/
const int GO_MAX_NUM_MOVES = (3 * SG_MAX_SIZE * SG_MAX_SIZE);

//----------------------------------------------------------------------------

/** Flags for moves. */
enum GoMoveInfoFlag
{
    /** The move was a repetition move. */
    GO_MOVEFLAG_REPETITION,

    /** The move caused self-removal of stones. */
    GO_MOVEFLAG_SUICIDE,

    /** The move captured one or more enemy stones. */
    GO_MOVEFLAG_CAPTURING,

    /** The move was illegal according to the current rules and allow ko
        settings.
    */
    GO_MOVEFLAG_ILLEGAL,

    _GO_NU_MOVEFLAG
};

typedef std::bitset<_GO_NU_MOVEFLAG> GoMoveInfo;

//----------------------------------------------------------------------------

/** Static list having enough room for all points on board and SG_PASS. */
typedef SgSList<SgPoint,SG_MAX_ONBOARD + 1> GoPointList;

/** Static list having enough room for longest move sequence supported by
    GoBoard.
*/
typedef SgSList<SgPoint,GO_MAX_NUM_MOVES> GoSequence;

//----------------------------------------------------------------------------

/** Go board.
    It maintains the state of each point when playing moves and taking them
    back. Setup stones are only supported in the initial position.
    It provides basic information about the board state, e.g. blocks and
    liberties.
    The actual storage representation and updating of stones and liberties is
    encapsulated, it can only be accessed with GoBoard::LibertyIterator,
    GoBoard::LibertyCopyIterator, and GoBoard::StoneIterator.

    Boards are thread-safe (w.r.t. different instances) after construction
    (the constructor is not thread-safe, because it uses global variables
    via SgBoardConst).

    @see
    - @ref goboardko
    - @ref goboardhash
*/
class GoBoard
{
public:
    /** Maximum number of immediate ko recaptures for GoBoard::m_koColor.
        Enforced only if ko modifies hash
        @see KoModifiesHash()
    */
    static const int MAX_KOLEVEL = 3;

    /** Marker that can be used in client code.
        This marker is never used by this class, it is intended for external
        functions that operate on the board and can profit from the fast clear
        operation of SgMarker (if reused), but cannot store its own
        marker (or don't want to use a global variable for thread-safety).
        Since only one function can use this marker at a time, you should
        assert with SgReserveMarker that the marker is not used in a
        conflicting way.
    */
    mutable SgMarker m_userMarker;

    explicit GoBoard(int size = GO_DEFAULT_SIZE,
                     const GoSetup& setup = GoSetup(),
                     const GoRules& rules = GoRules());

    ~GoBoard();

    const SgBoardConst& BoardConst() const;

    /** Number of calls to Play since creation of this board. */
    uint64_t CountPlay() const;

    /** Re-initializes the board with new size.
        Keeps old GoRules.
    */
    void Init(int size, const GoSetup& setup = GoSetup());

    /** Re-initializes the board with new size and rules. */
    void Init(int size, const GoRules& rules,
              const GoSetup& setup = GoSetup());

    /** Non-const access to current game rules.
        The game rules are attached to a GoBoard for convenient access
        by the players only.
        The players and the class GoBoard should not assume that they are
        immutable; they can be changed from the outside using this function at
        anytime.
    */
    GoRules& Rules();

    /** Current game rules. */
    const GoRules& Rules() const;

    /** Return the size of this board. */
    SgGrid Size() const;

    /** Check if sufficient space on internal stacks.
        Should be checked before executing a move.
        If the internal stacks overflow, assertions will (hopefully)
        trigger in debug mode, but undefined behaviour occurs in release mode.
    */
    bool StackOverflowLikely() const;

    /** Check if move at point would be the first move there. */
    bool IsFirst(SgPoint p) const;

    /** Check if board is in a definitely new situation,
        with no possibility of repetition */
    bool IsNewPosition() const;

    /** Check if point is occupied by a stone.
        Can be called with border points.
    */
    bool Occupied(SgPoint p) const;

    bool IsEmpty(SgPoint p) const;

    bool IsBorder(SgPoint p) const;

    bool IsColor(SgPoint p, int c) const;

    SgBoardColor GetColor(SgPoint p) const;

    SgBlackWhite GetStone(SgPoint p) const;

    /** %Player whose turn it is to play. */
    SgBlackWhite ToPlay() const;

    /** Opponent of player whose turn it is to play. */
    SgBlackWhite Opponent() const;

    /** Set the current player. */
    void SetToPlay(SgBlackWhite player);

    /** See SgBoardConst::Line */
    SgGrid Line(SgPoint p) const;

    /** See SgBoardConst::Pos */
    SgGrid Pos(SgPoint p) const;

    /** Returns the offset to the point on the line above this point.
        Returns zero for points outside the board, and for the center
        point(s).
    */
    int Up(SgPoint p) const;

    /** Returns the offset along left side of the board.
        Left and right are as seen from the edge toward the center of the
        board.
        Returns zero for the same points as Up does.
    */
    int Left(SgPoint p) const;

    /** Returns the offset along right side of the board.
        @see Left for more info.
    */
    int Right(SgPoint p) const;

    /** Same as Left/Right, but the side is passed in as an index (0 or 1). */
    int Side(SgPoint p, int index) const;

    bool IsSuicide(SgPoint p, SgBlackWhite toPlay) const;

    bool IsValidPoint(SgPoint p) const;

    bool HasEmptyNeighbors(SgPoint p) const;

    int NumEmptyNeighbors(SgPoint p) const;

    /** Includes diagonals. */
    int Num8EmptyNeighbors(SgPoint p) const;

    bool HasNeighbors(SgPoint p, SgBlackWhite c) const;

    int NumNeighbors(SgPoint p, SgBlackWhite c) const;

    /** Includes diagonals. */
    int Num8Neighbors(SgPoint p, SgBlackWhite c) const;

    bool HasDiagonals(SgPoint p, SgBoardColor c) const;

    int NumDiagonals(SgPoint p, SgBoardColor c) const;

    int NumEmptyDiagonals(SgPoint p) const;

    bool HasNeighborsOrDiags(SgPoint p, SgBlackWhite c) const;

    /** @name Point sets */
    //@{

    SgPointSet Occupied() const;

    const SgPointSet& All(SgBlackWhite color) const;

    const SgPointSet& AllEmpty() const;

    const SgPointSet& AllPoints() const;

    /** See SgBoardConst::Corners */
    const SgPointSet& Corners() const;

    /** See SgBoardConst::Edges */
    const SgPointSet& Edges() const;

    /** See SgBoardConst::Centers */
    const SgPointSet& Centers() const;

    /** See SgBoardConst::SideExtensions */
    const SgPointSet& SideExtensions() const;

    /** See SgBoardConst::LineSet */
    const SgPointSet& LineSet(SgGrid line) const;

    //@}

    bool InCorner(SgPoint p) const;

    bool OnEdge(SgPoint p) const;

    bool InCenter(SgPoint p) const;

    /** See SgBoardConst::FirstBoardPoint */
    int FirstBoardPoint() const;

    /** See SgBoardConst::FirstBoardPoint */
    int LastBoardPoint() const;

    /** Information about the most recent call to Play.
        Guaranteed to be valid only directly after a call to Play.
        @see GoMoveInfoFlag
    */
    bool LastMoveInfo(GoMoveInfoFlag flag) const;

    GoMoveInfo GetLastMoveInfo() const;

    void AllowKoRepetition(bool allowKo);

    /** Make all repetition moves legal.
        @see @ref goboardko
    */
    void AllowAnyRepetition(bool allowAny);

    /** Enable modification of hash code by Ko moves.
        Can be used if Ko repetition is allowed.
        @warning You have to disable it in the same position, where it was
        enabled, otherwise the incremental update of the hash code does not
        work; use KoHashModifier in GoBoardUtil to do this automatically.
    */
    void SetKoModifiesHash(bool modify);

    bool KoRepetitionAllowed() const;

    /** Are all repetition moves legal?
        @see @ref goboardko
    */
    bool AnyRepetitionAllowed() const;

    bool KoModifiesHash() const;

    /** Play a move.
        Move needs to be SG_PASS or on-board empty point.
        If move is not legal according to the current GoRules, the
        move flag isIllegal will be set.
        After playing the color to play ys the opposite color of the color
        of the move.
    */
    void Play(SgPoint p, SgBlackWhite player);

    /** Play a move for the current player.
        @see Play(SgPoint,SgBlackWhite);
    */
    void Play(SgPoint p);

    /** Play a move.
        @see Play(SgPoint,SgBlackWhite);
    */
    void Play(GoPlayerMove move);

    /** Undo the most recent move. */
    void Undo();

    /** Whether there is any move to undo. */
    bool CanUndo() const;

    /** Check whether the move at 'p' is legal.
        Since it's not clear how 'p' was arrived at, any value of 'p' is
        admissible, even out of point range and on border points; just return
        false on such input.
        LastMoveInfo is guaranteed to be vaild after this call.
        Suicide moves are only legal, if SetSelfRemovalAllowed(true) was
        called.
    */
    bool IsLegal(int p, SgBlackWhite player) const;

    /** Check whether the move at 'p' is legal for color to play.
        @see IsLegal(int, SgBlackWhite).
    */
    bool IsLegal(int p) const;

    bool IsSuicide(SgPoint p) const;

    /** Whether the most recent move captured any stones. */
    bool CapturingMove() const;

    /** The stones removed from the board by the most recent move.
        Can be used for incremental update of other data structures.
        Includes captures and suicide stones.
        Only valid directly after a GoBoard::Play, otherwise undefined.
    */
    const GoPointList& CapturedStones() const;

    /** The stones captured by the most recent move.
        @see CapturedStones
    */
    int NuCapturedStones() const;

    /** The total number of stones of 'color' that have been
        captured by the opponent throughout the game. */
    int NumPrisoners(SgBlackWhite color) const;

    /** Setup information of the first position. */
    const GoSetup& Setup() const;

    /** Return the current number moves. */
    int MoveNumber() const;

    /** Return move with a certain move number.
        @param i The move number (starting with 0).
        @return The ith move.
     */
    GoPlayerMove Move(int i) const;

    /** Return last move played.
        @return The last move played or SG_NULLMOVE, if
        - No move was played yet
        - The last move was not by the opposite color of the current player
    */
    SgPoint GetLastMove() const;

    /** 2nd Last move = last move by ToPlay().
        Conditions similar to GetLastMove().
    */
    SgPoint Get2ndLastMove() const;

    /** Point which is currently illegal due to simple ko rule.
        Note that there could be more points illegal if superko rules
        are used.
        @return The ko point or SG_NULLPOINT, if none exists.
    */
    SgPoint KoPoint() const;

    /** Return hash code for this position.
        @warning Hash code for empty positions is always 0, independent of
        the board size.
    */
    const SgHashCode& GetHashCode() const;

    /** Return hash code for this position, modified by whose turn it
        is to play.
        Note that GetHashCode() != GetHashCodeInclToPlay(), regardless
        of whose turn it is to play.
    */
    SgHashCode GetHashCodeInclToPlay() const;

    /** Return the number of stones in the block at 'p'.
        Not defined for empty or border points.
    */
    int NumStones(SgPoint p) const;

    /** Return NumStones(p) == 1. */
    bool IsSingleStone(SgPoint p) const;

    /** Return whether the two stones are located in the same block.
        Return false if one of the stones is an empty or border point.
    */
    bool AreInSameBlock(SgPoint stone1, SgPoint stone2) const;

    /** Return the smallest point of the block at a point.
        Requires: Occupied(p) <br>
    */
    SgPoint Anchor(SgPoint p) const;

    /** Efficient combined test if point is occupied and belongs to a block.
        @return true, if point is occupied and belongs to the block with
        the given anchor.
    */
    bool IsInBlock(SgPoint p, SgPoint anchor) const;

    /** Check if empty point is a liberty of block.
        @param p The point the check.
        @param anchor The anchor of the block.
        @return true If point is liberty of block.
    */
    bool IsLibertyOfBlock(SgPoint p, SgPoint anchor) const;

    /** Get adjacent opponent blocks with a maximum number of liberties for a
        given block.
        Not defined for empty points.
        @param p The block to check.
        @param maxLib The maximum number of liberties of the neighbors.
        @param anchors Resulting neighbor anchors and an additional END_POINT.
        @param maxAnchors Array size of anchors (for detecting overflow in
        debug mode)
        @return Number of anchors (without the END_POINT)
    */
    int AdjacentBlocks(SgPoint p, int maxLib, SgPoint anchors[],
                       int maxAnchors) const;

    /** %List anchor of each block of color 'c' adjacent to the
        empty point 'p'.
        Assert if 'p' is not empty.
        Fill an array of points, terminated by END_POINT.
    */
    void NeighborBlocks(SgPoint p, SgBlackWhite c, SgPoint anchors[]) const;

    /** %List anchor of each block of color 'c' with at most 'maxLib'
        liberties adjacent to the empty point 'p'.
        Assert if 'p' is not empty.
        Fill an array of points, terminated by END_POINT.
    */
    void NeighborBlocks(SgPoint p, SgBlackWhite c, int maxLib,
                        SgPoint anchors[]) const;

    /** Number of stones currently on the board. */
    const SgBWArray<int>& TotalNumStones() const;

    int TotalNumStones(SgBlackWhite color) const;

    /** Number of empty points currently on the board. */
    int TotalNumEmpty() const;

    /** Return the liberty of 'blockInAtari' which must have exactly
        one liberty.
    */
    SgPoint TheLiberty(SgPoint blockInAtari) const;

    /** Return the number of liberties of the block at 'p'.
        Not defined for empty or border points.
    */
    int NumLiberties(SgPoint p) const;

    /** Return whether block has at most n liberties. */
    bool AtMostNumLibs(SgPoint block, int n) const;

    /** Return whether block has at least n liberties. */
    bool AtLeastNumLibs(SgPoint block, int n) const;

    /** Return whether the number of liberties of the block at 'p' is one.
        Requires: Occupied(p)
    */
    bool InAtari(SgPoint p) const;

    /** Check if point is occupied and in atari.
        Faster than Occupied(p) || InAtari(p).
        May be called for border points.
    */
    bool OccupiedInAtari(SgPoint p) const;

    /** Return whether playing colour c at p can capture anything,
        ignoring any possible repetition.
    */
    bool CanCapture(SgPoint p, SgBlackWhite c) const;

    /** %Player who has immediately retaken a ko.
        It is SG_EMPTY if no player has done it.
    */
    SgEmptyBlackWhite KoColor() const;

    /** Number of times that KoColor has immediately retaken a ko. */
    int KoLevel() const;

    /** %Player who will lose any ko.
        KoLoser is a player who is a priori determined to lose ko fights.
        therefore he is never allowed to become 'KoColor'
        If KoLoser is empty, no such prior bias is assumed.
    */
    SgEmptyBlackWhite KoLoser() const;

    /** See KoLoser. */
    void SetKoLoser(SgEmptyBlackWhite color);

    /** Checks whether all the board data structures are in a consistent
        state.
    */
    void CheckConsistency() const;

    /** Remember current position for quickly undoing a sequence of moves.
        Note that for short sequences of moves this can take longer than
        incrementally restoring the state by multiple calls to Undo().
    */
    void TakeSnapshot();

    /** Restore a snapshot.
        Can only be called, if previously TakeSnapshot() was called and
        the current position is a followup position of the snapshot position.
        RestoreSnapshot() can used multiple times for the same snapshot.
        @see TakeSnapshot()
    */
    void RestoreSnapshot();

private:
    /** Data related to a block of stones on the board. */
    class Block
    {
    public:
        /** Upper limit for liberties.
            Proof?
        */
        static const int MAX_LIBERTIES = (SG_MAX_SIZE / 3) * 2 * SG_MAX_SIZE;

        typedef SgSList<SgPoint,MAX_LIBERTIES> LibertyList;

        typedef LibertyList::Iterator LibertyIterator;

        typedef GoPointList::Iterator StoneIterator;

        SgPoint Anchor() const { return m_anchor; }

        void UpdateAnchor(SgPoint p) { if (p < m_anchor) m_anchor = p; }

        void AppendLiberty(SgPoint p) { m_liberties.PushBack(p); }

        void AppendStone(SgPoint p) { m_stones.PushBack(p); }

        SgBlackWhite Color() const { return m_color; }

        void ExcludeLiberty(SgPoint p) { m_liberties.Exclude(p); }

        void Init(SgBlackWhite c, SgPoint anchor)
        {
            SG_ASSERT_BW(c);
            m_color = c;
            m_anchor = anchor;
            m_stones.SetTo(anchor);
            m_liberties.Clear();
        }

        void Init(SgBlackWhite c, SgPoint anchor, GoPointList stones,
                  LibertyList liberties)
        {
            SG_ASSERT_BW(c);
            SG_ASSERT(stones.Contains(anchor));
            m_color = c;
            m_anchor = anchor;
            m_stones = stones;
            m_liberties = liberties;
        }

        const LibertyList& Liberties() const { return m_liberties; }

        int NumLiberties() const { return m_liberties.Length(); }

        int NumStones() const { return m_stones.Length(); }

        void PopStone() { m_stones.PopBack(); }

        void SetAnchor(SgPoint p) { m_anchor = p; }

        const GoPointList& Stones() const { return m_stones; }

    private:
        SgPoint m_anchor;

        SgBlackWhite m_color;

        LibertyList m_liberties;

        GoPointList m_stones;
    };

    /** Board hash code.
        @see @ref goboardhash
    */
    class HashCode
    {
    public:
        void Clear();

        const SgHashCode& Get() const;

        SgHashCode GetInclToPlay(SgBlackWhite toPlay) const;

        void XorCaptured(int moveNumber, SgPoint firstCapturedStone);

        void XorStone(SgPoint p, SgBlackWhite c);

        void XorWinKo(int level, SgBlackWhite c);

    private:
        // Index ranges used in global Zobrist table
        static const int START_INDEX_TOPLAY = 1;
        static const int END_INDEX_TOPLAY = 2;
        static const int START_INDEX_STONES = 3;
        static const int END_INDEX_STONES = 2 * SG_MAXPOINT;
        static const int START_INDEX_WINKO = 2 * SG_MAXPOINT + 1;
        static const int END_INDEX_WINKO = 2 * SG_MAXPOINT + SG_MAX_SIZE + 1;
        static const int START_INDEX_CAPTURES
        = 2 * SG_MAXPOINT + SG_MAX_SIZE + 2;
        static const int END_INDEX_CAPTURES = 3 * SG_MAXPOINT + 63;

        // Certain values for SG_MAX_SIZE and WIN_KO_LEVEL can break the
        // assumption that the above ranges don't overlap
        BOOST_STATIC_ASSERT(START_INDEX_TOPLAY >= 0);
        BOOST_STATIC_ASSERT(END_INDEX_TOPLAY > START_INDEX_TOPLAY);
        BOOST_STATIC_ASSERT(START_INDEX_STONES > END_INDEX_TOPLAY);
        BOOST_STATIC_ASSERT(END_INDEX_STONES > START_INDEX_STONES);
        BOOST_STATIC_ASSERT(END_INDEX_WINKO > START_INDEX_WINKO);
        BOOST_STATIC_ASSERT(START_INDEX_CAPTURES > END_INDEX_WINKO);
        BOOST_STATIC_ASSERT(END_INDEX_CAPTURES > START_INDEX_CAPTURES);
        BOOST_STATIC_ASSERT(START_INDEX_WINKO + MAX_KOLEVEL * 3 - 1
                            <= END_INDEX_WINKO);
        BOOST_STATIC_ASSERT(END_INDEX_CAPTURES
                        < SgHashZobristTable::MAX_HASH_INDEX);

        SgHashCode m_hash;
    };

    /** Information to undo a move.
        Holds information necessary to undo a play.
    */
    struct StackEntry
    {
        /** Color of the move. */
        SgBlackWhite m_color;

        /** Location of the move. */
        SgPoint m_point;

        /** Old value of m_isFirst[m_point].
            Only defined if m_point is not SG_PASS
        */
        bool m_isFirst;

        /** Old value of m_isNewPosition */
        bool m_isNewPosition;

        Block* m_stoneAddedTo;

        /** @name Only defined if m_stoneAddedTo != 0 */
        //@{

        SgPoint m_oldAnchor;

        SgSList<SgPoint,4> m_newLibs;

        SgSList<Block*,4> m_merged;

        //@}

        /** @name Only defined if m_type == PLAY */
        //@{

        /** Old value of m_toPlay */
        SgBlackWhite m_toPlay;

        /** Old value of m_hash */
        HashCode m_hash;

        /** Old value of m_koPoint */
        SgPoint m_koPoint;

        /** Old value of m_koLevel */
        int m_koLevel;

        /** Old value of m_koColor */
        SgEmptyBlackWhite m_koColor;

        /** Old value of m_koLoser */
        SgEmptyBlackWhite m_koLoser;

        /** Old value of m_koModifiesHash */
        bool m_koModifiesHash;

        Block* m_suicide;

        SgSList<Block*,4> m_killed;
        //@}
    };

    /** Data that can be restored quickly with TakeSnapshot/RestoreSnapshot.
        Corresponds to the current state, excluding block data (which is
        stored in the stack m_blockList), data, which is only defined
        immediately after a function call, ot data, which is not expected
        to change during a TakeSnapshot/RestoreSnapshot (e.g. rules)
    */
    struct State
    {
        /** Point which is currently illegal for simple Ko rule. */
        SgPoint m_koPoint;

        /** Whose turn it is to play. */
        SgBlackWhite m_toPlay;

        /** Hash code for this board position. */
        HashCode m_hash;

        SgBWSet m_all;

        SgPointSet m_empty;

        SgArray<Block*,SG_MAXPOINT> m_block;

        /** Number of prisoners of each color */
        SgBWArray<int> m_prisoners;

        /** Number of stones currently on the board. */
        SgBWArray<int> m_numStones;

        /** Number of 'illegal' ko recaptures by m_koColor. */
        int m_koLevel;

        /** The current board position. */
        SgArray<int,SG_MAXPOINT> m_color;

        /** Number of black and white neighbors. */
        SgArray<int,SG_MAXPOINT> m_nuNeighborsEmpty;

        /** Number of black and white neighbors. */
        SgBWArray<SgArray<int,SG_MAXPOINT> > m_nuNeighbors;

        /** Flag if point has not been modified yet. */
        SgArray<bool,SG_MAXPOINT> m_isFirst;

        /** Flag if position is definitely new (no possibility of repetition),
            set to true when move is played at any point for the first time,
            and set to false at the next capture. */
        bool m_isNewPosition;
    };

    struct Snapshot
    {
        /** Move number; -1, if no snapshot was made. */
        int m_moveNumber;

        int m_blockListSize;

        State m_state;

        /** State of blocks currently on the board. */
        SgPointArray<Block> m_blockArray;
    };

    State m_state;

    std::auto_ptr<Snapshot> m_snapshot;

    /** See CountPlay */
    uint64_t m_countPlay;

    /** Data that's constant for this board size. */
    SgBoardConst m_const;

    /** The current board size. */
    SgGrid m_size;

    /** Rules for this board.
        Can be modified anytime with GoBoard::Rules()
    */
    GoRules m_rules;

    /** Setup stones in the root position. */
    GoSetup m_setup;

    GoMoveInfo m_moveInfo;

    /** Block data (stored in a stack).
        Maximum number: A move can create zero or one new block.
    */
    SgSList<Block,GO_MAX_NUM_MOVES>* m_blockList;

    // The following members are mutable since they're used while computing
    // stones and liberties, but are either restored to their previous setting
    // (stack), or don't matter to the client (marks).

    mutable SgMarker m_marker;

    GoPointList m_capturedStones;

    /** Arbitrary repetition for both players. */
    bool m_allowAnyRepetition;

    /** Allow take-back of ko repetition. */
    bool m_allowKoRepetition;

    bool m_koModifiesHash;

    SgEmptyBlackWhite m_koColor;

    /** m_koLoser can never become m_koColor. */
    SgEmptyBlackWhite m_koLoser;

    SgArray<bool,SG_MAXPOINT> m_isBorder;

    SgSList<StackEntry,GO_MAX_NUM_MOVES>* m_moves;

    static bool IsPass(SgPoint p);

    /** Not implemented. */
    GoBoard(const GoBoard&);

    /** Not implemented. */
    GoBoard& operator=(const GoBoard&);

    /** Check if move violates Ko rule.
        Sets isRepetition and updates m_koLevel, m_koColor and hash
        (if KoModifiesHash)
        @return false if isRepetition
    */
    bool CheckKo(SgBlackWhite player);

    void AddLibToAdjBlocks(SgPoint p);

    void AddLibToAdjBlocks(SgPoint p, SgBlackWhite c);

    void AddStoneToBlock(SgPoint p, SgBlackWhite c, Block* block,
                         StackEntry& entry);

    Block& CreateNewBlock();

    void CreateSingleStoneBlock(SgPoint p, SgBlackWhite c);

    SgSList<Block*,4> GetAdjacentBlocks(SgPoint p) const;

    SgSList<Block*,4> GetAdjacentBlocks(SgPoint p, SgBlackWhite c) const;

    void InitBlock(GoBoard::Block& block, SgBlackWhite c, SgPoint anchor);

    bool IsAdjacentTo(SgPoint p, const Block* block) const;

    void MergeBlocks(SgPoint p, SgBlackWhite c,
                     const SgSList<Block*,4>& adjBlocks);

    void RemoveLibAndKill(SgPoint p, SgBlackWhite opp, StackEntry& entry);

    void RemoveLibFromAdjBlocks(SgPoint p, SgBlackWhite c);

    void RestoreKill(Block* block, SgBlackWhite c);

    void UpdateBlocksAfterAddStone(SgPoint p, SgBlackWhite c,
                                   StackEntry& entry);

    void UpdateBlocksAfterUndo(const StackEntry& entry);

    void CheckConsistencyBlock(SgPoint p) const;

    bool FullBoardRepetition() const;

    /** Kill own block if no liberties.
        Sets isSuicide flag.
        @return false if move was suicide and suicide not allowed by current
        rules
    */
    bool CheckSuicide(SgPoint p, StackEntry& entry);

    void AddStone(SgPoint p, SgBlackWhite c);

    void RemoveStone(SgPoint p);

    void AddStoneForUndo(SgPoint p, SgBlackWhite c);

    void RemoveStoneForUndo(SgPoint p);

    void KillBlock(const Block* block);

    bool HasLiberties(SgPoint p) const;

    /** Restore state. */
    void RestoreState(const StackEntry& entry);

    /** Save state.
        @param entry The stack entry to save information to; must already
        have a valid m_type field.
    */
    void SaveState(StackEntry& entry);

    friend class LibertyCopyIterator;
    friend class LibertyIterator;
    friend class StoneIterator;

public:
    /** Iterate through all the stones of a block.
        Point 'p' must be occupied.
        Also, the stones can only be accessed for the current board position.
    */
    class StoneIterator
    {
    public:
        StoneIterator(const GoBoard& bd, SgPoint p);

        /** Advance the state of the iteration to the next stone. */
        void operator++();

        /** Return the current stone. */
        SgPoint operator*() const;

        /** Return true if iteration is valid, otherwise false. */
        operator bool() const;

    private:
        /** Iterator over original list in GoBoard::Block::StoneList.
            No copy of list is necessary, even if moves are played and undone
            while iterating over the list, since the implementation of GoBoard
            does guarantee that the order of the block's stone list is
            preserved.
        */
        GoBoard::Block::StoneIterator m_it;

        const GoBoard& m_board;

#ifndef NDEBUG
        uint64_t m_countPlay;
#endif

        /** Not implemented. */
        StoneIterator(const StoneIterator&);

        /** Not implemented. */
        StoneIterator& operator=(const StoneIterator&);
    };

    /** Iterate through all points. */
    class Iterator
        : public SgPointRangeIterator
    {
    public:
        Iterator(const GoBoard& bd);
    };

    /** Iterate through all the liberties of a block.
        Point 'p' must be occupied.
        Liberties should only be accessed for the current board position.
        No moves are allowed to be executed during the iteration.
    */
    class LibertyIterator
    {
    public:
        LibertyIterator(const GoBoard& bd, SgPoint p);

        /** Advance the state of the iteration to the next liberty. */
        void operator++();

        /** Return the current liberty. */
        SgPoint operator*() const;

        /** Return true if iteration is valid, otherwise false. */
        operator bool() const;

    private:
        Block::LibertyList::Iterator m_it;

        const GoBoard& m_board;

#ifndef NDEBUG
        uint64_t m_countPlay;
#endif

        /** Not implemented. */
        LibertyIterator(const LibertyIterator&);

        /** Not implemented. */
        LibertyIterator& operator=(const LibertyIterator&);
    };

    /** Iterate through all the liberties of a block.
        Point 'p' must be occupied.
        Like GoBoard::LibertyIterator, but allows moves to be executed during
        the iteration (uses a copy of the liberty list, if required by the
        implementation).
    */
    class LibertyCopyIterator
    {
    public:
        LibertyCopyIterator(const GoBoard& bd, SgPoint p);

        /** Advance the state of the iteration to the next liberty. */
        void operator++();

        /** Return the current liberty. */
        int operator*() const;

        /** Return true if iteration is valid, otherwise false. */
        operator bool() const;

    private:
        /** Copy of liberty list.
            Necessary, because if moves are played and undone while iterating
            over liberty list, the implementation of GoBoard does not
            guarantee, that the order of the block's liberty list is
            preserved.
        */
        Block::LibertyList m_liberties;

        Block::LibertyList::Iterator m_it;

        const GoBoard& m_board;

#ifndef NDEBUG
        SgHashCode m_oldHash;
#endif

        /** Not implemented. */
        LibertyCopyIterator(const LibertyCopyIterator&);

        /** Not implemented. */
        LibertyCopyIterator& operator=(const LibertyCopyIterator&);
    };
};

inline GoBoard::StoneIterator::StoneIterator(const GoBoard& bd, SgPoint p)
    : m_it(bd.m_state.m_block[p]->Stones()),
      m_board(bd)
{
    SG_ASSERT(m_board.Occupied(p));
#ifndef NDEBUG
    m_countPlay = m_board.CountPlay();
#endif
}

inline void GoBoard::StoneIterator::operator++()
{
    ++m_it;
}

inline SgPoint GoBoard::StoneIterator::operator*() const
{
    SG_ASSERT(m_board.CountPlay() == m_countPlay);
    return *m_it;
}

inline GoBoard::StoneIterator::operator bool() const
{
    return m_it;
}

inline GoBoard::Iterator::Iterator(const GoBoard& bd)
    : SgPointRangeIterator(bd.BoardConst().BoardIterAddress(),
                           bd.BoardConst().BoardIterEnd())
{
}

inline GoBoard::LibertyIterator::LibertyIterator(const GoBoard& bd, SgPoint p)
    : m_it(bd.m_state.m_block[p]->Liberties()),
      m_board(bd)
{
    SG_ASSERT(m_board.Occupied(p));
#ifndef NDEBUG
    m_countPlay = m_board.CountPlay();
#endif
}

inline void GoBoard::LibertyIterator::operator++()
{
    ++m_it;
}

inline SgPoint GoBoard::LibertyIterator::operator*() const
{
    SG_ASSERT(m_board.CountPlay() == m_countPlay);
    return *m_it;
}

inline GoBoard::LibertyIterator::operator bool() const
{
    return m_it;
}

inline GoBoard::LibertyCopyIterator::LibertyCopyIterator(const GoBoard& bd,
                                                         SgPoint p)
    : m_liberties(bd.m_state.m_block[p]->Liberties()),
      m_it(m_liberties),
      m_board(bd)
{
    SG_ASSERT(m_board.Occupied(p));
#ifndef NDEBUG
    m_oldHash = m_board.GetHashCode();
#endif
}

inline void GoBoard::LibertyCopyIterator::operator++()
{
    ++m_it;
}

inline int GoBoard::LibertyCopyIterator::operator*() const
{
    SG_ASSERT(m_board.GetHashCode() == m_oldHash);
    return *m_it;
}

inline GoBoard::LibertyCopyIterator::operator bool() const
{
    return m_it;
}

inline void GoBoard::HashCode::Clear()
{
    m_hash.Clear();
}

inline const SgHashCode& GoBoard::HashCode::Get() const
{
    return m_hash;
}

inline SgHashCode GoBoard::HashCode::GetInclToPlay(SgBlackWhite toPlay) const
{
    SgHashCode hash = m_hash;
    BOOST_STATIC_ASSERT(SG_BLACK == 0);
    BOOST_STATIC_ASSERT(SG_WHITE == 1);
    int index = toPlay + 1;
    SG_ASSERTRANGE(index, START_INDEX_TOPLAY, END_INDEX_TOPLAY);
    SgHashUtil::XorZobrist(hash, index);
    return hash;
}

inline void GoBoard::HashCode::XorCaptured(int moveNumber,
                                           SgPoint firstCapturedStone)
{
    int index = 2 * SG_MAXPOINT + moveNumber % 64 + firstCapturedStone;
    SG_ASSERTRANGE(index, START_INDEX_CAPTURES, END_INDEX_CAPTURES);
    SgHashUtil::XorZobrist(m_hash, index);
}

inline void GoBoard::HashCode::XorStone(SgPoint p, SgBlackWhite c)
{
    SG_ASSERT_BOARDRANGE(p);
    SG_ASSERT_BW(c);
    BOOST_STATIC_ASSERT(SG_BLACK == 0);
    BOOST_STATIC_ASSERT(SG_WHITE == 1);
    int index = p + c * SG_MAXPOINT;
    SG_ASSERTRANGE(index, START_INDEX_STONES, END_INDEX_STONES);
    SgHashUtil::XorZobrist(m_hash, index);
}

inline void GoBoard::HashCode::XorWinKo(int level, SgBlackWhite c)
{
    SG_ASSERT(level > 0 && level <= MAX_KOLEVEL);
    SG_ASSERT_BW(c);
    BOOST_STATIC_ASSERT(SG_BLACK == 0);
    BOOST_STATIC_ASSERT(SG_WHITE == 1);
    int index = level + MAX_KOLEVEL * c + 2 * SG_MAXPOINT;
    SG_ASSERTRANGE(index, START_INDEX_WINKO, END_INDEX_WINKO);
    SgHashUtil::XorZobrist(m_hash, index);
}

inline const SgPointSet& GoBoard::All(SgBlackWhite color) const
{
    return m_state.m_all[color];
}

inline const SgPointSet& GoBoard::AllEmpty() const
{
    return m_state.m_empty;
}

inline void GoBoard::AllowAnyRepetition(bool allowAny)
{
    m_allowAnyRepetition = allowAny;
}

inline void GoBoard::AllowKoRepetition(bool allowKo)
{
    m_allowKoRepetition = allowKo;
}

inline const SgPointSet& GoBoard::AllPoints() const
{
    return SgPointSet::AllPoints(Size());
}

inline SgPoint GoBoard::Anchor(SgPoint p) const
{
    SG_ASSERT(Occupied(p));
    return m_state.m_block[p]->Anchor();
}

inline bool GoBoard::AnyRepetitionAllowed() const
{
    return m_allowAnyRepetition;
}

inline bool GoBoard::AreInSameBlock(SgPoint p1, SgPoint p2) const
{
    return Occupied(p1) && Occupied(p2) && Anchor(p1) == Anchor(p2);
}

inline bool GoBoard::AtLeastNumLibs(SgPoint block, int n) const
{
    return NumLiberties(block) >= n;
}

inline bool GoBoard::AtMostNumLibs(SgPoint block, int n) const
{
    return NumLiberties(block) <= n;
}

inline bool GoBoard::CanUndo() const
{
    return (m_moves->Length() > 0);
}

inline const GoPointList& GoBoard::CapturedStones() const
{
    return m_capturedStones;
}

inline bool GoBoard::CapturingMove() const
{
    return ! m_capturedStones.IsEmpty();
}

inline const SgPointSet& GoBoard::Centers() const
{
    return m_const.Centers();
}

inline const SgPointSet& GoBoard::Corners() const
{
    return m_const.Corners();
}

inline uint64_t GoBoard::CountPlay() const
{
    return m_countPlay;
}

inline const SgPointSet& GoBoard::Edges() const
{
    return m_const.Edges();
}

inline int GoBoard::FirstBoardPoint() const
{
    return m_const.FirstBoardPoint();
}

inline const SgBoardConst& GoBoard::BoardConst() const
{
    return m_const;
}

inline SgPoint GoBoard::Get2ndLastMove() const
{
    int moveNumber = MoveNumber();
    if (moveNumber < 2)
        return SG_NULLMOVE;
    const StackEntry& entry1 = (*m_moves)[moveNumber - 1];
    const StackEntry& entry2 = (*m_moves)[moveNumber - 2];
    SgBlackWhite toPlay = ToPlay();
    if (entry1.m_color != SgOppBW(toPlay) || entry2.m_color != toPlay)
        return SG_NULLMOVE;
    return entry2.m_point;
}

inline SgBoardColor GoBoard::GetColor(SgPoint p) const
{
    return m_state.m_color[p];
}

inline const SgHashCode& GoBoard::GetHashCode() const
{
    return m_state.m_hash.Get();
}

inline SgHashCode GoBoard::GetHashCodeInclToPlay() const
{
    return m_state.m_hash.GetInclToPlay(ToPlay());
}

inline SgPoint GoBoard::GetLastMove() const
{
    int moveNumber = MoveNumber();
    if (moveNumber == 0)
        return SG_NULLMOVE;
    const StackEntry& entry = (*m_moves)[moveNumber - 1];
    if (entry.m_color != SgOppBW(ToPlay()))
        return SG_NULLMOVE;
    return entry.m_point;
}

inline GoMoveInfo GoBoard::GetLastMoveInfo() const
{
    return m_moveInfo;
}

inline SgBlackWhite GoBoard::GetStone(SgPoint p) const
{
    SG_ASSERT(Occupied(p));
    return m_state.m_color[p];
}

inline bool GoBoard::HasDiagonals(SgPoint p, SgBoardColor c) const
{
    return (IsColor(p - SG_NS - SG_WE, c)
            || IsColor(p - SG_NS + SG_WE, c)
            || IsColor(p + SG_NS - SG_WE, c)
            || IsColor(p + SG_NS + SG_WE, c));
}

inline bool GoBoard::HasEmptyNeighbors(SgPoint p) const
{
    return m_state.m_nuNeighborsEmpty[p] != 0;
}

inline bool GoBoard::HasLiberties(SgPoint p) const
{
    return NumLiberties(p) > 0;
}

inline bool GoBoard::HasNeighbors(SgPoint p, SgBlackWhite c) const
{
    return (m_state.m_nuNeighbors[c][p] > 0);
}

inline bool GoBoard::HasNeighborsOrDiags(SgPoint p, SgBlackWhite c) const
{
    return HasNeighbors(p, c) || HasDiagonals(p, c);
}

inline bool GoBoard::InAtari(SgPoint p) const
{
    SG_ASSERT(Occupied(p));
    return AtMostNumLibs(p, 1);
}

inline bool GoBoard::IsInBlock(SgPoint p, SgPoint anchor) const
{
    SG_ASSERT(Occupied(anchor));
    SG_ASSERT(Anchor(anchor) == anchor);
    const Block* b = m_state.m_block[p];
    return (b != 0 && b->Anchor() == anchor);
}

inline bool GoBoard::IsLibertyOfBlock(SgPoint p, SgPoint anchor) const
{
    SG_ASSERT(IsEmpty(p));
    SG_ASSERT(Occupied(anchor));
    SG_ASSERT(Anchor(anchor) == anchor);
    const Block* b = m_state.m_block[anchor];
    if (m_state.m_nuNeighbors[b->Color()][p] == 0)
        return false;
    return (   m_state.m_block[p - SG_NS] == b
            || m_state.m_block[p - SG_WE] == b
            || m_state.m_block[p + SG_WE] == b
            || m_state.m_block[p + SG_NS] == b);
}

inline bool GoBoard::CanCapture(SgPoint p, SgBlackWhite c) const
{
    SgBlackWhite opp = SgOppBW(c);
    for (SgNb4Iterator nb(p); nb; ++nb)
        if (IsColor(*nb, opp) && AtMostNumLibs(*nb, 1))
            return true;
    return false;
}

inline bool GoBoard::InCenter(SgPoint p) const
{
    return Centers()[p];
}

inline bool GoBoard::InCorner(SgPoint p) const
{
    return Corners()[p];
}

inline void GoBoard::Init(int size, const GoSetup& setup)
{
    Init(size, m_rules, setup);
}

inline bool GoBoard::IsSuicide(SgPoint p, SgBlackWhite toPlay) const
{
    if (HasEmptyNeighbors(p))
        return false;
    SgBlackWhite opp = SgOppBW(toPlay);
    for (SgNb4Iterator it(p); it; ++it)
    {
        if (IsBorder(*it))
            continue;
        SgEmptyBlackWhite c = GetColor(*it);
        if (c == toPlay && NumLiberties(*it) > 1)
            return false;
        if (c == opp && NumLiberties(*it) == 1)
            return false;
    }
    return true;
}

inline bool GoBoard::IsBorder(SgPoint p) const
{
    SG_ASSERT(p != SG_PASS);
    return m_isBorder[p];
}

inline bool GoBoard::IsColor(SgPoint p, int c) const
{
    SG_ASSERT(p != SG_PASS);
    SG_ASSERT_EBW(c);
    return m_state.m_color[p] == c;
}

inline bool GoBoard::IsEmpty(SgPoint p) const
{
    SG_ASSERT(p != SG_PASS);
    return m_state.m_color[p] == SG_EMPTY;
}

inline bool GoBoard::IsFirst(SgPoint p) const
{
    SG_ASSERT(IsEmpty(p));
    return m_state.m_isFirst[p];
}

inline bool GoBoard::IsLegal(int p, SgBlackWhite player) const
{
    SG_ASSERT_BW(player);
    if (IsPass(p))
        return true;
    SG_ASSERT(SgPointUtil::InBoardRange(p));
    if (! IsEmpty(p))
        return false;
    // Suicide
    if (! Rules().AllowSuicide() && IsSuicide(p, player))
        return false;
    // Repetition
    if (IsFirst(p))
        return true;
    if (p == m_state.m_koPoint && m_state.m_toPlay == player)
        return (AnyRepetitionAllowed() || KoRepetitionAllowed());
    if (Rules().GetKoRule() == GoRules::SIMPLEKO)
        return true;
    if (IsNewPosition() && ! CanCapture(p, player))
        return true;
    // None of the easy cases, so check by executing move. Casting away
    // const is okay since board is restored to exactly the same state,
    // appears const to the client.
    GoBoard* bd = const_cast<GoBoard*>(this);
    bd->Play(p, player);
    bool isLegal = ! LastMoveInfo(GO_MOVEFLAG_ILLEGAL);
    bd->Undo();
    return isLegal;
}

inline bool GoBoard::IsNewPosition() const
{
    return m_state.m_isNewPosition;
}

inline bool GoBoard::IsLegal(int p) const
{
    return IsLegal(p, ToPlay());
}

/** Check if point is a pass move or a coupon move, which is handled like a
    pass move.
*/
inline bool GoBoard::IsPass(SgPoint p)
{
    return (p == SG_PASS || SgMoveUtil::IsCouponMove(p));
}

inline bool GoBoard::IsSingleStone(SgPoint p) const
{
    return (Occupied(p) && NumNeighbors(p, GetColor(p)) == 0);
}

inline bool GoBoard::IsSuicide(SgPoint p) const
{
    return IsSuicide(p, ToPlay());
}

inline bool GoBoard::IsValidPoint(SgPoint p) const
{
    return SgPointUtil::InBoardRange(p) && ! IsBorder(p);
}

inline SgEmptyBlackWhite GoBoard::KoColor() const
{
    return m_koColor;
}

inline int GoBoard::KoLevel() const
{
    return m_state.m_koLevel;
}

inline SgEmptyBlackWhite GoBoard::KoLoser() const
{
    return m_koLoser;
}

inline bool GoBoard::KoModifiesHash() const
{
    return m_koModifiesHash;
}

inline SgPoint GoBoard::KoPoint() const
{
    return m_state.m_koPoint;
}

inline bool GoBoard::KoRepetitionAllowed() const
{
    return m_allowKoRepetition;
}

inline int GoBoard::LastBoardPoint() const
{
    return m_const.LastBoardPoint();
}

inline bool GoBoard::LastMoveInfo(GoMoveInfoFlag flag) const
{
    return m_moveInfo.test(flag);
}

inline int GoBoard::Left(SgPoint p) const
{
    return m_const.Left(p);
}

inline SgGrid GoBoard::Line(SgPoint p) const
{
    return m_const.Line(p);
}

inline const SgPointSet& GoBoard::LineSet(SgGrid line) const
{
    return m_const.LineSet(line);
}

inline int GoBoard::MoveNumber() const
{
    return m_moves->Length();
}

inline int GoBoard::Num8Neighbors(SgPoint p, int c) const
{
    return NumNeighbors(p, c) + NumDiagonals(p, c);
}

inline int GoBoard::Num8EmptyNeighbors(SgPoint p) const
{
    return NumEmptyNeighbors(p) + NumEmptyDiagonals(p);
}

inline int GoBoard::NuCapturedStones() const
{
    return m_capturedStones.Length();
}

inline int GoBoard::NumDiagonals(SgPoint p, SgBoardColor c) const
{
    int n = 0;
    if (IsColor(p - SG_NS - SG_WE, c))
        ++n;
    if (IsColor(p - SG_NS + SG_WE, c))
        ++n;
    if (IsColor(p + SG_NS - SG_WE, c))
        ++n;
    if (IsColor(p + SG_NS + SG_WE, c))
        ++n;
    return n;
}

inline int GoBoard::NumEmptyDiagonals(SgPoint p) const
{
    return NumDiagonals(p, SG_EMPTY);
}

inline int GoBoard::NumEmptyNeighbors(SgPoint p) const
{
    return m_state.m_nuNeighborsEmpty[p];
}

inline int GoBoard::NumLiberties(SgPoint p) const
{
    SG_ASSERT(IsValidPoint(p));
    SG_ASSERT(Occupied(p));
    return m_state.m_block[p]->NumLiberties();
}

inline int GoBoard::NumNeighbors(SgPoint p, SgBlackWhite c) const
{
    return m_state.m_nuNeighbors[c][p];
}

inline int GoBoard::NumPrisoners(SgBlackWhite color) const
{
    return m_state.m_prisoners[color];
}

inline int GoBoard::NumStones(SgPoint block) const
{
    SG_ASSERT(Occupied(block));
    return m_state.m_block[block]->NumStones();
}

inline SgPointSet GoBoard::Occupied() const
{
    return m_state.m_all[SG_BLACK] | m_state.m_all[SG_WHITE];
}

inline bool GoBoard::Occupied(SgPoint p) const
{
    return (m_state.m_block[p] != 0);
}

inline bool GoBoard::OccupiedInAtari(SgPoint p) const
{
    const Block* b = m_state.m_block[p];
    return (b != 0 && b->NumLiberties() <= 1);
}

inline bool GoBoard::OnEdge(SgPoint p) const
{
    return Edges()[p];
}

inline SgBlackWhite GoBoard::Opponent() const
{
    return SgOppBW(m_state.m_toPlay);
}

inline void GoBoard::Play(GoPlayerMove move)
{
    Play(move.Point(), move.Color());
}

inline void GoBoard::Play(SgPoint p)
{
    Play(p, ToPlay());
}

inline SgGrid GoBoard::Pos(SgPoint p) const
{
    return m_const.Pos(p);
}

inline int GoBoard::Right(SgPoint p) const
{
    return m_const.Right(p);
}

inline GoRules& GoBoard::Rules()
{
    return m_rules;
}

inline const GoRules& GoBoard::Rules() const
{
    return m_rules;
}

inline void GoBoard::SetKoLoser(SgEmptyBlackWhite color)
{
    SG_ASSERT(KoLevel() == 0);
    m_koLoser = color;
}

inline void GoBoard::SetKoModifiesHash(bool modify)
{
    m_koModifiesHash = modify;
}

inline void GoBoard::SetToPlay(SgBlackWhite player)
{
    SG_ASSERT_BW(player);
    m_state.m_toPlay = player;
}

inline const GoSetup& GoBoard::Setup() const
{
    return m_setup;
}

inline int GoBoard::Side(SgPoint p, int index) const
{
    return m_const.Side(p, index);
}

inline const SgPointSet& GoBoard::SideExtensions() const
{
    return m_const.SideExtensions();
}

inline SgGrid GoBoard::Size() const
{
    return m_size;
}

inline SgPoint GoBoard::TheLiberty(SgPoint p) const
{
    SG_ASSERT(Occupied(p));
    SG_ASSERT(NumLiberties(p) == 1);
    return m_state.m_block[p]->Liberties()[0];
}

inline SgBlackWhite GoBoard::ToPlay() const
{
    return m_state.m_toPlay;
}

inline int GoBoard::TotalNumEmpty() const
{
    return (Size() * Size() - m_state.m_numStones[SG_BLACK]
            - m_state.m_numStones[SG_WHITE]);
}

inline const SgBWArray<int>& GoBoard::TotalNumStones() const
{
    return m_state.m_numStones;
}

inline int GoBoard::TotalNumStones(SgBlackWhite color) const
{
    return m_state.m_numStones[color];
}

inline int GoBoard::Up(SgPoint p) const
{
    return m_const.Up(p);
}

//----------------------------------------------------------------------------

#endif // GO_BOARD_H

