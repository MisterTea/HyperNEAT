//----------------------------------------------------------------------------
/** @file GoMotive.cpp
    See GoMotive.h
*/
//----------------------------------------------------------------------------

#include "SgSystem.h"
#include "GoMotive.h"

#include <iostream>

using namespace std;

//----------------------------------------------------------------------------

ostream& operator<<(ostream& out, GoMotive motive)
{
    static const char* s_string[_GO_NU_MOTIVE] =
    {
        "None",
        "Random",
        "Capture",
        "Escape",
        "Connect",
        "Cut",
        "ToDivide",
        "Urgent",
        "Expand",
        "Stabilize",
        "Reduce",
        "Defend",
        "Invade",
        "Sente",
        "Forced",
        "OneEye",
        "TwoEye",
        "Yose",
        "Attack",
        "Save",
        "DoubleAtari",
        "Atari",
        "KoThreat"
    };

    out << s_string[motive];
    return out;
}

//----------------------------------------------------------------------------

ostream& operator<<(ostream& out, GoModifier modifier)
{
    static const char* s_string[_GO_NU_MODIFIER] =
    {
        "normal",
        "veryBig",
        "big",
        "small",
        "verySmall"
    };

    out << s_string[modifier];
    return out;
}

//----------------------------------------------------------------------------

