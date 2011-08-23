#ifndef SETINTERFACE_H_INCLUDED
#define SETINTERFACE_H_INCLUDED

#ifndef DEBUG_SET_INTERFACE
#define DEBUG_SET_INTERFACE (0)
#endif

#include <utility>
#include <cstdlib>
#include "JGTL_LocatedException.h"

#if DEBUG_SET_INTERFACE
#include <iostream>
using namespace std;
#endif

namespace JGTL
{
	/**
	* @class SetInterface
	* @brief This class acts as a base class for the Set construct
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class Data>
	class SetInterface
	{
	public:
		typedef Data* iterator;
		typedef const Data* const_iterator;

	protected:
		int numElements,maxElements;

		//dataList pointer.  This is used by the derivatives of this class.
		Data *dataList;

	public:
		/*
		* Constructor
		*/
		SetInterface()
			:
		numElements(0),
			maxElements(0),
			dataList(NULL)
		{}

		/*
		* Destructor
		*/
		virtual ~SetInterface()
		{
			/*NOTE:
			The derived class is responsible for freeing the memory
			*/
		}

		/*
		* Tests each element of two maps for equality
		*/
		bool operator==(const SetInterface &other) const
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

		bool operator!=(const SetInterface<Data> &other) const
		{
			return !((*this)==other);
		}

		/*
		* Resizes the capacity of the map
		* @return True if the map was resized, false if the resize failed
		*/
		virtual bool resize(int newSize)
		{
			//By default, don't support resizing.
			return false;
		}

		/*
		* Inserts a new item into the map
		*/
		virtual iterator insert(const Data &data)
		{
			if (!numElements)
			{
				if (!maxElements)
				{
					if (!this->resize(16))
					{
						throw CREATE_LOCATEDEXCEPTION_INFO("Tried to add too many elements to a sorted list!");
					}
				}

				new(dataList) Data(data);

				++numElements;

				return dataList;
			}

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if (data == dataList[i])
				{
					//Data already exists, ignore
					return &dataList[i];
				}
				else if ( data < dataList[i] )
					high = i;
				else
					low  = i;
			}
			if ( data==dataList[high] )
			{
				//Data already exists, ignore
				return &dataList[high];
			}
			else
			{
				if (numElements==maxElements)
				{
					int newSize = maxElements;

					if (newSize==0)
						newSize = 16;
					else
						newSize *= 2;

					if (!this->resize(newSize))
					{
						throw CREATE_LOCATEDEXCEPTION_INFO("Tried to add too many elements to a sorted list!");
					}
				}

				if ( data < dataList[high] )
				{
					//The key does not exist, insert before high

					memmove(
						dataList+high+1,
						dataList+high,
						sizeof(Data)*(numElements-high)
						);

					new(dataList+high) Data(data);

					++numElements;
					return dataList+high;
				}
				else if(data == dataList[high])
				{
					//Done
					return dataList+high;
				}
				else //if ( data > dataList[high] )
				{
					//The key does not exist, insert after high
					high++;

					memmove(
						dataList+high+1,
						dataList+high,
						sizeof(Data)*(numElements-high)
						);

					new(dataList+high) Data(data);

					++numElements;
					return dataList+high;
				}
			}

#if DEBUG_SET_INTERFACE
			cout << "DEBUG:\n";
			for (int a=0;a<numElements;a++)
			{
				cout << dataList[a] << endl;
			}
#endif
		}

		/*
		* Retrieves the number of elements in the map
		*/
		inline int size() const
		{
			return numElements;
		}

		/*
		* Returns whether or not the map is empty
		*/
		inline bool empty() const
		{
			return numElements==0;
		}

		/*
		* Deletes all items from the map
		*/
		void clear()
		{
			for (int a=0;a<numElements;a++)
				(dataList+a)->~Data();

			numElements=0;
		}

		/*
		* Returns an iterator to the first item in the map
		*/
		iterator begin()
		{
			return dataList;
		}

		/*
		* Returns a const iterator to the first item in the map
		*/
		const_iterator begin() const
		{
			return dataList;
		}

		/*
		* Returns an iterator to the location just 
		* past the end of the map
		*/
		iterator end()
		{
			return dataList+numElements;
		}

		/*
		* Returns a const iterator to the location just 
		* past the end of the map
		*/
		const_iterator end() const
		{
			return dataList+numElements;
		}

		/*
		* Returns whether or not the data exists in a set
		* @param data The data to check for
		*/
		const bool hasData(const Data &data) const
		{
			if (!numElements)
				return false;

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if (data == dataList[i])
					return true;
				else if ( data < dataList[i] )
					high = i;
				else
					low  = i;
			}
			if ( data == dataList[high] )
				return( true );
			else
				return( false );
		}

		/*
		* Returns an iterator to an item by its data
		* @param data The data to look up
		*/
		iterator find(const Data &data)
		{
			if (!numElements)
				return end();

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if ( data < dataList[i] )
					high = i;
				else if ( dataList[i] < data )
					low  = i;
				else //they are equal
					return dataList+i;
			}
			if ( data == dataList[high] )
				return( dataList + high ); //They are equal
			else
				return( end() );
		}

		/*
		* Erases an item from the set
		* @param data The data to erase
		*/
		void erase(const Data &data)
		{
			iterator it = find(data);

			if(it==end())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("TRIED TO ERASE SOMETHING WHICH DOES NOT EXIST!");
			}
			else
			{
				erase(it);
			}
		}

		/*
		* Erases an item from the set
		* @param iter A const iterator to an item in the set
		*/
		void erase(const_iterator iter)
		{
			if(iter==end())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to erase an invalid iterator!");
			}

			using namespace std;

			int index = int(iter-dataList);
			dataList[index].~Data();

			memmove(
				dataList+index,
				dataList+index+1,
				sizeof(Data)*(numElements-(index+1))
				);

			numElements--;
		}

		/*
		* Erases an item from the set given it's index
		* @param iter An iterator to an index in the set
		*/
		void eraseIndex(int index)
		{
			erase(begin()+index);
		}

		/*
		* Gets a copy of an item from the set given it's index
		* @param iter An index in the set
		*/
		inline Data getIndex(int index) const
		{
			return dataList[index];
		}

		/*
		* Gets a reference to an item from the set given it's index
		* @param iter An index in the set
		*/
		inline const Data &getIndexRef(int index) const
		{
			return dataList[index];
		}

		/*
		* Gets an iterator to an item from the set given it's index
		* @param iter An index in the set
		*/
		inline iterator getIndexPtr(int index)
		{
			return &dataList[index];
		}

		/*
		* Gets a const iterator to an item from the set given it's index
		* @param iter An index in the set
		*/
		inline const_iterator getIndexPtr(int index) const
		{
			return &dataList[index];
		}

	protected:

	};

}

#endif // SETINTERFACE_H_INCLUDED


