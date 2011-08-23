//----------------------------------------------------------------------------
/** @file SpSimplePlayer.cpp
    See SpSimplePlayer.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SpSimplePlayer.h"

#include <limits>
#include <cstdio>
#include <iostream>
#include <string>
#include "GoSafetySolver.h"
#include "SgDebug.h"
#include "SgEvaluatedMoves.h"
#include "SgNode.h"
#include "SgWrite.h"
#include "SpCapturePlayer.h"
#include "SpMoveGenerator.h"
#include "SpRandomPlayer.h"
#include "SpUtil.h"

//----------------------------------------------------------------------------

SpSimplePlayer::SpSimplePlayer(GoBoard& board, SpMoveGenerator* generator)
    : GoPlayer(board),
      m_generator(generator),
      m_randomGenerator(new SpRandomMoveGenerator(board))
{
}

SpSimplePlayer::~SpSimplePlayer()
{
    SG_ASSERT(m_generator);
    SG_ASSERT(m_randomGenerator);
    delete m_generator;
    m_generator = 0;
    delete m_randomGenerator;
    m_randomGenerator = 0;
}

int SpSimplePlayer::MoveValue(SgPoint p)
{
    SgPointSet relevant = Board().AllEmpty();
    if (UseFilter())
    {
        GoSafetySolver solver(Board());
        SgBWSet safe;
        solver.FindSafePoints(&safe);
        relevant -= safe[SG_BLACK];
        relevant -= safe[SG_WHITE];
    }
    if (relevant[p])
    {
        return m_generator->EvaluateMove(p);
    }
    return std::numeric_limits<int>::min();
}

SgPoint SpSimplePlayer::GenMove(const SgTimeRecord& time, SgBlackWhite toPlay)
{
    SG_UNUSED(time);
    SgPointSet relevant =
        SpUtil::GetRelevantMoves(Board(), toPlay, UseFilter());
    // Generate moves.
    SgEvaluatedMovesArray moves(relevant);
    
    // note: generators can disable certain unwanted moves by calling
    // Disable(p)
    m_generator->GenerateMoves(moves, toPlay);
    // Generate random moves if no other found
    if ((moves.BestMove() == SG_PASS) && m_randomGenerator)
        m_randomGenerator->GenerateMoves(moves, toPlay);
    return moves.BestMove();
}

//----------------------------------------------------------------------------

