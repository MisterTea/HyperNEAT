//----------------------------------------------------------------------------
/** @file SgPointSet.h
    Sets of points on the board.
*/
//----------------------------------------------------------------------------

#ifndef SG_POINTSET_H
#define SG_POINTSET_H

#include <algorithm>
#include <bitset>
#include <cstring>
#include <iosfwd>
#include <memory>
#include "SgArray.h"
#include "SgPoint.h"
#include "SgRect.h"
#include "SgVector.h"

//----------------------------------------------------------------------------

/** Set of points.
    Represents a set of points on the Go board. This class is efficient for
    bit-level operations on the board as a whole.
*/
class SgPointSet
{
public:
    SgPointSet();

    ~SgPointSet();

    explicit SgPointSet(const SgVector<SgPoint>& vector);

    SgPointSet& operator-=(const SgPointSet& other);

    SgPointSet& operator&=(const SgPointSet& other);

    SgPointSet& operator|=(const SgPointSet& other);

    SgPointSet& operator^=(const SgPointSet& other);

    bool operator==(const SgPointSet& other) const;

    bool operator!=(const SgPointSet& other) const;

    /** Return whether point 'p' is set. */
    bool operator[](SgPoint p) const;
    
    bool Disjoint(const SgPointSet& s) const;
    
    /** Test if this contains a Point adjacent to a Point in s */
    bool Adjacent(const SgPointSet& s) const;

    bool Adjacent8To(SgPoint p) const;
    
    /** Test if all points adjacent to this are contained in s */
    bool AdjacentOnlyTo(const SgPointSet& s, int boardSize) const;

    bool AdjacentTo(SgPoint p) const;
    
    /** Test if all Points in this are adjacent to some Point in s */
    bool AllAdjacentTo(const SgPointSet& s) const;

    static const SgPointSet& AllPoints(int boardSize);

    int Size() const;

    bool IsSize(int size) const;

    bool MinSetSize(int size) const;

    bool MaxSetSize(int size) const;

    bool IsEmpty() const;

    bool NonEmpty() const;

    /** 4-Neighbor points of set */
    SgPointSet Border(int boardSize) const;

    /** 8-Neighbor points of set */
    SgPointSet Border8(int boardSize) const;

    /** Compute border without clipping to board size. */
    SgPointSet BorderNoClip() const;

    /** SgPoint as close to the center of set as possible */
    SgPoint Center() const;

    /** Return whether point 'p' is set. */
    bool CheckedContains(SgPoint p, bool doRangeCheck = true,
                         bool onBoardCheck = false) const;
    
    SgPointSet& Clear();

    /** Compute connected component by iterative Border calculation.
        @note Slow for large diameter sets.
    */
    SgPointSet Component(SgPoint p) const;

    /** Good for small diameter sets */
    SgPointSet Component8(SgPoint p) const;

    /** Good for large diameter sets */
    SgPointSet ConnComp(SgPoint p) const;

    /** 8-neighbors */
    SgPointSet ConnComp8(SgPoint p) const;

    /** Check if contains point.
     Can be called with out-of-board points, otherwise use
     ContainsPoints().
     */
    bool Contains(SgPoint p) const;
    
    /** Check if contains point.
     Can only be called with on-board points, otherwise use Contains().
     */
    bool ContainsPoint(SgPoint p) const;
    
    SgRect EnclosingRect() const;
    
    SgPointSet& Exclude(SgPoint p);

    SgPointSet& Exclude(const SgVector<SgPoint>& vector);

    /** Include 4-neighbor points in set */
    void Grow(int boardSize);
    
    /** Returns newly added points */
    void Grow(SgPointSet* newArea, int boardSize);
    
    /** Include 8-neighbor points in set */
    void Grow8(int boardSize);

    SgPointSet& Include(SgPoint p);

    /** Whether set is connected or not.
        @return True if connected or set is empty.
        @note Slow for large diameters.
    */
    bool IsConnected() const;

    /** Whether set is connected or not. */
    bool Is8Connected() const;

    /** Points of set surrounded by set */
    SgPointSet Kernel(int boardSize) const;

    /** At most 'max' common points. */
    bool MaxOverlap(const SgPointSet& other, int max) const;
    
    /** At least 'min' common points. */
    bool MinOverlap(const SgPointSet& s, int min) const;
    
    bool NewMark(SgPoint p);
    
    bool Overlaps(const SgPointSet& other) const;
    
    /** First point of set.
        @return First (smallest) point of set or SG_NULLPOINT for empty set.
    */
    SgPoint PointOf() const;

    /** Is this set a subset of s? */
    bool SubsetOf(const SgPointSet& other) const;
    
    /** Is this set a superset of s? */
    bool SupersetOf(const SgPointSet& other) const;
    
    void Swap(SgPointSet& other) throw();
    
    SgPointSet& Toggle(SgPoint p);

    void ToVector(SgVector<SgPoint>* vector) const;

    void Write(std::ostream& out, int boardSize) const;
    
