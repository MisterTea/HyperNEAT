#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
using namespace std;

#include "JGTL_Variant.h"
#include "JGTL_PolyVariant.h"
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
	try
	{
		Variant<int> v;
		Variant<int,double> v2;
		Variant<int,double,Arr> v3;

		int intType=3;
		double doubleType=2.4;
		Arr arrType;
		strcpy(arrType.info,"This is a test!");

		cout << "Test" << endl;
		cout << sizeof(int) << ',' << sizeof(double) << ',' <<
			sizeof(string) << ',' << sizeof(Arr) << endl;
		cout << STATIC_MAX_SIZE<int,double,string,Arr>::RESULT << endl;
		cout << sizeof(v3) << ' ' << sizeof(Variant<int,double,Arr>) << endl;

		cout << "*************" << endl;

		v3.setValue<int>(((char)intType));
		cout << (v3.getValue<int>()) << endl;

		cout << v3.isOfType<int>() << ' ' << v3.isOfType<double>() << ' ' << v3.isOfType<Arr>() << endl;

		v3.setValue(doubleType);
		cout << (v3.getValue<double>()) << endl;

		cout << v3.isOfType<int>() << ' ' << v3.isOfType<double>() << ' ' << v3.isOfType<Arr>() << endl;

		v3.setValue(arrType);
		cout << (v3.getValue<Arr>().info) << endl;

		cout << v3.isOfType<int>() << ' ' << v3.isOfType<double>() << ' ' << v3.isOfType<Arr>() << endl;

		cout << endl << "Testing PolyVariant:" << endl;

		{
			PolyVariant<C1,C2,C3> testVariant;

			C2 testC2(10);
			C3 testC3(10.5);

			testVariant.setValue<C2>(testC2);

			cout << testVariant.getValue<C2>().getValue();
			cout << " == " << testVariant->getValue() << endl;
			cout << testVariant.isOfType<C1>() << " " << testVariant.isOfType<C2>() << ' ' << testVariant.isOfType<C3>() << endl;

			testVariant.setValue<C3>(testC3);

			cout << testVariant.getValue<C3>().getValue();
			cout << " == " << testVariant->getValue() << endl;
			cout << testVariant.isOfType<C1>() << " " << testVariant.isOfType<C2>() << ' ' << testVariant.isOfType<C3>() << endl;
		}

		{
			cout << "\n\nTesting polymorphism:\n";

			PolyVariant<FuncClassBase,FuncClass1,FuncClass2,FuncClass3> testVariant;

			double factor = 3.14159/2.0;

			FuncClass1 test1(factor);
			FuncClass2 test2(factor);
			FuncClass3 test3(factor);

			testVariant.setValue(test1);
			cout << sin(factor) << " == " << testVariant->process() << endl;
			{
				PolyVariant<FuncClassBase,FuncClass1,FuncClass2,FuncClass3> p2;
				p2.setValue(test1);
				testVariant = p2;
			}
			cout << sin(factor) << " == " << testVariant->process() << endl;

			{
				PolyVariant<FuncClassBase,FuncClass1,FuncClass2,FuncClass3> p2;
				p2.setValue(test2);
				testVariant = p2;
			}
			cout << cos(factor) << " == " << testVariant->process() << endl;
			testVariant.setValue(test2);
			cout << cos(factor) << " == " << testVariant->process() << endl;

			testVariant.setValue<FuncClass3>(test3);
			cout << tan(factor) << " == " << testVariant->process() << endl;
			{
				PolyVariant<FuncClassBase,FuncClass1,FuncClass2,FuncClass3> p2;
				p2.setValue(test3);
				testVariant = p2;
			}
			cout << tan(factor) << " == " << testVariant->process() << endl;
		}
		
		{
			Variant<string,int,double> myVariant;
			{
				string testString = "Test of string";
				myVariant.setValue<string>(testString);
			}
			cout << myVariant.getValue<string>();
		}
	}
	catch(const std::exception &ex)
	{
		cout << ex.what() << endl;
	}

	CREATE_PAUSE("");
	return 0;
}
