//----------------------------------------------------------------------------
/** @file SgStringUtil.h */
//----------------------------------------------------------------------------

#ifndef SG_STRINGUTIL_H
#define SG_STRINGUTIL_H

#include <vector>
#include <string>

//----------------------------------------------------------------------------

/** String utility functions. */
namespace SgStringUtil
{
    /** Split command line into arguments.
        Allows " for words containing whitespaces.
    */
    std::vector<std::string> SplitArguments(std::string s);
}

//----------------------------------------------------------------------------

#endif // SG_STRINGUTIL_H
