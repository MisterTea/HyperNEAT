#ifndef DYNAMICPOOLSET_H_INCLUDED
#define DYNAMICPOOLSET_H_INCLUDED

#ifndef DEBUG_DYNAMIC_POOL_SET
#define DEBUG_DYNAMIC_POOL_SET (0)
#endif

#include <utility>
#include <cstdlib>

#include "JGTL_LocatedException.h"

#include "JGTL_SetInterface.h"

#if DEBUG_DYNAMIC_POOL_SET
#include <iostream>
using namespace std;
#endif

namespace JGTL
{

	template<class Data>
	class DynamicPoolSet : public SetInterface<Data>
	{
	public:
		using SetInterface<Data>::dataList;
		using SetInterface<Data>::clear;

	protected:
		using SetInterface<Data>::numElements;
		using SetInterface<Data>::maxElements;

	public:
		DynamicPoolSet()
			:
		SetInterface<Data>()
		{
		}

		DynamicPoolSet(const DynamicPoolSet<Data> &other)
		{
			copyFrom(other);
		}

		const DynamicPoolSet &operator=(const DynamicPoolSet<Data> &other)
		{
			clear();
			copyFrom(other);

			return *this;
		}

		inline void copyFrom(const DynamicPoolSet &other)
		{
			numElements = other.numElements;
			maxElements = other.maxElements;

			dataList = (Data*)realloc( dataList, sizeof(Data)*maxElements );

			for (int a=0;a<numElements;a++)
			{
				new(dataList+a) Data(other.dataList[a]);
			}
		}

		virtual ~DynamicPoolSet()
		{
			clear();

			if(dataList)
			{
				free(dataList);
			}
		}

		bool operator==(const DynamicPoolSet &other) const
		{
			if (numElements != other.numElements)
			{
				return false;
			}

			for (int a=0;a<numElements;a++)
			{
				if (dataList[a]!=other.dataList[a])
				{
					return false;
				}
			}

			return true;
		}

		virtual bool resize(int newSize)
		{
			if (newSize<=numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: TRIED TO resize LESS SPACE THAN WHAT IS CURRENTLY NEEDED!");
			}

			maxElements = newSize;
			dataList = (Data*)realloc(dataList,sizeof(Data)*maxElements);

			return true;
		}
	};

}

#endif // DYNAMICPOOLSET_H_INCLUDED
