//----------------------------------------------------------------------------
/** @file GoBoardUtil.h
    GoBoard-related utility classes.
*/
//----------------------------------------------------------------------------

#ifndef GO_BOARDUTIL_H
#define GO_BOARDUTIL_H

#include "GoBoard.h"
#include "SgBoardColor.h"
#include "SgDebug.h"
#include "SgPoint.h"
#include "SgPointArray.h"
#include "SgStack.h"
#include "SgVector.h"

//----------------------------------------------------------------------------

/** Utility functions for users of class GoBoard.
    Some of the functions that the board class as a template argument,
    such that they can be used with specialized variants of GoBoard that
    share only a sub-functionality.
*/
namespace GoBoardUtil
{
    /** Add anchors of neighbor blocks to list. */
    void AddNeighborBlocksOfColor(const GoBoard& bd,
                                  SgPoint p,
                                  SgBlackWhite color,
                                  SgVector<SgPoint>& neighbors);

    /** Add wall of stones in color to the board.
        @param bd
        @param color color of the wall.
        @param start Starting point for the wall.
        @param length number of stones in wall
        @param direction offset from one stone to next.
            e.g. direction = NS builds a North-South wall.
            can also build diagonal walls,
            e.g. by using direction = NS + WE,
            or even jumps.
        Precondition: all these squares must be empty,
                      and playing on them must be legal.
    */
    void AddWall(GoBoard& bd,
                 SgBlackWhite color,
                 SgPoint start,
                 int length,
                 int direction);

    /** Get list of stones adjacent to a block. */
    GoPointList AdjacentStones(const GoBoard& bd, SgPoint point);

    /** SgVector version of GoBoard::AdjacentBlocks */
    void AdjacentBlocks(const GoBoard& bd, SgPoint p, int maxLib,
                        SgVector<SgPoint>* blocks);

    /** Estimate second order liberties of point p for given block
        This is fast and approximate, may double count libs */
    int Approx2Libs(const GoBoard& board, SgPoint block, SgPoint p,
                    SgBlackWhite color);

    /** Return whether 'block1' and 'block2' have at least two shared
        liberties.
        Not defined for empty or border points.
    */
    bool AtLeastTwoSharedLibs(const GoBoard& bd, SgPoint block1,
                              SgPoint block2);

    bool BlockIsAdjacentTo(const GoBoard& bd, SgPoint block,
                           const SgPointSet& walls);

    void BlocksAdjacentToPoints(const GoBoard& bd,
                                const SgVector<SgPoint>& points,
                                SgBlackWhite c,
                                SgVector<SgPoint>* anchors);

    /** List the anchors of all blocks of color 'c' adjacent to the region
        consisting of 'points'. */
    void BlocksAdjacentToPoints(const GoBoard& bd, const SgPointSet& points,
                                SgBlackWhite c, SgVector<SgPoint>* anchors);

    /** Compute the common fate graph distance from all points to a given
        point.
        The common fate graph distance ist the shortest path between points
        with an edge cost of 0 for edges between stones of the same block,
        and an edge cost of 1 otherwise.
        @param bd
        @param p
        @param maxDist The maximum distance to search (points with a
        distance &gt; maxDist will get the value numeric_limits<int>::max())
        @return The array containing the distances; for blocks only the
        element at the block anchor is defined.
    */
    SgPointArray<int> CfgDistance(const GoBoard& bd, SgPoint p,
                               int maxDist = std::numeric_limits<int>::max());

    /** Is p contained in anchor[] ?
        anchor[] must be terminated by END_POINT.
    */
    bool ContainsAnchor(const SgPoint anchor[], const SgPoint p);

   /** Get diagonal points with a color.
       @param bd The board.
       @param p The point.
       @param c The color.
       @param diagonals Resulting point list. Will be cleared before
       adding the points.
    */
    void DiagonalsOfColor(const GoBoard& bd, SgPoint p, int c,
                          SgVector<SgPoint>* diagonals);

    /** Write board including move history to stream.
        This function is intended for printing the current board state
        for debugging or after a crash. The move history is written in SGF
        format.
    */
    void DumpBoard(const GoBoard& bd, std::ostream& out = SgDebug());

    /** Return whether the game is finished.
        (two or three consecutive pass moves).
        For the choice of two or three: @see GoRules constructor.
    */
    bool EndOfGame(const GoBoard& bd);

    /** Add other stones of blocks to SgPointSet if one is in set */
    void ExpandToBlocks(const GoBoard& board, SgPointSet& pointSet);

    /** Find a neighboring point in color c.
        Precondition: Call only if such a point exists.
    */
    template<class BOARD>
    SgPoint FindNeighbor(const BOARD& bd, SgPoint p, SgEmptyBlackWhite c);

    /** Include move in list if it is legal */
    bool GenerateIfLegal(const GoBoard& bd,
                         SgPoint move,
                         SgVector<SgPoint>* moves);

