//----------------------------------------------------------------------------
/** @file SgCmdLineOptTest.cpp
    Unit tests for SgCmdLineOpt.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include "SgCmdLineOpt.h"
#include "SgException.h"

using namespace std;

//----------------------------------------------------------------------------

namespace {

const double epsilon = 1e-4;

} // namespace

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(SgCmdLineOptTestParse)
{
    vector<string> specs;
    specs.push_back("option1");
    specs.push_back("option2:");
    const char* argv[] = { 0, "-option1", "arg1", "-option2", "arg2" };
    const int argc = sizeof(argv) / sizeof(argv[0]);
    SgCmdLineOpt opt;    
    opt.Parse(argc, argv, specs);
    BOOST_CHECK_EQUAL(opt.GetArguments().size(), static_cast<size_t>(1));
    BOOST_CHECK_EQUAL(opt.GetArguments()[0], "arg1");
    BOOST_CHECK(opt.Contains("option1"));
    BOOST_CHECK(opt.Contains("option2"));
    BOOST_CHECK_EQUAL(opt.GetString("option2"), "arg2");
}

BOOST_AUTO_TEST_CASE(SgCmdLineOptTestGetDouble)
{
    vector<string> specs;
    specs.push_back("option1:");
    specs.push_back("option2:");
    const char* argv[] = { 0, "-option1", "5.5", "-option2", "foo" };
    const int argc = sizeof(argv) / sizeof(argv[0]);
    SgCmdLineOpt opt;    
    opt.Parse(argc, argv, specs);
    BOOST_CHECK_CLOSE(opt.GetDouble("option1"), 5.5, epsilon);
    BOOST_CHECK_THROW(opt.GetDouble("option2"), SgException);
}

BOOST_AUTO_TEST_CASE(SgCmdLineOptTestGetDoubleDefault)
{
    vector<string> specs;
    specs.push_back("option");
    char* argv[] = { 0 };
    const int argc = sizeof(argv) / sizeof(argv[0]);
    SgCmdLineOpt opt;    
    opt.Parse(argc, argv, specs);
    BOOST_CHECK_CLOSE(opt.GetDouble("option", 5.5), 5.5, epsilon);
}

BOOST_AUTO_TEST_CASE(SgCmdLineOptTestGetInteger)
{
    vector<string> specs;
    specs.push_back("option1:");
    specs.push_back("option2:");
    const char* argv[] = { 0, "-option1", "5", "-option2", "foo" };
    const int argc = sizeof(argv) / sizeof(argv[0]);
    SgCmdLineOpt opt;    
    opt.Parse(argc, argv, specs);
    BOOST_CHECK_EQUAL(opt.GetInteger("option1"), 5);
    BOOST_CHECK_THROW(opt.GetInteger("option2"), SgException);
}

BOOST_AUTO_TEST_CASE(SgCmdLineOptTestGetIntegerDefault)
{
    vector<string> specs;
    specs.push_back("option");
    char* argv[] = { 0 };
    const int argc = sizeof(argv) / sizeof(argv[0]);
    SgCmdLineOpt opt;    
    opt.Parse(argc, argv, specs);
    BOOST_CHECK_EQUAL(opt.GetInteger("option", 5), 5);
}

BOOST_AUTO_TEST_CASE(SgCmdLineOptTestGetStringDefault)
{
    vector<string> specs;
    specs.push_back("option");
    char* argv[] = { 0 };
    const int argc = sizeof(argv) / sizeof(argv[0]);
    SgCmdLineOpt opt;    
    opt.Parse(argc, argv, specs);
    BOOST_CHECK_EQUAL(opt.GetString("option", "foo"), "foo");
}

} // namespace

//----------------------------------------------------------------------------

