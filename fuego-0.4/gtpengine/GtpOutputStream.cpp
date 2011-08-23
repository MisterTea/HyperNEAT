//----------------------------------------------------------------------------
/** @file GtpOutputStream.cpp
 */
//----------------------------------------------------------------------------
#include "GtpOutputStream.h"

using namespace std;

//----------------------------------------------------------------------------

GtpOutputStream::GtpOutputStream(ostream &out)
    : m_out(out)
{
}

GtpOutputStream::~GtpOutputStream()
{
}

void GtpOutputStream::Write(const string &line)
{
    m_out << line;
}

void GtpOutputStream::Flush()
{
    m_out.flush();
}
    
//----------------------------------------------------------------------------
