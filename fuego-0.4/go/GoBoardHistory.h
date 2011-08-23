//----------------------------------------------------------------------------
/** @file GoBoardHistory.h */
//----------------------------------------------------------------------------

#ifndef GOBOARDHISTORY_H
#define GOBOARDHISTORY_H

#include <vector>
#include "GoBoard.h"

//----------------------------------------------------------------------------

/** Identifier for board state including history.
    This class can be used, for instance, to uniquely remember a board
    position for reusing parts of previous computations. The state includes:
    - the board size
    - the rules
    - the setup stones and color to play before any moves were played
    - the history of moves (needed if super-ko rules are used)
    - the current color to play
 */
class GoBoardHistory
{
public:
    /** Constructor.
        The initial state is that the history does not correspond to any
        valid position.
    */
    GoBoardHistory();

    /** Initialize from a current board position. */
    void SetFromBoard(const GoBoard& bd);

    /** Check if this position is a alternate-play follow-up to another one.
        @param other The other position
        @param[out] sequence The sequence leading from the other position to
        this one
        @return @c true If the position is a follow-up
    */
    bool IsAlternatePlayFollowUpOf(const GoBoardHistory& other,
                                   std::vector<SgPoint>& sequence);

private:
    int m_boardSize;

    GoRules m_rules;

    GoSetup m_setup;

    SgSList<GoPlayerMove,GO_MAX_NUM_MOVES> m_moves;

    SgBlackWhite m_toPlay;
};

inline GoBoardHistory::GoBoardHistory()
    : m_boardSize(-1)
{
}

//----------------------------------------------------------------------------

#endif // GOBOARDHISTORY_H
