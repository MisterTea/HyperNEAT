//----------------------------------------------------------------------------
/** @file SpAveragePlayer.h
    Average liberty player
*/
//----------------------------------------------------------------------------

#ifndef SP_AVERAGEPLAYER_H
#define SP_AVERAGEPLAYER_H

#include <string>
#include "SpSimplePlayer.h"
#include "SpMoveGenerator.h"

//----------------------------------------------------------------------------

/** Tries to maximize liberty average of own minus opponent blocks.
    Tends to build long chains with many liberties and some eyes.
    The strongest of the original simple players.
*/
class SpAverageMoveGenerator
    : public Sp1PlyMoveGenerator
{
public:
    explicit SpAverageMoveGenerator(GoBoard& board)
        : Sp1PlyMoveGenerator(board)
    { }

    int Evaluate();
};

//----------------------------------------------------------------------------

/** Simple player using SpAverageMoveGenerator */
class SpAveragePlayer
    : public SpSimplePlayer
{
public:
    SpAveragePlayer(GoBoard& board)
        : SpSimplePlayer(board, new SpAverageMoveGenerator(board))
    { }

    std::string Name() const
    {
        return "AverageLib";
    }
    
    bool UseFilter() const
    { 
        return true; 
    }
};

//----------------------------------------------------------------------------

/** Average number of liberties of blocks of color, multiplied by 10 */
int LibertyAveragex10(const GoBoard& board, SgBlackWhite color);

//----------------------------------------------------------------------------

#endif

