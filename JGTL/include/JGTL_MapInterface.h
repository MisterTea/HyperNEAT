#ifndef MAPINTERFACE_H_INCLUDED
#define MAPINTERFACE_H_INCLUDED

#ifndef DEBUG_MAP_INTERFACE
#define DEBUG_MAP_INTERFACE (0)
#endif

#include <utility>
#include <cstdlib>
#include <cstring>
#include "JGTL_LocatedException.h"

#if DEBUG_MAP_INTERFACE
#include <iostream>
using namespace std;
#endif

namespace JGTL
{
	/**
	* @class MapInterface
	* @brief This class acts as a base class for the Map construct
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class Key,class Data>
	class MapInterface
	{
	public:
		typedef std::pair<Key,Data>* iterator;
		typedef const std::pair<Key,Data>* const_iterator;

	protected:
		int numElements,maxElements;

	public:
		/*
		* Constructor
		*/
		MapInterface()
			:
		numElements(0),
			maxElements(0)
		{
			/*NOTE:
			The derived class is responsible for allocating the memory
			*/
		}

		/*
		* Destructor
		*/
		virtual ~MapInterface()
		{
			/*NOTE:
			The derived class is responsible for freeing the memory
			*/
		}

		/*
		* Tests each element of two maps for equality
		*/
		bool operator==(const MapInterface &other) const
		{
			if (numElements != other.numElements)
			{
				return false;
			}

			for (int a=0;a<numElements;a++)
			{
				if (getIndex(a)!=other.getIndex(a))
				{
					return false;
				}
			}

			return true;
		}

		/*
		* Resizes the capacity of the map
		* @return True if the map was resized, false if the resize failed
		*/
		virtual bool resize(int newSize) = 0;

		/*
		* Inserts a new item into the map
		*/
		virtual void insert(const Key &key,const Data &data)
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

				setIndex(std::pair<Key,Data>(key,data),0);

				++numElements;

				return;
			}

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if(i<0||i>=numElements)
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
				}
				if (key == getIndex(i).first)
				{
					//The key exists, replace the data
					getIndex(i).second = data;
					return;
				}
				else if ( key < getIndex(i).first )
					high = i;
				else
					low  = i;
			}
			if(high<0||high>=numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
			}
			if ( key==getIndex(high).first )
			{
				//The key exists, replace the data
				getIndex(high).second = data;
				return;
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

				if(high<0||high>=numElements)
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
				}
				if ( key < getIndex(high).first )
				{
					//The key does not exist, insert before high

					if(high != numElements)
					{
						if(high<0||high+1>=maxElements)
						{
							throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
						}
						/*
						memmove(
							dataList+high+1,
							dataList+high,
							sizeof(std::pair<Key,Data>)*(numElements-high)
							);
							*/
						for(int a=numElements;a>high;a--)
						{
							setIndex(getIndex(a-1),a);
						}
					}

					setIndex(std::pair<Key,Data>(key,data),high);

					++numElements;
					return;
				}
				else //if ( key > dataList[high].first )
				{
					//The key does not exist, insert after high
					high++;

					if(high<0||high>=maxElements)
					{
						throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
					}

					if(high != numElements)
					{
						if(high<0||high+1>=maxElements)
						{
							throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
						}
						/*
						memmove(
							dataList+high+1,
							dataList+high,
							sizeof(std::pair<Key,Data>)*(numElements-high)
							);
							*/
						for(int a=numElements;a>high;a--)
						{
							setIndex(getIndex(a-1),a);
						}
					}

					setIndex(std::pair<Key,Data>(key,data),high);

					++numElements;
					return;
				}
			}

#if DEBUG_MAP_INTERFACE
			cout << "DEBUG:\n";
			for (int a=0;a<numElements;a++)
			{
				cout << getIndex(a).first << ',' << getIndex(a).second << endl;
			}
