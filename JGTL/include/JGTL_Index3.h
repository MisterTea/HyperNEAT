#ifndef INDEX3_H_INCLUDED
#define INDEX3_H_INCLUDED

#include <algorithm>
#include <iostream>
#include <cmath>
using namespace std;

#ifdef USE_DILUCULUM_WRAPPER

/** Diluculum header **/
#include <Diluculum/LuaWrappers.hpp>

using namespace Diluculum;
#endif

namespace JGTL
{

	class Index3
	{
	public:
		int x,y,z;

		Index3()
			:
		x(0),
			y(0),
			z(0)
		{}

		Index3(int _x,int _y,int _z)
			:
		x(_x),
			y(_y),
			z(_z)
		{}

		Index3(std::string s)
		{
			std::istringstream istr(s);

			istr >> x >> y >> z;
		}

#ifdef USE_DILUCULUM_WRAPPER
		// The constructor taking a 'LuaValueList' parameter.
		Index3(const LuaValueList& params)
		{
			x = params[0].asInteger();
			y = params[1].asInteger();
			z = params[2].asInteger();
		}

		LuaValueList getX(const LuaValueList& params)
		{
			LuaValueList retval;
			retval.push_back(x);
			return retval;
		}

		LuaValueList getY(const LuaValueList& params)
		{
			LuaValueList retval;
			retval.push_back(y);
			return retval;
		}

		LuaValueList getZ(const LuaValueList& params)
		{
			LuaValueList retval;
			retval.push_back(z);
			return retval;
		}

		LuaValueList setX(const LuaValueList& params)
		{
			LuaValueList retval;
			x = params[0].asInteger();
			return retval;
		}

		LuaValueList setY(const LuaValueList& params)
		{
			LuaValueList retval;
			y = params[0].asInteger();
			return retval;
		}

		LuaValueList setZ(const LuaValueList& params)
		{
			LuaValueList retval;
			z = params[0].asInteger();
			return retval;
		}

		inline LuaValue getLuaValue()
		{
			return LuaValue(this,"Index3");
		}
#endif

		inline bool operator<(const Index3 &index) const
		{
			return (
				z<index.z ||
				(z==index.z&&y<index.y) ||
				(z==index.z&&y==index.y&&x<index.x)
				);
		}

		inline bool operator==(const Index3 &index) const
		{
			return (x==index.x&&y==index.y&&z==index.z);
		}

		inline bool operator!=(const Index3 &index) const
		{
			return (x!=index.x||y!=index.y||z!=index.z);
		}

		inline Index3 operator*(const Index3 &index) const
		{
			return Index3(x*index.x,y*index.y,z*index.z);
		}

		template<class TT>
		inline Index3 operator*(TT i) const
		{
			return Index3(int(x*i),int(y*i),int(z*i));
		}

		inline Index3 operator/(int i) const
		{
			return Index3(x/i,y/i,z/i);
		}

		inline Index3 operator-(const Index3 &index) const
		{
			return Index3(x-index.x,y-index.y,z-index.z);
		}

		inline void operator-=(const Index3 &index)
		{
			x-=index.x;
			y-=index.y;
			z-=index.z;
		}

		inline Index3 operator+(const Index3 &index) const
		{
			return Index3(x+index.x,y+index.y,z+index.z);
		}

		inline void operator+=(const Index3 &index)
		{
			x+=index.x;
			y+=index.y;
			z+=index.z;
		}

		inline double magnitude() const
		{
			double d = (x*x)+(y*y)+(z*z);
			return sqrt(d);
		}

		inline int magnitudeSquared() const
		{
			return (x*x)+(y*y)+(z*z);
		}

		inline int distanceSquared(const Index3 &other) const
		{
			return
				(x-other.x)*(x-other.x) +
				(y-other.y)*(y-other.y) +
				(z-other.z)*(z-other.z);
		}

		inline int distanceSquared(int _x,int _y,int _z) const
		{
			return
				(x-_x)*(x-_x) +
				(y-_y)*(y-_y) +
				(z-_z)*(z-_z);
		}

		inline int chessDistance(const Index3 &other) const
		{
			return max(max(abs(x-other.x),abs(y-other.y)),abs(z-other.z));
		}

		inline int manhatDistance(const Index3 &other) const
		{
			return abs(x-other.x)+abs(y-other.y)+abs(z-other.z);
		}
	};

	inline ostream& operator<<(ostream& stream, const Index3& d)
	{
		stream << d.x << " " << d.y << " " << d.z;

		return stream;
	}

	inline istream& operator>>(istream& stream, Index3& d)
	{
		stream >> d.x >> d.y >> d.z;

		return stream;
	}

	class RectangleIndex3
	{
	public:
		Index3 topLeft,size;

		RectangleIndex3()
		{}

		RectangleIndex3(const Index3 &_topLeft,const Index3 &_size)
			:
		topLeft(_topLeft),
			size(_size)
		{}

		RectangleIndex3(int x,int y,int z,int sizex,int sizey,int sizez)
			:
		topLeft(x,y,z),
			size(sizex,sizey,sizez)
		{}

#ifdef USE_DILUCULUM_WRAPPER
		RectangleIndex3(const LuaValueList &params)
		{}
#endif

		bool contains(const Index3 &point) const
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

		bool contains(const RectangleIndex3 &other) const
		{
			return contains(other.topLeft) && contains(other.topLeft+other.size);
		}

		const Index3 &getFirstPoint() const
		{
			return topLeft;
		}

        Index3 getCenter() const
        {
            return (topLeft+(size/2));
        }

		bool getNextPoint(Index3 &currentPoint) const
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

		int getArea() const
		{
			return size.x*size.y*size.z;
		}

	};

	inline ostream& operator<<(ostream& stream, const RectangleIndex3& d)
	{
		stream << d.topLeft << ' ' << d.size;

		return stream;
	}

	inline istream& operator>>(istream& stream, RectangleIndex3& d)
	{
		stream >> d.topLeft >> d.size;

		return stream;
	}
}

#endif // INDEX3_H_INCLUDED

#ifdef INSERT_DILUCULUM_WRAPPER_ONCE

namespace JGTL
{
	/*
	This code needs to go in one of your objects if you want to use this class with the wrapper.

	NOTE: DO NOT SET THIS FROM WITHIN A .h FILE!  DO NOT LEAVE THIS DEFINED AFTER THE #INCLUDE
	*/
	DILUCULUM_BEGIN_CLASS (Index3);
	DILUCULUM_CLASS_METHOD (Index3, getX);
	DILUCULUM_CLASS_METHOD (Index3, getY);
	DILUCULUM_CLASS_METHOD (Index3, getZ);
	DILUCULUM_CLASS_METHOD (Index3, setX);
	DILUCULUM_CLASS_METHOD (Index3, setY);
	DILUCULUM_CLASS_METHOD (Index3, setZ);
	DILUCULUM_END_CLASS (Index3);

	void RegisterIndex3(LuaState &lua)
	{
		DILUCULUM_REGISTER_CLASS (lua["Index3"], Index3);
	}

	DILUCULUM_BEGIN_CLASS (RectangleIndex3);
	DILUCULUM_END_CLASS (RectangleIndex3);

	void RegisterRectangleIndex(LuaState &lua)
	{
		DILUCULUM_REGISTER_CLASS (lua["RectangleIndex3"], RectangleIndex3);
	}
}

#endif
