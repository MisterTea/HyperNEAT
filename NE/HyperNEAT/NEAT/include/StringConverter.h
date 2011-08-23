#ifndef STRINGCONVERTER_H_INCLUDED
#define STRINGCONVERTER_H_INCLUDED

#include <iostream>
#include <sstream>

template<typename T>
inline T stringTo(const std::string& s)
{
    T x;
    std::istringstream iss(s);
    iss >> x;
    return x;
}

template<typename T>
inline std::string toString(const T& x)
{
    std::ostringstream oss;
    oss << x;
    return oss.str();
}

#endif // STRINGCONVERTER_H_INCLUDED
