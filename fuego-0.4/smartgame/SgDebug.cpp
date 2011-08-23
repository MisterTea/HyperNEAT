//----------------------------------------------------------------------------
/** @file SgDebug.cpp
    See SgDebug.h
 */
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgDebug.h"

#include <fstream>
#include <iostream>
#include <memory>

using namespace std;

//----------------------------------------------------------------------------

/** Null stream.
    This file stream will never be opened and acts as a null stream
    for SgDebug().
*/
static ofstream s_nullStream;

static auto_ptr<ofstream> s_fileStream;


ostream* g_debugStrPtr(&cerr);

std::ostream& SgDebug()
{
    if (! g_debugStrPtr->good())
    {
        // does not just use a direct SG_ASSERT(g_debugStrPtr->good())
        // in order to allow a breakpoint to be set on the line below.
        //JJG: Disable assert
        //SG_ASSERT(false);
    }
    return *g_debugStrPtr;
}

std::ostream& SgWarning()
{
    SgDebug() << "WARNING: ";
    return SgDebug();
}

//----------------------------------------------------------------------------

void SgDebugToWindow()
{
    g_debugStrPtr = &cerr;
}

void SgDebugToFile(const char* filename)
{
    if (s_fileStream.get() == 0)
        s_fileStream.reset(new ofstream(filename, ios::app));
    g_debugStrPtr = s_fileStream.get();
}

void SgDebugToNull()
{
    g_debugStrPtr = &s_nullStream;
}

ostream* SgSwapDebugStr(ostream* newStr)
{
    ostream* t = g_debugStrPtr;
    g_debugStrPtr = newStr;
    return t;
}

//----------------------------------------------------------------------------

SgDebugToNewFile::SgDebugToNewFile(const char* filename)
    : m_old(SgSwapDebugStr(new ofstream(filename, ios::app)))
{
}

SgDebugToNewFile::SgDebugToNewFile()
    : m_old(NULL)
{
}

void SgDebugToNewFile::SetFile(const char* filename)
{
    m_old = SgSwapDebugStr(new ofstream(filename, ios::app));
}

SgDebugToNewFile::~SgDebugToNewFile()
{
    if (m_old)
    {
        ostream* t = SgSwapDebugStr(m_old);
        delete t;
    }
}

//----------------------------------------------------------------------------

SgDebugToString::SgDebugToString(bool writeToOldDebugStr)
    : m_writeToOldDebugStr(writeToOldDebugStr)
{
    m_old = SgSwapDebugStr(&m_str);
}

SgDebugToString::~SgDebugToString()
{
    if (m_writeToOldDebugStr)
        (*m_old) << GetString();
    SgSwapDebugStr(m_old);
}

//----------------------------------------------------------------------------

