//----------------------------------------------------------------------------
/** @file SgBWSet.h
    Pair of SgPointSet for Black and White
*/
//----------------------------------------------------------------------------

#ifndef SG_BWSET_H
#define SG_BWSET_H

#include "SgPointSet.h"
#include "SgBWArray.h"
#include "SgPointSetUtil.h"

//----------------------------------------------------------------------------

/** Pair of SgPointSet's indexed by color SG_BLACK, SG_WHITE */
class SgBWSet
{
public:
    SgBWSet()
    {
    }

    SgBWSet(const SgPointSet& black, const SgPointSet& white)
        : m_set(black, white)
    {
    }

    const SgPointSet& operator[](SgBlackWhite c) const
    {
        return m_set[c];
    }

    SgPointSet& operator[](SgBlackWhite c)
    {
        return m_set[c];
    }

    bool operator==(const SgBWSet& other) const;

    bool operator!=(const SgBWSet& other) const;

    SgBWSet& operator|=(const SgBWSet& other);

    void Clear()
    {
        m_set[SG_BLACK].Clear();
        m_set[SG_WHITE].Clear();
    }

    bool BothEmpty() const
    {
        // not called IsEmpty to avoid possible confusion with
        // test on single SgPointSet
        return m_set[SG_BLACK].IsEmpty() && m_set[SG_WHITE].IsEmpty();
    }

    bool Disjoint() const
    {
        return m_set[SG_BLACK].Disjoint(m_set[SG_WHITE]);
    }

    void AssertDisjoint() const
    {
        SG_ASSERT(Disjoint());
    }

    SgPointSet Both() const
    {
        return m_set[SG_BLACK] | m_set[SG_WHITE];
    }

    bool OneContains(SgPoint p) const
    {
        return m_set[SG_BLACK].Contains(p) 
            || m_set[SG_WHITE].Contains(p);
    }

    bool BothContain(SgPoint p) const
    {
        return m_set[SG_BLACK].Contains(p)
            && m_set[SG_WHITE].Contains(p);
    }

private:
    SgBWArray<SgPointSet> m_set;
};

inline bool SgBWSet::operator==(const SgBWSet& other) const
{
    return (m_set[SG_BLACK] == other.m_set[SG_BLACK]
            && m_set[SG_WHITE] == other.m_set[SG_WHITE]);
}

inline bool SgBWSet::operator!=(const SgBWSet& other) const
{
    return ! operator==(other);
}

inline SgBWSet& SgBWSet::operator|=(const SgBWSet& other)
{
    m_set[SG_BLACK] |= other.m_set[SG_BLACK];
    m_set[SG_WHITE] |= other.m_set[SG_WHITE];
    return (*this);
}

inline std::ostream& operator<<(std::ostream& out, const SgBWSet& set)
{
    out << set[SG_BLACK] << set[SG_WHITE];
    return out;
}

//----------------------------------------------------------------------------

#endif // SG_BWSET_H
