#ifndef __JGTL_CONSOLE_COLOR_H__
#define __JGTL_CONSOLE_COLOR_H__

#include "JGTL_LocatedException.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

#define UNIX_TEXT_RESET					"\033[0m"

#define UNIX_TEXT_BRIGHT				"\033[1m"
#define UNIX_TEXT_DIM					"\033[2m"
#define UNIX_TEXT_UNDERLINE				"\033[3m"
#define UNIX_TEXT_REVERSE_COLORS		"\033[7m"

#define UNIX_TEXT_FG_BLACK				"\033[30m"
#define UNIX_TEXT_FG_RED				"\033[31m"
#define UNIX_TEXT_FG_GREEN				"\033[32m"
#define UNIX_TEXT_FG_YELLOW				"\033[33m"
#define UNIX_TEXT_FG_BLUE				"\033[34m"
#define UNIX_TEXT_FG_MAGENTA			"\033[35m"
#define UNIX_TEXT_FG_CYAN				"\033[36m"
#define UNIX_TEXT_FG_WHITE				"\033[37m"

#define UNIX_TEXT_BG_BLACK				"\033[40m"
#define UNIX_TEXT_BG_RED				"\033[41m"
#define UNIX_TEXT_BG_GREEN				"\033[42m"
#define UNIX_TEXT_BG_YELLOW				"\033[43m"
#define UNIX_TEXT_BG_BLUE				"\033[44m"
#define UNIX_TEXT_BG_MAGENTA			"\033[45m"
#define UNIX_TEXT_BG_CYAN				"\033[46m"
#define UNIX_TEXT_BG_WHITE				"\033[47m"

namespace JGTL
{

	enum ConsoleTextColor
	{
		CTC_BLACK,
		CTC_RED,
		CTC_GREEN,
		CTC_BLUE,
		CTC_YELLOW,
		CTC_MAGENTA,
		CTC_CYAN,
		CTC_WHITE,

		CTC_BRIGHT_RED,
		CTC_BRIGHT_GREEN,
		CTC_BRIGHT_YELLOW,
		CTC_BRIGHT_BLUE,
		CTC_BRIGHT_MAGENTA,
		CTC_BRIGHT_CYAN,
		CTC_BRIGHT_WHITE
	};

#ifdef _MSC_VER
}
#include <windows.h>
namespace JGTL
{

