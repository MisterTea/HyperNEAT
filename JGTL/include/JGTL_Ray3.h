#ifndef RAY3_H_INCLUDED
#define RAY3_H_INCLUDED

#include "JGTL_Vector3.h"

namespace JGTL
{
	/**
	* @class Ray3
	* @brief This class handles 3D Rays and Line Segments
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class T>
	class Ray3
	{
	protected:
		/*
		* This points from (0,0,0) to the base/origin of the Ray
		*/
		Vector3<T> base;

		/*
		* This points from the base to the destination of the Ray
		*/
		Vector3<T> direction;

	public:
		/*
		* Constructor
		* Creates a Ray with a base of (0,0) and a direction of (0,0)
		*/
		Ray3()
		{}

		/*
		* Constructor
		* Creates a Ray from a base and direction
		*/
		template <class TT,class TTT>
		Ray3(const Vector3<TT> &_base,const Vector3<TTT> &_direction)
			:
		base(_base),
			direction(_direction)
		{}

		/*
		* Gets the direction of the ray
		*/
		Vector3<T> getDirection() const
		{
			return direction;
		}

		/*
		* Gets the base of the ray
		*/
		Vector3<T> getBase() const
		{
			return base;
		}

		/*
		* Sets the base of the ray
		*/
		template <class TT>
		void setBase(Vector3<TT> newBase)
		{
			base = newBase;
		}

		/*
		* Sets the direction of the ray
		*/
		template <class TT>
		void setDirection(const Vector3<TT> &newDirection)
		{
			direction = newDirection;
		}

		/*
		* Projects a point onto the Ray and returns the T-Value of that
		* projection.
		*/
		template <class TT>
		T getProjectionTVal(const Vector3<TT> &Vector) const
		{
			return ( (Vector-base).dot(direction) / (direction.getMagnitudeSquared()) );

			// This worked for everything but ellipses, make sure
			//the new code doesn't break anything
			//return (Vector-base).dot(direction);
		}

		/*
		* Returns a point given the t value using the equation
		* base + direction*t
		*/
		Vector3<T> getProjectionVector(T t) const
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

	};

}

#endif // RAY3_H_INCLUDED

