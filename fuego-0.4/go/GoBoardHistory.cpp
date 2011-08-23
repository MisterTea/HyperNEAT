//----------------------------------------------------------------------------
/** @file GoBoardHistory.cpp
    See GoBoardHistory.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoBoardHistory.h"

using namespace std;

//----------------------------------------------------------------------------

void GoBoardHistory::SetFromBoard(const GoBoard& bd)
{
    m_boardSize = bd.Size();
    m_rules = bd.Rules();
    m_setup = bd.Setup();
    m_moves.Clear();
    for (int i = 0; i < bd.MoveNumber(); ++i)
        m_moves.PushBack(bd.Move(i));
    m_toPlay = bd.ToPlay();
}

bool GoBoardHistory::IsAlternatePlayFollowUpOf(const GoBoardHistory& other,
                                               vector<SgPoint>& sequence)
{
    if (m_boardSize != other.m_boardSize
        || m_rules != other.m_rules
        || m_setup != other.m_setup
        || m_moves.Length() < other.m_moves.Length())
        return false;
    for (int i = 0; i < other.m_moves.Length(); ++i)
        if (m_moves[i] != other.m_moves[i])
            return false;
    sequence.clear();
    SgBlackWhite toPlay = other.m_toPlay;
    for (int i = other.m_moves.Length(); i < m_moves.Length(); ++i)
    {
        GoPlayerMove m = m_moves[i];
        if (m.Color() != toPlay)
            return false;
        sequence.push_back(m.Point());
        toPlay = SgOppBW(toPlay);
    }
    if (toPlay != m_toPlay)
        return false;
    return true;
}

//----------------------------------------------------------------------------
