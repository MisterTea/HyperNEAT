#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

#include "../include/JGTL_Ray2.h"
using namespace JGTL;

int main()
{
	JGTL::Ray2<float> target(
			JGTL::Vector2<float>(10,1),
			JGTL::Vector2<float>(0,-1)
			);

	JGTL::Vector2<float> source(0,0);

	float sourceSpeed = 1.0;

	/*
	float a = source.y * sourceSpeed - target.getBase().y * sourceSpeed;
	float b = target.getBase().x * sourceSpeed - source.y * sourceSpeed;

	float angleOfAttack = acos( ((target.getBase().x - source.x)*target.getDirection().y + (source.y - target.getBase().x)*target.getDirection().x) / sqrtf((a*a) + (b*b)) ) - atan2(-b,a);

	cout << angleOfAttack << endl;
	*/

	float p1x = target.getBase().x;
	float p1y = target.getBase().y;

	float v1x = target.getDirection().x;
	float v1y = target.getDirection().y;

	float p2x = source.x;
	float p2y = source.y;

	float a = p2y*sourceSpeed - p1y*sourceSpeed;
	float b = p1x*sourceSpeed - p2x*sourceSpeed;

	cout << p1x << ',' << p1y << ',' << v1x << ',' << v1y << ',' << p2x << ',' << p2y << ',' << sourceSpeed << endl;

	float theta = 4.71239;

	if( 
			fabs(sourceSpeed * cos(theta) - v1x)<1e-3 ||
			fabs(sourceSpeed * sin(theta) - v1y)<1e-3
	  )
	{
		cout << "Crap\n";
	}


	cout 	<< (  ( p1x - p2x) / (sourceSpeed * cos(theta) - v1x) )
		<< " == "
		<< (  ( p1y - p2y) / (sourceSpeed * sin(theta) - v1y) )
		<< endl;

	cout 	<< (p1x*sourceSpeed*sin(theta) - p1x*v1y - p2x*sourceSpeed*sin(theta) + p2x*v1y) << " == " 
		<< (p1y*sourceSpeed*cos(theta) - p1y*v1x - p2y*sourceSpeed*cos(theta) + p2y*v1x)
		<< endl;

	cout 	<< ((p1x*sourceSpeed - p2x*sourceSpeed) * sin(theta) + (p2y*sourceSpeed - p1y*sourceSpeed) * cos(theta)) << " == " 
		<< ( p1x * v1y - p2x * v1y - p1y*v1x + p2y*v1x )
		<< endl;

	cout 	<< ( (theta + atan2(-b,a)) ) << " == " 
		<< acos( ( p1x * v1y - p2x * v1y - p1y*v1x + p2y*v1x ) / sqrtf((a*a)+(b*b)) )
		<< endl;

	cout << ( acos( ( p1x * v1y - p2x * v1y - p1y*v1x + p2y*v1x ) / sqrtf((a*a)+(b*b)) ) - atan2(-b,a) ) << " or " 
		<< ( -1*acos( ( p1x * v1y - p2x * v1y - p1y*v1x + p2y*v1x ) / sqrtf((a*a)+(b*b)) ) - atan2(-b,a) )
		<< endl;

	/*
t = (x0' + sqrt(s*x0'^2 - y0'^2*(s-1)/s)) / (s - 1)
theta = arcsin(y0'/(st)) 
	 */
}
