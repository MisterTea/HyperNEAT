//----------------------------------------------------------------------------
/** @file SgSearchValue.h
    Scalar values used in search tasks derived from class SgSearch.
*/
//----------------------------------------------------------------------------

#ifndef SG_SEARCHVALUE_H
#define SG_SEARCHVALUE_H

#include <cstdlib>
#include <string>
#include "SgBlackWhite.h"
#include "SgSearch.h"

//----------------------------------------------------------------------------

/** Value used in search tasks derived from class SgSearch.
    There's a range of values that indicate that the problem has been
    solved (at a certain depth), a range of values for solutions that involve
    ko, and a range of values for problems that have not been solved yet.
    Value is always stored with positive values being good for black, negative
    values being good for white.
    <pre>
    int v = Board().ToPlay() == SG_WHITE ? -value : +value;
    </pre>
*/
class SgSearchValue
{
public:
    enum {

        MAX_LEVEL = 125,

        /** The best possible search result - highest possible value.
        A win in n ply is encoded with a value of MAX_VALUE - n.
        A loss is encoded as -value if the win would be encoded as value.
        MAX_VALUE==32000.
        */
        MAX_VALUE = MAX_LEVEL * SgSearch::MAX_DEPTH,
        
        /** The worst possible search result. 
            All search results are in the range [MIN_VALUE..MAX_VALUE].
        */
        MIN_VALUE = -MAX_VALUE,

        /** The maximum number of Ko recaptures allowed in a search. */
        MAX_KO_LEVEL = 3,

        /** KO_VALUE is the result for win by ko.
            Similarly with MAX_VALUE, a win by Ko in n ply is encoded as 
            KO_VALUE - n.
            KO_VALUE==32000 - 256 = 31744.
        */
        KO_VALUE = MAX_VALUE - SgSearch::MAX_DEPTH,

        /** MIN_PROVEN_VALUE is the lowest score that indicates a proven win.
            1. values in the range [MIN_VALUE..-MIN_PROVEN_VALUE] are 
               proven losses (possibly by ko).
            2. values in the range [-MIN_PROVEN_VALUE+1..MIN_PROVEN_VALUE-1] 
               are heuristic scores.
            3. values in range [MIN_PROVEN_VALUE..MAX_VALUE] 
               are proven wins (possibly by ko).
        */
        MIN_PROVEN_VALUE = MAX_VALUE
                               - (MAX_KO_LEVEL + 1) * SgSearch::MAX_DEPTH
    };

    SgSearchValue();

    explicit SgSearchValue(int v);

    SgSearchValue(SgBlackWhite goodForPlayer, int depth);

    SgSearchValue(SgBlackWhite goodForPlayer, int depth, int koLevel);

    /** Return current value as an integer. */
    operator int() const;

    int Depth() const;

    /** Convert 'string' to a value and set this value.
        Return true if the string could be converted to a valid value,
        otherwise false.
    */
    bool FromString(const std::string& s);

    bool IsEstimate() const;

    bool IsKoValue() const;

    bool IsPositive() const;

    /** is value == +MAX_VALUE or -MAX_VALUE? */
    static inline bool IsSolved(int value);

    /** return +MAX_VALUE for win,  -MAX_VALUE for loss */
    static inline bool SolvedValue(bool isWin);
    
    bool IsSureValue() const;

    int KoLevel() const;

    void SetValueForPlayer(SgBlackWhite player);

    int ValueForBlack() const;

    int ValueForPlayer(SgBlackWhite player) const;

    int ValueForWhite() const;

    /** Set '*s' to the string for this value, e.g. "B+3.5", "W+20",
        or "W+(ko)[12]". The value is divided by 'unitPerPoint' to determine
        the number of points.
    */
    std::string ToString(int unitPerPoint = 1) const;

private:
    int m_value;
};

inline SgSearchValue::SgSearchValue()
    : m_value(0)
{ }

inline SgSearchValue::SgSearchValue(int v)
    : m_value(v)
{
    SG_ASSERT(-MAX_VALUE <= v && v <= MAX_VALUE);
}

inline SgSearchValue::SgSearchValue(SgBlackWhite goodForPlayer, int depth)
    : m_value(MAX_VALUE - depth)
{
    SG_ASSERT_BW(goodForPlayer);
    SG_ASSERT(0 <= depth && depth < SgSearch::MAX_DEPTH);
    SetValueForPlayer(goodForPlayer);
    // Make sure value gets encoded/decoded consistently.
    SG_ASSERT(KoLevel() == 0);
    SG_ASSERT(Depth() == depth);
}

inline SgSearchValue::SgSearchValue(SgBlackWhite goodForPlayer, int depth, int koLevel)
    : m_value(MAX_VALUE - depth - koLevel * SgSearch::MAX_DEPTH)
{
    SG_ASSERT_BW(goodForPlayer);
    SG_ASSERT(0 <= depth && depth < SgSearch::MAX_DEPTH);
    SG_ASSERT(0 <= koLevel && koLevel <= MAX_KO_LEVEL);
    SetValueForPlayer(goodForPlayer);
    // Make sure value gets encoded/decoded consistently.
    SG_ASSERT(KoLevel() == koLevel);
    SG_ASSERT(Depth() == depth);
}

inline SgSearchValue::operator int() const
{
    return m_value;
}

inline int SgSearchValue::Depth() const
{
    if (IsEstimate())
        return 0;
    else return (SgSearch::MAX_DEPTH - 1) 
              - (std::abs(m_value) - 1) % SgSearch::MAX_DEPTH;
}

inline bool SgSearchValue::IsEstimate() const
{
    return -MIN_PROVEN_VALUE < m_value && m_value < MIN_PROVEN_VALUE;
}

inline bool SgSearchValue::IsKoValue() const
{
    return IsSureValue() && -KO_VALUE < m_value && m_value < KO_VALUE;
}

inline bool SgSearchValue::IsPositive() const
{
    return 0 <= m_value;
}

inline bool SgSearchValue::IsSureValue() const
{
    return m_value <= -MIN_PROVEN_VALUE || MIN_PROVEN_VALUE <= m_value;
}

inline bool SgSearchValue::IsSolved(int value)
{
    return abs(value) == MAX_VALUE;
}

inline bool SgSearchValue::SolvedValue(bool isWin)
{
    return isWin ? +MAX_VALUE : -MAX_VALUE;
}

inline void SgSearchValue::SetValueForPlayer(SgBlackWhite player)
{
    if (player == SG_WHITE)
        m_value = -m_value;
}

inline int SgSearchValue::ValueForBlack() const
{
    return +m_value;
}

inline int SgSearchValue::ValueForPlayer(SgBlackWhite player) const
{
    SG_ASSERT_BW(player);
    return player == SG_WHITE ? -m_value : +m_value;
}

inline int SgSearchValue::ValueForWhite() const
{
    return -m_value;
}

//----------------------------------------------------------------------------

#endif // SG_SEARCHVALUE_H