    /** Convert the given move to human-readable coordinates.
        (lower left A1 to upper right T19, leaving out column I).
    */
    void GetCoordString(SgMove move, std::string* s, int boardSize);

    /** Convert the given move to human-readable coordinates.
        (lower left A1 to upper right T19, leaving out column I).
    */
    void GetCoordString(const GoBoard& board, SgMove move, std::string* s);


    /** Which intersections were modified with the last move.
        Can check either before or after move is played (set premove)
    */
    SgRect GetDirtyRegion(const GoBoard& bd, SgMove move, SgBlackWhite color,
                          bool checklibs = false, bool premove = false);

    /** Return whether block has at least one adjacent opponent
        block with at most maxLib liberties.
    */
    bool HasAdjacentBlocks(const GoBoard& bd, SgPoint p, int maxLib);

    bool HasStonesOfBothColors(const GoBoard& bd,
                               const SgVector<SgPoint>& stones);

    /** Return if point is surrounded by one color and no adjacent block is
        in atari.
        Good criterion for move generation in Monte-Carlo. See:
        Remi Coulom: Efficient selectivity and backup operators in
        Monte-Carlo tree search, CG2006, Appendix A.1,
        http://remi.coulom.free.fr/CG2006/
    */
    template<class BOARD>
    bool IsCompletelySurrounded(const BOARD& bd, SgPoint p);

    bool IsHandicapPoint(SgGrid size, SgGrid col, SgGrid row);

    template<class BOARD>
    bool IsNeighborOfSome(const BOARD& bd, SgPoint p, SgPoint anchors[],
                          SgBlackWhite toPlay);

    /** Does block have two shared liberties with some other block? 
        WARNING: for efficiency this checks only the first two liberties
        of the block. So it is accurate for two-liberty blocks,
        and a heuristic for blocks with more liberties.
    */
    template<class BOARD>
    bool IsSimpleChain(const BOARD& bd, SgPoint block, SgPoint& other);
    
    /** Is lib a simple eye of block?
        Eyes is a list of other eye points, that do not need to be
        occupied for lib to be an eye.
        Precondition (not tested): lib is surrounded by stones of color.
    */
    bool IsSimpleEyeOfBlock(const GoBoard& bd, SgPoint lib,
                            SgPoint blockAnchor,
                            const SgVector<SgPoint>& eyes);

    /** Check if the move just played on p was a snapback.
        A snapback is a single stone in atari which can be captured by a
        legal move, if the move creates a block with more than one stone
        in atari.
    */
    bool IsSnapback(const GoBoard& bd, SgPoint p);

    /** all points on lines [from..to] */
    SgPointSet Lines(const GoBoard& bd, SgGrid from, SgGrid to);

    bool ManySecondaryLibs(const GoBoard& bd, SgPoint block);

    /** Either move is not legal, or the block at move is in atari
        after the move.
    */
    bool MoveNotLegalOrAtari(GoBoard& bd, SgPoint move);

    /** Move is legal and the block at move is not in atari
        after the move.
    */
    bool MoveLegalAndNotAtari(GoBoard& bd, SgPoint move);

    /** Get adjacent points with a color.
        @param bd The board.
        @param p The point.
        @param c The color.
        @return Resulting point list.
    */
    SgSList<SgPoint,4> NeighborsOfColor(const GoBoard& bd, SgPoint p, int c);

    /** Get adjacent points with a color (SgVector version).
        @param bd The board.
        @param p The point.
        @param c The color.
        @param neighbors Resulting point list. Will be cleared before
        adding the points.
    */
    void NeighborsOfColor(const GoBoard& bd, SgPoint p, int c,
                          SgVector<SgPoint>* neighbors);

    /** Check if Tromp-Taylor rules and pass wins. */
    bool PassWins(const GoBoard& bd, SgBlackWhite toPlay);

    /** Play a move if legal
        @param bd The board.
        @param p Move to play; SG_PASS or on-board point.
        @param player Color to play.
        @return true if the move was executed.
    */
    bool PlayIfLegal(GoBoard& bd, SgPoint p, SgBlackWhite player);

    /** Play a move for the current player if legal.
        @param bd The board.
        @param p Move to play; SG_PASS or on-board point.
        @return true if the move was executed.
    */
    bool PlayIfLegal(GoBoard& bd, SgPoint p);

    /** Keep only the anchor of each block in the list.
        Points not occupied are removed from the list. The initial list may
        contain duplicate stones; these will be thrown out. The returned list
        will be sorted by anchors.
    */
    void ReduceToAnchors(const GoBoard& bd, SgVector<SgPoint>* stones);

    /** Keep only the anchor of each block in the list.
        Points not occupied are removed from the list. The initial list may
        contain duplicate stones; these will be thrown out. The returned list
        will not be sorted by anchors.
    */
    void ReduceToAnchors(const GoBoard& bd, const SgVector<SgPoint>& stones,
                         SgSList<SgPoint,SG_MAXPOINT> &anchors);

