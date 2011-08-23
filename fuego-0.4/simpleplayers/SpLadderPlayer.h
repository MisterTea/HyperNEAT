//----------------------------------------------------------------------------
/** @file SpLadderPlayer.h
    Ladder catcher player
*/
//----------------------------------------------------------------------------

#ifndef SP_LADDERPLAYER_H
#define SP_LADDERPLAYER_H

#include "SpSimplePlayer.h"
#include "SpMoveGenerator.h"

//----------------------------------------------------------------------------

/** Generates ladder captures/escapes */
class SpLadderMoveGenerator
    : public SpStaticMoveGenerator
{
public:
    explicit SpLadderMoveGenerator(GoBoard& board)
        : SpStaticMoveGenerator(board)
    { }

    virtual void GenerateMoves(SgEvaluatedMoves& eval, SgBlackWhite toPlay);
    
    virtual int Score(SgPoint p);
};

//----------------------------------------------------------------------------

/** Simple player using SpLadderMoveGenerator */
class SpLadderPlayer
    : public SpSimplePlayer
{
public:
    SpLadderPlayer(GoBoard& board)
        : SpSimplePlayer(board, new SpLadderMoveGenerator(board))
    { }

    std::string Name() const
    {
        return "Ladder";
    }
};

//----------------------------------------------------------------------------

#endif

