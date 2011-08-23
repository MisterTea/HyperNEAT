#ifndef _JGTL_LOCATEDEXCEPTION_H_INCLUDED
#define _JGTL_LOCATEDEXCEPTION_H_INCLUDED

#include <string>
#include <sstream>
#include <cstdio>

#define CREATE_PAUSE(X) {cout << X << "\nPress enter to continue" << endl;string line;getline(cin,line);}

#ifdef _MSC_VER
//Nice try, visual studio
#pragma warning (disable : 4996)
#endif

namespace JGTL
{
	/**
	* @class LocatedException
	* @brief This class handles throwing exceptions which include the file and
	* line number.
	*
	* Example: 
	* 
	* throw CREATE_LOCATEDEXCEPTION_INFO("ERROR!");
	*
	* @author Jason Gauci
	* @Date 2008
	*/

	class LocatedException : public std::exception
	{
		char text[4096];

	public:
		LocatedException(const char *_reason,const char *_fileName,const int _lineNumber)
		{
			sprintf(text,"%s at %s:%d",_reason,_fileName,_lineNumber);
		}

		LocatedException(const std::string &_reason,const char *_fileName,const int _lineNumber)
		{
			sprintf(text,"%s at %s:%d",_reason.c_str(),_fileName,_lineNumber);
		}

		virtual const char* what() const throw()
		{
			return text;
		}
	};

}

#define CREATE_LOCATEDEXCEPTION_INFO(X) JGTL::LocatedException( (X) ,__FILE__,__LINE__); 

#endif // _JGTL_LOCATEDEXCEPTION_H_INCLUDED
