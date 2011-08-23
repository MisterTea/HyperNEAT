//----------------------------------------------------------------------------
/** @file SgWrite.cpp
    See SgWrite.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgWrite.h"

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "SgVector.h"

using namespace std;

//----------------------------------------------------------------------------

ostream& operator<<(ostream& out, const SgWriteLabel& w)
{
    string label = w.m_label + " ";
    out << left << setw(15) << label;
    return out;
}

ostream& operator<<(ostream& out, const SgWriteLine &w)
{
    SG_UNUSED(w);
    out <<
        "----------"
        "----------"
        "----------"
        "----------"
        "----------"
        "----------"
        "----------"
        "--------\n"; // 78 chars
    return out;
}

//----------------------------------------------------------------------------

ostream& operator<<(ostream& out, const SgWriteMove& w)
{
    out << SgBW(w.m_color) << ' ' << SgWritePoint(w.m_point) << ' ';
    return out;
}

//----------------------------------------------------------------------------

SgWritePointList::SgWritePointList(const vector<SgPoint>& pointList,
                                   std::string label, bool writeSize)
    : m_writeSize(writeSize),
      m_pointList(pointList),
      m_label(label)
{
}

SgWritePointList::SgWritePointList(const SgVector<SgPoint>& pointList,
                                   string label, bool writeSize)
    : m_writeSize(writeSize),
      m_label(label)
{
    for (SgVectorIterator<SgPoint> it(pointList); it; ++it)
        m_pointList.push_back(*it);
}

ostream& SgWritePointList::Write(ostream& out) const
{
    const size_t charPerLine = 60;
    size_t size = m_pointList.size();
    if (m_label != "")
        out << SgWriteLabel(m_label);
    ostringstream buffer;
    if (m_writeSize)
        buffer << size;
    if (size > 0)
    {
        if (m_writeSize)
            buffer << "  ";
        for (vector<SgPoint>::const_iterator it = m_pointList.begin();
             it != m_pointList.end(); ++it)
        {
            if (buffer.str().size() > charPerLine)
            {
                out << buffer.str() << '\n';
                buffer.str("");
                if (m_label != "")
                    out << SgWriteLabel("");
            }
            buffer << SgWritePoint(*it) << ' ';
        }
    }
    out << buffer.str() << '\n';
    return out;
}

ostream& operator<<(ostream& out, const SgWritePointList& write)
{
    return write.Write(out);
}

ostream& operator<<(ostream& out, const SgWriteBoolean &w)
{
    if (w.m_value)
        out << "true";
    else
        out << "false";
    return out;
}

//----------------------------------------------------------------------------

SgWriteBoolAsInt::SgWriteBoolAsInt(bool value)
    : m_value(value)
{
}

ostream& SgWriteBoolAsInt::Write(ostream& out) const
{
    return out << (m_value ? "1" : "0");
}

ostream& operator<<(ostream& out, const SgWriteBoolAsInt& write)
{
    return write.Write(out);
}

//----------------------------------------------------------------------------

