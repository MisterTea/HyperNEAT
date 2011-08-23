#ifndef RECTANGLE3_H_INCLUDED
#define RECTANGLE3_H_INCLUDED

#include <algorithm>
#include <iostream>
#include <cmath>

#ifdef USE_DILUCULUM_WRAPPER
/** lua headers **/
#include "lua.hpp"

/** Diluculum header **/
#include <Diluculum/LuaWrappers.hpp>

using namespace Diluculum;
#endif

namespace JGTL
{

	template<class T>
	class Rectangle3
	{
	public:
		Vector3<T> topLeft,size;

		Rectangle3()
		{}

		Rectangle3(const Vector3<T> &_topLeft,const Vector3<T> &_size)
			:
		topLeft(_topLeft),
			size(_size)
		{}

#ifdef USE_DILUCULUM_WRAPPER
		Rectangle3(const LuaValueList &params)
		{}
#endif

		bool contains(const Vector3<T> &point) const
		{
			if (
				point.x>=topLeft.x &&
				point.y>=topLeft.y &&
				point.z>=topLeft.z)
			{
				if (
					(point.x<(topLeft.x+size.x)) &&
					(point.y<(topLeft.y+size.y)) &&
					(point.z<(topLeft.z+size.z))
					)
				{
					return true;
				}

			}
			return false;
		}

		const Vector3<T> &getFirstPoint() const
		{
			return topLeft;
		}

		bool getNextDiscretePoint(Vector3<T> &currentPoint) const
		{
			if ( currentPoint.x+1 < topLeft.x+size.x )
			{
				currentPoint.x++;
				return true;
			}
			else if ( currentPoint.y+1 < topLeft.y+size.y )
			{
				currentPoint.x = topLeft.x;
				currentPoint.y++;
				return true;
			}
			else if ( currentPoint.z+1 < topLeft.z+size.z )
			{
				currentPoint.x = topLeft.x;
				currentPoint.y = topLeft.y;
				currentPoint.z++;
				return true;
			}

			return false;
		}
	};

}

#endif