#endif
		}

		/*
		* Inserts all the items from another map in this map
		*/
		virtual void insert(const MapInterface<Key,Data> &other)
        {
            for(int a=0;a<other.size();a++)
            {
                insert(other.getIndex(a).first,other.getIndex(a).second);
            }
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
		virtual void clear()
		{
			for(int a=0;a<numElements;a++)
			{
				setIndex(std::pair<Key,Data>(),a);
			}
			numElements = 0;
		}

		/*
		* Returns an iterator to the first item in the map
		*/
		iterator begin()
		{
			return getIndexPtr(0);
		}

		/*
		* Returns a const iterator to the first item in the map
		*/
		const_iterator begin() const
		{
			return getIndexPtr(0);
		}

		/*
		* Returns an iterator to the location just 
		* past the end of the map
		*/
		iterator end()
		{
			return getIndexPtr(0)+numElements;
		}

		/*
		* Returns a const iterator to the location just 
		* past the end of the map
		*/
		const_iterator end() const
		{
			return getIndexPtr(0)+numElements;
		}

		/*
		* Returns whether or not the key exists in a map
		* @param key The key to check for
		*/
		const bool hasKey(const Key &key) const
		{
			if (!numElements)
				return false;

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if(i<0||i>=numElements)
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
				}
				if (key == getIndex(i).first)
					return true;
				else if ( key < getIndex(i).first )
					high = i;
				else
					low  = i;
			}
			if(high<0||high>=numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
			}
			if ( key==getIndex(high).first )
				return( true );
			else
				return( false );
		}

		/*
		* Returns an iterator to an item by its key
		* @param key The key to look up
		*/
		iterator find(const Key &key)
		{
			if (!numElements)
				return end();

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if(i<0||i>=numElements)
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
				}
				if ( key < getIndex(i).first )
					high = i;
				else if ( getIndex(i).first < key )
					low  = i;
				else //they are equal
					return begin()+i;
			}
			if(high<0||high>=numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
			}
			if ( key == getIndex(high).first )
				return( begin() + high ); //They are equal
			else
				return( end() );
		}

		/*
		* Returns a const iterator to an item by its key
		* @param key The key to look up
		*/
		const_iterator find(const Key &key) const
		{
			if (!numElements)
				return end();

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if(i<0||i>=numElements)
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
				}
				if ( key < getIndex(i).first )
					high = i;
				else if ( getIndex(i).first < key )
					low  = i;
				else //they are equal
					return begin()+i;
			}
			if(high<0||high>=numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
			}
			if ( key == getIndex(high).first )
				return( begin() + high ); //They are equal
			else
				return( end() );
		}

		/*
		* Returns a pointer to an item by its key
		* @param key The key to look up
		*/
		Data *getData(const Key &key)
		{
			if (!numElements)
				return NULL;

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if(i<0||i>=numElements)
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
				}
				if (key == getIndex(i).first)
					return &(getIndex(i).second);
				else if ( key < getIndex(i).first )
					high = i;
				else
					low  = i;
			}
			if(high<0||high>=numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
			}
			if ( key==getIndex(high).first )
				return( &(getIndex(high).second) );
			else
			{
#if DEBUG_MAP_INTERFACE
				for (int a=0;a<numElements;a++)
				{
					if (getIndex(a).first==key)
					{
						throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Binary search failed!");
					}
				}
#endif
				return NULL;
			}
		}

		/*
		* Returns a const pointer to an item by its key
		* @param key The key to look up
		*/
		const Data *getData(const Key &key) const
		{
			if (!numElements)
				return NULL;

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if(i<0||i>=numElements)
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
				}
				if (key == getIndex(i).first)
					return &(getIndex(i).second);
				else if ( key < getIndex(i).first )
					high = i;
				else
					low  = i;
			}
			if(high<0||high>=numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
			}
			if ( key==getIndex(high).first )
				return( &(getIndex(high).second) );
			else
				return( NULL );
		}

		/*
		* Returns a reference to an item by its key
		* @param key The key to look up
		*/
		Data &getDataRef(const Key &key)
		{
			if (!numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to get a null reference!");
			}

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if(i<0||i>=numElements)
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
				}
				if (key == getIndex(i).first)
					return (getIndex(i).second);
				else if ( key < getIndex(i).first )
					high = i;
				else
					low  = i;
			}
			if(high<0||high>=numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
			}
			if ( key==getIndex(high).first )
			{
				return( (getIndex(high).second) );
			}
			else
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to get a null reference!");
			}
		}

		/*
		* Returns a const reference to an item by its key
		* @param key The key to look up
		*/
		const Data &getDataRef(const Key &key) const
		{
			if (!numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to get a null reference!");
			}

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if (key == getIndex(i).first)
					return (getIndex(i).second);
				else if ( key < getIndex(i).first )
					high = i;
				else
					low  = i;
			}
			if ( key==getIndex(high).first )
			{
				return( (getIndex(high).second) );
			}
			else
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to get a null reference!");
			}
		}

		/*
		* Erases an item from the map
		* @param iter An iterator to an item in the map
		*/
        void erase(const Key &key)
        {
            erase(find(key));
        }

		/*
		* Erases an item from the map
		* @param iter An iterator to an item in the map
		*/
		void erase(const_iterator iter)
		{
			if(iter==end())
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to erase an invalid iterator!");
			}

			using namespace std;

			int index = int(iter-begin());
			for(int a=index;a<numElements-1;a++)
			{
				setIndex(getIndex(a+1),a);
			}
			setIndex(pair<Key,Data>(),numElements-1);
			numElements--;
		}

		/*
		* Erases an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		void eraseIndex(int index)
		{
			erase(begin()+index);
		}

		/*
		* Erased all the items from another map in this map
		*/
		virtual void erase(const MapInterface<Key,Data> &other)
        {
            for(int a=0;a<other.size();a++)
            {
                erase(other.getIndex(a).first);
            }
        }

		/*
		* Gets an item from the map given it's index
		* @param iter An index in the map
		*/
		inline const Data &getIndexData(int index) const
		{
			return getIndex(index).second;
		}

		/*
		* Gets an item from the map given it's index
		* @param iter An index in the map
		*/
		inline Data &getIndexData(int index)
		{
			return getIndex(index).second;
		}

		/*
		* Gets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		inline Data *getIndexDataPtr(int index)
		{
			return &getIndex(index).second;
		}

		/*
		* Gets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		inline const Data *getIndexDataPtr(int index) const
		{
			return &getIndex(index).second;
		}

		/*
		* Gets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		virtual std::pair<Key,Data> &getIndex(int index) = 0;
		virtual const std::pair<Key,Data> &getIndex(int index) const = 0;

		/*
		* Sets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		virtual void setIndex(const std::pair<Key,Data> &entry,int index) = 0;

        /*
		* Gets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		inline iterator getIndexPtr(int index)
		{
			return &getIndex(index);
		}

		/*
		* Gets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		inline const_iterator getIndexPtr(int index) const
		{
			return &getIndex(index);
		}

	protected:

	};

}

#endif // MAPINTERFACE_H_INCLUDED