	inline void SetConsoleText(ConsoleTextColor fg,ConsoleTextColor bg,bool underline)
	{
		WORD attributes=0;
		if(underline)
			attributes |= COMMON_LVB_UNDERSCORE;

		switch(fg)
		{
		case CTC_BLACK:
			break;

		case CTC_BRIGHT_RED:
			attributes |= FOREGROUND_INTENSITY;
		case CTC_RED:
			attributes |= FOREGROUND_RED;
			break;

		case CTC_BRIGHT_GREEN:
			attributes |= FOREGROUND_INTENSITY;
		case CTC_GREEN:
			attributes |= FOREGROUND_GREEN;
			break;
		case CTC_BRIGHT_BLUE:
			attributes |= FOREGROUND_INTENSITY;
		case CTC_BLUE:
			attributes |= FOREGROUND_BLUE;
			break;

		case CTC_BRIGHT_YELLOW:
			attributes |= FOREGROUND_INTENSITY;
		case CTC_YELLOW:
			attributes |= FOREGROUND_RED | FOREGROUND_GREEN;
			break;

		case CTC_BRIGHT_MAGENTA:
			attributes |= FOREGROUND_INTENSITY;
		case CTC_MAGENTA:
			attributes |= FOREGROUND_RED | FOREGROUND_BLUE;
			break;

		case CTC_BRIGHT_CYAN:
			attributes |= FOREGROUND_INTENSITY;
		case CTC_CYAN:
			attributes |= FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;

		case CTC_BRIGHT_WHITE:
			attributes |= FOREGROUND_INTENSITY;
		case CTC_WHITE:
			attributes |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			break;
		}

		switch(bg)
		{
		case CTC_BLACK:
			break;

		case CTC_BRIGHT_RED:
			attributes |= BACKGROUND_INTENSITY;
		case CTC_RED:
			attributes |= BACKGROUND_RED;
			break;

		case CTC_BRIGHT_GREEN:
			attributes |= BACKGROUND_INTENSITY;
		case CTC_GREEN:
			attributes |= BACKGROUND_GREEN;
			break;
		case CTC_BRIGHT_BLUE:
			attributes |= BACKGROUND_INTENSITY;
		case CTC_BLUE:
			attributes |= BACKGROUND_BLUE;
			break;

		case CTC_BRIGHT_YELLOW:
			attributes |= BACKGROUND_INTENSITY;
		case CTC_YELLOW:
			attributes |= BACKGROUND_RED | BACKGROUND_GREEN;
			break;

		case CTC_BRIGHT_MAGENTA:
			attributes |= BACKGROUND_INTENSITY;
		case CTC_MAGENTA:
			attributes |= BACKGROUND_RED | BACKGROUND_BLUE;
			break;

		case CTC_BRIGHT_CYAN:
			attributes |= BACKGROUND_INTENSITY;
		case CTC_CYAN:
			attributes |= BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;

		case CTC_BRIGHT_WHITE:
			attributes |= BACKGROUND_INTENSITY;
		case CTC_WHITE:
			attributes |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
			break;
		}

		SetConsoleTextAttribute( 
			GetStdHandle( STD_OUTPUT_HANDLE ), 
			attributes
			);
	}
#else
	inline void SetConsoleText(ConsoleTextColor fg,ConsoleTextColor bg,bool underline)
	{
		if(
            fg != CTC_BLACK && bg!=CTC_BLACK &&
            (
    			(fg>=CTC_BRIGHT_RED&&bg<CTC_BRIGHT_RED) ||
    			(bg>=CTC_BRIGHT_RED&&fg<CTC_BRIGHT_RED)
			)
          )
		{
            //Cannot mix bright and dark colors, assume the BG is whatever the FG is
    	    if(fg>=CTC_BRIGHT_RED) bg = ConsoleTextColor(int(bg)-7);
            else bg = ConsoleTextColor(int(bg)+7);
		}

		printf(UNIX_TEXT_RESET);

		if(underline)
			printf(UNIX_TEXT_UNDERLINE);

		switch(fg)
		{
		case CTC_BLACK:
			break;

		case CTC_BRIGHT_RED:
			printf(UNIX_TEXT_BRIGHT);
		case CTC_RED:
			printf(UNIX_TEXT_FG_RED);
			break;

		case CTC_BRIGHT_GREEN:
			printf(UNIX_TEXT_BRIGHT);
		case CTC_GREEN:
			printf(UNIX_TEXT_FG_GREEN);
			break;

		case CTC_BRIGHT_BLUE:
			printf(UNIX_TEXT_BRIGHT);
		case CTC_BLUE:
			printf(UNIX_TEXT_FG_BLUE);
			break;

		case CTC_BRIGHT_YELLOW:
			printf(UNIX_TEXT_BRIGHT);
		case CTC_YELLOW:
			printf(UNIX_TEXT_FG_YELLOW);
			break;

		case CTC_BRIGHT_MAGENTA:
			printf(UNIX_TEXT_BRIGHT);
		case CTC_MAGENTA:
			printf(UNIX_TEXT_FG_MAGENTA);
			break;

		case CTC_BRIGHT_CYAN:
			printf(UNIX_TEXT_BRIGHT);
		case CTC_CYAN:
			printf(UNIX_TEXT_FG_CYAN);
			break;

		case CTC_BRIGHT_WHITE:
			printf(UNIX_TEXT_BRIGHT);
		case CTC_WHITE:
			printf(UNIX_TEXT_FG_WHITE);
			break;
		}

		switch(bg)
		{
		case CTC_BLACK:
			break;

		case CTC_BRIGHT_RED:
			printf(UNIX_TEXT_BRIGHT);
		case CTC_RED:
			printf(UNIX_TEXT_BG_RED);
			break;

		case CTC_BRIGHT_GREEN:
			printf(UNIX_TEXT_BRIGHT);
		case CTC_GREEN:
			printf(UNIX_TEXT_BG_GREEN);
			break;

		case CTC_BRIGHT_BLUE:
			printf(UNIX_TEXT_BRIGHT);
		case CTC_BLUE:
			printf(UNIX_TEXT_BG_BLUE);
			break;

		case CTC_BRIGHT_YELLOW:
			printf(UNIX_TEXT_BRIGHT);
		case CTC_YELLOW:
			printf(UNIX_TEXT_BG_YELLOW);
			break;

		case CTC_BRIGHT_MAGENTA:
			printf(UNIX_TEXT_BRIGHT);
		case CTC_MAGENTA:
			printf(UNIX_TEXT_BG_MAGENTA);
			break;

		case CTC_BRIGHT_CYAN:
			printf(UNIX_TEXT_BRIGHT);
		case CTC_CYAN:
			printf(UNIX_TEXT_BG_CYAN);
			break;

		case CTC_BRIGHT_WHITE:
			printf(UNIX_TEXT_BRIGHT);
		case CTC_WHITE:
			printf(UNIX_TEXT_BG_WHITE);
			break;
		}

		fflush(stdout);
	}
#endif

