//----------------------------------------------------------------------------
/** @file GtpInputStream.cpp
 */
//----------------------------------------------------------------------------
#include "GtpInputStream.h"

using namespace std;

//----------------------------------------------------------------------------

GtpInputStream::GtpInputStream(istream &in) 
    : m_in(in)
{
    // Tying of input to output stream (like used by std::cin/cout) is not
    // needed by GtpEngine and potentially harmful if threads are enabled
    // (see GTPENGINE_PONDER) and the standard library implementation does not
    // support simultaneous writes to output stream by multiple threads
    m_in.tie(0);
}

GtpInputStream::~GtpInputStream()
{
}

bool GtpInputStream::EndOfInput()
{
    return m_in.fail();
}

bool GtpInputStream::GetLine(string &line)
{
    return getline(m_in, line);
}

//----------------------------------------------------------------------------
