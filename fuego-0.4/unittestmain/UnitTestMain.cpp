//----------------------------------------------------------------------------
/** @file UnitTestMain.cpp
    Main function for running unit tests from all modules in project Fuego.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include "GoInit.h"
#include "SgInit.h"
#include <boost/version.hpp>

#define BOOST_VERSION_MAJOR (BOOST_VERSION / 100000)
#define BOOST_VERSION_MINOR (BOOST_VERSION / 100 % 1000)

//----------------------------------------------------------------------------

namespace {

void Init()
{
    SgInit();
    GoInit();
}

void Fini()
{
    GoFini();
    SgFini();
}

} // namespace

//----------------------------------------------------------------------------

#if BOOST_VERSION_MAJOR == 1 && BOOST_VERSION_MINOR == 33

#include <cstdlib>
#include <boost/test/auto_unit_test.hpp>

boost::unit_test::test_suite* init_unit_test_suite(int argc, char* argv[])
{
    SG_UNUSED(argc);
    SG_UNUSED(argv);
    try
    {
        Init();
    }
    catch (const std::exception& e)
    {
        return 0;
    }
    if (std::atexit(Fini) != 0)
        return 0;
    return boost::unit_test::auto_unit_test_suite();
}

//----------------------------------------------------------------------------

#elif BOOST_VERSION_MAJOR == 1 && BOOST_VERSION_MINOR >= 34

// Handling of unit testing framework initialization is messy and not
// documented in the Boost 1.34 documentation. See also:
// http://lists.boost.org/Archives/boost/2006/11/112946.php

#include <cstdlib>
//#define BOOST_TEST_DYN_LINK // Must be defined before including unit_test.hpp
#include <boost/test/included/unit_test.hpp>

bool init_unit_test()
{
    try
    {
        Init();
    }
    catch (const std::exception& e)
    {
        return false;
    }
    if (std::atexit(Fini) != 0)
        return false;
    return true;
}

int main(int argc, char* argv[])
{
    return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}

//----------------------------------------------------------------------------

#else
#error "Unknown Boost version"
#endif

//----------------------------------------------------------------------------
