//----------------------------------------------------------------------------
/** @file SpInfluencePlayer.h
    Greedy influence player
*/
//----------------------------------------------------------------------------

#ifndef SP_INFLUENCEPLAYER_H
#define SP_INFLUENCEPLAYER_H

#include "SpSimplePlayer.h"
#include "SpMoveGenerator.h"

//----------------------------------------------------------------------------

/** The influence move generator tries to maximize influence */
class SpInfluenceMoveGenerator
    : public Sp1PlyMoveGenerator
{
public:
    explicit SpInfluenceMoveGenerator(GoBoard& board)
        : Sp1PlyMoveGenerator(board)
    { }

    int Evaluate();
};

//----------------------------------------------------------------------------

/** Simple player using SpInfluenceMoveGenerator */
class SpInfluencePlayer
    : public SpSimplePlayer
{
public:
    SpInfluencePlayer(GoBoard& board)
        : SpSimplePlayer(board, new SpInfluenceMoveGenerator(board))
    { }

    std::string Name() const
    {
        return "Influence";
    }
};

//----------------------------------------------------------------------------

#endif

