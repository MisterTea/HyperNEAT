//----------------------------------------------------------------------------
/** @file SgSearchValue.cpp
    See SgSearchValue.h.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgSearchValue.h"

#include <algorithm>
#include <iomanip>
#include <limits>
#include <sstream>
#include <math.h>

using namespace std;

//----------------------------------------------------------------------------

/** Set '*s' to the string for this value.
    e.g. "B+3.5", "W+20", or "W+(ko)[12]". The value is divided by
    'unitPerPoint' to determine the number of points.
*/
string SgSearchValue::ToString(int unitPerPoint) const
{
    if (m_value == 0)
        return "0";
    ostringstream stream;
    stream << (m_value > 0 ? "B+" : "W+");
    if (IsEstimate())
    {
        if (unitPerPoint == 1)
            stream << (abs(m_value) / unitPerPoint);
        else
            stream << setprecision(1)
              << (static_cast<float>(abs(m_value)) / unitPerPoint);
    }
    else
    {
        if (KoLevel() != 0)
            stream << "(ko)";
        if (Depth() != 0)
        {
            stream << " (" << Depth() << " moves)";
        }
    }
    return stream.str();
}

bool SgSearchValue::FromString(const string& s)
{
    SG_UNUSED(s);
    SG_ASSERT(false); // AR: not yet implemented
    return false;
}

int SgSearchValue::KoLevel() const
{
    if (IsEstimate())
        return 0;
    else
    {
        int level = (abs(m_value) - 1) / SgSearch::MAX_DEPTH;
        return (MAX_LEVEL - 1) - level;
    }
}

//----------------------------------------------------------------------------

