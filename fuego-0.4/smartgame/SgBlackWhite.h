//----------------------------------------------------------------------------
/** @file SgBlackWhite.h
    Color of player in two-player games (black/white).
*/
//----------------------------------------------------------------------------

#ifndef SG_BLACKWHITE_H
#define SG_BLACKWHITE_H

#include <boost/static_assert.hpp>

//----------------------------------------------------------------------------

/** Black stone, black player. */
const int SG_BLACK = 0;

/** White stone, white player. */
const int SG_WHITE = 1;

// must be consecutive for color for-loops
BOOST_STATIC_ASSERT(SG_BLACK + 1 == SG_WHITE);

/** SG_BLACK or SG_WHITE */
typedef int SgBlackWhite;

inline bool SgIsBlackWhite(int c)
{
    return c == SG_BLACK || c == SG_WHITE;
}

#define SG_ASSERT_BW(c) SG_ASSERT(SgIsBlackWhite(c))

inline SgBlackWhite SgOppBW(SgBlackWhite c)
{
    SG_ASSERT_BW(c);
    return SG_BLACK + SG_WHITE - c;
}

inline char SgBW(SgBlackWhite color)
{
    SG_ASSERT_BW(color);
    return color == SG_BLACK ? 'B' : 'W';
}

//----------------------------------------------------------------------------

/** Iterator over both colors, Black and White.
    The function Opp() returns the opponent since this is often needed too.

    Usage example:
    @verbatim
    for (SgBWIterator it; it; ++it)
    { 
        "this section will be executed twice:"
        "first with *it == SG_BLACK, then with *it == SG_WHITE"
        (unless it encounters a break or return inside)
    }
    @endverbatim
*/
class SgBWIterator
{
public:
    SgBWIterator()
        : m_color(SG_BLACK)
    { }

    /** Advance the state of the iteration to the next element. */
    void operator++()
    {
        SG_ASSERT_BW(m_color);
        ++m_color;
    }

    /** Return the value of the current element. */
    SgBlackWhite operator*() const
    {
        return m_color;
    }

    /** Return the value of the current element. */
    SgBlackWhite Opp() const
    {
        return SgOppBW(m_color);
    }

    /** Return true if iteration is valid, otherwise false. */
    operator bool() const
    {
        return m_color <= SG_WHITE;
    }

private:
    int m_color;

    /** Not implemented */
    SgBWIterator(const SgBWIterator&);

    /** Not implemented */
    SgBWIterator& operator=(const SgBWIterator&);
};

//----------------------------------------------------------------------------

#endif // SG_BLACKWHITE_H
