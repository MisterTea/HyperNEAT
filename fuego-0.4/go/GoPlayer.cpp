//----------------------------------------------------------------------------
/** @file GoPlayer.cpp
    See GoPlayer.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoPlayer.h"

#include <limits>
#include "GoBoard.h"

using namespace std;

//----------------------------------------------------------------------------

GoPlayer::GoPlayer(const GoBoard& bd)
    : GoBoardSynchronizer(bd),
      m_currentNode(0),
      m_bd(bd.Size(), GoSetup(), bd.Rules()),
      m_variant(0)
{
    SetSubscriber(m_bd);
}

GoPlayer::~GoPlayer()
{
}

int GoPlayer::MoveValue(SgPoint p)
{
    SG_UNUSED(p);
    return numeric_limits<int>::min();
}

std::string GoPlayer::Name() const
{
    return "Unknown";
}

void GoPlayer::OnGameFinished()
{
}

void GoPlayer::OnNewGame()
{
}

void GoPlayer::Ponder()
{
}

//----------------------------------------------------------------------------

