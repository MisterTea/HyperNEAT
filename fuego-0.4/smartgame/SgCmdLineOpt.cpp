//----------------------------------------------------------------------------
/** @file SgCmdLineOpt.cpp
    See SgCmdLineOpt.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgCmdLineOpt.h"

#include <algorithm>
#include <sstream>
#include "SgDebug.h"
#include "SgException.h"

using namespace std;

//----------------------------------------------------------------------------

SgCmdLineOpt::SgCmdLineOpt()
{
}

bool SgCmdLineOpt::Contains(const char* option) const
{
    return (m_map.find(option) != m_map.end());
}

const vector<string>& SgCmdLineOpt::GetArguments() const
{
    return m_args;
}

double SgCmdLineOpt::GetDouble(const char* option) const
{
    return GetDouble(option, 0.0);
}

double SgCmdLineOpt::GetDouble(const char* option, double defaultValue) const
{
    map<string, string>::const_iterator it = m_map.find(option);
    if (it == m_map.end())
        return defaultValue;
    string s = it->second;
    istringstream in(s);
    double value;
    in >> value;
    if (! in)
        throw SgException(string("Option ") + option + " needs float value");
    return value;
}


int SgCmdLineOpt::GetInteger(const char* option) const
{
    return GetInteger(option, 0);
}

int SgCmdLineOpt::GetInteger(const char* option, int defaultValue) const
{
    map<string, string>::const_iterator it = m_map.find(option);
    if (it == m_map.end())
        return defaultValue;
    string s = it->second;
    istringstream in(s);
    int value;
    in >> value;
    if (! in)
        throw SgException(string("Option ") + option
                          + " needs integer value");
    return value;
}

string SgCmdLineOpt::GetString(const char* option) const
{
    return GetString(option, "");
}

string SgCmdLineOpt::GetString(const char* option,
                               const string& defaultValue) const
{
    map<string, string>::const_iterator it = m_map.find(option);
    if (it == m_map.end())
        return defaultValue;
    return it->second;
}

void SgCmdLineOpt::Parse(int argc, char* argv[],
                         const vector<std::string>& specs)
{
    m_args.clear();
    m_map.clear();
    int n = 1;
    bool endOfOptions = false;
    while (n < argc)
    {
        string s = argv[n];
        ++n;
        if (! endOfOptions && s.size() > 0 && s[0] == '-')
        {
            if (s == "--")
            {
                endOfOptions = true;
                continue;
            }
            s = s.substr(1);
            bool needsArg = false;
            vector<string>::const_iterator spec;
            spec = find(specs.begin(), specs.end(), s);
            if (spec == specs.end())
            {
                spec = find(specs.begin(), specs.end(), s + ":");
                if (spec == specs.end())
                    throw SgException("Unknown option -" + s);
                needsArg = true;
            }
            string value;
            if (needsArg)
            {
                if (n >= argc)
                    throw SgException("Option -" + s + " needs value");
                value = argv[n];
                ++n;
                if (value.size() > 0 && value[0] == '-')
                    throw SgException("Option -" + s + " needs value");
            }
            m_map.insert(pair<string, string>(s, value));
        }
        else
            m_args.push_back(s);
    }
}

void SgCmdLineOpt::Parse(int argc, const char* argv[],
                         const vector<std::string>& specs)
{
    Parse(argc, const_cast<char**>(argv), specs);
}

//----------------------------------------------------------------------------

