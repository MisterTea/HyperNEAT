#ifndef __FRACTION_VALUE_H__
#define __FRACTION_VALUE_H__

namespace JGTL
{
	/**
	* @class FractionValue
	* @brief This class uses an integer to represent a smaller fractional value
	*
	* @author Jason Gauci
	* @Date 2010
	*/

	template<class T,T denominator>
	class FractionValue
	{
	protected:
		T numerator;

	public:
		FractionValue(
			const T &_numerator
			)
			:
			numerator(_numerator)
		{
		}

		FractionValue(
			)
			:
			numerator(0)
		{
		}

		const FractionValue<T,denominator> &operator =(const FractionValue<T,denominator> &other)
		{
			numerator = other.numerator;
			return *this;
		}

		template<class TT>
		FractionValue<T,denominator> operator *(TT otherValue)
		{
			return FractionValue<T,denominator>(
				T(numerator*otherValue)
				);
		}

        void operator +=(const FractionValue<T,denominator> &other)
        {
            numerator += other.numerator;
        }

        void operator -=(const FractionValue<T,denominator> &other)
        {
            numerator -= other.numerator;
        }

		inline T getNumerator() const
		{
			return numerator;
		}

		inline void setNumerator(T otherNum)
		{
			numerator = otherNum;
		}

		template<class TT>
		inline TT decimal() const
		{
			return TT(numerator)/denominator;
		}

		template<class TT>
		inline void loadDecimal(TT val) const
		{
			numerator = T(val*denominator);
		}

	protected:
	};

	template<class T,int denominator>
	inline std::ostream& operator<<(std::ostream& stream, const FractionValue<T,denominator>& d)
	{
		stream << d.getNumerator() << " " << denominator;

		return stream;
	}

	template<class T,int denominator>
	inline std::istream& operator>>(std::istream& stream, FractionValue<T,denominator>& d)
	{
		T x,y;
		stream >> x >> y;

		if(y==denominator)
		{
			d.setNumerator(x);
		}
		else
		{
			d.setNumerator( (double(x)/y)*denominator );
		}

		return stream;
	}
}

#endif
