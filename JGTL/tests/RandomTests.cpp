#include "JGTL_Random.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>

using namespace JGTL;
using namespace std;

int main()
{
	for(int a=2;a<20;a++)
	{
		Random RNG(a/2);

		cout << RNG.getInt(1) << ", ";
		cout << RNG.getInt(1) << ", ";
		cout << RNG.getInt(1) << ", ";
		cout << RNG.getInt(1) << ", ";
		cout << RNG.getInt(1) << ", ";

		cout << RNG.getInt(10) << ", ";
		cout << RNG.getInt(10) << ", ";
		cout << RNG.getInt(10) << ", ";
		cout << RNG.getInt(10) << ", ";
		cout << RNG.getInt(10) << ", ";

		cout << RNG.getInt(100) << ", ";
		cout << RNG.getInt(100) << ", ";
		cout << RNG.getInt(100) << ", ";
		cout << RNG.getInt(100) << ", ";
		cout << RNG.getInt(100) << ", ";
		cout << endl;

		cout << RNG.getInt(1,100) << ", ";
		cout << RNG.getInt(1,100) << ", ";
		cout << RNG.getInt(1,100) << ", ";
		cout << RNG.getInt(1,100) << ", ";
		cout << RNG.getInt(1,100) << ", ";

		cout << RNG.getInt(100,100) << ", ";
		cout << RNG.getInt(100,100) << ", ";
		cout << RNG.getInt(100,100) << ", ";
		cout << RNG.getInt(100,100) << ", ";
		cout << RNG.getInt(100,100) << ", ";

		cout << RNG.getInt(10,100) << ", ";
		cout << RNG.getInt(10,100) << ", ";
		cout << RNG.getInt(10,100) << ", ";
		cout << RNG.getInt(10,100) << ", ";
		cout << RNG.getInt(10,100) << ", ";
		cout << endl;

		cout << RNG.getDouble() << ", ";
		cout << RNG.getDouble() << ", ";
		cout << RNG.getDouble() << ", ";
		cout << RNG.getDouble() << ", ";
		cout << RNG.getDouble() << ", ";

		cout << RNG.getDouble(0.0,1.0) << ", ";
		cout << RNG.getDouble(0.0,1.0) << ", ";
		cout << RNG.getDouble(0.0,1.0) << ", ";
		cout << RNG.getDouble(0.0,1.0) << ", ";
		cout << RNG.getDouble(0.0,1.0) << ", ";

		cout << RNG.getDouble(0.0,10.0) << ", ";
		cout << RNG.getDouble(0.0,10.0) << ", ";
		cout << RNG.getDouble(0.0,10.0) << ", ";
		cout << RNG.getDouble(0.0,10.0) << ", ";
		cout << RNG.getDouble(0.0,10.0) << ", ";

		cout << RNG.getDouble(0.0,100.0) << ", ";
		cout << RNG.getDouble(0.0,100.0) << ", ";
		cout << RNG.getDouble(0.0,100.0) << ", ";
		cout << RNG.getDouble(0.0,100.0) << ", ";
		cout << RNG.getDouble(0.0,100.0) << ", ";
		cout << endl;

		cout << RNG.rollD2() << ", ";
		cout << RNG.rollD2() << ", ";
		cout << RNG.rollD2() << ", ";
		cout << RNG.rollD2() << ", ";
		cout << RNG.rollD2() << ", ";

		cout << RNG.rollD4() << ", ";
		cout << RNG.rollD4() << ", ";
		cout << RNG.rollD4() << ", ";
		cout << RNG.rollD4() << ", ";
		cout << RNG.rollD4() << ", ";

		cout << RNG.rollD6() << ", ";
		cout << RNG.rollD6() << ", ";
		cout << RNG.rollD6() << ", ";
		cout << RNG.rollD6() << ", ";
		cout << RNG.rollD6() << ", ";

		cout << RNG.rollD8() << ", ";
		cout << RNG.rollD8() << ", ";
		cout << RNG.rollD8() << ", ";
		cout << RNG.rollD8() << ", ";
		cout << RNG.rollD8() << ", ";

		cout << RNG.rollD10() << ", ";
		cout << RNG.rollD10() << ", ";
		cout << RNG.rollD10() << ", ";
		cout << RNG.rollD10() << ", ";
		cout << RNG.rollD10() << ", ";

		cout << RNG.rollD20() << ", ";
		cout << RNG.rollD20() << ", ";
		cout << RNG.rollD20() << ", ";
		cout << RNG.rollD20() << ", ";
		cout << RNG.rollD20() << ", ";

		cout << RNG.rollD100() << ", ";
		cout << RNG.rollD100() << ", ";
		cout << RNG.rollD100() << ", ";
		cout << RNG.rollD100() << ", ";
		cout << RNG.rollD100() << ", ";
		cout << endl;

		cout << RNG.rollDice(3,6) << ", ";
		cout << RNG.rollDice(3,6) << ", ";
		cout << RNG.rollDice(3,6) << ", ";
		cout << RNG.rollDice(3,6) << ", ";
		cout << RNG.rollDice(3,6) << ", ";

		cout << RNG.rollDice(3,6,2) << ", ";
		cout << RNG.rollDice(3,6,2) << ", ";
		cout << RNG.rollDice(3,6,2) << ", ";
		cout << RNG.rollDice(3,6,2) << ", ";
		cout << RNG.rollDice(3,6,2) << ", ";
		cout << endl;

		cout << RNG.getNormalDistributionDouble(1.0,-3.0,3.0) << ", ";
		cout << RNG.getNormalDistributionDouble(1.0,-3.0,3.0) << ", ";
		cout << RNG.getNormalDistributionDouble(1.0,-3.0,3.0) << ", ";
		cout << RNG.getNormalDistributionDouble(1.0,-3.0,3.0) << ", ";
		cout << RNG.getNormalDistributionDouble(1.0,-3.0,3.0) << ", ";
		cout << RNG.getNormalDistributionDouble(1.0,-3.0,3.0) << ", ";
		cout << RNG.getNormalDistributionDouble(1.0,-3.0,3.0) << ", ";
		cout << RNG.getNormalDistributionDouble(1.0,-3.0,3.0) << ", ";
		cout << RNG.getNormalDistributionDouble(1.0,-3.0,3.0) << ", ";
		cout << RNG.getNormalDistributionDouble(1.0,-3.0,3.0) << ", ";
		cout << endl;

		cout << endl;
	}

	system("PAUSE");

	return 0;
}

