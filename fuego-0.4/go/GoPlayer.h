//----------------------------------------------------------------------------
/** @file GoPlayer.h
    Player.
*/
//----------------------------------------------------------------------------

#ifndef GO_PLAYER_H
#define GO_PLAYER_H

#include <string>
#include "GoBoard.h"
#include "GoBoardSynchronizer.h"
#include "GoPlayerMove.h"
#include "SgBoardColor.h"
#include "SgPoint.h"

class SgNode;
class SgTimeRecord;

//----------------------------------------------------------------------------

/** Player. */
class GoPlayer
    : public GoBoardSynchronizer
{
public:
    /** Constructor.
    */
    GoPlayer(const GoBoard& bd);

    virtual ~GoPlayer();

    /** See m_board */
    GoBoard& Board();

    /** See m_board */
    const GoBoard& Board() const;

    /** Generate a move. */
    virtual SgPoint GenMove(const SgTimeRecord& time,
                            SgBlackWhite toPlay) = 0;

    /** Get the name of this player.
        Default implementation returns "Unknown"
    */
    virtual std::string Name() const;

    /** Get node for appending search traces */
    SgNode* CurrentNode() const;

    /** Set node for appending search traces */
    void SetCurrentNode(SgNode* node);

    /** Return value for a move.
        Not all players assign values to moves.
        If a player cannot score moves in general, or cannot score this move
        in particular, it should return numeric_limits<int>::min()
        (which is what the default implementation does).
        Values can be positive or negative; better moves should have higher
        values; the units of the values are not specified.
     */
    virtual int MoveValue(SgPoint p);

    /** Inform the player that the game was finished.
        This function gives the player the opportunity to do some work at
        the end of a game, for example perform some learning.
        However it is not guaranteed that this function will be called at
        all, the player should not rely on it. The reason is that a game
        start and end is not always well-defined (setup, undo, etc.)
        For example, it will be called in the GTP interface if after a change
        on the board GoBoardUtil::EndOfGame() is true.
        The default implementation does nothing.
    */
    virtual void OnGameFinished();

    /** Inform the player that a new game was started.
        This function gives the player the opportunity to clear some cached
        data, that is useful only within the same game.
        However it is not guaranteed that this function will be called at
        all, the player should not rely on it. The reason is that a game
        start and end is not always well-defined (setup, undo, etc.)
        For example, it will be called in the GTP interface in
        GoGtpEngine::Init(), or on the clear_board and loadsgf commands.
        The default implementation does nothing.
    */
    virtual void OnNewGame();

    /** Think during opponent's time.
        This function should poll SgUserAbort() and return immediately if
        it returns true.
        It is good style to enable pondering in the player with a parameter
        and return immediately if it is not enabled. If it is enabled, it
        should stop after some resource limit is reached to avoid that a
        program is hogging the CPU if it is just waiting for commands.
        The function Ponder() is typically called from a different thread but
        without overlap with other uses of the player, so the player does not
        have to care about thread-safety.
        Default implementation does nothing and returns immediately.
    */
    virtual void Ponder();

    /** See m_variant */
    int Variant() const;

    /** See m_variant */
    void SetVariant(int variant);

protected:
    /** Node in game tree. Used for appending search traces */
    SgNode* m_currentNode;

private:
    /** The player's own Go board */
    GoBoard m_bd;

    /** Player variant. Used for short-term testing of small modifications.
        The default variant is 0.
        Do not use to create significantly different players -
        implement a new player instead.
    */
    int m_variant;

    /** Not implemented */
    GoPlayer(const GoPlayer&);

    /** Not implemented */
    GoPlayer& operator=(const GoPlayer&);
};

inline GoBoard& GoPlayer::Board()
{
    return m_bd;
}

inline const GoBoard& GoPlayer::Board() const
{
    return m_bd;
}

inline SgNode* GoPlayer::CurrentNode() const
{
    return m_currentNode;
}

inline void GoPlayer::SetCurrentNode(SgNode* node)
{
    m_currentNode = node;
}

inline int GoPlayer::Variant() const
{
    return m_variant;
}

inline void GoPlayer::SetVariant(int variant)
{
    m_variant = variant;
}

//----------------------------------------------------------------------------

#endif // GO_PLAYER_H

