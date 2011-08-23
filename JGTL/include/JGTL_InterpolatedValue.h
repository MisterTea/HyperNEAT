#ifndef __INTERPOLATED_VALUE_H__
#define __INTERPOLATED_VALUE_H__

namespace JGTL
{
	/**
	* @class InterpolatedValue
	* @brief The InterpolatedValue Class handles values which approach a limit
	*        using the formula NewValue = actualValue + (potentialValue-actualValue)*interpolationCoeff;
	* 
	*
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class T>
	class InterpolatedValue
	{
	public:
		/*
		* This is the limit of the value
		*/
		T potentialValue;

		/**
		* This is the current value.  This value is approaching
		* the potential value
		*/
		T actualValue;

		/**
		* This handles the rate at which the actual value approaches
		* the potential value
		*/
		float interpolationCoeff;

		/*
		* Constructor
		*/
		InterpolatedValue()
			:
		interpolationCoeff(0.1f)
		{
		}

		/*
		* Constructor
		* @param _interpolationCoeff The coefficient of interpolation.
		*/
		InterpolatedValue(float _interpolationCoeff)
			:
		potentialValue(0),
			actualValue(0),
			interpolationCoeff(_interpolationCoeff)
		{
		}

		/*
		* Constructor
		* @param _value An initial value for the actual and potential values
		* @param _interpolationCoeff The coefficient of interpolation.
		*/
		InterpolatedValue(const T &_value,float _interpolationCoeff)
			:
		potentialValue(_value),
			actualValue(_value),
			interpolationCoeff(_interpolationCoeff)
		{
		}
		
		virtual ~InterpolatedValue()
		{
		}

		/*
		* Potential Value Assignment Operator
		* @brief This assignment set the potential value
		* @param _potentialValue A new potential value
		*/
		const InterpolatedValue<T> &operator =(T _potentialValue)
		{
			setValue(_potentialValue);
			return *this;
		}

		/*
		* Gets the Interpolation Coefficient
		*/
		float getInterpolationCoeff()
		{
			return interpolationCoeff;
		}

		/*
		* Sets the Interpolation Coefficient
		*/
		void setCoeff(float _coeff)
		{
			interpolationCoeff = _coeff;
		}

		/*
		* Sets the potential value
		*/
		virtual void setValue(const T &_value)
		{
			potentialValue = _value;
		}

		/*
		* Increases the potential value
		*/
		void operator +=(const T &_value)
		{
			setValue(potentialValue + _value);
		}

		/*
		* Decreases the potential value
		*/
		void operator -=(const T &_value)
		{
			setValue(potentialValue - _value);
		}

		/*
		* Gets the potential value
		*/
		T &getPotentialValue()
		{
			return potentialValue;
		}
		const T &getPotentialValue() const
		{
			return potentialValue;
		}

		/*
		* Gets the actual value
		*/
		T &getActualValue()
		{
			return actualValue;
		}
		const T &getActualValue() const
		{
			return actualValue;
		}

		void setActualValue(const T& t)
		{
			actualValue = t;
		}

		/*
		* Forces the actual value to the potential value.
		* This is theoretically equivalent to updating an 
		* infinite amount of times
		*/
		void forceValue()
		{
			actualValue = potentialValue;
		}

		/*
		* Updates the actual value
		* @param times The number of times to update
		*/
		virtual void update(int times=1)
		{
			if (times<=0)
				actualValue = potentialValue;
			else if(times==1)
			{
				T regDiff = potentialValue-actualValue;
				actualValue += (potentialValue-actualValue)*interpolationCoeff;
			}
			else
			{
				//The closed form equation is:
				//x_i = C*x_f*(1-powf(1-C,a))/(1-(1-C)) + powf(1-C,a)*x_i;
				float oneMinusInter = 1.0f-interpolationCoeff;
				float powInterTimes = powf(oneMinusInter,float(times));
				actualValue = potentialValue*interpolationCoeff*(1-powInterTimes)/(interpolationCoeff) + actualValue*powInterTimes;
			}
		}
	};

	template<class T>
	inline std::ostream& operator<<(std::ostream& stream, const InterpolatedValue<T>& d)
	{
		stream << d.getActualValue() << " " << d.getPotentialValue();

		return stream;
	}

	template<class T>
	inline std::istream& operator>>(std::istream& stream, InterpolatedValue<T>& d)
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
