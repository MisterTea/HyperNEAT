#ifndef __WRAPPEDINTERPOLATED_VALUE_H__
#define __WRAPPEDINTERPOLATED_VALUE_H__

#include "JGTL_InterpolatedValue.h"

namespace JGTL
{
	/**
	* @class WrappedInterpolatedValue
	* @brief The WrappedInterpolatedValue Class handles values which approach a limit
	*        using the formula NewValue = actualValue + (potentialValue-actualValue)*interpolationCoeff;
	*        This special instance of an InterpolatedValue is for values which wrap (angles, for example,
	*        which wrap around 2*PI)
	* 
	*
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class T>
	class WrappedInterpolatedValue : public InterpolatedValue<T>
	{
		using InterpolatedValue<T>::actualValue;
		using InterpolatedValue<T>::potentialValue;
		using InterpolatedValue<T>::interpolationCoeff;
	protected:
		T minValue,maxValue;
		T spread;

	public:
		WrappedInterpolatedValue(
			const T &_minValue,
			const T &_maxValue
			)
			:
		InterpolatedValue<T>(0.1),
			minValue(_minValue),
			maxValue(_maxValue),
			spread(maxValue - minValue)
		{
		}

		WrappedInterpolatedValue(
			float _interpolationCoeff,
			const T &_minValue,
			const T &_maxValue
			)
			:
		InterpolatedValue<T>(_interpolationCoeff),
			minValue(_minValue),
			maxValue(_maxValue),
			spread(maxValue - minValue)
		{
		}

		WrappedInterpolatedValue(
			const T &_value,
			float _interpolationCoeff,
			const T &_minValue,
			const T &_maxValue
			)
			:
		InterpolatedValue<T>(_value,_interpolationCoeff),
			minValue(_minValue),
			maxValue(_maxValue),
			spread(maxValue - minValue)
		{
			clampValues();
		}

		/*
		* Potential Value Assignment Operator
		* @brief This assignment set the potential value
		* @param _potentialValue A new potential value
		*/
		const WrappedInterpolatedValue<T> &operator =(T _potentialValue)
		{
			setValue(_potentialValue);
			return *this;
		}

		virtual void setValue(const T &_value)
		{
			InterpolatedValue<T>::setValue(_value);

			clampValues();
		}

		virtual void update(int times=1)
		{
			if (times<=0)
				actualValue = potentialValue;
			else
			{
				for (int a=0;a<times;a++)
				{
					T regDiff = potentialValue-actualValue;

					//This means the values wrap around

					if (fabs(regDiff)*2 > spread)
					{
						//This means you need to take "the long" way to get
						//there faster (wrap around)

						T diff;

						if (potentialValue > actualValue)
						{
							diff =
								(maxValue - potentialValue) +
								(actualValue - minValue);

							actualValue -= diff*interpolationCoeff;

							if (actualValue<minValue)
							{
								actualValue += spread;
							}
						}
						else
						{
							diff =
								(maxValue - actualValue) +
								(potentialValue - minValue);

							actualValue += diff*interpolationCoeff;

							if (actualValue>maxValue)
							{
								actualValue -= spread;
							}
						}
					}
					else
					{
						actualValue += regDiff*interpolationCoeff;
					}
				}
			}
		}

	protected:
		void clampValues()
		{
			while(potentialValue<minValue)
			{
				potentialValue += spread;
			}

			while(potentialValue>maxValue)
			{
				potentialValue -= spread;
			}

			while(actualValue<minValue)
			{
				actualValue += spread;
			}

			while(actualValue>maxValue)
			{
				actualValue -= spread;
			}
		}
	};

	template<class T>
	inline std::ostream& operator<<(std::ostream& stream, const WrappedInterpolatedValue<T>& d)
	{
		stream << d.getActualValue() << " " << d.getPotentialValue();

		return stream;
	}

	template<class T>
	inline std::istream& operator>>(std::istream& stream, WrappedInterpolatedValue<T>& d)
	{
		T x,y;
		stream >> x >> y;

		d.setValue(x);
		d.forceValue();
		d.setValue(y);

		return stream;
	}
}

#endif
