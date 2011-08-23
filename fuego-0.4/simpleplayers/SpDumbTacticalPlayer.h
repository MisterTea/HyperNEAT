//----------------------------------------------------------------------------
/** @file SpDumbTacticalPlayer.h
    Dumb tactical player
*/
//----------------------------------------------------------------------------

#ifndef SP_DUMBTACTICALPLAYER_H
#define SP_DUMBTACTICALPLAYER_H

#include "SpSimplePlayer.h"
#include "SpMoveGenerator.h"

//----------------------------------------------------------------------------

/** Plays mostly to extend/reduce liberties.
    Plays the following, in priority order:
    1. If own group has single liberty, play there
    2. Amongst opponent groups without two eyes, find the one with the least
       libs, and fill the one with the most second order liberties. If there
       is a tie pick one at random.
    3. Make a random move that doesn't fill own eye
    4. Pass
*/
class SpDumbTacticalMoveGenerator
    : public SpStaticMoveGenerator
{
public:
    explicit SpDumbTacticalMoveGenerator(GoBoard& board);
    
    virtual void GenerateMoves(SgEvaluatedMoves& eval, SgBlackWhite toPlay);

    virtual bool ExecuteMoveForScoring()
    {
        return false;
    }

    virtual int Score(SgPoint p);

private:
    bool m_useLadders;
    
    void GenerateAttackMoves(SgEvaluatedMoves& eval);

    void GenerateDefendMoves(SgEvaluatedMoves& eval);
};

//----------------------------------------------------------------------------

/** A SpSimplePlayer using the SpDumbTacticalMoveGenerator */
class SpDumbTacticalPlayer
    : public SpSimplePlayer
{
public:
    SpDumbTacticalPlayer(GoBoard& board)
        : SpSimplePlayer(board, new SpDumbTacticalMoveGenerator(board))
    { }
    
    std::string Name() const
    {
        return "DumbTactical";
    }
};

//----------------------------------------------------------------------------

#endif // SP_DUMBTACTICALPLAYER_H

