//----------------------------------------------------------------------------
/** @file SgRect.cpp
    See SgRect.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgRect.h"

#include <iostream>
#include <limits>

using namespace std;

//----------------------------------------------------------------------------

SgRect::SgRect()
    : m_left(numeric_limits<int>::max()),
      m_right(numeric_limits<int>::min()),
      m_top(numeric_limits<int>::max()),
      m_bottom(numeric_limits<int>::min())
{ }

std::ostream& operator<<(std::ostream& stream, const SgRect& rect)
{
    stream << "((" <<  rect.Left()  << ',' << rect.Top()  << "),"
           << '(' <<   rect.Right() << ',' << rect.Bottom() << "))";
    return stream;
}

void SgRect::Include(SgPoint p)
{
    int x = SgPointUtil::Col(p);
    int y = SgPointUtil::Row(p);
    if (x < m_left)
        m_left = x;
    if (x > m_right)
        m_right = x;
    if (y < m_top)
        m_top = y;
    if (y > m_bottom)
        m_bottom = y;
}

void SgRect::Include(const SgRect& rect)
{
    if (rect.m_left < m_left)
        m_left = rect.m_left;
    if (rect.m_right > m_right)
        m_right = rect.m_right;
    if (rect.m_top < m_top)
        m_top = rect.m_top;
    if (rect.m_bottom > m_bottom)
        m_bottom = rect.m_bottom;
}

void SgRect::Intersect(const SgRect& rect)
{
    m_left = max(m_left, rect.m_left);
    m_right = min(m_right, rect.m_right);
    m_top = max(m_top, rect.m_top);
    m_bottom = min(m_bottom, rect.m_bottom);
}

SgPoint SgRect::Center() const
{
    SG_ASSERT(! IsEmpty());
    return SgPointUtil::Pt((m_left + m_right) / 2, (m_top + m_bottom) / 2);
}

bool SgRect::InRect(SgPoint p) const
{
    //AR: if (BorderSet[p]) return false;
    int x = SgPointUtil::Col(p);
    int y = SgPointUtil::Row(p);
    return (x >= m_left) && (x <= m_right) && (y >= m_top) && (y <= m_bottom);
}

bool SgRect::Contains(const SgRect& rect) const
{
    return (m_left <= rect.m_left) && (m_right >= rect.m_right)
        && (m_top <= rect.m_top) && (m_bottom >= rect.m_bottom);
}

bool SgRect::Overlaps(const SgRect& rect2) const
{
    return (((m_left >= rect2.m_left) && (m_left <= rect2.m_right))
            || ((rect2.m_left >= m_left) && (rect2.m_left <= m_right)))
        && (((m_top >= rect2.m_top) && (m_top <= rect2.m_bottom))
            || ((rect2.m_top >= m_top) && (rect2.m_top <= m_bottom)));
}

void SgRect::Expand(int margin)
{
    m_left -= margin;
    m_right += margin;
    m_top -= margin;
    m_bottom += margin;
}

//----------------------------------------------------------------------------

