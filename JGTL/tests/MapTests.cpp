#define DEBUG_POOL_MAP (1)

#include "JGTL_StackMap.h"
#include "JGTL_QuickProf.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <map>

using namespace std;
using namespace JGTL;

#define MAX_RAND_NUM (1000000)

void incRandNum(int &curRandNum)
{
	curRandNum++;
	if(curRandNum == MAX_RAND_NUM)
	{
		curRandNum = 0;
	}
}

struct MyStruct
{
	int i;
	float f;
	double d;

	MyStruct(int _i,float _f,double _d)
		:
	i(_i),
		f(_f),
		d(_d)
	{
	}

	MyStruct()
		:
		i(0),
		f(0.0f),
		d(0.0)
	{
	}

	bool operator==(const MyStruct &other) const
	{
		return i == other.i && d == other.d && f == other.f;
	}

	bool operator<(const MyStruct &other) const
	{
		return i < other.i || 
			(i == other.i && d < other.d) ||
			(i == other.i && d == other.d && f < other.f);
	}
};

int main()
{
#ifdef _MSC_VER
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	//_CrtSetBreakAlloc(160);
#endif

    try
    {
		Profiler *profiler = new Profiler();
		profiler->init();

		//srand((unsigned int)time(NULL));
		srand(0);

		std::string *randStrings = new std::string[MAX_RAND_NUM];

		for(int a=0;a<MAX_RAND_NUM;a++)
		{
			randStrings[a] = "1234567890";

			for(int b=0;b<10;b++)
			{
				randStrings[a][b] = (char)(int(float(rand())/RAND_MAX*26) + 65);
			}
		}

		MyStruct *randStructs = new MyStruct[MAX_RAND_NUM];

		for(int a=0;a<MAX_RAND_NUM;a++)
		{
			randStructs[a].d = float(rand())/RAND_MAX;
			randStructs[a].f = float(rand())/RAND_MAX;
			randStructs[a].i = rand();
		}

		int curRandNum = 0;

		DynamicPoolMap<MyStruct,int> poolMap;
		StackMap<MyStruct,int,1024> stackMap;
		std::map<MyStruct,int> stdMap;

		for(int cycle=0;cycle<100;cycle++)
		{
			int prevCurRandNum = curRandNum;

			for(int a=0;a<100;a++)
			{
				stackMap.clear();

				profiler->beginBlock("StackMap_create");
				for(int b=0;b<1024;b++)
				{
					//stackMap.insert(randStrings[curRandNum],b);
					stackMap.insert(randStructs[curRandNum],b);
					incRandNum(curRandNum);
				}
				profiler->endBlock("StackMap_create");

				profiler->beginBlock("StackMap_iterate");
				for(int e=0;e<20;e++)
				{
					StackMap<MyStruct,int>::iterator it = stackMap.begin();
					for(int d=0;d<(int)stackMap.size();d++)
					{
						it++;
					}
				}
				profiler->endBlock("StackMap_iterate");
			}

			curRandNum = prevCurRandNum;

			for(int a=0;a<100;a++)
			{
				poolMap.clear();

				profiler->beginBlock("PoolMap_create");
				for(int b=0;b<1024;b++)
				{
					//stackMap.insert(randStrings[curRandNum],b);
					poolMap.insert(randStructs[curRandNum],b);
					incRandNum(curRandNum);
				}
				profiler->endBlock("PoolMap_create");

				profiler->beginBlock("PoolMap_iterate");
				for(int e=0;e<20;e++)
				{
					DynamicPoolMap<MyStruct,int>::iterator it = poolMap.begin();
					for(int d=0;d<(int)poolMap.size();d++)
					{
						it++;
					}
				}
				profiler->endBlock("PoolMap_iterate");
			}

			curRandNum = prevCurRandNum;

			for(int a=0;a<100;a++)
			{
				stdMap.clear();

				profiler->beginBlock("std::map_create");
				for(int b=0;b<1024;b++)
				{
					//stdMap[randStrings[curRandNum] ] = b;
					stdMap[randStructs[curRandNum] ] = b;
					incRandNum(curRandNum);
				}
				profiler->endBlock("std::map_create");

				profiler->beginBlock("std::map_iterate");
				for(int e=0;e<20;e++)
				{
					std::map<MyStruct,int>::iterator it = stdMap.begin();
					for(int d=0;d<(int)stdMap.size();d++)
					{
						it++;
					}
				}
				profiler->endBlock("std::map_iterate");
			}

			profiler->endCycle();
			cout << "Done with cycle: " << cycle << endl;
			profiler->beginCycle();
		}

		cout << profiler->getSummary(MILLISECONDS) << endl;

		delete profiler;

		delete[] randStrings;

		delete[] randStructs;
    }
    catch (const std::exception &ex)
    {
        cout << ex.what() << endl;
    }

	system("PAUSE");
}
