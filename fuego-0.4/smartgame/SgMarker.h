//----------------------------------------------------------------------------
/** @file SgMarker.h
    Class SgMarker.
*/
//----------------------------------------------------------------------------

#ifndef SG_MARKER_H
#define SG_MARKER_H

#include "SgArray.h"
#include "SgPoint.h"
#include "SgPointSet.h"

//----------------------------------------------------------------------------

/** Used to mark points on the board.
    Like SgPointSet but uses more memory and is faster at marking and clearing
    all marks.
*/
class SgMarker
{
public:
    SgMarker();

    void Include(SgPoint p);

    bool Contains(SgPoint p) const;

    /** Mark a point and return true if it was not already marked */
    bool NewMark(SgPoint p);

    void Clear();

    void GetPoints(SgPointSet* points) const;

private:
    void Init();

    /** Current marker number */
    int m_thisMark;

    /** Marked points */
    SgArray<int,SG_MAXPOINT> m_mark;

#ifndef NDEBUG
    /** See ReserveBoardMarker */
    bool m_markerInUse;

    friend class SgReserveMarker;
#endif

    /** Not implemented. */
    SgMarker(const SgMarker&);

    /** Not implemented. */
    SgMarker& operator=(const SgMarker&);
};

inline SgMarker::SgMarker()
#ifndef NDEBUG
    : m_markerInUse(false)
#endif
{
    Init();
}

inline void SgMarker::Clear()
{
    if (++m_thisMark == 0)
        Init();
}

inline bool SgMarker::Contains(SgPoint p) const
{
    return m_mark[p] == m_thisMark;
}

inline void SgMarker::GetPoints(SgPointSet* points) const
{
    points->Clear();
    for (SgPoint p = 0; p < SG_MAXPOINT; ++p)
        if (Contains(p))
            points->Include(p);
}

inline void SgMarker::Include(SgPoint p)
{
    SG_ASSERT_BOARDRANGE(p);
    m_mark[p] = m_thisMark;
}

inline void SgMarker::Init()
{
    m_thisMark = 1;
    m_mark.Fill(0);
}

inline bool SgMarker::NewMark(SgPoint p)
{
    if (Contains(p))
        return false;
    Include(p);
    return true;
}

//----------------------------------------------------------------------------

/** Declare a variable of this class on the stack to reserve a board marker.
    It asserts that nobody else is using the same marker at the same time.
*/
class SgReserveMarker
{
public:
    /** In debug build, assert that the marker is not already in use. */
    SgReserveMarker(SgMarker& marker);

    ~SgReserveMarker();

private:
#ifndef NDEBUG
    SgMarker& m_marker;
#endif
};

#ifndef NDEBUG

inline SgReserveMarker::SgReserveMarker(SgMarker& marker)
    : m_marker(marker)
{
    SG_ASSERT(! marker.m_markerInUse);
    m_marker.m_markerInUse = true;
}

inline SgReserveMarker::~SgReserveMarker()
{
    m_marker.m_markerInUse = false;
}

#else

inline SgReserveMarker::SgReserveMarker(SgMarker& marker)
{
    SG_UNUSED(marker);
}

inline SgReserveMarker::~SgReserveMarker()
{
}

#endif

//----------------------------------------------------------------------------

#endif // SG_MARKER_H
