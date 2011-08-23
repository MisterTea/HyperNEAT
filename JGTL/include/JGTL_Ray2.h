#ifndef RAY2_H_INCLUDED
#define RAY2_H_INCLUDED

#include "JGTL_Vector2.h"

#include <string>
#include <iostream>
#include <utility>
#include <cmath>

namespace JGTL
{
	enum IntersectionState
	{
		IS_NONE,
		IS_ONE,
		IS_INFINITE
	};

	/**
	* @class Ray2
	* @brief This class handles 2D Rays and Line Segments
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class T>
	class Ray2
	{
	protected:

		/*
		* This points from (0,0) to the base/origin of the Ray
		*/
		Vector2<T> base;

		/*
		* This points from the base to the destination of the Ray
		*/
		Vector2<T> direction;

	public:
		/*
		* Constructor
		* Creates a Ray with a base of (0,0) and a direction of (0,0)
		*/
		Ray2()
		{}

		/*
		* Constructor
		* Creates a Ray from a base and direction
		*/
		template <class TT,class TTT>
		Ray2(const Vector2<TT> &_base,const Vector2<TTT> &_direction)
			:
		base(_base),
			direction(_direction)
		{}

		/*
		* Gets the direction of the ray
		*/
		Vector2<T> getDirection() const
		{
			return direction;
		}

		/*
		* Gets the base of the ray
		*/
		Vector2<T> getBase() const
		{
			return base;
		}

		/*
		 * Gets the endpoint of the ray if the ray is being used as a line segment
		 */
		Vector2<T> getEndPoint() const
		{
			return base+direction;
		}

		/*
		* Sets the base of the ray
		*/
		template <class TT>
		void setBase(const Vector2<TT> &newBase)
		{
			base = newBase;
		}

		/*
		* Sets the direction of the ray
		*/
		template <class TT>
		void setDirection(const Vector2<TT> &newDirection)
		{
			direction = newDirection;
		}

		/*
		* Projects a point onto the Ray and returns the T-Value of that
		* projection.
		*/
		template <class TT>
		T getProjectionTVal(const Vector2<TT> &Vector) const
		{
            return (Vector-base).dot(direction) / direction.magnitudeSquared();
		}

		/*
		* Returns a point given the t value using the equation
		* base + direction*t
		*/
		Vector2<T> getProjectionVector(T t) const
		{
			return (base+direction*t);
		}

		/*
		* Normalizes the direction of the ray
		*/
		void normalize()
		{
			direction.normalize();
		}

		/*
		* Gets the Intersection of two rays
		* @param other The other ray to intersect
		* @param lineSegment Wheter we are interesting infinite rays 
		* (any value for t) or line segments (0<=t<=1)
		* @return An IntersectionState representing the type of intersection
		* (none, one, or infinite), and a t-value for the current-ray which
		* represents the point of intersection
		*/
		template<class TT>
		std::pair<IntersectionState,float> getIntersection(const Ray2<TT> &other,bool lineSegment=false) const
		{
			std::pair<IntersectionState,float> retval;

			T denom = (T)other.direction.cross(direction);

			JGTL::Vector2<T> deltaOrigin = other.base-base;

			T numer1 = (T)other.direction.cross(deltaOrigin);

			T numer2 = (T)direction.cross(deltaOrigin);

			if(fabsf(denom)<1.0e-6)
			{
				if((fabsf(numer1)<1.0e-6))
				{
					//The rays are co-linear

					if(!lineSegment)
					{
						retval.first = IS_INFINITE;
						return retval;
					}
					else
					{
						Vector2<T> P1 = base;
						Vector2<T> P2 = base+direction;
						Vector2<T> Q1 = other.base;
						Vector2<T> Q2 = other.base+other.direction;

						Vector2<T> isectPoint;
						int numIsect =
							(within(P1.x,Q1.x,P2.x)&&within(P1.y,Q1.y,P2.y)&&putwhere(Q1.x,Q1.y,isectPoint,1))
							+  (within(P1.x,Q2.x,P2.x)&&within(P1.y,Q2.y,P2.y)&&putwhere(Q2.x,Q2.y,isectPoint,0))
							+  (within(Q1.x,P1.x,Q2.x)&&within(Q1.y,P1.y,Q2.y)&&putwhere(P1.x,P1.y,isectPoint,0))
							+  (within(Q1.x,P2.x,Q2.x)&&within(Q1.y,P2.y,Q2.y)&&putwhere(P2.x,P2.y,isectPoint,0));

						if(numIsect==0)
						{
							retval.first = IS_NONE;
						}
						else if(numIsect==1)
						{
							retval.first = IS_ONE;
							retval.second = getProjectionTVal(isectPoint);
						}
						else
						{
							retval.first = IS_INFINITE;
						}

						return retval;
					}
				}
			}
			else if(within(0,numer1,denom)&&within(0,numer2,denom) )
			{
				float t = ((float)numer1)/denom;

				if(lineSegment)
				{
					if(t<0 || t>1)
					{
						retval.first = IS_NONE;
						return retval;
					}
				}

				retval.first = IS_ONE;
				retval.second = t;
				//putwhere(origin.x + t*direction.x,origin.y + t*direction.y,where,1);
				return retval;
			}

			retval.first = IS_NONE;
			return retval;
		}

	protected:
		//Return true if b is within a and c
		//Take out the = to avoid checking for ends of segments meeting at a point.
		bool within(T a,T b,T c) const
		{
			if(a>(c + 1e-5))
				return( b>=(c - 1e-5) && b<=(a + 1e-5) );
			else
				return( b>=(a - 1e-5) && b<=(c + 1e-5) );
		}

		//putwhere, puts values in where if the previous &&'s in the corrosponding line are true
		//Adds values if a unique point.  This is to make sure the special case where two
		//segments touch at an endpoint is discovered as one intersection and not 2.
		//first is needed to fix checking against a non-initialized where.
		bool putwhere(T x,T y,Vector2<T> &where,bool first) const
		{
			if(
					first==false && 
					fabs((double)(x-where.x))<1e-5 && 
					fabs((double)(y-where.y))<1e-5
					)
			{
				//The point is incidental to the previous point, ignore
				return 0;
			}

			where.x = x;
			where.y = y;
			return 1;
		}

	    inline friend std::ostream& operator<<(std::ostream& stream, const Ray2<T>& d)
	    {
		    stream << d.base.x << " " << d.base.y << " " << d.direction.x << " " << d.direction.y;

		    return stream;
	    }

	    inline friend std::istream& operator>>(std::istream& stream, Ray2<T>& d)
	    {
		    stream >> d.base.x >> d.base.y >> d.direction.x >> d.direction.y;

		    return stream;
	    }
    };
}

#endif // RAY2_H_INCLUDED

