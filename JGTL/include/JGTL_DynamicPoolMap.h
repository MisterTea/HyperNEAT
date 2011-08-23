#ifndef DYNAMICPOOLMAP_H_INCLUDED
#define DYNAMICPOOLMAP_H_INCLUDED

#ifndef DEBUG_DYNAMIC_POOL_MAP
#define DEBUG_DYNAMIC_POOL_MAP (0)
#endif

#include "JGTL_MapInterface.h"

#include <utility>
#include <cstdlib>
#include <vector>

#include "JGTL_LocatedException.h"

#if DEBUG_DYNAMIC_POOL_MAP
#include <iostream>
using namespace std;
#endif

namespace JGTL
{
	/**
	* @class DynamicPoolMap
	* @brief The DynamicPoolMap Class is a resizable array-based 
	* associative map structure.
	*
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class Key,class Data>
	class DynamicPoolMap : public MapInterface<Key,Data>
	{
	public:
		using MapInterface<Key,Data>::clear;

	protected:
		using MapInterface<Key,Data>::numElements;
		using MapInterface<Key,Data>::maxElements;

		std::vector< std::pair<Key,Data> > elementVector;

	public:
		/*
		* Constructor
		*/
		DynamicPoolMap()
			:
		MapInterface<Key,Data>()
		{
			resize(16);
		}

		/*
		* Constructor
		* @param _capacity Sets the initial capacity of the array
		*/
		DynamicPoolMap(int _capacity)
			:
		MapInterface<Key,Data>()
		{
			resize(_capacity);
		}

		/*
		* Copy Constructor
		*/
		DynamicPoolMap(const DynamicPoolMap &other)
		{
			copyFrom(other);
		}

		/*
		* Assignment Operator
		*/
		const DynamicPoolMap &operator=(const DynamicPoolMap &other)
		{
            clear();

			copyFrom(other);

			return *this;
		}

		/*
		* Destructor Operator
		*/
		virtual ~DynamicPoolMap()
		{
			clear();
		}

		/*
		* Resizes the capacity of the map
		* @return True if the map was resized, false if the resize failed
		*/
		virtual bool resize(int newSize)
		{
			if (newSize<=numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: TRIED TO RESERVE LESS SPACE THAN WHAT IS CURRENTLY NEEDED!");
			}

			maxElements = newSize;
			elementVector.resize(newSize);

			return true;
		}

		/*
		* Gets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		virtual std::pair<Key,Data> &getIndex(int index)
		{
			return elementVector[index];
		}

		virtual const std::pair<Key,Data> &getIndex(int index) const
		{
			return elementVector[index];
		}

		/*
		* Sets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		virtual void setIndex(const std::pair<Key,Data> &entry,int index)
        {
            elementVector[index]=entry;
        }

	protected:
		virtual void copyFrom(const DynamicPoolMap &other)
		{
			numElements = other.numElements;
			maxElements = other.maxElements;

			elementVector.clear();
			elementVector.resize(maxElements);

			for (int a=0;a<numElements;a++)
			{
				elementVector[a] = std::pair<Key,Data>(other.elementVector[a]);
			}
		}

	};

}

#endif // DYNAMICPOOLMAP_H_INCLUDED
