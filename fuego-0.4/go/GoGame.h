//----------------------------------------------------------------------------
/** @file GoGame.h
    GoGameRecord class, play and replay moves in a game tree.

    GoGame implements the GoGameRecord class, which provides functions for
    playing and replaying moves in a game tree.
    Derived from it is the GoGame class, which allows two Player objects
    to play a game against each other.
*/
//----------------------------------------------------------------------------

#ifndef GO_GAME_H
#define GO_GAME_H

#include <string>
#include "GoBoard.h"
#include "GoBoardUpdater.h"
#include "GoBoardUtil.h"
#include "SgNode.h"
#include "SgPoint.h"
#include "SgTimeRecord.h"

class GoPlayer;
class GoPlayerMove;
class SgSearchStatistics;

//----------------------------------------------------------------------------

/** Game stored as a tree of moves.
    @note What is wrong with this class? It exposes non-const access to its
    data members and therefore cannot guarantee the integrity of its state. In
    the future, this class should only provide read access to the board, tree,
    and current node pointer. All modifications to and navigation in the tree
    should only be possible through member functions of this class, such that
    it can guarantee that its invariants are met (i.e. the board position
    always belongs to the current node and the current node always points to a
    valid node in the tree).
*/
class GoGameRecord
{
public:
    /** Create a game record for replaying games on the given board. */
    explicit GoGameRecord(GoBoard& board);

    virtual ~GoGameRecord();

    /** Delete the old game record and start with a fresh one.
        If 'root' is 0, create a root node to start with,
        otherwise use the game tree passed in 'root'.
        If 'fTakeOwnership', the game record will delete this tree when
        it's destroyed, otherwise the client is reponsible
        for deleting the tree.
        @todo AR: change comment
    */
    virtual void InitFromRoot(SgNode* root, bool fTakeOwnership);

    /** Delete the old game record and start with a fresh one.
        Init the board with the given parameters, and create a root node
        to start with.
    */
    virtual void Init(int size, const GoRules& rules);

    /** Hook for subclasses.
        Default implementation does nothing.
    */
    virtual void OnGoToNode(SgNode* dest);

    /** Override to initialize other boards in derived classes.
        @see GoBoard
    */
    virtual void OnInitBoard(int size, const GoRules& rules);

    virtual void OnInitHandicap(const GoRules& rules, SgNode* root);

    /** Get the board associated with this game record. */
    const GoBoard& Board() const;

    /** Return the root of this tree. */
    SgNode& Root() const;

    /** Add move to the game record.
        Add move as the next move at the current position.
        If a node with that move already exists, then don't add a new one.
        Return the node with that move.
        Also add any statistics from 'stat' and time left to that node.
    */
    SgNode* AddMove(SgMove move, SgBlackWhite player,
                    const SgSearchStatistics* stat = 0);

    /** Add a node with a comment that a player resigned.
        For informational purposes only, the resign node will not be made
        the current node.
    */
    SgNode* AddResignNode(SgBlackWhite player);

    /** Play to the given node.
        @c dest must be in this tree, or 0.
        Also updates the clock.
    */
    void GoToNode(SgNode* dest);

    /** Play to the next node in the given direction. */
    void GoInDirection(SgNode::Direction dir);

    /** Return whether there is a next node in the given direction. */
    bool CanGoInDirection(SgNode::Direction dir) const;

    /** Set the current player. */
    void SetToPlay(SgBlackWhite player);

    /** Return whether the game is finished. */
    bool EndOfGame() const;

    /** Get the name of the given player.
        Return the blank string if unknown.
        Override to get information from an ongoing game rather than fron the
        game record.
    */
    virtual std::string GetPlayerName(SgBlackWhite player) const;

    /** Return the current score (black minus white).
        @todo: in what units?
        Return true if there's a score recorded in the tree, otherwise false.
        Override to return information about the score from a player.
    */
    virtual bool GetScore(int* score) const;

    /** The time left in the game at the current position. */
    SgTimeRecord& Time();

    /** The time left in the game at the current position. */
    const SgTimeRecord& Time() const;

    /** Return the current position in the tree.
        @todo changed from protected to public because of getting
        the current time left.
    */
    SgNode* CurrentNode();

    /** Return the current position in the tree.
        @todo changed from protected to public because of getting
        the current time left.
    */
    const SgNode* CurrentNode() const;

    /** Whether this game record owns the game tree. */
    bool OwnsTree() const;

    bool CanDeleteCurrentNode() const;

    /** Return the move of the current node.
        Return NullMove if no current move.
    */
    SgMove CurrentMove() const;

    /** Get the number of moves since root or last node with setup
        properties.
    */
    int CurrentMoveNumber() const;

private:
    /** The position in the current tree. */
    SgNode* m_current;

