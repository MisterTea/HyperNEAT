//----------------------------------------------------------------------------
/** @file SpMaxEyePlayer.h
    Plays to complete simple eyes as quickly as possible, or preventing
    opponent from doing so.
*/
//----------------------------------------------------------------------------

#ifndef SP_MAXEYEPLAYER_H
#define SP_MAXEYEPLAYER_H

//----------------------------------------------------------------------------

#include "SpSimplePlayer.h"
#include "SpMoveGenerator.h"

//----------------------------------------------------------------------------

/** Tries to maximize simple eye score of any point.
*/
class SpMaxEyeMoveGenerator
    : public Sp1PlyMoveGenerator
{
public:
    explicit SpMaxEyeMoveGenerator(GoBoard& board, bool eyego = false)
        : Sp1PlyMoveGenerator(board), m_eyeGo(eyego)
    { }

    int Evaluate();
    
    float Heuristic(SgPoint p, SgBlackWhite color);
    
private:
    bool m_eyeGo;
};

//----------------------------------------------------------------------------

/** Simple player using SpMaxEyeMoveGenerator */
class SpMaxEyePlayer
    : public SpSimplePlayer
{
public:
    SpMaxEyePlayer(GoBoard& board, bool eyego = false)
        : SpSimplePlayer(board, new SpMaxEyeMoveGenerator(board, eyego))
    { }

    std::string Name() const
    {
        return "MaxEye";
    }    
};

//----------------------------------------------------------------------------

#endif // SP_MAXEYEPLAYER_H

