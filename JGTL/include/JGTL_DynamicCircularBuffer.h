#ifndef DYNAMICCIRCULARBUFFER_H_INCLUDED
#define DYNAMICCIRCULARBUFFER_H_INCLUDED

#ifndef DEBUG_DYNAMIC_CIRCULAR_BUFFER
#define DEBUG_DYNAMIC_CIRCULAR_BUFFER (0)
#endif

#include <utility>
#include <cstdlib>
#include <cstring>

#include "JGTL_LocatedException.h"
#include "JGTL_CircularBufferInterface.h"

#if DEBUG_DYNAMIC_CIRCULAR_BUFFER
#include <iostream>
using namespace std;
#endif

namespace JGTL
{
	/**
	* @class DynamicCircularBuffer
	* @brief The DynamicCircularBuffer Class handles a Circular Buffer.
	*
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class Data>
	class DynamicCircularBuffer : public CircularBufferInterface<Data>
	{
	public:
		using CircularBufferInterface<Data>::clear;
		using CircularBufferInterface<Data>::size;

	protected:
		using CircularBufferInterface<Data>::elementStart;
		using CircularBufferInterface<Data>::elementEnd;
		using CircularBufferInterface<Data>::maxElements;
		using CircularBufferInterface<Data>::dataList;
		using CircularBufferInterface<Data>::enqueueLast;

	public:
		/*
		* Constructor
		* This creates a DynamicCircularBuffer
		* @param _maxElements The maximum size of the DynamicCircularBuffer
		*/
		DynamicCircularBuffer(int initialSize=0)
			:
		CircularBufferInterface<Data>()
		{
			if(initialSize)
			{
				resize(initialSize);
			}
		}

		/*
		* Copy Constructor
		*/
		DynamicCircularBuffer(const DynamicCircularBuffer &other)
		{
			copyFrom(other);
		}

		/*
		* Assignment Operator
		*/
		const DynamicCircularBuffer &operator=(const DynamicCircularBuffer &other)
		{
			copyFrom(other);

			return this;
		}

		/*
		* Destructor. Frees memory and deletes all items in the list.
		*/
		virtual ~DynamicCircularBuffer()
		{
			clear();

			free(dataList);
		}

		/*
		* Resizes the capacity of the buffer
		* @return True if the buffer was resized, false if the resize failed
		*/
		virtual bool resize(int newSize)
		{
			if (newSize<=size())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: TRIED TO RESERVE LESS SPACE THAN WHAT IS CURRENTLY NEEDED!");
			}

			int prevSize = maxElements;

			maxElements = newSize;

			dataList = (Data*)realloc(dataList,sizeof(Data)*maxElements);

			int sizeChange = maxElements - prevSize;

			if(sizeChange<0)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("Shrinking a circular buffer is not supported yet!");
			}

			if(size() && sizeChange>0)
			{
				if(
						elementEnd<elementStart ||
						(
							elementEnd==elementStart &&
							enqueueLast
						)
						)
				{
					//We need to move all of the elements from elementStart
					//to the end of the array and create/increase the gap
					//between elementStart and elementEnd
					memmove(
							dataList+elementStart+sizeChange,
							dataList+elementStart,
							sizeof(Data)*(prevSize-elementStart)
							);
					elementStart += sizeChange;
				}
				else
				{
					//We can just append to the end of the array
					dataList = (Data*)realloc(dataList,sizeof(Data)*maxElements);
				}
			}

			return true;
		}

	protected:
		inline void copyFrom(const DynamicCircularBuffer &other)
		{
			elementStart = other.elementStart;
			elementEnd = other.elementEnd;
			enqueueLast = other.enqueueLast;
			maxElements = other.maxElements;

			if (maxElements)
			{
				dataList = (Data*)malloc( sizeof(Data)*maxElements );
			}
			else
			{
				dataList = NULL;
			}

			throw CREATE_LOCATEDEXCEPTION_INFO("Not Supported yet!");

            /*
			for (int a=elementStart;a!=elementEnd;incCounter(a))
			{
				new(dataList+a) Data(other.dataList[a]);
			}
			*/
		}
	};

}

#endif // DYNAMICCIRCULARBUFFER_H_INCLUDED

