#ifndef __JGTL_STACKVECTOR_H__
#define __JGTL_STACKVECTOR_H__

#include "JGTL_LocatedException.h"

namespace JGTL
{
	template<class T,int SIZE>
	class StackVector
	{
		T array[SIZE];
		int count;

	public:
		StackVector()
			:
			count(0)
		{
		}

		inline int size() { return count; }

		inline bool empty() { return count==0; }

		void push_back(const T &other)
		{
			if(count==SIZE)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ARRAY IS TOO FULL");
			}
			array[count++] = other;
		}

		void pop_back()
		{
			if(count==0)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("CANNOT POP EMPTY ARRAY");
			}
			count--;
		}

		T &operator[](int i)
		{
			if(i>=count)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("INVALID INDEX");
			}
			return array[i];
		}

		T* begin() { return array; }

		T* end() { return &(array[SIZE]); }

		void erase(T *it)
		{
			int index = int(it-begin());
			for(int a=index;a<(count-1);a++)
			{
				array[a] = array[a+1];
			}
			count--;
		}
	};
}

#endif