    GoBoard& m_board;

    GoBoardUpdater m_updater;

    /** A record of the clock settings and time left. */
    SgTimeRecord m_time;

    /** Whether this game record owns the game tree. */
    bool m_ownsTree;

    /** Comment display. */
    SgNode* m_oldCommentNode;

    /** Moves inserted into a line of play instead of added at the end. */
    int m_numMovesToInsert;

    /** Not implemented. */
    GoGameRecord(const GoGameRecord&);

    /** Not implemented. */
    GoGameRecord& operator=(const GoGameRecord&);

    /** Delete the tree and initialize the state associated with the position
        in the tree.
    */
    void DeleteTreeAndInitState();
};

inline const GoBoard& GoGameRecord::Board() const
{
    return m_board;
}

inline SgNode& GoGameRecord::Root() const
{
    SG_ASSERT(m_current);
    return *m_current->Root();
}

inline SgTimeRecord& GoGameRecord::Time()
{
    return m_time;
}

inline const SgTimeRecord& GoGameRecord::Time() const
{
    return m_time;
}

inline SgNode* GoGameRecord::CurrentNode()
{
    return m_current;
}

inline const SgNode* GoGameRecord::CurrentNode() const
{
    return m_current;
}

inline bool GoGameRecord::OwnsTree() const
{
    return m_ownsTree;
}

//----------------------------------------------------------------------------

/** Utility functions for GoGameRecord. */
namespace GoGameUtil
{
    /** Goto last node in main variation before move number.
        This function can be used for implementing the loadsgf GTP command.
        @param game (current node must be root)
        @param moveNumber move number (-1 means goto last node in main
        variation)
        @return false if moveNumber greater than moves in main variation
    */
    bool GotoBeforeMove(GoGameRecord* game, int moveNumber);
}

//----------------------------------------------------------------------------

/** Game record played between two players.
    A GoGame object handles the synchronization between the players
    and the board they're playing on, and keeps track of the time.
*/
class GoGame
    : public GoGameRecord
{
public:
    explicit GoGame(GoBoard& board);

    virtual ~GoGame();

    /** Needed to avoid hiding of inherited virtual function by the
        other variants of GoGame::Init.
    */
    virtual void InitFromRoot(SgNode* root, bool fTakeOwnership)
    {
        GoGameRecord::InitFromRoot(root, fTakeOwnership);
    }

    /** Needed to avoid hiding of inherited virtual function by the
        other variants of GoGame::Init.
    */
    void Init(int size, const GoRules& rules);

    void Init(SgNode* root, bool fTakeOwnership, bool fDeletePlayers);

    void Init(int size, const GoRules& rules, bool fDeletePlayers);

    void OnGoToNode(SgNode* dest);

    /** Turn the clock on or off.
        Call this method instead of Time().TurnClockOn
        so that computer player is started and stopped appropriately.
    */
    void TurnClockOn(bool turnOn);

    /** Return whether the clock is running (not stopped or suspended). */
    bool ClockIsRunning() const { return Time().ClockIsRunning(); }

    /** Set the player 'color' to the player algorithm 'player'.
        The old player of that color is stopped and deleted, and the new
        player is initialized to the current board state.
        A 0 player means user interaction.
    */
    void SetPlayer(SgBlackWhite color, GoPlayer* player);

    /** Delete the player 'color' (set it to interactive input).
        @note If black and white point to the same player, delete will be
        called only once, but both players will be set to null.
    */
    void DeletePlayer(SgBlackWhite color);

    /** Whether the human move at 'move' can be played at this point.
        The move must be legal, and either it's a human player to play
        or the clock is inactive.
    */
    bool CanPlayHumanMove(SgMove move, SgBlackWhite player) const;

    /** Adds a move to the game record and tells all other players to update
        their boards.
        Returns whether the move was added to the game record.
        @todo AR: to be replaced by HumanPlayer.
    */
    bool PlayHumanMove(SgMove move, SgBlackWhite player);

    void PlayComputerMove(const GoPlayerMove* playerMove);

    GoPlayer* Player(SgBlackWhite player);

    /** Compute and play one move using the given player. */
    GoPlayerMove PlayOneMove(SgBlackWhite playerColor);

private:
    /** Black and white player. */
    SgBWArray<GoPlayer*> m_player;

    /** Not implemented. */
    GoGame(const GoGame&);

    /** Not implemented. */
    GoGame& operator=(const GoGame&);

    void UpdatePlayer(SgBlackWhite color);

    void UpdatePlayers();
};

inline GoPlayer* GoGame::Player(SgBlackWhite player)
{
    return m_player[player];
}

//----------------------------------------------------------------------------

#endif // GO_GAME_H

