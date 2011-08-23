//----------------------------------------------------------------------------
/** @file SgPointIterator.h
    Class SgPointIterator.
*/
//----------------------------------------------------------------------------

#ifndef SG_POINTITERATOR_H
#define SG_POINTITERATOR_H

#include "SgPoint.h"

//----------------------------------------------------------------------------

/** Iterate through an array of points
    terminated by END_POINT (defined to be zero for performance).
*/
class SgPointIterator
{
public:
    SgPointIterator(const SgPoint* first);

    virtual ~SgPointIterator();

    /** Advance the state of the iteration to the next element. */
    void operator++();

    /** Return the value of the current element. */
    SgPoint operator*() const;

    /** Return true if iteration is valid, otherwise false. */
    operator bool() const;

private:
    const SgPoint* m_point;

    /** Not implemented. */
    SgPointIterator(const SgPointIterator&);

    /** Not implemented. */
    SgPointIterator& operator=(const SgPointIterator&);
};

inline SgPointIterator::SgPointIterator(const SgPoint* first)
    : m_point(first)
{
}

inline SgPointIterator::~SgPointIterator()
{
}

inline void SgPointIterator::operator++()
{
    ++m_point;
}

inline SgPoint SgPointIterator::operator*() const
{
    return *m_point;
}

inline SgPointIterator::operator bool() const
{
    return *m_point != SG_ENDPOINT;
}

//----------------------------------------------------------------------------

/** Iterate through an array of points with the range defined by pointers. */
class SgPointRangeIterator
{
public:
    /** Constructor.
        @param first Pointer to first element.
        @param end Pointer to last element + 1.
    */
    SgPointRangeIterator(const SgPoint* first, const SgPoint* end);

    virtual ~SgPointRangeIterator();

    /** Advance the state of the iteration to the next element. */
    void operator++();

    /** Return the value of the current element. */
    SgPoint operator*() const;

    /** Return true if iteration is valid, otherwise false. */
    operator bool() const;

private:
    const SgPoint* m_point;

    const SgPoint* m_end;

    /** Not implemented. */
    SgPointRangeIterator(const SgPointRangeIterator&);

    /** Not implemented. */
    SgPointRangeIterator& operator=(const SgPointRangeIterator&);
};

inline SgPointRangeIterator::SgPointRangeIterator(const SgPoint* first,
                                                  const SgPoint* end)
    : m_point(first),
      m_end(end)
{
}

inline SgPointRangeIterator::~SgPointRangeIterator()
{
}

inline void SgPointRangeIterator::operator++()
{
    ++m_point;
}

inline SgPoint SgPointRangeIterator::operator*() const
{
    return *m_point;
}

inline SgPointRangeIterator::operator bool() const
{
    return m_point != m_end;
}

//----------------------------------------------------------------------------

#endif // SG_POINTITERATOR_H