    /** Return whether point 'p' is close to a point in set.
        in implementation: const int MAX_CLOSE_DISTANCE = 3;
    */
    bool IsCloseTo(SgPoint p) const;
    
private:
    /** Precomputed point sets with all points depending on board size. */
    class PrecompAllPoints
    {
    public:
        PrecompAllPoints();

        const SgPointSet& Get(int boardSize)
        {
            SG_ASSERT(boardSize >= SG_MIN_SIZE);
            SG_ASSERT(boardSize <= SG_MAX_SIZE);
            return *m_allPoints[boardSize];
        }

    private:
        SgArray<std::auto_ptr<SgPointSet>,SG_MAX_SIZE + 1> m_allPoints;
    };

    friend class SgSetIterator;

    std::bitset<SG_MAXPOINT> m_a;

    static PrecompAllPoints s_allPoints;

    SgPointSet operator>>(int n) const;

    SgPointSet operator<<(int n) const;

};


/** Compute difference between point sets.
    @relatesalso SgPointSet
*/
inline SgPointSet operator-(const SgPointSet& L, const SgPointSet& R)
{
    return (SgPointSet(L) -= R);
}

/** Compute intersection between point sets.
    @relatesalso SgPointSet
*/
inline SgPointSet operator&(const SgPointSet& L, const SgPointSet& R)
{
    return (SgPointSet(L) &= R);
}

/** Compute union between point sets.
    @relatesalso SgPointSet
*/
inline SgPointSet operator|(const SgPointSet& L, const SgPointSet& R)
{
    return (SgPointSet(L) |= R);
}

/** Compute XOR between point sets.
    @relatesalso SgPointSet
*/
inline SgPointSet operator^(const SgPointSet& L, const SgPointSet& R)
{
    return (SgPointSet(L) ^= R);
}

inline SgPointSet::SgPointSet()
{
}

inline SgPointSet::~SgPointSet()
{
}

inline void SgPointSet::Swap(SgPointSet& other) throw()
{
    std::swap(m_a, other.m_a);
}

inline SgPointSet& SgPointSet::operator-=(const SgPointSet& other)
{
    m_a &= ~other.m_a;
    return (*this);
}

inline SgPointSet& SgPointSet::operator&=(const SgPointSet& other)
{
    m_a &= other.m_a;
    return (*this);
}

inline SgPointSet& SgPointSet::operator|=(const SgPointSet& other)
{
    m_a |= other.m_a;
    return (*this);
}

inline SgPointSet& SgPointSet::operator^=(const SgPointSet& other)
{
    m_a ^= other.m_a;
    return (*this);
}

inline bool SgPointSet::operator==(const SgPointSet& other) const
{
    return m_a == other.m_a;
}

inline bool SgPointSet::operator!=(const SgPointSet& other) const
{
    return m_a != other.m_a;
}

inline const SgPointSet& SgPointSet::AllPoints(int boardSize)
{
    return s_allPoints.Get(boardSize);
}

inline bool SgPointSet::Overlaps(const SgPointSet& other) const
{
    return (m_a & other.m_a).any();
}

inline bool SgPointSet::MaxOverlap(const SgPointSet& other, int max) const
{
    SgPointSet s(*this);
    s &= other;
    return s.Size() <= max;
}

inline bool SgPointSet::MinOverlap(const SgPointSet& s, int min) const
{
    return ! MaxOverlap(s, min - 1);
}

inline bool SgPointSet::Disjoint(const SgPointSet& s) const
{
    return ! Overlaps(s);
}

inline bool SgPointSet::AdjacentTo(SgPoint p) const
{
    SG_ASSERT_BOARDRANGE(p);
    return Contains(p + SG_NS)
        || Contains(p - SG_NS)
        || Contains(p + SG_WE)
        || Contains(p - SG_WE);
}

inline bool SgPointSet::Adjacent8To(SgPoint p) const
{
    SG_ASSERT_BOARDRANGE(p);
    return Contains(p + SG_NS) || Contains(p - SG_NS) || Contains(p + SG_WE)
        || Contains(p - SG_WE) || Contains(p + SG_NS + SG_WE)
        || Contains(p + SG_NS - SG_WE) || Contains(p - SG_NS + SG_WE)
        || Contains(p - SG_NS - SG_WE);
}

inline bool SgPointSet::SubsetOf(const SgPointSet& other) const
{
    return (m_a & ~other.m_a).none();
}

inline bool SgPointSet::SupersetOf(const SgPointSet& other) const
{
    return (other.m_a & ~m_a).none();
}

inline int SgPointSet::Size() const
{
    return static_cast<int>(m_a.count());
}

inline bool SgPointSet::IsEmpty() const
{
    return m_a.none();
}

inline bool SgPointSet::NonEmpty() const
{
    return ! IsEmpty();
}

inline SgPointSet& SgPointSet::Exclude(SgPoint p)
{
    SG_ASSERT_BOARDRANGE(p);
    m_a.reset(p); 
    return (*this);
}

inline SgPointSet& SgPointSet::Include(SgPoint p)
{
    SG_ASSERT_BOARDRANGE(p);
    m_a.set(p);
    return (*this);
}

inline SgPointSet& SgPointSet::Clear()
{
    m_a.reset();
    return *this;
}

