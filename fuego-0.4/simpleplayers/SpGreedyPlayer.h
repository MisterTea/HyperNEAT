//----------------------------------------------------------------------------
/** @file SpGreedyPlayer.h
    Greedy player
*/
//----------------------------------------------------------------------------

#ifndef SP_GREEDYPLAYER_H
#define SP_GREEDYPLAYER_H

#include "SpSimplePlayer.h"
#include "SpMoveGenerator.h"

//----------------------------------------------------------------------------

/** GreedyMoveGenerator goes for liberties, and some influence */
class SpGreedyMoveGenerator
    : public Sp1PlyMoveGenerator
{
public:
    explicit SpGreedyMoveGenerator(GoBoard& board)
        : Sp1PlyMoveGenerator(board)
    { }

    int Evaluate();
};

//----------------------------------------------------------------------------

/** Simple player using SpGreedyMoveGenerator */
class SpGreedyPlayer
    : public SpSimplePlayer
{
public:
    SpGreedyPlayer(GoBoard& board)
        : SpSimplePlayer(board, new SpGreedyMoveGenerator(board))
    { }

    std::string Name() const
    {
        return "Greedy";
    }
};

#endif

