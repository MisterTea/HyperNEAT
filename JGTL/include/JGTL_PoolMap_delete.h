#ifndef POOLMAP_H_INCLUDED
#define POOLMAP_H_INCLUDED

#ifndef DEBUG_POOL_MAP
#define DEBUG_POOL_MAP (0)
#endif

#include <utility>
#include <cstdlib>

#include "JGTL_LocatedException.h"

#if DEBUG_POOL_MAP
#include <iostream>
using namespace std;
#endif

namespace JGTL
{

	template<class Key,class Data>
	class PoolMap
	{
	public:
		typedef std::pair<Key,Data>* iterator;
		typedef const std::pair<Key,Data>* const_iterator;

	protected:
		int numElements,maxElements;

		std::pair<Key,Data> *dataList;


	public:
		PoolMap(int _maxElements)
			:
		numElements(0),
			maxElements(_maxElements)
		{
			dataList = (std::pair<Key,Data>*)malloc(sizeof(std::pair<Key,Data>)*maxElements);
		}

		PoolMap(const PoolMap<Key,Data> &other)
		{
			copyFrom(other);
		}

		const PoolMap &operator=(const PoolMap<Key,Data> &other)
		{
			copyFrom(other);

			return *this;
		}

		inline void copyFrom(const PoolMap<Key,Data> &other)
		{
			numElements = other.numElements;
			maxElements = other.maxElements;

			dataList = (std::pair<Key,Data>*)malloc(sizeof(std::pair<Key,Data>)*maxElements);

			for (int a=0;a<numElements;a++)
			{
				new(dataList+a) std::pair<Key,Data>(other.dataList[a]);
			}
		}

		virtual ~PoolMap()
		{
			clear();

			free(dataList);
		}

		void insert(const Key &key,const Data &data)
		{
			if (numElements==maxElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("Tried to add too many elements to a sorted list!\n");
			}

			if (!numElements)
			{
				new(dataList) std::pair<Key,Data>(key,data);

				++numElements;

				return;
			}

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if (key == dataList[i].first)
				{
					//The key exists, replace the data
					dataList[i].second = data;
					return;
				}
				else if ( key < dataList[i].first )
					high = i;
				else
					low  = i;
			}
			if ( key==dataList[high].first )
			{
				//The key exists, replace the data
				dataList[high].second = data;
				return;
			}
			else if ( key < dataList[high].first )
			{
				//The key does not exist, insert before high

				memmove(
					dataList+high+1,
					dataList+high,
					sizeof(std::pair<Key,Data>)*(numElements-high)
					);

				new(dataList+high) std::pair<Key,Data>(key,data);

				++numElements;
				return;
			}
			else //if ( key > dataList[high].first )
			{
				//The key does not exist, insert after high
				high++;

				memmove(
					dataList+high+1,
					dataList+high,
					sizeof(std::pair<Key,Data>)*(numElements-high)
					);

				new(dataList+high) std::pair<Key,Data>(key,data);

				++numElements;
				return;
			}


			/* The old way of inserting
			bool done=false;

			for (int a=0;!done&&a<numElements;a++)
			{
			if (key<dataList[a].first)
			{
			memmove(
			dataList+a+1,
			dataList+a,
			sizeof(std::pair<Key,Data>)*(numElements-a)
			);

			new(dataList+a) std::pair<Key,Data>(key,data);

			++numElements;

			done=true;
			}
			}

			if (!done)
			{
			new(dataList+numElements) std::pair<Key,Data>(key,data);

			++numElements;
			}
			*/

#if DEBUG_POOL_MAP
			cout << "DEBUG:\n";
			for (int a=0;a<numElements;a++)
			{
				cout << dataList[a].first << ',' << dataList[a].second << endl;
			}
#endif
		}

		inline int size()
		{
			return numElements;
		}

		void clear()
		{
			using namespace std; //std::~pair<Key,Data>() does not work

			for (int a=0;a<numElements;a++)
				(dataList+a)->~pair<Key,Data>();

			numElements=0;
		}

		iterator begin()
		{
			return dataList;
		}

		iterator end()
		{
			return dataList+numElements;
		}

		const bool hasKey(const Key &key) const
		{
			if (!numElements)
				return false;

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if (key == dataList[i].first)
					return true;
				else if ( key < dataList[i].first )
					high = i;
				else
					low  = i;
			}
			if ( key==dataList[high].first )
				return( true );
			else
				return( false );
		}

		iterator find(const Key &key)
		{
			if (!numElements)
				return end();

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if ( key < dataList[i].first )
					high = i;
				else if ( dataList[i].first < key )
					low  = i;
				else //they are equal
					return dataList+i;
			}
			if ( key == dataList[high].first )
				return( dataList + high ); //They are equal
			else
				return( end() );
		}

		Data *getData(const Key &key)
		{
			if (!numElements)
				return NULL;

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if (key == dataList[i].first)
					return &(dataList[i].second);
				else if ( key < dataList[i].first )
					high = i;
				else
					low  = i;
			}
			if ( key==dataList[high].first )
				return( &(dataList[high].second) );
			else
			{
#if DEBUG_POOL_MAP
				for (int a=0;a<numElements;a++)
				{
					if (dataList[a].first==key)
					{
						throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Binary search failed!");
					}
				}
#endif
				return NULL;
			}
		}

		const Data *getData(const Key &key) const
		{
			if (!numElements)
				return NULL;

			int high, i, low;

			for ( low=(-1), high=numElements-1;  high-low > 1;  )
			{
				i = (high+low) / 2;
				if (key == dataList[i].first)
					return &(dataList[i].second);
				else if ( key < dataList[i].first )
					high = i;
				else
					low  = i;
			}
			if ( key==dataList[high].first )
				return( &(dataList[high].second) );
			else
				return( NULL );
		}

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
				if (key == dataList[i].first)
					return (dataList[i].second);
				else if ( key < dataList[i].first )
					high = i;
				else
					low  = i;
			}
			if ( key==dataList[high].first )
			{
				return( (dataList[high].second) );
			}
			else
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to get a null reference!");
			}
		}

		inline const Data &getIndexData(int index) const
		{
			return dataList[index].second;
		}

		inline const Data *getIndexDataPtr(int index) const
		{
			return &dataList[index].second;
		}

	protected:

	};

}

#endif // POOLMAP_H_INCLUDED
