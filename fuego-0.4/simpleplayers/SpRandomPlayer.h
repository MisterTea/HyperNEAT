//----------------------------------------------------------------------------
/** @file SpRandomPlayer.h
    Random player
*/
//----------------------------------------------------------------------------

#ifndef SP_RANDOMPLAYER_H
#define SP_RANDOMPLAYER_H

#include "SpSimplePlayer.h"
#include "SpMoveGenerator.h"
#include "SgDebug.h"
#include "SgWrite.h"

//----------------------------------------------------------------------------

/** Plays random moves, but does not fill obvious 1-point eyes */
class SpRandomMoveGenerator
    : public SpStaticMoveGenerator
{
public:
    SpRandomMoveGenerator(GoBoard& board)
        : SpStaticMoveGenerator(board)
    { }

    virtual int Score(SgPoint p);
};

//----------------------------------------------------------------------------

/** Simple player using SpRandomMoveGenerator */
class SpRandomPlayer
    : public SpSimplePlayer
{
public:
    SpRandomPlayer(GoBoard& board)
        : SpSimplePlayer(board, new SpRandomMoveGenerator(board))
    { }

    std::string Name() const
    {
        return "Random";
    }

protected:
    bool UseFilter() const
    {
        return false;
    }
};

//----------------------------------------------------------------------------

#endif

