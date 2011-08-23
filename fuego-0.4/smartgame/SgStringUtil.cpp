//----------------------------------------------------------------------------
/** @file SgStringUtil.cpp
    See SgStringUtil.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "SgStringUtil.h"

#include <cctype>
#include <sstream>

using namespace std;

//----------------------------------------------------------------------------

vector<string> SgStringUtil::SplitArguments(string s)
{
    vector<string> result;
    bool escape = false;
    bool inString = false;
    ostringstream token;
    for (size_t i = 0; i < s.size(); ++i)
    {
        char c = s[i];
        if (c == '"' && ! escape)
        {
            if (inString)
            {
                result.push_back(token.str());
                token.str("");
            }
            inString = ! inString;
        }
        else if (isspace(c) && ! inString)
        {
            if (! token.str().empty())
            {
                result.push_back(token.str());
                token.str("");
            }
        }
        else
            token << c;
        escape = (c == '\\' && ! escape);
    }
            if (! token.str().empty())
        result.push_back(token.str());
    return result;
}

//----------------------------------------------------------------------------
