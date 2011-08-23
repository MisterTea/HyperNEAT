//----------------------------------------------------------------------------
/** @file SpSafePlayer.h
    Safe player
*/
//----------------------------------------------------------------------------

#ifndef SP_SAFEPLAYER_H
#define SP_SAFEPLAYER_H

#include "SpSimplePlayer.h"
#include "SpMoveGenerator.h"

//----------------------------------------------------------------------------

/** Uses GoSafetySolver and one ply search.
    Strong in the end, random in the beginning
*/
class SpSafeMoveGenerator
    : public Sp1PlyMoveGenerator
{
public:
    explicit SpSafeMoveGenerator(GoBoard& board)
        : Sp1PlyMoveGenerator(board)
    { }

    int Evaluate();
};

//----------------------------------------------------------------------------

/** Simple player using SpSafeMoveGenerator */
class SpSafePlayer
    : public SpSimplePlayer
{
public:
    SpSafePlayer(GoBoard& board)
        : SpSimplePlayer(board, new SpSafeMoveGenerator(board))
    { }

    std::string Name() const
    {
        return "Safe";
    }
};

//----------------------------------------------------------------------------

#endif // SP_SAFEPLAYER_H