    /** Compute the hash code for region of this board position. */
    void RegionCode(const GoBoard& bd, const SgVector<SgPoint>& region,
                    SgHashCode* c);

    /** Returns true iff during the first N moves of a Chinese handicap game.
    */
    bool RemainingChineseHandicap(const GoBoard& bd);

    /** Check if move would be self-atari.
        Faster than Executing the move, then calling InAtari().
    */
    template<class BOARD>
    bool SelfAtari(const BOARD& bd, SgPoint p);

    /** Same as above, but also compute number of stones put into selfatari.
         numStones is set only if the return value is 'true'.
    */
    template<class BOARD>
    bool SelfAtari(const BOARD& bd, SgPoint p, int& numStones);

    /** Check if move would be self-atari for given color.
        That color may be different from bd.ToPlay().
    */
    template<class BOARD>
    bool SelfAtariForColor(const BOARD& bd, SgPoint p,
                           SgBlackWhite toPlay);

    /** Return all points that are liberties of both 'block1' and 'block2'.
        Not defined for empty or border points.
    */
    void SharedLiberties(const GoBoard& bd, SgPoint block1, SgPoint block2,
                         SgVector<SgPoint>* sharedLibs);

    void SharedLibertyBlocks(const GoBoard& bd, SgPoint anchor, int maxLib,
                             SgVector<SgPoint>* blocks);

    /** Count score given the set of dead stones.
        Checks all regions that are surrounded by stones that are not dead,
        and counts the score according to the board rules
        (Chinese/Japanese) and komi. Black points are counted positive.
        Cannot handle neutral eye points that can occur in seki with Japanese
        rules.
        @param bd
        @param deadStones
        @param[out] score
        @return @c false if position cannot be scored, because the dead
        stones information is not consistent (a region with dead stones of
        both colors exists or dead stones of a color in a region of that
        color).
    */
    bool ScorePosition(const GoBoard& bd, const SgPointSet& deadStones,
                       float& score);

    /** Helper function used in ScoreSimpleEndPosition */
    template<class BOARD>
    SgEmptyBlackWhite ScorePoint(const BOARD& bd, SgPoint p, bool noCheck);

    /** Score position with given safe stones and only simple eyes.
        This is a fast scoring function (e.g. suitable for Monte-Carlo),
        that can be used if playing continues as long as there are legal moves
        which do not fill the player's single point eyes.
        Precomputed safety status of points is used, all other empty points
        must be single empty points surrounded by one color.
        The score is counted using 1 point for all black stones or empty
        points with only black stones adjacent, and -1 point for white
        stones or empty points with only white stones adjacent.
        Komi of board is taken into account.
        @param bd
        @param komi Komi (bd.Rules().Komi() is not used to avoid multiple
        conversions of komi to float)
        @param safe
        @param noCheck
        @param scoreBoard Optional board to fill in the status of each
        point (SG_EMPTY means dame); null if not needed
    */
    template<class BOARD>
    float ScoreSimpleEndPosition(const BOARD& bd, float komi,
                                 const SgBWSet& safe, bool noCheck,
                                 SgPointArray<SgEmptyBlackWhite>* scoreBoard);

    /** Score position with all stones safe and only simple eyes.
        This is a fast scoring function (e.g. suitable for Monte-Carlo),
        that can be used if playing continues as long as there are legal moves
        which do not fill the player's single point eyes.
        All stones are considered safe, all empty points must be single
        empty points surrounded by one color.
        The score is counted using 1 point for all black stones or empty
        points with only black stones adjacent, and -1 point for white
        stones or empty points with only white stones adjacent.
        Komi of board is taken into account.
        @param bd The board with the position
        @param komi Komi (bd.Rules().Komi() is not used to avoid multiple
        conversions of komi to float)
        @param noCheck Don't throw an exception if not all empty points are
        single empty points (there are cases, where this score function is
        useful even if it is sometimes wrong)
        @throws SgException If there are empty points, which are not single
        empty points or with stones of both colors adjacent.
        @return Score including komi, positive for black.
    */
    float ScoreSimpleEndPosition(const GoBoard& bd, float komi,
                                 bool noCheck = false);

    /** Fill stones in an array.
        Kishi: I added this code to store stones to an array,
        because the list version first copies stones to an array,
        then copies an array to a list. For me, it's not necessary because
        I use arrays.
        @note Consider using GoBoard::StoneIterator instead, if you don't need
        to keep the array
    */
    int Stones(const GoBoard& bd, SgPoint p, SgPoint stones[]);

    void TestForChain(GoBoard& bd, SgPoint block, SgPoint block2, SgPoint lib,
                      SgVector<SgPoint>* extended);

