//----------------------------------------------------------------------------
/** @file SgRect.h
    Rectangle on the Go board.
*/
//----------------------------------------------------------------------------

#ifndef SG_RECT_H
#define SG_RECT_H

#include "SgPoint.h"

//----------------------------------------------------------------------------

/** Rectangle on the Go board. 
    top < bottom, coordinates grow from top to bottom.
*/
class SgRect
{
public:
    SgRect();
    
    SgRect(int left, int right, int top, int bottom)
    :   m_left(left),
        m_right(right),
        m_top(top),
        m_bottom(bottom)
    { }
    
    SgRect(const SgPoint& topleft, const SgPoint& bottomright)
    :   m_left(SgPointUtil::Col(topleft)),
        m_right(SgPointUtil::Col(bottomright)),
        m_top(SgPointUtil::Row(topleft)),
        m_bottom(SgPointUtil::Row(bottomright))
    { }

    bool operator==(const SgRect& rhs) const
    {
        return m_left == rhs.m_left
            && m_right == rhs.m_right
            && m_top == rhs.m_top
            && m_bottom == rhs.m_bottom;
    }

    void Set(int left, int right, int top, int bottom)
    {
        m_left = left;
        m_right = right;
        m_top = top;
        m_bottom = bottom;
    }
    
    void MirrorX(int boardSize)
    {
        int temp = m_left;
        m_left   = boardSize + 1 - m_right;
        m_right  = boardSize + 1 - temp;
    }

    void MirrorY(int boardSize)
    {
        int temp = m_top;
        m_top    = boardSize + 1 - m_bottom;
        m_bottom = boardSize + 1 - temp;
    }
    
    void SwapXY()
    {
        std::swap(m_top, m_left);
        std::swap(m_bottom, m_right);
    }
    
    void Include(SgPoint p);

    void Include(const SgRect& rect);

    void Intersect(const SgRect& rect);

    bool IsEmpty() const
    {
        // no points inside, see constructor
        return m_left > m_right;
    }

    bool InRect(SgPoint p) const;

    SgPoint Center() const;

    bool Contains(SgPoint p) const
    {
        return InRect(p);
    }

    bool Contains(const SgRect& rect) const;

    bool Overlaps(const SgRect& rect) const;
    
    void Expand(int margin);
    
    int Left() const
    {
        return m_left;
    }
    
    int Right() const
    {
        return m_right;
    }
    
    int Top() const
    {
        return m_top;
    }
    
    int Bottom() const
    {
        return m_bottom;
    }
    
    int Width() const
    {
        SG_ASSERT(! IsEmpty());
        return m_right - m_left + 1;
    }

    int Height() const
    {
        SG_ASSERT(! IsEmpty());
        return m_bottom - m_top + 1;
    }
    
    int Area() const
    {
        return Width() * Height();
    }
    
    void IncLeft()
    {
        ++m_left;
    }
    
    void DecRight()
    {
        --m_right;
    }
    
    void IncTop()
    {
        ++m_top;
    }
    
    void DecBottom()
    {
        --m_bottom;
    }
    
    void SetLeft(int value)
    {
        m_left = value;
    }
    
    void SetRight(int value)
    {
        m_right = value;
    }
    
    void SetTop(int value)
    {
        m_top = value;
    }
    
    void SetBottom(int value)
    {
        m_bottom = value;
    }
    
private:
    int m_left;

    int m_right;
    
    int m_top;

    int m_bottom;
};

//----------------------------------------------------------------------------

/** Iterator for rectangle on the Go board, based on SgRect object
*/
class SgRectIterator
{
public:

    SgRectIterator(const SgRect& rect)
    :   m_rect(rect), 
        m_cursor(SgPointUtil::Pt(rect.Left(), rect.Top())),
        m_end(SgPointUtil::Pt(rect.Right(), rect.Bottom()))
    { }

    /** Iterate through rectangle: left to right, top to bottom */
    void operator++()
    {
        SG_ASSERT(m_rect.Contains(m_cursor));
        if (SgPointUtil::Col(m_cursor) == m_rect.Right())
            m_cursor += SG_NS + m_rect.Left() - m_rect.Right();
        else
            m_cursor += SG_WE;
    }

    /** Return the value of the current element. */
    SgPoint operator*() const
    {
        return m_cursor;
    }

    /** Return true if iteration is valid, otherwise false. */
    operator bool() const
    {
        return m_cursor <= m_end;
    }
    
private:
    const SgRect& m_rect;

    SgPoint m_cursor;

    SgPoint m_end;
};

std::ostream& operator<<(std::ostream& stream, const SgRect& rect);

//----------------------------------------------------------------------------

#endif // SG_RECT_H
