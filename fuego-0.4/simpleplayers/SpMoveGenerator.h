//----------------------------------------------------------------------------
/** @file SpMoveGenerator.h
    Simple move generator for simple players
*/
//----------------------------------------------------------------------------

#ifndef SP_MOVEGENERATOR_H
#define SP_MOVEGENERATOR_H

#include "GoBoard.h"

class SgEvaluatedMoves;

//----------------------------------------------------------------------------

/** Simple move generators.
    Either static or 1-ply lookahead
*/
class SpMoveGenerator
{
public:
    explicit SpMoveGenerator(GoBoard& board)
        : m_board(board)
    { }

    virtual ~SpMoveGenerator() { }

    virtual void GenerateMoves(SgEvaluatedMoves& eval, SgBlackWhite toPlay);
    
    /** Execute move if necessary, and then score */
    virtual int EvaluateMove(SgPoint p) = 0;

protected:
    /** Reference to go board on which to play */
    GoBoard& m_board;

    /** Not implemented */
    SpMoveGenerator(const SpMoveGenerator&);

    /** Not implemented */
    SpMoveGenerator& operator=(const SpMoveGenerator&);
};

/** Statically assigns a value to a given move */
class SpStaticMoveGenerator
    : public SpMoveGenerator
{
public:
    explicit SpStaticMoveGenerator(GoBoard& board)
        : SpMoveGenerator(board)
    { }
    
    virtual int Score(SgPoint p) = 0;

    /** Static move evaluation */
    int EvaluateMove(SgPoint p);
};

/** Evaluates move by executing it, then evaluating the board */
class Sp1PlyMoveGenerator
    : public SpMoveGenerator
{
public:
    explicit Sp1PlyMoveGenerator(GoBoard& board)
        : SpMoveGenerator(board)
    { }
    
    virtual int Evaluate() = 0;

    /** Execute move, then evaluate board */
    int EvaluateMove(SgPoint p);
};

//----------------------------------------------------------------------------

#endif // SP_MOVEGENERATOR_H

