//----------------------------------------------------------------------------
/** @file GtpEngineTest.cpp
    Unit tests for GtpEngine.
*/
//----------------------------------------------------------------------------

#include "../GtpEngine.h"

#ifdef GTPENGINETEST_MAIN
#define BOOST_AUTO_TEST_MAIN
#endif

#include <boost/test/auto_unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/test_tools.hpp>

using namespace std;

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(GtpCommandTest_ArgToLower)
{
    GtpCommand cmd("command cAsE");
    BOOST_CHECK_EQUAL(cmd.ArgToLower(0), "case");
}

BOOST_AUTO_TEST_CASE(GtpCommandTest_BoolArg)
{
    BOOST_CHECK(! GtpCommand("command 0").BoolArg(0));
    BOOST_CHECK(GtpCommand("command 1").BoolArg(0));
    BOOST_CHECK_THROW(GtpCommand("command 2").BoolArg(0), GtpFailure);
    BOOST_CHECK_THROW(GtpCommand("command foo").BoolArg(0), GtpFailure);
    BOOST_CHECK_THROW(GtpCommand("command").BoolArg(0), GtpFailure);
}

BOOST_AUTO_TEST_CASE(GtpCommandTest_CheckNuArg0)
{
    GtpCommand cmd("20 command");
    BOOST_CHECK_NO_THROW(cmd.CheckArgNone());
    BOOST_CHECK_THROW(cmd.CheckNuArg(1), GtpFailure);
    BOOST_CHECK_NO_THROW(cmd.CheckNuArgLessEqual(2));
}

BOOST_AUTO_TEST_CASE(GtpCommandTest_CheckNuArg3)
{
    GtpCommand cmd("command arg1 arg2 arg3");
    BOOST_CHECK_THROW(cmd.CheckArgNone(), GtpFailure);
    BOOST_CHECK_THROW(cmd.CheckNuArg(2), GtpFailure);
    BOOST_CHECK_NO_THROW(cmd.CheckNuArg(3));
    BOOST_CHECK_THROW(cmd.CheckNuArg(4), GtpFailure);
    BOOST_CHECK_THROW(cmd.CheckNuArgLessEqual(2), GtpFailure);
    BOOST_CHECK_NO_THROW(cmd.CheckNuArgLessEqual(3));
    BOOST_CHECK_NO_THROW(cmd.CheckNuArgLessEqual(4));
}

BOOST_AUTO_TEST_CASE(GtpCommandTest_FloatArg)
{
    GtpCommand cmd("command abc 9.1");
    BOOST_CHECK_THROW(cmd.FloatArg(0), GtpFailure);
    BOOST_CHECK_CLOSE(cmd.FloatArg(1), 9.1, 1e-4);
}

BOOST_AUTO_TEST_CASE(GtpCommandTest_Init)
{
    GtpCommand cmd("10 command1 arg1 arg2");
    BOOST_CHECK_EQUAL(cmd.ID(), "10");
    BOOST_CHECK_EQUAL(cmd.Name(), "command1");
    BOOST_CHECK_EQUAL(cmd.NuArg(), 2u);
    BOOST_CHECK_EQUAL(cmd.Arg(0), "arg1");
    BOOST_CHECK_EQUAL(cmd.Arg(1), "arg2");
    cmd.Init("20 command2 arg3");
    BOOST_CHECK_EQUAL(cmd.ID(), "20");
    BOOST_CHECK_EQUAL(cmd.Name(), "command2");
    BOOST_CHECK_EQUAL(cmd.NuArg(), 1u);
    BOOST_CHECK_EQUAL(cmd.Arg(0), "arg3");
}

BOOST_AUTO_TEST_CASE(GtpCommandTest_IntArg)
{
    GtpCommand cmd("command 9 abc");
    BOOST_CHECK_EQUAL(cmd.IntArg(0), 9);
    BOOST_CHECK_THROW(cmd.IntArg(1), GtpFailure);
}

BOOST_AUTO_TEST_CASE(GtpCommandTest_IntArgMin)
{
    GtpCommand cmd("command 9");
    BOOST_CHECK_EQUAL(cmd.IntArg(0, 5), 9);
    BOOST_CHECK_THROW(cmd.IntArg(0, 10), GtpFailure);
}

BOOST_AUTO_TEST_CASE(GtpCommandTest_IntArgMinMax)
{
    GtpCommand cmd("command 9");
    BOOST_CHECK_EQUAL(cmd.IntArg(0, 5, 10), 9);
    BOOST_CHECK_THROW(cmd.IntArg(0, 0, 5), GtpFailure);
    BOOST_CHECK_THROW(cmd.IntArg(0, 10, 15), GtpFailure);
}

BOOST_AUTO_TEST_CASE(GtpCommandTest_Parse)
{
    GtpCommand cmd("10 boardsize 9");
    BOOST_CHECK_EQUAL(cmd.Line(), "10 boardsize 9");
    BOOST_CHECK_EQUAL(cmd.ArgLine(), "9");
    BOOST_CHECK_EQUAL(cmd.ID(), "10");
    BOOST_CHECK_EQUAL(cmd.Name(), "boardsize");
    BOOST_CHECK_EQUAL(cmd.NuArg(), 1u);
    BOOST_CHECK_EQUAL(cmd.Arg(0), "9");
    BOOST_CHECK_THROW(cmd.Arg(1), GtpFailure);
}

