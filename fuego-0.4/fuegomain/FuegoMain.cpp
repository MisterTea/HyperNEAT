//----------------------------------------------------------------------------
/** @file FuegoMain.cpp
    Main function for Fuego
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include <iostream>
#include <boost/filesystem/path.hpp>
#include "FuegoMainEngine.h"
#include "FuegoMainUtil.h"
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
using boost::filesystem::path;
namespace po = boost::program_options;

//----------------------------------------------------------------------------

namespace {

/** @name Settings from command line options */
// @{

bool g_noHandicap = false;

bool g_noBook = false;

bool g_quiet = false;

int g_fixedBoardSize;

int g_maxGames;

string g_config;

path g_programDir;

const char* g_programPath;

int g_srand;

// @} // @name

/** Get program directory from program path.
    @param programPath Program path taken from @c argv[0] in
    @c main. According to ANSI C, this can be @c 0.
 */
path GetProgramDir(const char* programPath)
{
    if (programPath == 0)
        return "";
    //TODO(Jgauci): fix.
    //return path(programPath, boost::filesystem::native).branch_path();
    return path();
}

void MainLoop()
{
    GtpInputStream gtpIn(cin);
    GtpOutputStream gtpOut(cout);
    FuegoMainEngine engine(gtpIn, gtpOut, g_fixedBoardSize, g_programPath,
                           g_noHandicap);
    GoGtpAssertionHandler assertionHandler(engine);
    if (g_maxGames >= 0)
        engine.SetMaxClearBoard(g_maxGames);
    if (! g_noBook)
        FuegoMainUtil::LoadBook(engine.Book(), g_programDir);
    if (g_config != "")
        engine.ExecuteFile(g_config);
    engine.MainLoop();
}

void Help(po::options_description& desc)
{
    cout << "Options:\n" << desc << "\n";
    exit(0);
}

void ParseOptions(int argc, char** argv)
{
    po::options_description options_desc;
    options_desc.add_options()
        ("config", 
         po::value<std::string>(&g_config)->default_value(""),
         "execuate GTP commands from file before starting main command loop")
        ("help", "Displays this help and exit")
        ("maxgames", 
         po::value<int>(&g_maxGames)->default_value(-1),
         "make clear_board fail after n invocations")
        ("nobook", "don't automatically load opening book")
        ("nohandicap", "don't support handicap commands")
        ("quiet", "don't print debug messages")
        ("srand", 
         po::value<int>(&g_srand)->default_value(0),
         "set random seed (-1:none, 0:time(0))")
        ("size", 
         po::value<int>(&g_fixedBoardSize)->default_value(0),
         "initial (and fixed) board size");
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
    if (vm.count("nobook"))
        g_noBook = true;
    if (vm.count("nohandicap"))
        g_noHandicap = true;
    if (vm.count("quiet"))
        g_quiet = true;
}

void PrintStartupMessage()
{
    SgDebug() <<
        "Fuego " << FuegoMainUtil::Version() << "\n"
        "Copyright (C) 2009-10 by the authors of the Fuego project.\n"
        "This program comes with ABSOLUTELY NO WARRANTY. This is\n"
        "free software and you are welcome to redistribute it under\n"
        "certain conditions. Type `fuego-license' for details.\n\n";
}

} // namespace

//----------------------------------------------------------------------------

int main(int argc, char** argv)
{
    if (argc > 0 && argv != 0)
    {
        g_programPath = argv[0];
        g_programDir = GetProgramDir(argv[0]);
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
        PrintStartupMessage();
        SgRandom::SetSeed(g_srand);
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

