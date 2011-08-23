//----------------------------------------------------------------------------
/** @file SgCmdLineOpt.h
    Parser for command line options.
*/
//----------------------------------------------------------------------------

#ifndef SG_CMDLINEOPT_H
#define SG_CMDLINEOPT_H

#include <map>
#include <string>
#include <vector>

//----------------------------------------------------------------------------

/** Parser for command line options.

    @deprecated Use boost::program_options instead

    Options start with the character '-'.
    The end of options can be indicated with "--" to allow arguments
    beginning with '-'.

    Example:<br>
    In this example the allowed options are:
    a required option with integer argument "-optint",
    a option with string argument "-optstr" and default value ""
    and a option with no argument "-optbool" and default off.
    @verbatim    
    static bool s_optBool;

    static int s_optInt;

    static string s_optStr;

    static void ParseOptions(int argc, char** argv)
    {
        SgCmdLineOpt cmdLineOpt;
        vector<string> specs;
        specs.push_back("optint:");
        specs.push_back("optstr:");
        specs.push_back("optbool");
        cmdLineOpt.Parse(argc, argv, specs);
        if (! cmdLineOpt.Contains("optint"))
            throw SgException("Requires option -optint");
        cmdLineOpt.GetInteger("optint", s_optInt);
        s_optStr = cmdLineOpt.GetString("optstr", "");
        s_optBool = cmdLineOpt.Contains("optbool");
    }

    int main(int argc, char** argv)
    {
        try
        {
            ParseOptions(argc, argv);
            // ...
        }
        catch (const std::exception& e)
        {
            SgDebug() << e.what() << '\n';
            return 1;
        }
        return 0;
    }
    @endverbatim
*/
class SgCmdLineOpt
{
public:
    SgCmdLineOpt();
    
    bool Contains(const char* option) const;

    /** Get a list of the remaining command line arguments that are not an
        option.
    */
    const std::vector<std::string>& GetArguments() const;

    /** Get value of a floating point option.
        @throws SgException on error
    */        
    double GetDouble(const char* option) const;

    /** Get value of a floating point option or use default value.
        @throws SgException on error
    */        
    double GetDouble(const char* option, double defaultValue) const;

    /** Get value of an integer option.
        @throws SgException on error
    */        
    int GetInteger(const char* option) const;

    /** Get value of an integer option or use default value.
        @throws SgException on error
    */        
    int GetInteger(const char* option, int defaultValue) const;

    /** Get value of a string option. */
    std::string GetString(const char* option) const;

    /** Get value of a string option or use default value. */        
    std::string GetString(const char* option,
                          const std::string& defaultValue) const;

    /** Parse options from main(argc, argv).
        @param argc Number of elements in argv
        @param argv Argument vector from main().
        @param specs
        Array of valid options (not including the leading '-').
        Append ':' to options that are allowed to have an argument.
        @throws SgException on error
    */
    void Parse(int argc, char* argv[], const std::vector<std::string>& specs);

    /** Parse options from user created array.
        Uses const char* arguments. The only portable version of a C++ main()
        function uses char* arguments, but for testing this class with a
        user created array, it is better to use const char*, because
        assigning a string constant to char* causes compiler warnings with
        some compilers.
    */
    void Parse(int argc, const char* argv[],
               const std::vector<std::string>& specs);

private:
    std::vector<std::string> m_args;

    std::map<std::string, std::string> m_map;
};

//----------------------------------------------------------------------------

#endif // SG_CMDLINEOPT_H
