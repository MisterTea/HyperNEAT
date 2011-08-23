#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
using namespace std;

#include "JGTL_Variant.h"
#include "JGTL_PolyVariant.h"
#include "JGTL_QuickProf.h"
using namespace JGTL;

struct Arr
{
	char info[234789];
};

class C1
{
public:
	C1()
	{
	}

	virtual double getValue() = 0;
};

class C2
{
public:
	int value;

	C2(int _value)
		:
	value(_value)
	{
	}

	virtual double getValue()
	{
		return value;
	}
};

class C3
{
public:
	double value;

	C3(double _value)
		:
	value(_value)
	{
	}

	virtual double getValue()
	{
		return value;
	}
};

class FuncClassBase
{
public:
	double factor;

	FuncClassBase(double _factor)
		:
	factor(_factor)
	{
	}

	virtual double process() = 0;
};

class FuncClass1 : FuncClassBase
{
public:
	FuncClass1(double _factor)
		:
	FuncClassBase(_factor)
	{
	}

	virtual double process()
	{
		return sin(factor);
	}
};

class FuncClass2 : FuncClassBase
{
public:
	FuncClass2(double _factor)
		:
	FuncClassBase(_factor)
	{
	}

	virtual double process()
	{
		return cos(factor);
	}
};

class FuncClass3 : FuncClassBase
{
public:
	FuncClass3(double _factor)
		:
	FuncClassBase(_factor)
	{
	}

	virtual double process()
	{
		return tan(factor);
	}
};

int main()
{
#ifdef _MSC_VER
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 

	//_CrtSetBreakAlloc(506);
#endif

	//try
	{
		Profiler *profiler = new Profiler();
		profiler->init();

		vector<FuncClassBase*> controlGroup;
		vector< PolyVariant<FuncClassBase,FuncClass1,FuncClass2,FuncClass3> > testGroup;

		for(int cycle=0;cycle<1;cycle++)
		{
			profiler->beginBlock("Control Group");
			for(int a=0;a<100;a++)
			{
				controlGroup.push_back((FuncClassBase*)new FuncClass1(1.0));
				controlGroup.push_back((FuncClassBase*)new FuncClass2(1.0));
				controlGroup.push_back((FuncClassBase*)new FuncClass3(1.0));
			}

			for(int a=0;a<(int)controlGroup.size();a++)
			{
				delete controlGroup[a];
			}
			controlGroup.clear();
			profiler->endBlock("Control Group");

			profiler->beginBlock("Variant Test Group");
			PolyVariant<FuncClassBase,FuncClass1,FuncClass2,FuncClass3> p2;
			for(int a=0;a<1;a++)
			{
				{
					p2.setValue(FuncClass1(1.0));
					testGroup.push_back(p2);

					p2.setValue(FuncClass2(1.0));
					testGroup.push_back(p2);

					p2.setValue(FuncClass3(1.0));
					testGroup.push_back(p2);

                    cout << sin(1.0) << " ?= " << testGroup[0].getBaseValue()->process() << endl;
                    cout << cos(1.0) << " ?= " << testGroup[1].getBaseValue()->process() << endl;
                    cout << tan(1.0) << " ?= " << testGroup[2].getBaseValue()->process() << endl;
				}
			}
			testGroup.clear();
			profiler->endBlock("Variant Test Group");

            {
                Variant<string,int,double> testVar(string("This is a string!"));
                cout << testVar.getValueRef<string>() << endl;
            }
            {
                Variant<string,int,double> testVar(4);
                cout << testVar.getValueRef<int>() << endl;
            }
            {
                Variant<string,int,double> testVar(10.5);
                cout << testVar.getValueRef<double>() << endl;
            }

            {
                Variant<string,int,double> testVar;
                testVar = string("This is a string!");
                cout << testVar.getValueRef<string>() << endl;
                testVar = 4;
                cout << testVar.getValueRef<int>() << endl;
                testVar = 10.5;
                cout << testVar.getValueRef<double>() << endl;
                testVar = string("This is a string!");
                cout << testVar.getValueRef<string>() << endl;
                testVar = 4;
                cout << testVar.getValueRef<int>() << endl;
                testVar = 10.5;
                cout << testVar.getValueRef<double>() << endl;
            }


			profiler->endCycle();
			cout << "Done with cycle: " << cycle << endl;
			profiler->beginCycle();
		}

		cout << profiler->getSummary(MILLISECONDS) << endl;
        delete profiler;
	}
    /*
	catch(const std::exception &ex)
	{
		cout << ex.what() << endl;
	}
    */

	system("PAUSE");
	return 0;
}
