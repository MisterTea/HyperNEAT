#ifndef STACKCIRCULARBUFFER_H_INCLUDED
#define STACKCIRCULARBUFFER_H_INCLUDED

#ifndef DEBUG_STACK_CIRCULAR_BUFFER
#define DEBUG_STACK_CIRCULAR_BUFFER (0)
#endif

#include <utility>
#include <cstdlib>

#include "JGTL_LocatedException.h"
#include "JGTL_CircularBufferInterface.h"

#if DEBUG_STACK_CIRCULAR_BUFFER
#include <iostream>
using namespace std;
#endif

namespace JGTL
{
	/**
	* @class StackCircularBuffer
	* @brief The StackCircularBuffer Class handles a Circular Buffer.
	*
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class Data, int MAX_ELEMENTS=32>
	class StackCircularBuffer : public CircularBufferInterface<Data>
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
	protected:
		unsigned char data[MAX_ELEMENTS*sizeof(Data)];

	public:
		/*
		* Constructor
		* This creates a StackCircularBuffer
		* @param _maxElements The maximum size of the StackCircularBuffer
		*/
		StackCircularBuffer()
			:
		CircularBufferInterface<Data>(MAX_ELEMENTS)
		{
			dataList = (Data*)data;
		}

		/*
		* Copy Constructor
		*/
		StackCircularBuffer(const StackCircularBuffer &other)
		{
			copyFrom(other);
		}

		/*
		* Assignment Operator
		*/
		const StackCircularBuffer &operator=(const StackCircularBuffer &other)
		{
			copyFrom(other);

			return this;
		}

		/*
		* Destructor. Frees memory and deletes all items in the list.
		*/
		virtual ~StackCircularBuffer()
		{
			clear();
		}

		/*
		* Resizes the capacity of the buffer
		* @return True if the buffer was resized, false if the resize failed
		*/
		virtual bool resize(int newSize)
		{
			//Don't support resizing.
			return false;
		}

	protected:
		inline void copyFrom(const StackCircularBuffer &other)
		{
			elementStart = other.elementStart;
			elementEnd = other.elementEnd;
			enqueueLast = other.enqueueLast;
			maxElements = other.maxElements;

			dataList = (Data*)data;

			throw CREATE_LOCATEDEXCEPTION_INFO("Not Supported yet!");

			/*
			for (int a=0;a<numElements;a++)
			{
				new(dataList+a) Data(other.dataList[a]);
			}
			*/
		}
	};

}

#endif // STACKCIRCULARBUFFER_H_INCLUDED