inline SgPointSet& SgPointSet::Toggle(SgPoint p)
{
    SG_ASSERT(p < static_cast<int>(m_a.size()));
    m_a.flip(p);
    return (*this);
}

inline bool SgPointSet::Contains(SgPoint p) const
{
    return m_a.test(p);
}

inline bool SgPointSet::CheckedContains(SgPoint p, bool doRangeCheck,
                                        bool onBoardCheck) const
{
    if (doRangeCheck)
    {   
        if (onBoardCheck)
            SG_ASSERT_BOARDRANGE(p);
        else
            // 1 larger for nbiterators
            SG_ASSERTRANGE(p, SgPointUtil::Pt(0, 0),
                           SgPointUtil::Pt(SG_MAX_SIZE + 1, SG_MAX_SIZE + 1));
    }
    return m_a.test(p);
}

inline bool SgPointSet::ContainsPoint(SgPoint p) const
{
    return CheckedContains(p, true, true);
}

inline bool SgPointSet::operator[](SgPoint p) const
{
    return Contains(p);
}

inline bool SgPointSet::NewMark(SgPoint p)
{
    if (Contains(p))
        return false;
    else
    {
        Include(p);
        return true;
    }
}

inline SgPointSet SgPointSet::operator>>(int n) const
{
    SgPointSet result(*this);
    result.m_a >>= n;
    return result;
}

inline SgPointSet SgPointSet::operator<<(int n) const
{
    SgPointSet result(*this);
    result.m_a <<= n;
    return result;
}

//----------------------------------------------------------------------------

/** Iterator to iterate through 'set'.
    Set may contain only board
    points, no 'Border' points.
*/
class SgSetIterator
{
public:
    /** Set may contain only board points, no 'Border' points. */
    SgSetIterator(const SgPointSet& set);
    
    /** Advance the state of the iteration to the next element. */
    void operator++();

    /** Return the value of the current element. */
    SgPoint operator*() const;

    /** Return true if iteration is valid, otherwise false. */
    operator bool() const;

private:
    const SgPointSet& m_set;

    int m_index;

    void FindNext();

    int Size() const;
};

inline SgSetIterator::SgSetIterator(const SgPointSet& set)
    : m_set(set),
      m_index(0)
{
    FindNext();
}

inline void SgSetIterator::operator++()
{
    SG_ASSERT(m_index < Size());
    FindNext();
}

inline SgPoint SgSetIterator::operator*() const
{
    SG_ASSERT(m_index <= Size());
    SG_ASSERT_BOARDRANGE(m_index);
    SG_ASSERT(m_set.m_a.test(m_index));
    return m_index;
}

inline SgSetIterator::operator bool() const
{
    return m_index < Size();
}

inline void SgSetIterator::FindNext()
{
    int size = Size();
    do
    {
        ++m_index;
    }
    while (m_index < size && ! m_set.m_a.test(m_index));
}

inline int SgSetIterator::Size() const
{
    return static_cast<int>(m_set.m_a.size());
}

//----------------------------------------------------------------------------

/** Point set efficient for marking and testing.
    A SgSimpleSet is like a SgPointSet, except that it's more efficient at
    marking points and testing for marked points, while taking more time to
    clear, and not providing bit operations on the whole set.
*/
class SgSimpleSet
{
public:
    SgSimpleSet();

    void Include(SgPoint p);

    void Exclude(SgPoint p);

    bool Contains(SgPoint p) const;

    void Clear();

    bool IsEmpty() const;

    bool NonEmpty() const;

    void GetPoints(SgPointSet* points) const;

    bool NewMark(SgPoint p);

private:
    /** Marked points. */
    bool m_mark[SG_MAXPOINT];
};

inline SgSimpleSet::SgSimpleSet()
{
    Clear();
}


inline void SgSimpleSet::Include(SgPoint p)
{
    SG_ASSERT_BOARDRANGE(p);
    m_mark[p] = true;
}

inline void SgSimpleSet::Exclude(SgPoint p)
{
    SG_ASSERT_BOARDRANGE(p);
    m_mark[p] = false;
}

inline bool SgSimpleSet::Contains(SgPoint p) const
{
    return m_mark[p];
}

inline void SgSimpleSet::Clear()
{
    std::memset(m_mark, 0, SG_MAXPOINT * sizeof(bool));
}

inline bool SgSimpleSet::IsEmpty() const
{
    for (SgPoint p = 0; p < SG_MAXPOINT; ++p)
        if (Contains(p))
            return false;
    return true;
}

inline bool SgSimpleSet::NonEmpty() const
{
    return ! IsEmpty();
}

inline void SgSimpleSet::GetPoints(SgPointSet* points) const
{
    points->Clear();
    for (SgPoint p = 0; p < SG_MAXPOINT; ++p)
        if (Contains(p))
            points->Include(p);
}

inline bool SgSimpleSet::NewMark(SgPoint p)
{
    if (Contains(p))
        return false;
    else
    {
        Include(p);
        return true;
    }
}

//----------------------------------------------------------------------------

#endif // SG_POINTSET_H
