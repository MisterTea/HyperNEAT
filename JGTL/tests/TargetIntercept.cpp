#define __USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

#include "../include/JGTL_Ray2.h"
#include "../include/JGTL_Vector2.h"
#include "../include/JGTL_Quadratic.h"
using namespace JGTL;

int main()
{
	JGTL::Vector2<float> p1(-5.21725,-3.11163);

	float p1x = p1.x;
	float p1y = p1.y;

	float speed = 34.16;

	JGTL::Vector2<float> p2(0.570189,6.19497);

	float p2x = p2.x;
	float p2y = p2.y;

	JGTL::Vector2<float> v2(-1.23323,-34.1377);

	float v2x = v2.x;
	float v2y = v2.y;

	float a = (v2x*v2x + v2y*v2y - speed*speed);
	float b = (2*p2x*v2x - 2*p1x*v2x+ 2*p2y*v2y  - 2*p1y*v2y);
	float c = (p2x*p2x - 2*p2x*p1x + p1x*p1x + p2y*p2y - 2*p2y*p1y + p1y*p1y);

	cout << "a = " << a << endl;	
	cout << "b = " << b << endl;	
	cout << "c = " << c << endl;

	float bestT;

	QuadraticSolution<float> solution = solveQuadratic<float>(a,b,c);

	switch(solution.numSolutions)
	{
		case 0:
			cout << "Uh o, quadratic has no solutions!!\n";
			break;
		case 1:
			bestT = solution.t1;
			break;
		case 2:
			float t1 = solution.t1;
			float t2 = solution.t2;
	
			cout << "Roots for t: " << t1 << ", " << t2 << endl;

			//We want the t that is closest to 0 without going negative
			bestT = t1;
			if(t1 < 0 || (t2 < t1 && t2 >= 0))
			{
				bestT = t2;
			}
			break;
	}

	if(bestT < 0)
	{
		cout << "No solution exists!\n";
	}
	else
	{

	JGTL::Vector2<float> isect(p2x + bestT*v2x,p2y + bestT*v2y);

	cout << "Intersection: " << isect << endl;

	cout << "DEBUG: " << (isect-p1).magnitude() << " should == " << (speed*bestT) << endl;

	JGTL::Vector2<float> v1(
			(isect.x - p1x)/(bestT*speed),
			(isect.y - p1y)/(bestT*speed)
			);

	cout << "Velocity to reach intersection: " << v1 << endl;

	float angle = atan2(v1.y,v1.x);

	cout << "Angle to reach intersection: " << angle << " rad (" << (angle*180/M_PI) << " degrees)" << endl;
	}

	return 0;
}

