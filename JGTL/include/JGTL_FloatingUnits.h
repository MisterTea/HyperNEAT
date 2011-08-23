#include <iostream>
#include <complex>
#include <sstream>

namespace JGTL
{
	typedef unsigned long long units_internal_ulong;

	template<class ValueType,units_internal_ulong SCALE_NUMERATOR,units_internal_ulong SCALE_DENOMINATOR>
	class FloatingUnits
	{
	protected:
		ValueType value;

	public:
		FloatingUnits(ValueType _value=0)
			:
		value(_value)
		{
		}

		template<class OtherValueType,units_internal_ulong OTHERSCALE_NUMERATOR,units_internal_ulong OTHERSCALE_DENOMINATOR>
		FloatingUnits(const FloatingUnits<OtherValueType,OTHERSCALE_NUMERATOR,OTHERSCALE_DENOMINATOR> &t)
		{
			setValue( changeScale(t) );
		}

		template<class OtherValueType,units_internal_ulong OTHERSCALE_NUMERATOR,units_internal_ulong OTHERSCALE_DENOMINATOR>
		inline const FloatingUnits &operator=(const FloatingUnits<OtherValueType,OTHERSCALE_NUMERATOR,OTHERSCALE_DENOMINATOR> &t)
		{
			setValue( changeScale(t) );

			return *this;
		}

		template<class OtherValueType,units_internal_ulong OTHERSCALE_NUMERATOR,units_internal_ulong OTHERSCALE_DENOMINATOR>
		ValueType changeScale(const FloatingUnits<OtherValueType,OTHERSCALE_NUMERATOR,OTHERSCALE_DENOMINATOR> &t)
		{
			ValueType retval;

			retval = ( ( t.getValue() * OTHERSCALE_NUMERATOR / OTHERSCALE_DENOMINATOR ) / ( ValueType(SCALE_NUMERATOR) / SCALE_DENOMINATOR ) );

			return retval;
		}

		inline ValueType getScale() const
		{
			return ValueType(SCALE_NUMERATOR)/SCALE_DENOMINATOR;
		}

		inline void setValue(ValueType _value)
		{
			//cout << "Calling setValue\n";
			value = _value;
		}

		inline ValueType getValue() const { return value; }

	};

	template<class ValueType,units_internal_ulong SCALE_NUMERATOR,units_internal_ulong SCALE_DENOMINATOR>
	inline std::ostream& operator<<(std::ostream& stream, const FloatingUnits<ValueType,SCALE_NUMERATOR,SCALE_DENOMINATOR>& d)
	{
		stream << d.getValue();

		return stream;
	}

	template<class ValueType,units_internal_ulong SCALE_NUMERATOR,units_internal_ulong SCALE_DENOMINATOR>
	inline std::istream& operator>>(std::istream& stream, FloatingUnits<ValueType,SCALE_NUMERATOR,SCALE_DENOMINATOR>& d)
	{
		ValueType value;
		stream >> value;
		d.setValue(value);

		return stream;
	}
}
