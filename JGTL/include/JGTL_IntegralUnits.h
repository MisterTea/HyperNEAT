#include <iostream>
#include <complex>
#include <sstream>

namespace JGTL
{
	typedef unsigned long long units_internal_ulong;

	template <bool condition, class Then, class Else>
	struct IF
	{
		typedef Then RET;
	};

	template <class Then, class Else>
	struct IF<false, Then, Else>
	{
		typedef Else RET;
	};

	template<class Type,Type a,Type b>
	struct STATIC_MOD
	{
		static const Type VALUE = (a%b);
	};

	template <class Type,bool condition, Type Then, Type Else>
	struct TYPEIF
	{
		static const Type RESULT=Then;
	};

	template <class Type,Type Then, Type Else>
	struct TYPEIF<Type,false, Then, Else>
	{
		static const Type RESULT=Else;
	};

	template<units_internal_ulong i,units_internal_ulong j>
	struct IntegralUnitsGCD
	{
		static const units_internal_ulong VALUE = TYPEIF< 
			units_internal_ulong,
			STATIC_MOD<units_internal_ulong,i,j>::VALUE==0, 
			(j), 
			TYPEIF< 
			units_internal_ulong,
			STATIC_MOD<units_internal_ulong,j,STATIC_MOD<units_internal_ulong,i,j>::VALUE>::VALUE==0,
			STATIC_MOD<units_internal_ulong,i,j>::VALUE,
			IntegralUnitsGCD<
			STATIC_MOD<units_internal_ulong,i,j>::VALUE,
			STATIC_MOD<units_internal_ulong,j,STATIC_MOD<units_internal_ulong,i,j>::VALUE>::VALUE
			>::VALUE
			>::RESULT 
		>::RESULT;
	};

	template<units_internal_ulong j>
	struct IntegralUnitsGCD<1,j>
	{
		static const units_internal_ulong VALUE = 1;
	};

	template<units_internal_ulong i>
	struct IntegralUnitsGCD<i,1>
	{
		static const units_internal_ulong VALUE = 1;
	};

	template<>
	struct IntegralUnitsGCD<1,1>
	{
		static const units_internal_ulong VALUE = 1;
	};

	template<units_internal_ulong j>
	struct IntegralUnitsGCD<0,j>
	{
		static const units_internal_ulong VALUE = 0;
	};

	template<units_internal_ulong i>
	struct IntegralUnitsGCD<i,0>
	{
		static const units_internal_ulong VALUE = 0;
	};

	template<>
	struct IntegralUnitsGCD<0,0>
	{
		static const units_internal_ulong VALUE = 0;
	};

	template<class T>
	inline units_internal_ulong GCD(T a, T b)
	{
		while( 1 )
		{
			a = a % b;
			if( a == 0 )
			{
				return b;
			}
			b = b % a;
			if( b == 0 )
			{
				return a;
			}
		}
	}

	template<>
	inline units_internal_ulong GCD(double a,double b)
	{
		return 1ULL;
	}

	template<>
	inline units_internal_ulong GCD(float a,float b)
	{
		return 1ULL;
	}

	template<class ValueType,ValueType SCALE,bool USEGCD>
	class IntegralUnits
	{
	protected:
		ValueType value;

	public:
		IntegralUnits(ValueType _value=0)
			:
		value(_value)
		{
		}

		template<class OtherValueType,OtherValueType OTHERSCALE,bool OTHERUSEGCD>
		IntegralUnits(const IntegralUnits<OtherValueType,OTHERSCALE,OTHERUSEGCD> &t)
		{
			setValue( changeScale(t) );
		}

		template<class OtherValueType,OtherValueType OTHERSCALE,bool OTHERUSEGCD>
		inline const IntegralUnits &operator=(const IntegralUnits<OtherValueType,OTHERSCALE,OTHERUSEGCD> &t)
		{
			setValue( changeScale(t) );

			return *this;
		}

		template<class OtherValueType,OtherValueType OTHERSCALE,bool OTHERUSEGCD>
		ValueType changeScale(const IntegralUnits<OtherValueType,OTHERSCALE,OTHERUSEGCD> &t)
		{
			ValueType retval;

			if(USEGCD)
			{
				//cout << "Scale: " << SCALE << " OtherScale: " << OTHERSCALE << endl;
				units_internal_ulong gcd = IntegralUnitsGCD<SCALE,OTHERSCALE>::VALUE;
				//cout << "GCD: " << gcd << endl;
				//cout << "Actual GCD: " << GCD(SCALE,OTHERSCALE) << endl;
				//cout << "Value: " << t.getValue() << endl;
				retval = ( ( t.getValue() * (OTHERSCALE/gcd) ) / (SCALE/gcd) );
				//cout << "Retval calculated: " << retval << endl;
			}
			else
			{
				retval = ( ( value * OTHERSCALE ) / SCALE );
			}
			return retval;
		}

		inline units_internal_ulong getScale() const
		{
			return SCALE;
		}

		inline void setValue(ValueType _value)
		{
			//cout << "Calling setValue\n";
			value = _value;
		}

		inline ValueType getValue() const { return value; }

	};

	template<class ValueType,ValueType SCALE,bool USEGCD>
	inline std::ostream& operator<<(std::ostream& stream, const IntegralUnits<ValueType,SCALE,USEGCD>& d)
	{
		stream << d.getValue();

		return stream;
	}

	template<class ValueType,ValueType SCALE,bool USEGCD>
	inline std::istream& operator>>(std::istream& stream, IntegralUnits<ValueType,SCALE,USEGCD>& d)
	{
		ValueType value;
		stream >> value;
		d.setValue(value);

		return stream;
	}
}
