//----------------------------------------------------------------------------
/** @file GoSearch.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoSearch.h"

#include <sstream>
#include "GoBoard.h"
#include "GoBoardUtil.h"
#include "SgNode.h"
#include "SgPoint.h"
#include "SgWrite.h"

using namespace std;

//----------------------------------------------------------------------------

GoSearch::GoSearch(GoBoard& board, SgSearchHashTable* hash)
    : SgSearch(hash),
      m_board(board)
{
    SetOpponentBest(true);
}

bool GoSearch::CheckDepthLimitReached() const
{
    return false;
}

bool GoSearch::EndOfGame() const
{
    return PrevMove() == SG_PASS && PrevMove2() == SG_PASS;
}

bool GoSearch::Execute(SgMove move, int* delta, int depth)
{
    SG_UNUSED(delta);
    SG_UNUSED(depth);
    GoBoard& bd = Board();
    SgBlackWhite toPlay = bd.ToPlay();
    if (! GoBoardUtil::PlayIfLegal(bd, move, toPlay))
        return false;
    return true;
}

SgHashCode GoSearch::GetHashCode() const
{
    return Board().GetHashCodeInclToPlay();
}

SgBlackWhite GoSearch::GetToPlay() const
{
    return Board().ToPlay();
}

std::string GoSearch::MoveString(SgMove move) const
{
    ostringstream buffer;
    buffer << SgWritePoint(move);
    return buffer.str();
}

void GoSearch::SetToPlay(SgBlackWhite toPlay)
{
    Board().SetToPlay(toPlay);
}

void GoSearch::TakeBack()
{
    Board().Undo();
}

//----------------------------------------------------------------------------