    /** Compute the Tromp-Taylor-score for the current positions.
        The Tromp-Taylor score is a chinese scoring method that assumes that
        all stones on the board are alive.
        @param bd The board with the position to score.
        @param komi The komi
        @param scoreBoard Optional board to fill in the status of each
        point (SG_EMPTY means dame)
        @return The score, black counting positive, komi included.
    */
    template<class BOARD>
    float TrompTaylorScore(const BOARD& bd, float komi,
                           SgPointArray<SgEmptyBlackWhite>* scoreBoard = 0);

    /** Check if the last two moves were two passes in a row, the first pass
        by the current color to play, the second by the opponent.
    */
    bool TwoPasses(const GoBoard& bd);

    /** Undo all moves or setup stones. */
    void UndoAll(GoBoard& bd);

} // namespace GoBoardUtil

inline bool GoBoardUtil::ContainsAnchor(const SgPoint anchor[],
                                        const SgPoint p)
{
    for (int i=0; anchor[i] != SG_ENDPOINT; ++i)
        if (p == anchor[i])
            return true;
    return false;
}

template<class BOARD>
inline SgPoint GoBoardUtil::FindNeighbor(const BOARD& bd, SgPoint p,
                                         SgEmptyBlackWhite c)
{
    if (bd.IsColor(p + SG_NS, c))
        return p + SG_NS;
    if (bd.IsColor(p - SG_NS, c))
        return p - SG_NS;
    if (bd.IsColor(p + SG_WE, c))
        return p + SG_WE;
    SG_ASSERT(bd.IsColor(p - SG_WE, c));
    return p - SG_WE;
}

inline void GoBoardUtil::GetCoordString(const GoBoard& board, SgMove move,
                                        std::string* s)
{
    GetCoordString(move, s, board.Size());
}

template<class BOARD>
inline bool GoBoardUtil::IsCompletelySurrounded(const BOARD& bd, SgPoint p)
{
    SG_ASSERT(bd.IsEmpty(p));
    if (bd.HasEmptyNeighbors(p))
        return false;
    if (bd.HasNeighbors(p, SG_BLACK) && bd.HasNeighbors(p, SG_WHITE))
        return false;
    if (! bd.IsBorder(p - SG_NS) && bd.NumLiberties(p - SG_NS) == 1)
        return false;
    if (! bd.IsBorder(p - SG_WE) && bd.NumLiberties(p - SG_WE) == 1)
        return false;
    if (! bd.IsBorder(p + SG_WE) && bd.NumLiberties(p + SG_WE) == 1)
        return false;
    if (! bd.IsBorder(p + SG_NS) && bd.NumLiberties(p + SG_NS) == 1)
        return false;
    return true;
}

template<class BOARD>
inline bool GoBoardUtil::IsNeighborOfSome(const BOARD& bd, SgPoint p,
                                          SgPoint anchors[],
                                          SgBlackWhite toPlay)
{
    for (SgNb4Iterator it(p); it; ++it)
    {
        const SgPoint nb = *it;
        if (bd.IsColor(nb, toPlay))
        {
            SgPoint anchor = bd.Anchor(nb);
            for (int i = 0; anchors[i] != SG_ENDPOINT; ++i)
                if (anchor == anchors[i])
                    return true;
        }
    }
    return false;
}

template<class BOARD>
bool GoBoardUtil::IsSimpleChain(const BOARD& bd,
                                SgPoint block,
                                SgPoint& other)
{
    if (bd.NumLiberties(block) < 2)
        return false;
    block = bd.Anchor(block);
    const SgBlackWhite color = bd.GetStone(block);
    typename BOARD::LibertyIterator it(bd, block);
    const SgPoint lib1 = *it; 
    ++it;
    const SgPoint lib2 = *it; 
    SgPoint anchors1[4 + 1];
    SgPoint anchors2[4 + 1];
    bd.NeighborBlocks(lib1, color, anchors1);
    bd.NeighborBlocks(lib2, color, anchors2);
    for (int i=0; anchors1[i] != SG_ENDPOINT; ++i)
    {
        const SgPoint anchor = anchors1[i];
        if (  anchor != block
           && GoBoardUtil::ContainsAnchor(anchors2, anchor)
           )
        {
            other = anchor;
            return true;
        }
    }
    return false;
}

inline bool GoBoardUtil::PlayIfLegal(GoBoard& bd, SgPoint p)
{
    return PlayIfLegal(bd, p, bd.ToPlay());
}

