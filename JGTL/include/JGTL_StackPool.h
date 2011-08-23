#ifndef __JGTL_STACKPOOL_H__
#define __JGTL_STACKPOOL_H__

#include "JGTL_LocatedException.h"
#include <cstdlib>
#include <cstring>

namespace JGTL
{
	template<class T,int SIZE>
	class StackPool
	{
		T array[SIZE];
		unsigned char valid[SIZE];
		int count;
		int placeIndex;

	public:
		StackPool()
			:
			count(0),
			placeIndex(0)
		{
			memset(valid,0,sizeof(unsigned char)*SIZE);
		}

		inline int size() { return count; }

		inline bool empty() { return count==0; }

		int add(const T &other)
		{
			if(count==SIZE)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ARRAY IS TOO FULL");
			}
			count++;
			array[placeIndex] = other;
			valid[placeIndex] = true;
			int placedOn = placeIndex;
			if(count != SIZE)
			{
				while(valid[placeIndex])
				{
					placeIndex = (placeIndex+1)%SIZE;
				}
			}
			return placedOn;
		}

		T &operator[](int i)
		{
			if(valid[i])
				return array[i];
			throw CREATE_LOCATEDEXCEPTION_INFO("INVALID INDEX");
		}

		T &getByIndex(int i)
		{
			if(count<=i)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("INVALID INDEX");
			}
			for(int a=0;;a++)
			{
				if(a==SIZE) throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
				if(valid[a]) i--;
				if(i==-1)
				{
					return array[a];
				}
			}
		}

		void erase(T *it)
		{
			int index = int(it-array);
			if(index<0 || index>SIZE) throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
			if(valid[index]==false) throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
			valid[index]=false;
			count--;
		}

		bool contains(T *it)
		{
			return it>=array && it<&array[SIZE];
		}

        int getIndex(T *it)
        {
			if( it>=array && it<&array[SIZE] )
            {
                return int(it-array);
            }
            return -1;
        }
	};
}

#endif

