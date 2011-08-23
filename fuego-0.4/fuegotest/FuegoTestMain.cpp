//----------------------------------------------------------------------------
/** @file FuegoTestMain.cpp
    Main function for FuegoTest.
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <iostream>
#include "FuegoTestEngine.h"
#include "GoInit.h"
#include "SgDebug.h"
#include "SgException.h"
#include "SgInit.h"

#include <boost/utility.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

using namespace std;
namespace po = boost::program_options;

//----------------------------------------------------------------------------

namespace {

/** @name Settings from command line options */
// @{

bool g_quiet;

string g_config;

/** Player string as in FuegoTestEngine::SetPlayer */
string g_player;

const char* g_programPath;

// @} // @name

void MainLoop()
{
    GtpInputStream gtpIn(cin);
    GtpOutputStream gtpOut(cout);
    FuegoTestEngine engine(gtpIn, gtpOut, 0, g_programPath, g_player);
    GoGtpAssertionHandler assertionHandler(engine);
    if (g_config != "")
        engine.ExecuteFile(g_config);
    engine.MainLoop();
}

void Help(po::options_description& desc)
{
    cout << "Options:\n" << desc << '\n';
    exit(1);
}

void ParseOptions(int argc, char** argv)
{
    int srand;
    po::options_description options_desc;
    options_desc.add_options()
        ("config", 
         po::value<std::string>(&g_config)->default_value(""),
         "execuate GTP commands from file before starting main command loop")
        ("help", "displays this help and exit")
        ("player", 
         po::value<std::string>(&g_player)->default_value(""),
         "player (average|ladder|liberty|maxeye|minlib|no-search|random|safe")
        ("quiet", "don't print debug messages")
        ("srand", 
         po::value<int>(&srand)->default_value(0),
         "set random seed (-1:none, 0:time(0))");
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, options_desc), vm);
        po::notify(vm);
    }
    catch(...) {
        Help(options_desc);
    }
    if (vm.count("help"))
        Help(options_desc);
    if (vm.count("quiet"))
        g_quiet = true;
    if (vm.count("srand"))
        SgRandom::SetSeed(srand);
}

} // namespace

//----------------------------------------------------------------------------

int main(int argc, char** argv)
{
    if (argc > 0 && argv != 0)
    {
        g_programPath = argv[0];
        try
        {
            ParseOptions(argc, argv);
        }
        catch (const SgException& e)
        {
            SgDebug() << e.what() << "\n";
            return 1;
        }
    }
    if (g_quiet)
        SgDebugToNull();
    try
    {
        SgInit();
        GoInit();
        MainLoop();
        GoFini();
        SgFini();
    }
    catch (const GtpFailure& e)
    {
        SgDebug() << e.Response() << '\n';
        return 1;
    }
    catch (const std::exception& e)
    {
        SgDebug() << e.what() << '\n';
        return 1;
    }
    return 0;
}

//----------------------------------------------------------------------------

