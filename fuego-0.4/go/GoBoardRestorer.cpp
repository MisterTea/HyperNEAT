//----------------------------------------------------------------------------
/** @file GoBoardRestorer.cpp
    See GoBoardRestorer.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoBoardRestorer.h"

#include "GoBoardUtil.h"

using namespace std;

//----------------------------------------------------------------------------

GoBoardRestorer::GoBoardRestorer(GoBoard& bd)
    : m_bd(bd),
      m_size(bd.Size()),
      m_rules(bd.Rules())
{
    for (int i = 0; i < bd.MoveNumber(); ++i)
        m_moves.PushBack(bd.Move(i));
}

GoBoardRestorer::~GoBoardRestorer()
{
    GoBoardUtil::UndoAll(m_bd);
    if (m_bd.Size() != m_size)
        m_bd.Init(m_size);
    for (MoveList::Iterator it(m_moves); it; ++it)
        m_bd.Play(*it);
}

//----------------------------------------------------------------------------
