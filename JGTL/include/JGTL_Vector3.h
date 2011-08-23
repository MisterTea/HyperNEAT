#ifndef VECTOR3_H_INCLUDED
#define VECTOR3_H_INCLUDED

#include <algorithm>
#include <iostream>
#include <cmath>

namespace JGTL
{

	template<class T>
	class Vector3
	{
	public:
		T x,y,z;

		Vector3()
			:
		x ( 0 ),
			y ( 0 ),
			z ( 0 )
		{}

		Vector3 ( T _x,T _y,T _z )
			:
		x ( _x ),
			y ( _y ),
			z ( _z )
		{}

		template <class TT>
		Vector3 ( const Vector3<TT> &other )
		{
			x = ( T ) other.x;
			y = ( T ) other.y;
			z = ( T ) other.z;
		}

		template <class TT>
		const Vector3 &operator = ( const Vector3<TT> &other )
		{
			if ( this != ( ( const Vector3<T>* ) &other ) )
			{
				x = ( T ) other.x;
				y = ( T ) other.y;
				z = ( T ) other.z;
			}

			return *this;
		}

		template <class TT>
		bool operator== ( const Vector3<TT> &other ) const
		{
			return ( x==other.x && y==other.y && z==other.z );
		}

		template <class TT>
		bool operator!= ( const Vector3<TT> &other ) const
		{
			return ( x!=other.x || y!=other.y || z!=other.z );
		}

		template <class TT>
		bool operator< ( const Vector3<TT> &other ) const
		{
			return (
				z<other.z ||
				( z==other.z&&y<other.y ) ||
				( z==other.z&&y==other.y&&x<other.x )
				);
		}

		template <class TT>
		Vector3<T> operator/ ( TT divisor ) const
		{
			return Vector3<T> ( x/divisor,y/divisor,z/divisor );
		}

		template <class TT>
		void operator/= ( TT divisor )
		{
			x/=divisor;
			y/=divisor;
			z/=divisor;
		}

		Vector3<T> operator-() const
		{
			Vector3<T> newVec ( -x,-y,-z );
			return newVec;
		}

		template <class TT>
		Vector3<T> operator- ( const Vector3<TT> &other ) const
		{
			return Vector3<T> ( x-other.x,y-other.y,z-other.z );
		}

		template <class TT>
		Vector3<T> operator+ ( const Vector3<TT> &other ) const
		{
			return Vector3<T> ( x+other.x,y+other.y,z+other.z );
		}

		template <class TT>
		Vector3<T> operator* ( const TT &coeff ) const
		{
			return Vector3<T> ( T(x*coeff),T(y*coeff),T(z*coeff) );
		}

		template <class TT>
		Vector3<T> componentMultiply ( const Vector3<TT> &other ) const
		{
			return Vector3<T> ( x*other.x,y*other.y,z*other.z );
		}

		template <class TT>
		void operator+= ( const Vector3<TT> &other )
		{
			x+=other.x;
			y+=other.y;
			z+=other.z;
		}

		template <class TT>
		void operator-= ( const Vector3<TT> &other )
		{
			x-=other.x;
			y-=other.y;
			z-=other.z;
		}

		template <class TT>
		void operator*= ( TT val )
		{
			x*=val;
			y*=val;
			z*=val;
		}

		template <class TT>
		T dot ( const Vector3<TT> &other ) const
		{
			return T ( x*other.x+y*other.y+z*other.z );
		}

		template <class TT>
		Vector3<T> cross ( const Vector3<TT> &other ) const
		{
			return Vector3<T> ( y*other.z - z*other.y, z*other.x - x*other.z, x*other.y - y*other.x );
		}

		inline T magnitude() const
		{
			T d = ( x*x ) + ( y*y ) + ( z*z );
			return sqrt ( d );
		}

		T magnitudeSquared() const
		{
			return dot ( *this );
		}

		void normalize()
		{
			T mag = sqrt ( magnitudeSquared() );

			if ( mag>1e-6 )
			{
				x/=mag;
				y/=mag;
				z/=mag;
			}
		}

		Vector3<T> normalizeCopy() const
		{
			Vector3<T> other ( *this );

			other.normalize();

			return other;
		}

		template <class TT>
		Vector3<T> projectOn ( const Vector3<TT> &other ) const
		{
			T dp = this->dot ( other );

			Vector3<T> proj = other* ( dp/other.getMagnitude() );

			return proj;
		}

		template <class TT>
		T distance ( const Vector3<TT> &other ) const
		{
			return (T)sqrt(
				( x-other.x ) * ( x-other.x ) +
				( y-other.y ) * ( y-other.y ) +
				( z-other.z ) * ( z-other.z )
				);
		}

		template <class TT>
		inline T distance ( TT _x,TT _y,TT _z ) const
		{
			return (T)sqrt(
				( x- ( T ) _x ) * ( x- ( T ) _x ) +
				( y- ( T ) _y ) * ( y- ( T ) _y ) +
				( z- ( T ) _z ) * ( z- ( T ) _z )
				);
		}

		template <class TT>
		T distanceSquared ( const Vector3<TT> &other ) const
		{
			return
				( x-other.x ) * ( x-other.x ) +
				( y-other.y ) * ( y-other.y ) +
				( z-other.z ) * ( z-other.z );
		}

		template <class TT>
		inline T distanceSquared ( TT _x,TT _y,TT _z ) const
		{
			return
				( x- ( T ) _x ) * ( x- ( T ) _x ) +
				( y- ( T ) _y ) * ( y- ( T ) _y ) +
				( z- ( T ) _z ) * ( z- ( T ) _z );
		}

		template <class TT>
		inline T chessDistance ( const Vector3<TT> &other ) const
		{
			return max ( max ( abs ( x- ( T ) other.x ),abs ( y- ( T ) other.y ) ),abs ( z- ( T ) other.z ) );
		}

		template <class TT>
		inline T manhatDistance ( const Vector3<TT> &other ) const
		{
			return abs ( x- ( T ) other.x ) +abs ( y- ( T ) other.y ) +abs ( z- ( T ) other.z );
		}

	};

	/*
	 * This function allows conversion between 3-D Vector types
	 */
	template<class T, class TT>
	T convertVector3(const TT &other)
	{
		return T(other.x,other.y,other.z);
	}

	template<class T>
	inline std::ostream& operator<< ( std::ostream& stream, const Vector3<T>& d )
	{
		stream << d.x << " " << d.y << " " << d.z;

		return stream;
	}

	template<class T>
	inline std::istream& operator>> ( std::istream& stream, Vector3<T>& d )
	{
		stream >> d.x >> d.y >> d.z;

		return stream;
	}

}

#endif // VECTOR3_H_INCLUDED


