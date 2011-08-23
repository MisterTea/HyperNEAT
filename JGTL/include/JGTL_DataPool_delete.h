#ifndef DATAPOOL_H_INCLUDED
#define DATAPOOL_H_INCLUDED

#ifndef DEBUG_DATA_POOL
#define DEBUG_DATA_POOL (0)
#endif

#include <memory>

#include "JGTL_LocatedException.h"

namespace JGTL
{

	template<class Data>
	class DataPool
	{
	public:
	protected:
		size_t numElements,maxElements;
		//vector<Data> dataList;
		Data* dataList;
		bool* used;
		map<string,Data *> dataMap,dataCaseInsensitiveMap;
		allocator<Data> alloc;
		allocator<bool> boolAlloc;

	public:
		DataPool(size_t _maxElements)
			:
		numElements(0),
			maxElements(_maxElements)
		{
			dataList = alloc.allocate(maxElements);
			used = boolAlloc.allocate(maxElements);
			//dataList = (Data*)malloc(sizeof(Data)*maxElements);
		}

		virtual ~DataPool()
		{
			//free(dataList);
			for (size_t a=0;a<numElements;a++)
				alloc.destroy(dataList+a);
			alloc.deallocate(dataList,maxElements);
			alloc.deallocate(used,maxElements);
		}

		inline void addData(const Data &data)
		{
			if (numElements==maxElements)
			{
				std::ostringstream oss;
				oss << maxElements;
				throw CREATE_LOCATEDEXCEPTION_INFO(string("Tried to add too many elements to a data manager! Size: ")+oss.str()+string("\n"));
			}

			//dataList.push_back(data);
			alloc.construct(dataList+numElements,data);
			//new(dataList+numElements) Data(data);

			Data *pointer = &dataList[numElements++];

			dataMap[pointer->getName()] = pointer;

			string tmpString = pointer->getName();
#if DEBUG_DATA_MANAGER
			cout << "Converting " << tmpString << " to upper case!\n";
#endif
			to_upper(tmpString);
			dataCaseInsensitiveMap[tmpString] = pointer;
		}

		//inline const size_t &getDataSize() const
		//{
		//return dataList.size();
		//}

		inline Data &getData(size_t index) const
		{
			return dataList[index];
		}

		inline Data &getData(const string &key,bool caseSensitive=true) const
		{
			if (caseSensitive)
			{
				if (!dataMap.count(key))
				{
					throw CREATE_LOCATEDEXCEPTION_INFO(string("Could not find data named ")+key+string("!"));
				}

				return *(dataMap[key]);
			}
			else
			{
				string key2 = key;
				to_upper(key2);
				if (!dataCaseInsensitiveMap.count(key2))
				{
					throw CREATE_LOCATEDEXCEPTION_INFO(string("Could not find data named ")+key+string("!"));
				}

				return *(dataCaseInsensitiveMap[key2]);
			}
		}

		inline Data *getDataPtr(size_t index) const
		{
			return &dataList[index];
		}

		inline Data* begin() const
		{
			return &dataList[0];
		}

		inline Data* end() const
		{
			return &dataList[numElements];
		}

		inline size_t getSize() const
		{
			return numElements;
		}

		inline Data *getDataPtr(const string &key,bool caseSensitive=true)
		{
			if (caseSensitive)
			{
				if (!dataMap.count(key))
				{
					return NULL;
				}

				return dataMap[key];
			}
			else
			{
				string key2 = key;
				to_upper(key2);
				if (!dataCaseInsensitiveMap.count(key2))
				{
					return NULL;
				}

				return dataCaseInsensitiveMap[key2];
			}
		}

	protected:
	};

}

#endif // DATAPOOL_H_INCLUDED
