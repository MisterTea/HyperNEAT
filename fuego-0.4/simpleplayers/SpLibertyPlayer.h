//----------------------------------------------------------------------------
/** @file SpLibertyPlayer.h
    Liberty player, concentrates on low liberty stones
*/
//----------------------------------------------------------------------------

#ifndef SP_LIBERTYPLAYER_H
#define SP_LIBERTYPLAYER_H

#include "SpSimplePlayer.h"
#include "SpMoveGenerator.h"

//----------------------------------------------------------------------------

/** Tries to capture and escape with low liberty stones
    Blocks with few liberties are very important to this player 
*/
class SpLibertyMoveGenerator
    : public SpStaticMoveGenerator
{
public:
    SpLibertyMoveGenerator(GoBoard& board)
        : SpStaticMoveGenerator(board)
    { }

    int Score(SgPoint p);
};

//----------------------------------------------------------------------------

/** Simple player using SpLibertyMoveGenerator */
class SpLibertyPlayer
    : public SpSimplePlayer
{
public:
    SpLibertyPlayer(GoBoard& board)
        : SpSimplePlayer(board, new SpLibertyMoveGenerator(board))
    { }

    std::string Name() const
    {
        return "Liberty";
    }
};

//----------------------------------------------------------------------------

int LibertyMinus2(const GoBoard& board, SgBlackWhite color);

#endif

