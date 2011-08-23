//----------------------------------------------------------------------------
/** @file GoKomi.cpp
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoKomi.h"

#include <sstream>

using namespace std;

//----------------------------------------------------------------------------

namespace {

string GetInvalidKomiErrorMessage(float komi)
{
    ostringstream buffer;
    buffer << "Invalid komi value: " << komi;
    return buffer.str();
}

} // namespace

//----------------------------------------------------------------------------

GoKomi::InvalidKomi::InvalidKomi(float komi)
    : SgException(GetInvalidKomiErrorMessage(komi))
{
}

GoKomi::InvalidKomi::InvalidKomi(const string& komi)
    : SgException("Invalid komi value: " + komi)
{
}

//----------------------------------------------------------------------------

GoKomi::GoKomi(const std::string& komi)
{
    {
        istringstream buffer(komi);
        string trimmedString;
        buffer >> trimmedString;
        if (! buffer)
        {
            m_isUnknown = true;
            m_value = 0;
            return;
        }
    }
    {
        istringstream buffer(komi);
        float value;
        buffer >> value;
        if (! buffer)
            throw InvalidKomi(komi);
        *this = GoKomi(value);
    }
}

string GoKomi::ToString() const
{
    if (m_isUnknown)
        return "";
    if (m_value % 2 == 0)
    {
        ostringstream buffer;
        buffer << (m_value / 2);
        return buffer.str();
    }
    else if (m_value == -1)
        return "-0.5";
    else
    {
        ostringstream buffer;
        buffer << (m_value / 2) << ".5";
        return buffer.str();
    }
}

//----------------------------------------------------------------------------
