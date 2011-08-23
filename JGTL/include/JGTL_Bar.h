#ifndef __JGTL_BAR_H_INCLUDED__
#define __JGTL_BAR_H_INCLUDED__

#include <iostream>

namespace JGTL
{
	/**
	* @class Bar
	* @brief The Bar Class handles a max/current value system (e.g. a Progress Bar)
	*
	* @author Jason Gauci
	* @Date 2008
	*/

	template <class BarValueType>
	class Bar
	{
	public:
		BarValueType currentValue,maxValue;

		/*
		* Constructor
		* This creates a bar with a currentValue and maxValue of 0
		*/
		Bar()
			:
		currentValue(0),
			maxValue(0)
		{}

		/*
		* Constructor
		* This creates a bar with a currentValue and maxValue
		*/
		Bar(const BarValueType &_currentValue,const BarValueType &_maxValue)
			:
		currentValue(_currentValue),
			maxValue(_maxValue)
		{}

		/*
		* Constructor
		* This creates a bar with a currentValue and maxValue equal 
		* to a single value
		*/
		Bar(const BarValueType &_value)
			:
		currentValue(_value),
			maxValue(_value)
		{}
	};

	template <class BarValueType>
	inline std::ostream& operator<<(std::ostream& stream, const Bar<BarValueType>& d)
	{
		stream << d.currentValue << " " << d.maxValue;

		return stream;
	}

	template <class BarValueType>
	inline std::istream& operator>>(std::istream& stream, Bar<BarValueType>& d)
	{
		stream >> d.currentValue >> d.maxValue;

		return stream;
	}

}

#endif // __JGTL_BAR_H_INCLUDED__
