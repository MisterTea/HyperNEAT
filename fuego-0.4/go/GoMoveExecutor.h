//----------------------------------------------------------------------------
/** @file GoMoveExecutor.h
    Class GoMoveExecutor.
*/
//----------------------------------------------------------------------------

#ifndef GO_MOVEEXECUTOR_H
#define GO_MOVEEXECUTOR_H

#include "GoBoardUtil.h"

//----------------------------------------------------------------------------

/** Used to execute and undo one move without having to worry about undoing
    the move.
*/
class GoMoveExecutor
{
public:
    GoMoveExecutor(GoBoard& board, SgPoint move)
        : m_board(board)
    {
        m_isLegal = GoBoardUtil::PlayIfLegal(m_board, move);
    }

    GoMoveExecutor(GoBoard& board, SgPoint move, SgBlackWhite player)
        : m_board(board)
    {
        m_isLegal = GoBoardUtil::PlayIfLegal(m_board, move, player);
    }

    ~GoMoveExecutor()
    {
        if (m_isLegal)
            m_board.Undo();
    }

    bool IsLegal() const
    {
        return m_isLegal;
    }

    /** Can be used to undo the move before the MoveExecutor is destructed, to
        force the situation back the previous state.
        Note that this can only be called for legal moves, and IsLegal returns
        false afterwards.
    */
    void UndoMove()
    {
        SG_ASSERT(m_isLegal);
        m_board.Undo();
        m_isLegal = false;
    }

private:
    GoBoard& m_board;

    bool m_isLegal;

    /** Not implemented. */
    GoMoveExecutor(const GoMoveExecutor&);

    /** Not implemented. */
    GoMoveExecutor& operator=(const GoMoveExecutor&);
};

//----------------------------------------------------------------------------

#endif // GO_MOVEEXECUTOR_H