BOOST_AUTO_TEST_CASE(GtpCommandTest_RemainingLine)
{
    GtpCommand cmd("mycommand arg1   \"arg2 \" arg3 ");
    BOOST_CHECK_EQUAL(cmd.RemainingLine(0), "\"arg2 \" arg3");
    BOOST_CHECK_EQUAL(cmd.RemainingLine(1), "arg3");
    BOOST_CHECK_EQUAL(cmd.RemainingLine(2), "");
}

BOOST_AUTO_TEST_CASE(GtpCommandTest_Response)
{
    GtpCommand cmd("name");
    cmd << "Funny";
    BOOST_CHECK_EQUAL(cmd.Response(), "Funny");
}

BOOST_AUTO_TEST_CASE(GtpCommandTest_SizeTypeArg)
{
    GtpCommand cmd("command 9 abc -1");
    BOOST_CHECK_EQUAL(cmd.SizeTypeArg(0), 9u);
    BOOST_CHECK_THROW(cmd.SizeTypeArg(1), GtpFailure);
    BOOST_CHECK_THROW(cmd.SizeTypeArg(2), GtpFailure);
}

BOOST_AUTO_TEST_CASE(GtpCommandTest_SizeTypeArgMin)
{
    GtpCommand cmd("command 9");
    BOOST_CHECK_EQUAL(cmd.SizeTypeArg(0, 5), 9u);
    BOOST_CHECK_THROW(cmd.SizeTypeArg(0, 10), GtpFailure);
}

} // namespace

//----------------------------------------------------------------------------

namespace {

BOOST_AUTO_TEST_CASE(GtpEngineTest_Command)
{
    istringstream in("version\n");
    ostringstream out;
    GtpInputStream gin(in);
    GtpOutputStream gout(out);
    GtpEngine engine(gin, gout);
    engine.MainLoop();
    BOOST_CHECK_EQUAL(out.str(), "= \n\n");
}

BOOST_AUTO_TEST_CASE(GtpEngineTest_CommandWithID)
{
    istringstream in("10 version\n");
    ostringstream out;
    GtpInputStream gin(in);
    GtpOutputStream gout(out);
    GtpEngine engine(gin, gout);
    engine.MainLoop();
    BOOST_CHECK_EQUAL(out.str(), "=10 \n\n");
}

/** GTP engine returning invalid responses for testing class GtpEngine.
    For testing that the base class GtpEngine sanitizes responses of
    subclasses that contain empty lines (see @ref GtpEngine::MainLoop).
*/
class InvalidResponseEngine
    : public GtpEngine
{
public:
    InvalidResponseEngine(GtpInputStream& in, GtpOutputStream& out);

    void CmdInvalidResponse(GtpCommand& cmd);

    void CmdInvalidResponse2(GtpCommand& cmd);
};

InvalidResponseEngine::InvalidResponseEngine(GtpInputStream& in, 
                                             GtpOutputStream& out)
    : GtpEngine(in, out)
{
    typedef GtpCallback<InvalidResponseEngine> Callback;
    Register("invalid_response",
             new Callback(this, &InvalidResponseEngine::CmdInvalidResponse));
    Register("invalid_response2",
             new Callback(this, &InvalidResponseEngine::CmdInvalidResponse2));
}

void InvalidResponseEngine::CmdInvalidResponse(GtpCommand& cmd)
{
    cmd << "This response is invalid\n"
        << "\n"
        << "because it contains an empty line";
}

void InvalidResponseEngine::CmdInvalidResponse2(GtpCommand& cmd)
{
    cmd << "This response is invalid\n"
        << "\n"
        << "\n"
        << "because it contains two empty lines";
}

/** Check that MainLoop sanitizes invalid responses with one empty line. */
BOOST_AUTO_TEST_CASE(GtpEngineTest_MainLoopEmptyLines)
{
    istringstream in("invalid_response\n");
    ostringstream out;
    GtpInputStream gin(in);
    GtpOutputStream gout(out);
    InvalidResponseEngine engine(gin, gout);
    engine.MainLoop();
    BOOST_CHECK_EQUAL(out.str(),
                      "= This response is invalid\n"
                      " \n"
                      "because it contains an empty line\n"
                      "\n");
}

/** Check that MainLoop sanitizes invalid responses with two empty lines. */
BOOST_AUTO_TEST_CASE(GtpEngineTest_MainLoopEmptyLines2)
{
    istringstream in("invalid_response2\n");
    ostringstream out;
    GtpInputStream gin(in);
    GtpOutputStream gout(out);
    InvalidResponseEngine engine(gin, gout);
    engine.MainLoop();
    BOOST_CHECK_EQUAL(out.str(),
                      "= This response is invalid\n"
                      " \n"
                      " \n"
                      "because it contains two empty lines\n"
                      "\n");
}

BOOST_AUTO_TEST_CASE(GtpEngineTest_UnknownCommand)
{
    istringstream in("unknowncommand\n");
    ostringstream out;
    GtpInputStream gin(in);
    GtpOutputStream gout(out);
    GtpEngine engine(gin, gout);
    engine.MainLoop();
    BOOST_REQUIRE(out.str().size() >= 2);
    BOOST_CHECK_EQUAL(out.str().substr(0, 2), "? ");
}

} // namespace

//----------------------------------------------------------------------------
