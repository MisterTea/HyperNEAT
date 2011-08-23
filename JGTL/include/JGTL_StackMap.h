#ifndef STACKMAP_H_INCLUDED
#define STACKMAP_H_INCLUDED

#ifndef DEBUG_STACK_MAP
#define DEBUG_STACK_MAP (0)
#endif

#include "JGTL_MapInterface.h"

#include <utility>
#include <stdexcept>
#include <cstdlib>

#if DEBUG_STACK_MAP
#include <iostream>
using namespace std;
#endif

namespace JGTL
{
	/**
	* @class StackMap
	* @brief The StackMap Class is a fixed, array-based, sorted key structure
	*
	* @author Jason Gauci
	* @Date 2008
	*/

	template<class Key,class Data,int MAX_ELEMENTS=8>
	class StackMap : public MapInterface<Key,Data>
	{
	public:
		using MapInterface<Key,Data>::clear;

	protected:
		using MapInterface<Key,Data>::numElements;
		using MapInterface<Key,Data>::maxElements;

		std::pair<Key,Data> data[MAX_ELEMENTS];


	public:
		/*
		* Constructor
		*/
		StackMap()
			:
		MapInterface<Key,Data>()
		{
			maxElements = MAX_ELEMENTS;
		}

		/*
		* Copy Constructor
		*/
		StackMap(const StackMap &other)
		{
			copyFrom(other);
		}

		/*
		* Assignment Operator
		*/
		const StackMap &operator=(const StackMap &other)
		{
            clear();
			copyFrom(other);

			return *this;
		}

		/*
		* Destructor Operator
		*/
		virtual ~StackMap()
		{
			clear();
		}

		/*
		* Resizes the capacity of the map
		* @return True if the map was resized, false if the resize failed
		*/
		virtual bool resize(int newSize)
		{
			//Don't support resizing.
			return false;
		}

		/*
		* Gets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		virtual std::pair<Key,Data> &getIndex(int index)
		{
			return data[index];
		}

		virtual const std::pair<Key,Data> &getIndex(int index) const
		{
			return data[index];
		}

		/*
		* Sets an item from the map given it's index
		* @param iter An iterator to an index in the map
		*/
		virtual void setIndex(const std::pair<Key,Data> &entry,int index)
        {
            data[index]=entry;
        }

	protected:
		virtual void copyFrom(const StackMap &other)
		{
			numElements = other.numElements;
			maxElements = MAX_ELEMENTS;

            if(numElements>maxElements)
            {
                throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
            }

			for (int a=0;a<numElements;a++)
			{
				data[a] = other.data[a];
			}
		}

	};

}

#endif // STACKMAP_H_INCLUDED
