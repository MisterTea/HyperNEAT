//----------------------------------------------------------------------------
/** @file GoPlayerMove.h */
//----------------------------------------------------------------------------

#ifndef GO_PLAYERMOVE_H
#define GO_PLAYERMOVE_H

#include <iosfwd>
#include "SgBlackWhite.h"
#include "SgPoint.h"

//----------------------------------------------------------------------------

/** Move and color of player who plays it. */
class GoPlayerMove
{
public:
    GoPlayerMove(SgBlackWhite color, SgPoint point = SG_NULLMOVE);

    GoPlayerMove();

    ~GoPlayerMove();

    bool operator==(const GoPlayerMove& move) const;

    bool operator!=(const GoPlayerMove& move) const;

    SgPoint Point() const;

    SgBlackWhite Color() const;

    void SetPoint(SgPoint move);

private:
    SgBlackWhite  m_color;

    SgPoint m_point;
};

inline GoPlayerMove::GoPlayerMove()
    : m_color(SG_BLACK),
      m_point(SG_NULLMOVE)
{
}

inline GoPlayerMove::GoPlayerMove(SgBlackWhite color, SgPoint point)
    : m_color(color),
      m_point(point)
{
    SG_ASSERT_BW(color);
}

inline GoPlayerMove::~GoPlayerMove()
{
}

inline bool GoPlayerMove::operator==(const GoPlayerMove& move) const
{
    return (m_color == move.m_color && m_point == move.m_point);
}

inline bool GoPlayerMove::operator!=(const GoPlayerMove& move) const
{
    return ! operator==(move);
}

inline SgPoint GoPlayerMove::Point() const
{
    return m_point;
}

inline void GoPlayerMove::SetPoint(SgPoint point)
{
    m_point = point;
}

inline SgBlackWhite GoPlayerMove::Color() const
{
    return m_color;
}

//----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, GoPlayerMove mv);

//----------------------------------------------------------------------------

#endif // GO_PLAYERMOVE_H