template<class BOARD>
SgEmptyBlackWhite GoBoardUtil::ScorePoint(const BOARD& bd, SgPoint p,
                                          bool noCheck)
{
    SG_DEBUG_ONLY(noCheck);
    SgEmptyBlackWhite c = bd.GetColor(p);
    if (c != SG_EMPTY)
        return c;
    // Position must have only completely surrounded empty points
    SG_ASSERT(noCheck || bd.NumEmptyNeighbors(p) == 0
              || GoBoardUtil::SelfAtari(bd, p));
    if (bd.NumNeighbors(p, SG_BLACK) > 0
        && bd.NumNeighbors(p, SG_WHITE) == 0)
        return SG_BLACK;
    else if (bd.NumNeighbors(p, SG_WHITE) > 0
             && bd.NumNeighbors(p, SG_BLACK) == 0)
    {
        SG_ASSERT(bd.NumNeighbors(p, SG_WHITE) > 0);
        return SG_WHITE;
    }
    else
    {
        // Position must have no dame points
        SG_ASSERT(noCheck || GoBoardUtil::SelfAtari(bd, p));
        return SG_EMPTY;
    }
}

template<class BOARD>
float GoBoardUtil::ScoreSimpleEndPosition(const BOARD& bd, float komi,
                                  const SgBWSet& safe, bool noCheck,
                                  SgPointArray<SgEmptyBlackWhite>* scoreBoard)
{
    float score = -komi;
    for (typename BOARD::Iterator it(bd); it; ++it)
    {
        SgPoint p = *it;
        SgEmptyBlackWhite c;
        if (safe[SG_BLACK].Contains(p))
            c = SG_BLACK;
        else if (safe[SG_WHITE].Contains(p))
            c = SG_WHITE;
        else
            c = ScorePoint(bd, p, noCheck);
        switch (c)
        {
        case SG_BLACK:
            ++score;
            break;
        case SG_WHITE:
            --score;
            break;
        default:
            break;
        }
        if (scoreBoard != 0)
            (*scoreBoard)[p] = c;
    }
    return score;
}

template<class BOARD>
inline bool GoBoardUtil::SelfAtari(const BOARD& bd, SgPoint p)
{
    return SelfAtariForColor(bd, p, bd.ToPlay());
}

template<class BOARD>
inline bool GoBoardUtil::SelfAtariForColor(const BOARD& bd, SgPoint p,
                                          SgBlackWhite toPlay)
{
    // This function is inline even if it is long, because it returns early
    // in many cases, which makes the function call an overhead.

    // This function has a lot of redundacy with
    // SelfAtari(const GoBoard&,SgPoint,int&). The two versions exist
    // for efficiency (this function is called very often in UCT simulations)

    SG_ASSERT(bd.IsEmpty(p));
    // No self-atari, enough liberties
    if (bd.NumEmptyNeighbors(p) >= 2)
        return false;
    const SgBlackWhite opp = SgOppBW(toPlay);
    SgPoint lib = SG_NULLPOINT;
    bool hasOwnNb = false;
    bool hasCapture = false;
    for (SgNb4Iterator it(p); it; ++it)
    {
        const SgPoint nb = *it;
        const SgBlackWhite nbColor = bd.GetColor(nb);
        if (nbColor == SG_EMPTY)
        {
            if (lib == SG_NULLPOINT)
                lib = nb;
            else if (lib != nb)
                return false;
        }
        else if (nbColor == toPlay) // own stones
        {
            if (bd.NumLiberties(nb) > 2)
                return false;
            else // check block's liberties other than p
                for (typename BOARD::LibertyIterator it(bd, nb); it; ++it)
                {
                    if (*it != p)
                    {
                        if (lib == SG_NULLPOINT)
                            lib = *it;
                        else if (lib != *it)
                            return false;
                    }
                }
            hasOwnNb = true;
        }
        else if (nbColor == opp) // opponent stones - count as lib if in atari
        {
            if (bd.InAtari(nb))
            {
                if (lib == SG_NULLPOINT)
                {
                    lib = *it;
                    hasCapture = true;
                }
                else if (lib != *it)
                    return false;
            }
        }
    }

    if (lib == SG_NULLPOINT) // suicide
        return false;
    if (! hasOwnNb && hasCapture) // ko-type capture, OK
         return false;
    if (hasOwnNb && hasCapture) // check if we gained other liberties
    {
        // lib == one of the captured stones.
       SgPoint anchors[4 + 1];
       bd.NeighborBlocks(p, toPlay, 1, anchors);
       SG_ASSERT(bd.IsColor(lib, opp));
       for (typename BOARD::StoneIterator it(bd, lib); it; ++it)
       {
           if (*it != lib && IsNeighborOfSome(bd, *it, anchors, toPlay))
               return false;
       }
    }
    return true;
}

