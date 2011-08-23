//----------------------------------------------------------------------------
/** @file SgException.cpp
    See SgException.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgException.h"

using namespace std;

//----------------------------------------------------------------------------

SgException::SgException()
{
}

SgException::SgException(const string& message)
{
    m_message = message;
}

SgException::~SgException() throw()
{
}

const char* SgException::what() const throw()
{
    return m_message.c_str();
}

//----------------------------------------------------------------------------

