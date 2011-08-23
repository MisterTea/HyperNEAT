#include "JGTL_ConsoleColor.h"

#include <stdio.h>
#include <iostream>
using namespace std;
using namespace JGTL;

int main()
{
	SetConsoleText(CTC_GREEN);
	printf("Green!\n");
	cout << "Green!" << endl;
	ResetConsoleText();
	cout << ConsoleText(CTC_BLUE) << "Blue" << ConsoleText() << endl;
	cout << ConsoleText(CTC_BLUE) << "Blue " << ConsoleText(CTC_RED) << "Red" << ConsoleText() << endl;

	CREATE_PAUSE("");

	return 0;
}