	//Helper Functions
	inline void SetConsoleText(ConsoleTextColor fg)
	{
		SetConsoleText(fg,CTC_BLACK,false);
	}

	inline void SetConsoleText(ConsoleTextColor fg,ConsoleTextColor bg)
	{
		SetConsoleText(fg,bg,false);
	}

	inline void SetConsoleText(ConsoleTextColor fg,bool underline)
	{
		SetConsoleText(fg,CTC_BLACK,underline);
	}

#ifdef _MSC_VER
	inline void ResetConsoleText()
	{
		SetConsoleText(CTC_WHITE,CTC_BLACK,false);
	}
#else
	inline void ResetConsoleText()
	{
		printf(UNIX_TEXT_RESET);
	}
#endif

	class ConsoleText
	{
		ConsoleTextColor fg,bg;
		bool underline;
        bool reset;

	public:
		ConsoleText(ConsoleTextColor _fg,ConsoleTextColor _bg,bool _underline)
			:
		fg(_fg),
			bg(_bg),
			underline(_underline),
            reset(false)
		{
		}

		ConsoleText(ConsoleTextColor _fg,ConsoleTextColor _bg)
			:
		fg(_fg),
			bg(_bg),
			underline(false),
            reset(false)
		{
		}

		ConsoleText(ConsoleTextColor _fg)
			:
		fg(_fg),
			bg(CTC_BLACK),
			underline(false),
            reset(false)
		{
		}

		ConsoleText()
			:
		fg(CTC_WHITE),
			bg(CTC_BLACK),
			underline(false),
            reset(true)
		{
		}

		friend inline std::ostream& operator<<(std::ostream& stream,const ConsoleText &ct);
		friend inline std::istream& operator>>(std::istream& stream,ConsoleText &ct);
	};

	inline std::ostream& operator<<(std::ostream& stream,const ConsoleText &ct)
	{
        if(ct.reset)
        {
            ResetConsoleText();
        }
        else
        {
    		SetConsoleText(ct.fg,ct.bg,ct.underline);
        }
		return stream;
	}

	inline std::istream& operator>>(std::istream& stream,ConsoleText &ct)
	{
        if(ct.reset)
        {
            ResetConsoleText();
        }
        else
        {
		    SetConsoleText(ct.fg,ct.bg,ct.underline);
        }
		return stream;
	}
}

#endif