template<class BOARD>
bool GoBoardUtil::SelfAtari(const BOARD& bd, SgPoint p, int& numStones)
{
    SG_ASSERT(bd.IsEmpty(p));
    // No self-atari, enough liberties
    if (bd.NumEmptyNeighbors(p) >= 2)
        return false;
    const SgBlackWhite toPlay = bd.ToPlay();
    const SgBlackWhite opp = SgOppBW(toPlay);
    SgPoint lib = SG_NULLPOINT;
    bool hasOwnNb = false;
    bool hasCapture = false;
    for (SgNb4Iterator it(p); it; ++it)
    {
        const SgPoint nb = *it;
        const SgBlackWhite nbColor = bd.GetColor(nb);
        if (nbColor == SG_EMPTY)
        {
            if (lib == SG_NULLPOINT)
                lib = nb;
            else if (lib != nb)
                return false;
        }
        else if (nbColor == toPlay) // own stones
        {
            if (bd.NumLiberties(nb) > 2)
                return false;
            else // check block's liberties other than p
                for (typename BOARD::LibertyIterator it(bd, nb); it; ++it)
                {
                    if (*it != p)
                    {
                        if (lib == SG_NULLPOINT)
                            lib = *it;
                        else if (lib != *it)
                            return false;
                    }
                }
            hasOwnNb = true;
        }
        else if (nbColor == opp) // opponent stones - count as lib if in atari
        {
            if (bd.InAtari(nb))
            {
                if (lib == SG_NULLPOINT)
                {
                    lib = *it;
                    hasCapture = true;
                }
                else if (lib != *it)
                    return false;
            }
        }
    }

    if (lib == SG_NULLPOINT) // suicide
        return false;
    if (! hasOwnNb && hasCapture) // ko-type capture, OK
         return false;
    if (hasOwnNb && hasCapture) // check if we gained other liberties
    {
        // lib == one of the captured stones.
       SgPoint anchors[4 + 1];
       bd.NeighborBlocks(p, toPlay, 1, anchors);
       SG_ASSERT(bd.IsColor(lib, opp));
       for (typename BOARD::StoneIterator it(bd, lib); it; ++it)
       {
           if (*it != lib && IsNeighborOfSome(bd, *it, anchors, toPlay))
               return false;
       }
    }
    numStones = 1;
    if (hasOwnNb)
    {
        SgPoint anchors[4 + 1];
        bd.NeighborBlocks(p, toPlay, 2, anchors);
        for (int i = 0; anchors[i] != SG_ENDPOINT; ++i)
            numStones += bd.NumStones(anchors[i]);
    }
    return true;
}

template<class BOARD>
float GoBoardUtil::TrompTaylorScore(const BOARD& bd, float komi,
                                  SgPointArray<SgEmptyBlackWhite>* scoreBoard)
{
    float score = -komi;
    // Mark empty points visited in one of the (non-overlapping) flood-fills
    SgMarker mark;
    for (typename BOARD::Iterator it(bd); it; ++it)
    {
        if (mark.Contains(*it))
            continue;
        SgEmptyBlackWhite c = bd.GetColor(*it);
        if (c == SG_BLACK)
        {
            ++score;
            if (scoreBoard != 0)
                (*scoreBoard)[*it] = SG_BLACK;
            continue;
        }
        if (c == SG_WHITE)
        {
            --score;
            if (scoreBoard != 0)
                (*scoreBoard)[*it] = SG_WHITE;
            continue;
        }
        SgStack<SgPoint,SG_MAXPOINT> stack;
        GoPointList list;
        SG_ASSERT(c == SG_EMPTY);
        stack.Push(*it);
        mark.Include(*it);
        list.PushBack(*it);
        SgBWArray<bool> adjacent(false);
        int size = 0;
        while (! stack.IsEmpty())
        {
            SgPoint p = stack.Pop();
            SG_ASSERT(bd.GetColor(p) == SG_EMPTY);
            ++size;
            if (bd.HasNeighbors(p, SG_BLACK))
                adjacent[SG_BLACK] = true;
            if (bd.HasNeighbors(p, SG_WHITE))
                adjacent[SG_WHITE] = true;
            for (SgNb4Iterator it2(p); it2; ++it2)
                if (! bd.IsBorder(*it2) && bd.GetColor(*it2) == SG_EMPTY
                    && ! mark.Contains(*it2))
                {
                    stack.Push(*it2);
                    mark.Include(*it2);
                    list.PushBack(*it2);
                }
        }
        if (adjacent[SG_BLACK] && ! adjacent[SG_WHITE])
        {
            score += size;
            c = SG_BLACK;
        }
        else if (! adjacent[SG_BLACK] && adjacent[SG_WHITE])
        {
            score -= size;
            c = SG_WHITE;
        }
        else
            c = SG_EMPTY;
        if (scoreBoard != 0)
            for (GoPointList::Iterator it2(list); it2; ++it2)
                (*scoreBoard)[*it2] = c;
    }
    return score;
}

//----------------------------------------------------------------------------

