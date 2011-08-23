#define DEBUG_POOL_MAP (1)

#include "JGTL_QuickProf.h"

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;
using namespace JGTL;

int main()
{
	try
	{
		Profiler* profiler = new Profiler();

		profiler->init();

		for(int a=0;a<500;a++)
		{
			profiler->beginCycle();

			profiler->beginBlock("Mult_Test12");
			profiler->beginBlock("Mult_Test23");
			profiler->beginBlock("Mult_Test123");

			{
				profiler->beginBlock("Test1");

				for(int b=0;b<100000;b++)
				{
					double somecalc = sin(double(b));
				}

				profiler->endBlock("Test1");
			}

			{
				ProfileBlockHandler handler(profiler,"Test2");

				for(int b=0;b<100000;b++)
				{
					double somecalc = cos(double(b));
				}
			}

			profiler->endBlock("Mult_Test12");
			profiler->beginBlock("Mult_Test23");

			{
				ProfileBlockHandler handler(profiler,"Test3");

				for(int b=0;b<100000;b++)
				{
					double somecalc = tan(double(b));
				}
			}

			profiler->endBlock("Mult_Test23");
			profiler->endBlock("Mult_Test123");

			profiler->endCycle();

			if(a%100==99)
			{
				//cout << profiler->getSummary(SECONDS) << endl;
				cout << profiler->getSummary(MILLISECONDS) << endl;
				//cout << profiler->getSummary(MICROSECONDS) << endl;
				//cout << profiler->getSummary(PERCENT) << endl;
				cout << endl << endl;
				profiler->reset();
			}
		}

		delete profiler;
	}
	catch (const std::exception &ex)
	{
		cout << ex.what() << endl;
	}

	system("PAUSE");
}
