#include "JGTL_IntegralUnits.h"
#include "JGTL_FloatingUnits.h"
using namespace JGTL;

#include <iostream>
#include <complex>
#include <cstdlib>
using namespace std;

typedef IntegralUnits<unsigned long long,1ULL,true>				Milliseconds;
typedef IntegralUnits<unsigned long long,1000ULL,true>			Seconds;
typedef IntegralUnits<unsigned long long,60000ULL,true>			Minutes;
typedef IntegralUnits<unsigned long long,3600000ULL,true>		Hours;
typedef IntegralUnits<unsigned long long,86400000ULL,true>		Days;
typedef IntegralUnits<unsigned long long,2592000000ULL,true>		Months;

typedef IntegralUnits<unsigned long long,1ULL,true>				Inches;
typedef IntegralUnits<unsigned long long,12ULL,true>			Feet;
typedef IntegralUnits<unsigned long long,36ULL,true>			Yards;
typedef IntegralUnits<unsigned long long,63360ULL,true>			Miles;

typedef FloatingUnits<double,1ULL,1ULL>			Radians;
typedef FloatingUnits<double,174532777ULL,10000000000ULL>		Degrees;

int main()
{
	{
		Months months(23);
		cout << months.getValue() << " months" << endl;
		Days days = months;
		cout << days.getValue() << " days" << endl;
		Hours hrs(months);
		cout << hrs.getValue() << " hrs." << endl;
		Minutes m(hrs);
		cout << m.getValue() << " min." << endl;
		Seconds s(m);
		cout << s.getValue() << " sec." << endl;
		Milliseconds ms;
		ms = s;
		cout << ms.getValue() << " ms" << endl;
	}

	cout << "---------------------\n";

	{
		Minutes min(10);
		cout << min.getValue() << " min. " << endl;
		Seconds sec = min;
		cout << sec.getValue() << " sec. " << endl;
	}

	cout << "---------------------\n";

	{
		Miles miles(24859); //circumfrence of the Earth
		cout << miles.getValue() << " miles" << endl;
		Yards yards(miles);
		cout << yards.getValue() << " yd." << endl;
		Feet feet(miles);
		cout << feet.getValue() << " ft." << endl;
		Inches inches(miles);
		cout << inches.getValue() << " in." << endl;
	}

	cout << "---------------------\n";

	{
		Minutes minutes(1000000ULL);
		Yards yards(minutes);
		cout << "There are " << (yards.getValue()/1000000.0) << " yards in a minute!\n";
	}

	cout << "---------------------\n";

	{
		Inches inches;
		Feet feet;
		Yards yards;

		string s = "12 12 12";

		std::istringstream iss(s);

		iss >> inches >> feet >> yards;

		cout << inches << ' ' << feet << ' ' << yards << endl;

		cout << inches << ' ' << ((Inches)feet) << ' ' << ((Inches)yards) << endl;
	}

	cout << "---------------------\n";

	{
		Radians r;

		r = 3.14159;

		cout << r << " radians is " << (Degrees)r << " degrees\n";

		Degrees d(r);

		Degrees d2;

		d2 = r;

		cout << (Degrees)r << " == " << d << " == " << d2 << endl;
	}

	system("PAUSE");

	return 0;
}