template<class BOARD>
std::ostream& GoWriteBoard(std::ostream& out, const BOARD& bd)
{
    // Write board to a buffer first to avoid intermingling if boards are
    // dumped from different threads at the same time (e.g. debugging
    // output after an assertion)
    std::ostringstream buffer;
    SgGrid size = bd.Size();
    if (size > 9)
        buffer << "   ";
    else
        buffer << "  ";
    SgGrid col;
    char c;
    for (col = 1, c = 'A'; col <= size; ++col, ++c)
    {
        if (c == 'I')
            ++c;
        buffer << c << ' ';
    }
    buffer << '\n';
    for (SgGrid row = size; row >= 1; --row)
    {
        if (size > 9 && row < 10)
            buffer << ' ';
        buffer << row << ' ';
        for (SgGrid col = 1; col <= size; ++col)
        {
            SgPoint p = SgPointUtil::Pt(col, row);
            switch (bd.GetColor(p))
            {
            case SG_BLACK:
                buffer << 'X';
                break;
            case SG_WHITE:
                buffer << 'O';
                break;
            case SG_EMPTY:
                if (GoBoardUtil::IsHandicapPoint(size, col, row))
                    buffer << '+';
                else
                    buffer << '.';
                break;
            default:
                SG_ASSERT(false);
            }
            buffer << ' ';
        }
        buffer << row;
        if (row <= 2)
        {
            if (size < 10)
                buffer << "  ";
            else
                buffer << "   ";
            // More important info first, because the number of infos shown
            // depends on the board size
            if (row == 1)
                buffer << SgBW(bd.ToPlay()) << " to play";
        }
        buffer << '\n';
    }
    if (size > 9)
        buffer << "   ";
    else
        buffer << "  ";
    for (col = 1, c = 'A'; col <= size; ++col, ++c)
    {
        if (c == 'I')
            ++c;
        buffer << c << ' ';
    }
    buffer << '\n';
    out << buffer.str();
    return out;
}

inline std::ostream& operator<<(std::ostream& out, const GoBoard& bd)
{
    return GoWriteBoard(out, bd);
}

//----------------------------------------------------------------------------

/** Used to restore GoBoard::Rules()::GetKoRule() to its current value in an
    exception-safe way.
    To use it, just declare a variable of this type on the stack for the
    desired scope.
*/
class GoRestoreKoRule
{
public:
    GoRestoreKoRule(GoBoard& board);

    ~GoRestoreKoRule();

private:
    GoBoard& m_board;

    GoRules::KoRule m_koRule;

    /** Not implemented. */
    GoRestoreKoRule(const GoRestoreKoRule&);

    /** Not implemented. */
    GoRestoreKoRule& operator=(const GoRestoreKoRule&);
};

inline GoRestoreKoRule::GoRestoreKoRule(GoBoard& board)
    : m_board(board),
      m_koRule(board.Rules().GetKoRule())
{
}

inline GoRestoreKoRule::~GoRestoreKoRule()
{
    m_board.Rules().SetKoRule(m_koRule);
}

//----------------------------------------------------------------------------

/** Used to restore ToPlay to its current value in an exception-safe way.
    To use it, just declare a RestoreToPlay variable on the stack for the
    desired scope.
*/
class GoRestoreToPlay
{
public:
    GoRestoreToPlay(GoBoard& board)
        : m_board(board),
          m_oldToPlay(board.ToPlay())
    { }

    ~GoRestoreToPlay()
    {
        m_board.SetToPlay(m_oldToPlay);
    }

private:
    GoBoard& m_board;

    SgBlackWhite m_oldToPlay;

    /** Not implemented. */
    GoRestoreToPlay(const GoRestoreToPlay&);

    /** Not implemented. */
    GoRestoreToPlay& operator=(const GoRestoreToPlay&);
};

//----------------------------------------------------------------------------

/** Iterate over all blocks' anchors on the board. */
class GoBlockIterator
{
public:
    GoBlockIterator(const GoBoard& board)
        : m_board(board),
          m_p(board)
    {
        if (! *this)
            ++(*this);
    }

    void operator++()
    {
        do
        {
            ++m_p;
        }
        while (m_p && ! *this);
    }

    SgPoint operator*() const
    {
        SG_ASSERT(*this);
        return *m_p;
    }

    operator bool() const
    {
        SgPoint p = *m_p;
        return (m_board.Occupied(p) && p == m_board.Anchor(p));
    }

private:
    const GoBoard& m_board;

    GoBoard::Iterator m_p;

    /** Not implemented. */
    GoBlockIterator(const GoBlockIterator&);

    /** Not implemented. */
    GoBlockIterator& operator=(const GoBlockIterator&);
};

//----------------------------------------------------------------------------

/** Used to permit/forbid self-removal for certain periods of play.
    Restores the setting to the previous value in an exception-safe way.
    To use it, just declare a SelfRemoval variable on the stack for the
    desired scope.
*/
class GoRestoreSuicide
{
public:
    GoRestoreSuicide(GoBoard& board, bool allow)
        : m_board(board),
          m_oldState(board.Rules().AllowSuicide())
    {
        m_board.Rules().SetAllowSuicide(allow);
    }

