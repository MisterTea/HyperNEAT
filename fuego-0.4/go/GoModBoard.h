//----------------------------------------------------------------------------
/** @file GoModBoard.h */
//----------------------------------------------------------------------------

#ifndef GO_MODBOARD_H
#define GO_MODBOARD_H

#include "GoAssertBoardRestored.h"

//----------------------------------------------------------------------------

/** Make a const board temporarily modifiable.
    Allows functions to use a const board for performing temporary operations
    on the board (e.g. searches), as long as the board is in the same state
    after the function is finished. This class facilitates const-correctness
    and encapsulation, because it allows the owner of a board, which is the
    only one who is allowed to do persistent changes on the board, to hand out
    only a const reference to other code. The other code can still use the
    board for temporary operations without needing a copy of the board.
    GoModBoard does a const_cast from the const reference to a non-const
    reference in its constructor and checks with GoAssertBoardRestored in its
    destructor that the board is returned in the same state.

    Example:
    @code
    // myFunction is not supposed to do persistent changes on the board
    // and therefore gets a const-reference. However it wants to use
    // the board temporarily
    void myFunction(const GoBoard& constBoard)
    {
        GoModBoard modBoard(constBoard);
        GoBoard& bd = modBoard.Board(); // get a nonconst-reference

        // ... play some moves and undo them

        // end of lifetime for modBoard, GoAssertBoardRestored is
        // automatically called in the destructor of modBoard
    }
    @endcode

    There are also functions that allow to lock and unlock the board
    explicitly, for cases in which the period of temporary modifications
    cannot be mapped to the lifetime of a GoModBoard instance (e.g. because
    the period starts end ends in different functions).
*/
class GoModBoard
{
public:
    /** Constructor.
        Remembers the current board state.
        @param bd The board
        @param locked Whether to start in locked mode (for explicit usage
        of Lock() and Unlock())
    */
    GoModBoard(const GoBoard& bd, bool locked = false);

    /** Destructor.
        Checks with assertions that the board state is restored.
    */
    ~GoModBoard();

    /** Explicit conversion to non-const reference.
        This function triggers an assertion, if the board is currently in
        locked mode.
    */
    GoBoard& Board() const;

    /** Automatic conversion to non-const reference.
        Allows to pass GoModBoard to functions that expect a non-const GoBoard
        reference without explicitely calling GoModBoard.Board().
        @see Board()
    */
    operator GoBoard&() const;

    /** Explicitely unlock the board. */
    void Unlock();

    /** Explicitely lock the board.
        Checks with assertions that the board state is restored.
        See Lock()
    */
    void Lock();

private:
    bool m_locked;

    GoBoard& m_bd;

    GoAssertBoardRestored m_assertRestored;
};

inline GoModBoard::GoModBoard(const GoBoard& bd, bool locked)
    : m_locked(locked),
      m_bd(const_cast<GoBoard&>(bd)),
      m_assertRestored(bd)
{
}

inline GoModBoard::~GoModBoard()
{
    // Destructor of m_assertRestored calls AssertRestored()
}

inline GoModBoard::operator GoBoard&() const
{
    return Board();
}

inline GoBoard& GoModBoard::Board() const
{
    SG_ASSERT(! m_locked);
    return m_bd;
}

inline void GoModBoard::Unlock()
{
    m_assertRestored.Init(m_bd);
    m_locked = false;
}

inline void GoModBoard::Lock()
{
    m_assertRestored.AssertRestored();
    m_assertRestored.Clear();
    m_locked = true;
}

//----------------------------------------------------------------------------

#endif // GO_MODBOARD_H
