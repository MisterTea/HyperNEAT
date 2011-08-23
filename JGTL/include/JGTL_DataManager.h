#ifndef DATAMANAGER_H_INCLUDED
#define DATAMANAGER_H_INCLUDED

#ifndef DEBUG_DATA_MANAGER
#define DEBUG_DATA_MANAGER (0)
#endif

#include <cstdlib>
#include <cctype> // for toupper
#include <string>
#include <algorithm>
#include <map>

#include "JGTL_LocatedException.h"

namespace JGTL
{

	template<class Data>
	class DataManager
	{
	public:
	protected:
		int numElements,maxElements;
		//vector<Data> dataList;
		Data* dataList;
		std::map<std::string,Data *> dataMap,dataCaseInsensitiveMap;

	public:
		DataManager(int _maxElements)
			:
		numElements(0),
			maxElements(_maxElements)
		{
			//dataList = alloc.allocate(maxElements);
			dataList = (Data*)malloc(sizeof(Data)*maxElements);
		}

		virtual ~DataManager()
		{
			for (int a=0;a<numElements;a++)
			{
				dataList[a].~Data();
			}
			free(dataList);
			//alloc.deallocate(dataList,maxElements);
		}

		void addData(Data &data)
		{
			if (numElements==maxElements)
			{
				std::ostringstream oss;
				oss << maxElements;
				throw CREATE_LOCATEDEXCEPTION_INFO(string("Tried to add too many elements to a data manager! Size: ")+oss.str()+string("\n"));
			}

			//dataList.push_back(data);
			//alloc.construct(dataList+numElements,data);
			new(dataList+numElements) Data(data);

			Data *pointer = &dataList[numElements++];

			dataMap[pointer->getName()] = pointer;

			std::string tmpString = pointer->getName();
#if DEBUG_DATA_MANAGER
			cout << "Converting " << tmpString << " to upper case!\n";
#endif

			transform(
				tmpString.begin(),
				tmpString.end(),
				tmpString.begin(),
				(int(*)(int)) toupper
				);
			dataCaseInsensitiveMap[tmpString] = pointer;
		}

		//inline const int &getDataSize() const
		//{
		//return dataList.size();
		//}

		inline Data &getData(int index) const
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

				return *((*dataMap.find(key)).second);
			}
			else
			{
				std::string key2 = key;
				transform(
					key2.begin(),
					key2.end(),
					key2.begin(),
					(int(*)(int)) toupper
					);
				if (!dataCaseInsensitiveMap.count(key2))
				{
					throw CREATE_LOCATEDEXCEPTION_INFO(string("Could not find data named ")+key+string("!"));
				}

				return *((*dataMap.find(key2)).second);
			}
		}

		inline Data *getDataPtr(int index) const
		{
			if (index<0||index>=numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to get a data pointer out of range!");
			}

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

		inline int getSize() const
		{
			return numElements;
		}

		Data *getDataPtr(const std::string &key,bool caseSensitive=true) const
		{
			if (caseSensitive)
			{
				if (!dataMap.count(key))
				{
					return NULL;
				}

				return (*dataMap.find(key)).second;
			}
			else
			{
				std::string key2 = key;
				transform(
					key2.begin(),
					key2.end(),
					key2.begin(),
					(int(*)(int)) toupper
					);
				if (!dataCaseInsensitiveMap.count(key2))
				{
					return NULL;
				}

				return (*dataCaseInsensitiveMap.find(key2)).second;
			}
		}

		inline int getIndex(const Data* data) const
		{
			if (data<end() && data>=begin())
				return int(data-begin());

			return int(end());
		}

		void refreshNames()
		{
			dataMap.clear();
			dataCaseInsensitiveMap.clear();

			for (int a=0;a<numElements;a++)
			{
				dataMap[dataList[a].getName()] = &dataList[a];

				std::string tmpString = dataList[a].getName();
#if DEBUG_DATA_MANAGER
				cout << "Converting " << tmpString << " to upper case!\n";
#endif

				transform(
					tmpString.begin(),
					tmpString.end(),
					tmpString.begin(),
					(int(*)(int)) toupper
					);
				dataCaseInsensitiveMap[tmpString] = &dataList[a];
			}
		}

	protected:
	};

}

#endif // DATAMANAGER_H_INCLUDED
