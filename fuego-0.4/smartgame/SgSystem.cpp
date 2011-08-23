//----------------------------------------------------------------------------
/** @file SgSystem.cpp
    See SgSystem.h
 */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <limits>
#include <list>
#include "SgTime.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

volatile bool s_userAbort = false;

/** Assertion handlers.
    Stored in a static function variable to ensure, that they exist at
    first usage, if this function is called from global variables in
    different compilation units.
*/
list<SgAssertionHandler*>& AssertionHandlers()
{
    static list<SgAssertionHandler*> s_assertionHandlers;
    return s_assertionHandlers;
}

} // namespace

//----------------------------------------------------------------------------

SgAssertionHandler::SgAssertionHandler()
{
    AssertionHandlers().push_back(this);
}

SgAssertionHandler::~SgAssertionHandler()
{
    AssertionHandlers().remove(this);
}

//----------------------------------------------------------------------------

#ifndef NDEBUG

/** Set the shell variable SMARTGAME_ASSERT_CONTINUE to drop into the debugger
    instead of aborting the program whenever an SG_ASSERT fails
*/
static bool s_assertContinue = std::getenv("SMARTGAME_ASSERT_CONTINUE");

void SgHandleAssertion(const char* expr, const char* file, int line)
{
    /** Set a breakpoint on the next line to drop into the debugger */
    cerr << "Assertion failed "
         << file << ':' << line << ": " << expr << '\n';
    for_each(AssertionHandlers().begin(), AssertionHandlers().end(),
             mem_fun(&SgAssertionHandler::Run));
    if (! s_assertContinue)
        abort();
}
#endif

//----------------------------------------------------------------------------

void SgSetUserAbort(bool aborted)
{
    s_userAbort = aborted;
}

bool SgUserAbort()
{
    return s_userAbort;
}

//----------------------------------------------------------------------------
