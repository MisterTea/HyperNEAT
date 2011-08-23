//----------------------------------------------------------------------------
/** @file SgUtil.h
    Small utility functions
*/
//----------------------------------------------------------------------------

#ifndef SG_UTIL_H
#define SG_UTIL_H

#include <iostream>
#include "SgBlackWhite.h"
#include "SgBWArray.h"
//----------------------------------------------------------------------------

namespace SgUtil {

template<class T>
inline void ForceInRange(const T& min, T* p, const T& max)
{
    if (*p < min)
        *p = min;
    if (*p > max)
        *p = max;
}

template <class T>
inline bool InRange(const T& i, const T& from, const T& to) 
{
    return (i >= from) && (i <= to);
}

template<class T>
inline void LowerLimit(T& x, const T& limit)
{
    if (x < limit)
        x = limit;
}

template<class T>
inline void UpperLimit(T& x, const T& limit)
{
    if (x > limit)
        x = limit;
}

} // namespace SgUtil

/** Utility class to assure balance between black and white plays.
    The difference between the number of plays by both colors
    is forced to be within the margin.
*/
class SgBalancer
{
public:
    SgBalancer(int margin) : 
        m_balance(0),
        m_nuCalls(0),
        m_margin(margin),
        m_played(0,0),
        m_rejected(0,0)
    { }
    
    bool Play(SgBlackWhite color)
    {
        SG_ASSERT(SgIsBlackWhite(color));
        ++m_nuCalls;
        if (color == SG_BLACK)
        {
            if (m_balance < m_margin)
            {
                ++m_balance;
                ++m_played[SG_BLACK];
                return true;
            }
        }
        else if (m_balance > -m_margin)
        {
            --m_balance;
            ++m_played[SG_WHITE];
            return true;
        }
        ++m_rejected[color];
        return false;
    }
    
    int Balance() const
    {
        return m_balance;
    }
    
    int Margin() const
    {
        return m_margin;
    }
    
    int NuCalls() const
    {
        return m_nuCalls;
    }
    
    int NuPlayed(SgBlackWhite color) const
    {
        return m_played[color];
    }
    
    int NuRejected(SgBlackWhite color) const
    {
        return m_rejected[color];
    }
    
private:
    int m_balance;
    int m_nuCalls;
    const int m_margin;
    SgBWArray<int> m_played;
    SgBWArray<int> m_rejected;
};

//----------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& stream, const SgBalancer& balancer);

//----------------------------------------------------------------------------

#endif // SG_UTIL_H

