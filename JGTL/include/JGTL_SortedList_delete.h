#ifndef SORTEDLIST_H_INCLUDED
#define SORTEDLIST_H_INCLUDED

#include "JGTL_LocatedException.h"

#define DEBUG_SORTED_LIST (0)

namespace JGTL
{

	template<class Data>
	class SortedList
	{
	public:
	protected:
		size_t numElements,maxElements;
		//vector<Data> dataList;
		Data* dataList;


	public:
		SortedList(size_t _maxElements)
			:
		numElements(0),
			maxElements(_maxElements)
		{
			allocator<Data> alloc;
			dataList = alloc.allocate(maxElements);
			//dataList = (Data*)malloc(sizeof(Data)*maxElements);
		}

		virtual ~SortedList()
		{
			allocator<Data> alloc;
			//free(dataList);
			for (size_t a=0;a<numElements;a++)
				alloc.destroy(dataList+a);
			alloc.deallocate(dataList,maxElements);
		}

		inline void addData(const Data &data)
		{
			allocator<Data> alloc;
			if (numElements==maxElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("Tried to add too many elements to a sorted list!\n");
			}

			//dataList.push_back(data);
			alloc.construct(dataList+numElements,data);
			//new(dataList+numElements) Data(data);
			numElements++;

			for (size_t a=0;a<(numElements-1);a++)
			{
				if (data<dataList[a])
				{
					for (size_t b=(numElements-1);b>=a;b--)
					{
						dataList[b] = dataList[b-1];
					}

					dataList[a] = data;

					return;
				}
			}
		}

		inline const size_t &getDataSize()
		{
			return dataList.size();
		}

		inline const bool hasData(const Data &other) const
		{
			if (!numElements)
				return false;

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if (other == dataList[i])
					return true;
				else if ( other < dataList[i] )
					high = i;
				else
					low  = i;
			}
			if ( other==dataList[high] )
				return( true );
			else
				return( false );
		}

		inline const Data &getData(size_t index) const
		{
			return dataList[index];
		}

		inline const Data *getDataPtr(size_t index) const
		{
			return &dataList[index];
		}

	protected:

	};

}

#endif // SORTEDLIST_H_INCLUDED
