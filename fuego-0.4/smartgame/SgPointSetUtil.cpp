//----------------------------------------------------------------------------
/** @file SgPointSetUtil.cpp
    See SgPointSetUtil.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgPointSetUtil.h"

#include <iostream>
#include "SgPointSet.h"
#include "SgWrite.h"

using namespace std;

//----------------------------------------------------------------------------

SgWritePointSet::SgWritePointSet(const SgPointSet& pointSet, string label,
                                 bool writeSize)
    : m_writeSize(writeSize),
      m_pointSet(pointSet),
      m_label(label)
{
}

ostream& SgWritePointSet::Write(ostream& out) const
{
    const size_t charPerLine = 60;
    int size = m_pointSet.Size();
    if (m_label != "")
        out << SgWriteLabel(m_label);
    ostringstream buffer;
    if (m_writeSize)
        buffer << size;
    if (size > 0)
    {
        if (m_writeSize)
            buffer << "  ";
        for (SgSetIterator point(m_pointSet); point; ++point)
        {
            if (buffer.str().size() > charPerLine)
            {
                out << buffer.str() << '\n';
                buffer.str("");
                if (m_label != "")
                    out << SgWriteLabel("");
            }
            buffer << SgWritePoint(*point) << ' ';
        }
    }
    out << buffer.str() << '\n';
    return out;
}

ostream& operator<<(ostream& out, const SgWritePointSet& write)
{
    return write.Write(out);
}

ostream& operator<<(ostream& out, const SgPointSet& pointSet)
{
    return out << SgWritePointSet(pointSet, "");
}

//----------------------------------------------------------------------------

ostream& operator<<(ostream& stream, const SgWritePointSetID& w)
{
    const SgPointSet& points = w.Points();
    if (points.IsEmpty())
        stream << "(none)";
    else
    {
        stream << SgWritePoint(points.Center())
               << ", Size " << points.Size();
    }
    return stream;
}

//----------------------------------------------------------------------------

SgReadPointSet::SgReadPointSet(SgPointSet& pointSet)
    : m_pointSet(pointSet)
{
}

istream& SgReadPointSet::Read(istream& in) const
{
    string pointstr;
    int size;
    in >> size;
    for (int i = 0; i < size; ++i)
    {
        // @todo Would be nice to have a full set of Read functions
        // but in the meanwhile calc the points here...
        in >> pointstr;
        if (pointstr.size() < 4) // not pass or null move
        {
            int col = toupper(pointstr[0]) - 'A' + 1;
            int row = toupper(pointstr[1]) - '0';
            if (pointstr.size() == 3)
                row = row * 10 + pointstr[2] - '0';
            m_pointSet.Include(SgPointUtil::Pt(col, row));
        }
    }
    return in;
}

istream& operator>>(istream& in, const SgReadPointSet& read)
{
    return read.Read(in);
}

istream& operator>>(istream& in, SgPointSet& pointSet)
{
    return in >> SgReadPointSet(pointSet);
}

//----------------------------------------------------------------------------
/* This function could probably be optimized if used in time-critical code */
void SgPointSetUtil::Rotate(int rotation, SgPointSet& pointSet, int boardSize)
{
    SgPointSet newSet;
    for (SgSetIterator it(pointSet); it; ++it)
        newSet.Include(SgPointUtil::Rotate(rotation, *it, boardSize));
    pointSet = newSet;
}
    
//----------------------------------------------------------------------------