    ~GoRestoreSuicide()
    {
        m_board.Rules().SetAllowSuicide(m_oldState);
    }

private:
    GoBoard& m_board;

    bool m_oldState;

    /** Not implemented. */
    GoRestoreSuicide(const GoRestoreSuicide&);

    /** Not implemented. */
    GoRestoreSuicide& operator=(const GoRestoreSuicide&);
};

//----------------------------------------------------------------------------

/** Used to alter state of repetition and self-removal for certain periods of
    play.
    Restores the settings to the previous values in an exception-safe way.
    To use it, just declare a RestoreRepetitionAndRemoval variable on the
    stack for the desired scope.
*/
class GoRestoreRepetitionAndSuicide
{
public:
    GoRestoreRepetitionAndSuicide(GoBoard& board, bool allowAnyRepetition,
                                  bool allowKoRepetition, bool allowSuicide)
        :  m_board(board),
           m_oldAnyRepetition(board.AnyRepetitionAllowed()),
           m_oldKoRepetition(board.KoRepetitionAllowed()),
           m_oldSuicide(board.Rules().AllowSuicide())
    {
        m_board.AllowAnyRepetition(allowAnyRepetition);
        m_board.AllowKoRepetition(allowKoRepetition);
        m_board.Rules().SetAllowSuicide(allowSuicide);
    }

    ~GoRestoreRepetitionAndSuicide()
    {
        m_board.AllowAnyRepetition(m_oldAnyRepetition);
        m_board.AllowKoRepetition(m_oldKoRepetition);
        m_board.Rules().SetAllowSuicide(m_oldSuicide);
    }

private:
    GoBoard& m_board;

    /** arbitrary repetition for both players */
    bool m_oldAnyRepetition;

    bool m_oldKoRepetition;

    /** whether self-removal is allowed */
    bool m_oldSuicide;

    /** Not implemented. */
    GoRestoreRepetitionAndSuicide(const GoRestoreRepetitionAndSuicide&);

    /** Not implemented. */
    GoRestoreRepetitionAndSuicide&
    operator=(const GoRestoreRepetitionAndSuicide&);
};

//----------------------------------------------------------------------------

/** Iterate through the anchors of all the blocks adjacent to the given
    point.
*/
class GoNeighborBlockIterator
    : public SgPointIterator
{
public:
    GoNeighborBlockIterator(const GoBoard& board, SgPoint p, SgBlackWhite c)
        : SgPointIterator(m_points)
    {
        board.NeighborBlocks(p, c, m_points);
    }

    GoNeighborBlockIterator(const GoBoard& board, SgPoint p, SgBlackWhite c,
                            int maxLib)
        : SgPointIterator(m_points)
    {
        board.NeighborBlocks(p, c, maxLib, m_points);
    }


private:
    /** At most 4 neighbor points, plus terminator. */
    SgPoint m_points[5];
};

//----------------------------------------------------------------------------

static const int MAX_ADJACENT = (SG_MAX_SIZE + 1) * (SG_MAX_SIZE + 1) / 4;

/** Iterate through the anchors of all the blocks adjacent to the given
    block.
*/
template<class BOARD>
class GoAdjBlockIterator
    : public SgPointIterator
{
public:
    GoAdjBlockIterator(const BOARD& board, SgPoint p, int maxLib);

private:
    /** Maximum number of adjacent blocks.
        Not quite sure this is an upper limit, but couldn't find an
        example that had more adjacent stones than a spiral block with
        adjacent single stones spaced one apart.
    */

    SgPoint m_points[MAX_ADJACENT];
};

template<class BOARD>
GoAdjBlockIterator<BOARD>::GoAdjBlockIterator(const BOARD& board,
                                              SgPoint p, int maxLib)
    : SgPointIterator(m_points)
{
    board.AdjacentBlocks(p, maxLib, m_points, MAX_ADJACENT);
}

//----------------------------------------------------------------------------

class GoNbIterator
    : public SgNbIterator
{
public:
    GoNbIterator(const GoBoard& bd, SgPoint p);
};

inline GoNbIterator::GoNbIterator(const GoBoard& bd, SgPoint p)
    : SgNbIterator(bd.BoardConst(), p)
{
}

//----------------------------------------------------------------------------

/** @todo move into its own file */
namespace GoBoardWrite
{

/** Write a map of the board, showing marks for SgPointSet */
class WriteMap
{
public:
    WriteMap(const GoBoard& bd, const SgPointSet& points)
    : m_bd(bd),
      m_points(points)
    {
    }

    const GoBoard& Board() const {return m_bd;}

    const SgPointSet& Points() const { return m_points; }

private:
    const GoBoard& m_bd;

    const SgPointSet& m_points;
};

} // namespace GoBoardWrite

std::ostream& operator<<(std::ostream& out, const GoBoardWrite::WriteMap& w);

//----------------------------------------------------------------------------

#endif // GO_BOARDUTIL_H
