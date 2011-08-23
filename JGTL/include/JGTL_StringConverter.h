#ifndef JGTL_STRINGCONVERTER_H_INCLUDED
#define JGTL_STRINGCONVERTER_H_INCLUDED

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "JGTL_LocatedException.h"

namespace JGTL
{
	/**
	* Handles conversion to and from strings
	*/

	template<typename T>
	inline T stringTo(const std::string& s)
	{
		T x;
		std::istringstream iss(s);
		iss >> x;
		return x;
	}

	//Special overload for handling string-string
	template<>
    inline std::string stringTo(const std::string& s)
	{
		return s;
	}

	template<typename T>
	inline void stringTo(const std::string& s,T& x)
	{
		std::istringstream iss(s);
		iss >> x;
	}

	template<typename T>
	inline std::string toString(const T& x)
	{
		std::ostringstream oss;
		oss << x;
		return oss.str();
	}

	template<typename T>
	inline std::string toString(const T& x,int width)
	{
		std::ostringstream oss;
		oss << std::setfill('0') << std::setw(width) << x;
		return oss.str();
	}

	template<class T>
	inline T getIndexFromName(const char *name,const char **names,T numNames)
	{
		for (T a=T(0);a<numNames;a = T(int(a)+1))
		{
			if (iequals(name,names[a]))
				return a;
		}

		return numNames;
	}

	template<class T>
	inline T getIndexFromName(const std::string &name,const char **names,T numNames)
	{
		for (T a=T(0);a<numNames;a = T(int(a)+1))
		{
			if (iequals(name.c_str(),names[a]))
				return a;
		}

		return numNames;
	}

	template<class T>
	inline T getIndexFromNameSafe(const char *name,const char **names,T numNames)
	{
		for (T a=T(0);a<numNames;a = T(int(a)+1))
		{
			if (iequals(name,names[a]))
				return a;
		}

        throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
	}

	template<class T>
	inline T getIndexFromNameSafe(const std::string &name,const char **names,T numNames)
	{
		for (T a=T(0);a<numNames;a = T(int(a)+1))
		{
			if (iequals(name.c_str(),names[a]))
				return a;
		}

        throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
	}
}

#endif // JGTL_STRINGCONVERTER_H_INCLUDED
