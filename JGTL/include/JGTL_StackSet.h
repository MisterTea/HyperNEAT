#ifndef STACKSET_H_INCLUDED
#define STACKSET_H_INCLUDED

#ifndef DEBUG_STACK_SET
#define DEBUG_STACK_SET (0)
#endif

#include "JGTL_SetInterface.h"

#include <utility>
#include <stdexcept>
#include <cstdlib>

#if DEBUG_STACK_SET
#include <iostream>
using namespace std;
#endif

#include "JGTL_SetInterface.h"

namespace JGTL
{

    template<class Data,int MAX_ELEMENTS=8>
    class StackSet : public SetInterface<Data>
    {
    public:
        using SetInterface<Data>::dataList;
        using SetInterface<Data>::clear;

    protected:
        using SetInterface<Data>::numElements;
        using SetInterface<Data>::maxElements;

        unsigned char data[MAX_ELEMENTS*sizeof(Data)];


    public:
        StackSet()
                :
                SetInterface<Data>()
        {
            maxElements = MAX_ELEMENTS;
            dataList = (Data*)data;
        }

        StackSet(const StackSet &other)
        {
            copyFrom(other);
        }

        const StackSet &operator=(const StackSet &other)
        {
			   clear();
            copyFrom(other);

            return *this;
        }

        virtual void copyFrom(const StackSet &other)
        {
            numElements = other.numElements;
            maxElements = MAX_ELEMENTS;

				if(numElements>maxElements)
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("OOPS");
				}

            dataList = (Data*)data;

            for (int a=0;a<numElements;a++)
            {
                new(dataList+a) Data(other.dataList[a]);
            }
        }

        virtual ~StackSet()
        {
            clear();
        }

    protected:

    };

}

#endif // STACKSET_H_INCLUDED

