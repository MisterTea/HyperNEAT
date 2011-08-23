#ifndef VECTOR4_H_INCLUDED
#define VECTOR4_H_INCLUDED

#include <algorithm>
#include <iostream>
#include <cmath>

namespace JGTL
{

	template<class T>
	class Vector4
	{
	public:
		T x,y,z,w;

		Vector4()
			:
		x ( 0 ),
			y ( 0 ),
			z ( 0 ),
			w(0)
		{}

		Vector4 ( T _x,T _y,T _z,T _w )
			:
		x ( _x ),
			y ( _y ),
			z ( _z ),
			w(_w)
		{}

		template <class TT>
		Vector4 ( const Vector4<TT> &other )
		{
			x = ( T ) other.x;
			y = ( T ) other.y;
			z = ( T ) other.z;
			w = (T)other.w;
		}

		template <class TT>
		const Vector4 &operator = ( const Vector4<TT> &other )
		{
			if ( this != ( ( const Vector4<T>* ) &other ) )
			{
				x = ( T ) other.x;
				y = ( T ) other.y;
				z = ( T ) other.z;
				w = (T)other.w;
			}

			return *this;
		}

		template <class TT>
		bool operator== ( const Vector4<TT> &other ) const
		{
			return ( x==other.x && y==other.y && z==other.z && w==other.w);
		}

		template <class TT>
		bool operator!= ( const Vector4<TT> &other ) const
		{
			return ( x!=other.x || y!=other.y || z!=other.z || w!=other.w);
		}

		template <class TT>
		bool operator< ( const Vector4<TT> &other ) const
		{
			return (
				w<other.w ||
				( w==other.w&&z<other.z ) ||
				( w==other.w&&z==other.z&&y<other.y ) ||
				( w==other.w&&z==other.z&&y==other.y&&x<other.x )
				);
		}

		Vector4<T> operator/ ( T divisor ) const
		{
			return Vector4<T> ( x/divisor,y/divisor,z/divisor,w/divisor );
		}

		void operator/= ( T divisor )
		{
			x/=divisor;
			y/=divisor;
			z/=divisor;
			w/=divisor;
		}

		Vector4<T> operator-() const
		{
			Vector4<T> newVec ( -x,-y,-z,-w );
			return newVec;
		}

		template <class TT>
		Vector4<T> operator- ( const Vector4<TT> &other ) const
		{
			return Vector4<T> ( x-other.x,y-other.y,z-other.z,w-other.w );
		}

		template <class TT>
		Vector4<T> operator+ ( const Vector4<TT> &other ) const
		{
			return Vector4<T> ( x+other.x,y+other.y,z+other.z,w+other.w );
		}

		Vector4<T> operator* ( const T &coeff ) const
		{
			return Vector4<T> ( x*coeff,y*coeff,z*coeff,w*coeff );
		}

		template <class TT>
		void operator+= ( const Vector4<TT> &other )
		{
			x+=other.x;
			y+=other.y;
			z+=other.z;
			w+=other.w;
		}

		template <class TT>
		void operator-= ( const Vector4<TT> &other )
		{
			x-=other.x;
			y-=other.y;
			z-=other.z;
			w-=other.w;
		}

		void operator*= ( T val )
		{
			x*=val;
			y*=val;
			z*=val;
			w*=val;
		}

		template <class TT>
		T dot ( const Vector4<TT> &other ) const
		{
			return T ( x*other.x+y*other.y+z*other.z+w*other.w );
		}

		inline T magnitude() const
		{
			T d = ( x*x ) + ( y*y ) + ( z*z ) + (w*w);
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
				w/=mag;
			}
		}

		Vector4<T> normalizeCopy() const
		{
			Vector4<T> other ( *this );

			other.normalize();

			return other;
		}

		template <class TT>
		Vector4<T> projectOn ( const Vector4<TT> &other ) const
		{
			T dp = this->dot ( other );

			Vector4<T> proj = other* ( dp/other.getMagnitude() );

			return proj;
		}

		template <class TT>
		T distance ( const Vector4<TT> &other ) const
		{
			return (T)sqrt(
				( x-other.x ) * ( x-other.x ) +
				( y-other.y ) * ( y-other.y ) +
				( z-other.z ) * ( z-other.z ) +
				(w-other.w) * (w-other.w)
				);
		}

		template <class TT>
		inline T distance ( TT _x,TT _y,TT _z ) const
		{
			return (T)sqrt(
				( x- ( T ) _x ) * ( x- ( T ) _x ) +
				( y- ( T ) _y ) * ( y- ( T ) _y ) +
				( z- ( T ) _z ) * ( z- ( T ) _z ) +
				( w- ( T ) _w ) * ( w- ( T ) _w )
				);
		}

		template <class TT>
		T distanceSquared ( const Vector4<TT> &other ) const
		{
			return
				( x-other.x ) * ( x-other.x ) +
				( y-other.y ) * ( y-other.y ) +
				( z-other.z ) * ( z-other.z ) +
				( w-other.w ) * ( w-other.w );
		}

		template <class TT>
		inline T distanceSquared ( TT _x,TT _y,TT _z ) const
		{
			return
				( x- ( T ) _x ) * ( x- ( T ) _x ) +
				( y- ( T ) _y ) * ( y- ( T ) _y ) +
				( z- ( T ) _z ) * ( z- ( T ) _z ) +
				( w- ( T ) _w ) * ( w- ( T ) _w );
		}

		template <class TT>
		inline T manhatDistance ( const Vector4<TT> &other ) const
		{
			return abs ( x- ( T ) other.x ) +abs ( y- ( T ) other.y ) +abs ( z- ( T ) other.z ) + abs(w - (T)other.w);
		}

	};

	/*
	 * This function allows conversion between 4-D Vector types
	 */
	template<class T, class TT>
	T convertVector4(const TT &other)
	{
		return T(other.x,other.y,other.z,other.w);
	}

	template<class T>
	inline std::ostream& operator<< ( std::ostream& stream, const Vector4<T>& d )
	{
		stream << d.x << " " << d.y << " " << d.z << " " << d.w;

		return stream;
	}

	template<class T>
	inline std::istream& operator>> ( std::istream& stream, Vector4<T>& d )
	{
		stream >> d.x >> d.y >> d.z >> d.w;

		return stream;
	}

}

#endif // VECTOR4_H_INCLUDED


