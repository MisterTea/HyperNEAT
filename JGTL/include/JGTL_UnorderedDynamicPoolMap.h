#ifndef DYNAMICPOOLMAP_H_INCLUDED
#define DYNAMICPOOLMAP_H_INCLUDED

#ifndef DEBUG_DYNAMIC_POOL_MAP
#define DEBUG_DYNAMIC_POOL_MAP (0)
#endif

#include "MapInterface.h"

#include <utility>
#include <cstdlib>

#include "JGTL_LocatedException.h"

#if DEBUG_DYNAMIC_POOL_MAP
#include <iostream>
using namespace std;
#endif

namespace JGTL
{

	template<class Key,class Data>
	class DynamicPoolMap : public MapInterface<Key,Data>
	{
	public:
		using MapInterface<Key,Data>::dataList;
		using MapInterface<Key,Data>::clear;

	protected:
		using MapInterface<Key,Data>::numElements;
		using MapInterface<Key,Data>::maxElements;

	public:
		DynamicPoolMap()
			:
		MapInterface<Key,Data>()
		{}

		DynamicPoolMap(const DynamicPoolMap &other)
		{
			copyFrom(other);
		}

		const DynamicPoolMap &operator=(const DynamicPoolMap &other)
		{
			copyFrom(other);

			return *this;
		}

		virtual void copyFrom(const DynamicPoolMap &other)
		{
			numElements = other.numElements;
			maxElements = other.maxElements;

			if (maxElements)
			{
				dataList = (TreeItem*)malloc( \
					sizeof(TreeItem)*maxElements \
					);
				nodeList = (TreeNode*)malloc( \
					sizeof(TreeNode)*maxElements \
					);
			}
			else
			{
				dataList = NULL;
				nodeList = NULL;
			}

			for (int a=0;a<numElements;a++)
			{
				new(dataList+a) TreeItem(other.dataList[a]);

				//Because the memory is contiguous,
				//we can pull some funky math to get the new pointers.

				BinaryTreeNode *newParent =
					(nodeList+(other.nodeList[a].parent-other.nodeList));
				BinaryTreeNode *newLeft =
					(nodeList+(other.nodeList[a].left-other.nodeList));
				BinaryTreeNode *newRight =
					(nodeList+(other.nodeList[a].right-other.nodeList));

				new(nodeList+a) TreeNode(
					newParent,
					newLeft,
					newRight
					);
			}
		}

		virtual ~DynamicPoolMap()
		{
			clear();

			if (dataList)
			{
				free(dataList);
				free(nodeList);
			}
		}

		virtual bool reserve(int newSize)
		{
			if (newSize<=numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: TRIED TO RESERVE LESS SPACE THAN WHAT IS CURRENTLY NEEDED!");
			}

			maxElements = newSize;
			dataList = (TreeItem*)realloc(dataList,sizeof(TreeItem)*maxElements);

			TreeNode *prevPtr = nodeList;

			nodeList = (TreeNode*)realloc(nodeList,sizeof(TreeNode)*maxElements);

			if (nodeList != prevPtr)
			{
				//We had to move locations in memory, recalculate pointers!

				int diff = (int)(nodeList-prevPtr);

				for (int a=0;a<numElements;a++)
				{
					nodeList[a].parent += diff;
					nodeList[a].left += diff;
					nodeList[a].right += diff;
				}
			}

			return true;
		}

	protected:

	};

}

#endif // DYNAMICPOOLMAP_H_INCLUDED
