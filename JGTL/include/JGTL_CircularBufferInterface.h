#ifndef CIRCULARBUFFERINTERFACE_H_INCLUDED
#define CIRCULARBUFFERINTERFACE_H_INCLUDED

#ifndef DEBUG_CIRCULAR_BUFFER_INTERFACE
#define DEBUG_CIRCULAR_BUFFER_INTERFACE (0)
#endif

#include <utility>
#include <cstdlib>

#include "JGTL_LocatedException.h"

#if DEBUG_CIRCULAR_BUFFER_INTERFACE
#include <iostream>
using namespace std;
#endif

namespace JGTL
{
	/**
	* @class CircularBufferInterface
	* @brief The CircularBufferInterface Class handles a Circular Buffer.
	*
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class Data>
	class CircularBufferInterface
	{
	public:
	protected:
		int elementStart,elementEnd,maxElements;

		Data* dataList;

		/*
		* This variable is important for finding out if we added 
		* too many elements.
		*/
		bool enqueueLast;

	public:
		/*
		* Constructor
		* This creates a CircularBufferInterface
		* @param _maxElements The maximum size of the CircularBufferInterface
		*/
		CircularBufferInterface(int _maxElements=0)
			:
		elementStart(0),
			elementEnd(0),
			maxElements(_maxElements),
			dataList(NULL),
		enqueueLast(false)
		{
			/*NOTE:
			The derived class is responsible for allocating the memory
			*/
		}

		/*
		* Destructor. Frees memory and deletes all items in the list.
		*/
		virtual ~CircularBufferInterface()
		{
			/*NOTE:
			The derived class is responsible for clearing the array 
			and freeing the memory
			*/
		}

		/*
		* This function enqueues a new item
		* @param data The data to enqueue
		*/
		void enqueue(const Data &data)
		{
#if DEBUG_CIRCULAR_BUFFER_INTERFACE
			cout << "Enqueuing " << data << endl;
#endif
			if(dataList==NULL)
			{
				if(!this->resize(16))
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("Error! Invalid buffer!\n");
				}
			}

			if (elementEnd==elementStart && enqueueLast)
			{
				/*
				cout << "PERFORMING RESIZE!!!\n";

				for(int c=0;c<size();c++)
				{
					cout << getIndexRef(c) << ' ';
				}
				cout << endl;
				*/

				if(!this->resize(maxElements*2))
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("Tried to add too many elements to a circular buffer!\n");
				}

				/*
				for(int c=0;c<size();c++)
				{
					cout << getIndexRef(c) << ' ';
				}
				cout << endl;

				system("PAUSE");
				*/
			}

#if DEBUG_CIRCULAR_BUFFER_INTERFACE
			cout << "Creating object\n";

			cout << dataList << ' ' << elementEnd << endl;
#endif

			new(dataList+elementEnd) Data(data);

			incCounter(elementEnd);

			enqueueLast=true;

#if DEBUG_CIRCULAR_BUFFER_INTERFACE
			cout << "done\n";
#endif
		}

		/*
		* Resizes the capacity of the map
		* @return True if the buffer was resized, false if the resize failed
		*/
		virtual bool resize(int newSize) = 0;
		
		/*
		* Returns a copy of the front item in the queue
		*/
		Data front()
		{
			if(empty())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("Error, tried to access the front of an empty buffer!");
			}

			return dataList[elementStart];
		}

		/*
		* Returns a reference to the front item in the queue
		*/
		Data &frontRef()
		{
			if(empty())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("Error, tried to access the front of an empty buffer!");
			}

			return dataList[elementStart];
		}

		/*
		* Returns a const reference to the front item in the queue
		*/
		const Data &frontRef() const
		{
			if(empty())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("Error, tried to access the front of an empty buffer!");
			}

			return dataList[elementStart];
		}

		/*
		* Returns a pointer to the front item in the queue
		*/
		Data* frontPtr()
		{
			if(empty())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("Error, tried to access the front of an empty buffer!");
			}

			return &dataList[elementStart];
		}

		/*
		* Returns a const pointer to the front item in the queue
		*/
		const Data* frontPtr() const
		{
			if(empty())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("Error, tried to access the front of an empty buffer!");
			}

			return &dataList[elementStart];
		}

		/*
		* Removes an item from the back of the queue
		*/
		void dequeue()
		{
			if (empty())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("Tried to pop too many elements from a circular buffer!\n");
			}

			dataList[elementStart].~Data();

			incCounter(elementStart);

			enqueueLast=false;
		}

		/*
		* Returns the amount of elements currently in the queue
		*/
		inline int size() const
		{
			if(!maxElements)
			{
				return 0;
			}

			int tmpsize = ( (elementEnd+maxElements) - elementStart)%maxElements;

			if (enqueueLast && tmpsize==0)
			{
				return maxElements;
			}

			return tmpsize;
		}

		/*
		* Returns the maximum size of the queue
		*/
		inline int capacity() const
		{
			return maxElements;
		}

		/*
		* Returns if the queue is empty or not
		*/
		inline bool empty() const
		{
			return (elementEnd==elementStart) && !enqueueLast;
		}

		/*
		* Returns if the queue is full or not
		*/
		inline bool full() const
		{
			return (elementEnd==elementStart) && enqueueLast;
		}

		/*
		* Removes all items from the queue
		*/
		void clear()
		{
			using namespace std; //std::~pair<Key,Data>() does not work

			for (int a=elementStart;a!=elementEnd;incCounter(a))
			{
				(dataList+a)->~Data();
			}

			elementStart=elementEnd=0;
			enqueueLast=false;
		}

		/*
		* Gets a pointer to the element from a specific index, 
		* where index 0 is the back
		*/
		Data* getIndex(int index)
		{
			if(index>=size())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: INVALID INDEX!");
			}

			int ptr=elementStart+index;

			if(ptr>=maxElements)
			{
				ptr -= maxElements;
			}

			return &dataList[ptr];
		}

		/*
		* Gets a const pointer to the element from a specific index, 
		* where index 0 is the back
		*/
		const Data* getIndex(int index) const
		{
			if(index>=size())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: INVALID INDEX!");
			}

			int ptr=elementStart+index;

			if(ptr>=maxElements)
			{
				ptr -= maxElements;
			}

			return &dataList[ptr];
		}

		/*
		* Gets a reference to the element from a specific index, 
		* where index 0 is the back
		*/
		Data &getIndexRef(int index)
		{
			if(index>=size())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: INVALID INDEX!");
			}

			int ptr=elementStart+index;

			if(ptr>=maxElements)
			{
				ptr -= maxElements;
			}

			return dataList[ptr];
		}

		/*
		* Gets a const reference to the element from a specific index, 
		* where index 0 is the back
		*/
		const Data &getIndexRef(int index) const
		{
			if(index>=size())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: INVALID INDEX!");
			}

			int ptr=elementStart+index;

			if(ptr>=maxElements)
			{
				ptr -= maxElements;
			}

			return dataList[ptr];
		}

	protected:
		inline void incCounter(int &counter)
		{
			counter = (counter+1)%maxElements;
		}
	};

}

#endif // CIRCULARBUFFERINTERFACE_H_INCLUDED
