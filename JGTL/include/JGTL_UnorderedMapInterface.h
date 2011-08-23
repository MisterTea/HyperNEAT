#ifndef MAPINTERFACE_H_INCLUDED
#define MAPINTERFACE_H_INCLUDED

#ifndef DEBUG_MAP_INTERFACE
#define DEBUG_MAP_INTERFACE (0)
#endif

#include <utility>
#include <cstdlib>

#include "JGTL_LocatedException.h"

#if DEBUG_MAP_INTERFACE
#include <iostream>
using namespace std;
#endif

namespace JGTL
{

	class BinaryTreeNode
	{
	public:
		BinaryTreeNode* parent;
		BinaryTreeNode* left;
		BinaryTreeNode* right;

		BinaryTreeNode()
		{
			parent = left = right = NULL;
		}

		BinaryTreeNode(BinaryTreeNode* _parent)
			:
		parent(_parent)
		{}

		BinaryTreeNode(
			BinaryTreeNode* _parent,
			BinaryTreeNode* _left,
			BinaryTreeNode* _right
			)
			:
		parent(_parent),
			left(_left),
			right(_right)
		{}

	protected:
		//non-copyable
		BinaryTreeNode(const BinaryTreeNode &other);

		const BinaryTreeNode &operator=(const BinaryTreeNode &other);

	};

	template<class Key,class Data>
	class MapInterface
	{
	public:
		typedef std::pair<Key,Data> TreeItem;
		typedef BinaryTreeNode TreeNode;

	protected:
		int numElements,maxElements;

		//dataList pointer.  This is allocated by the derivatives of this class.
		TreeItem *dataList;

		//nodeList pointer.  This is allocated by the derivatives of this class.
		TreeNode *nodeList;

		int rootIndex;

	public:
		MapInterface()
			:
		numElements(0),
			maxElements(0),
			dataList(NULL),
			nodeList(NULL),
			rootIndex(-1)
		{}

		virtual ~MapInterface()
		{
			/*NOTE:
			The derived class is responsible for freeing the memory
			*/
		}

		bool operator==(const MapInterface &other) const
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

		virtual bool reserve(int newSize)
		{
			//By default, don't support resizing.
			return false;
		}

		virtual void insert(const Key &key,const Data &data)
		{
			if (!numElements)
			{
				if (!maxElements)
				{
					if (!this->reserve(16))
					{
						throw CREATE_LOCATEDEXCEPTION_INFO( \
							"Tried to add too many elements to a sorted list!\n" \
							);
					}
				}

				new(dataList) TreeItem(key,data);
				new(nodeList) TreeNode();

				rootIndex = 0;

				++numElements;

				return;
			}

			bool lastMoveLeft;
			int parentIndex=-1;
			int tmpIndex = rootIndex;

			while (tmpIndex!=-1)
			{
				if (key == dataList[tmpIndex].first)
				{
					//The key exists, replace the data
					dataList[tmpIndex].second = data;
					return;
				}
				else if ( key < dataList[tmpIndex].first )
				{
					//Go down the left branch
					lastMoveLeft=true;
					parentIndex = tmpindex;
					tmpindex = nodeList[tmpIndex].left;
				}
				else
				{
					//Go down the right branch
					lastMoveLeft=false;
					parentIndex = tmpindex;
					tmpindex = nodeList[tmpIndex].right;
				}
			}

			if (numElements==maxElements)
			{
				int newSize = maxElements;

				if (newSize==0)
					newSize = 16;
				else
					newSize *= 2;

				if (!this->reserve(newSize))
				{
					throw CREATE_LOCATEDEXCEPTION_INFO("Tried to add too many elements to a MapInterface!");
				}
			}

			new(dataList+numElements) TreeItem(key,data);
			new(nodeList+numElements) TreeNode(parentIndex);

			if (lastMoveLeft)
			{
				nodeList[parentIndex].left = numElements;
			}
			else
			{
				nodeList[parentIndex].right = numElements;
			}

			++numElements;

#if DEBUG_MAP_INTERFACE
			cout << "DEBUG:\n";
			for (int a=0;a<numElements;a++)
			{
				cout << dataList[a].first << ',' << dataList[a].second << endl;
			}
#endif

			return;
		}

		inline int size() const
		{
			return numElements;
		}

		void clear()
		{
			using namespace std; //std::~pair<Key,Data>() does not work

			for (int a=0;a<numElements;a++)
			{
				(dataList+a)->~TreeItem();
				(nodeList+a)->~TreeNode();
			}

			numElements=0;
		}

		const bool hasKey(const Key &key) const
		{
			if (!numElements)
				return false;

			int tmpIndex = rootIndex;

			while (tmpIndex!=-1)
			{
				if (key == dataList[tmpIndex].first)
				{
					//The key exists
					return true;
				}
				else if ( key < dataList[tmpIndex].first )
				{
					//Go down the left branch
					tmpindex = nodeList[tmpIndex].left;
				}
				else
				{
					//Go down the right branch
					tmpindex = nodeList[tmpIndex].right;
				}
			}

			return false;
		}

		iterator find(const Key &key)
		{
			if (!numElements)
				return end();

			int tmpIndex = rootIndex;

			while (tmpIndex!=-1)
			{
				if (key == dataList[tmpIndex].first)
				{
					//The key exists
					return true;
				}
				else if ( key < dataList[tmpIndex].first )
				{
					//Go down the left branch
					tmpindex = nodeList[tmpIndex].left;
				}
				else
				{
					//Go down the right branch
					tmpindex = nodeList[tmpIndex].right;
				}
			}

			return false;
		}

		const_iterator find(const Key &key) const
		{
			if (!numElements)
				return end();

			int tmpIndex = rootIndex;

			while (tmpIndex!=-1)
			{
				if (key == dataList[tmpIndex].first)
				{
					//The key exists
					return true;
				}
				else if ( key < dataList[tmpIndex].first )
				{
					//Go down the left branch
					tmpindex = nodeList[tmpIndex].left;
				}
				else
				{
					//Go down the right branch
					tmpindex = nodeList[tmpIndex].right;
				}
			}

			return false;
		}

		Data *getData(const Key &key)
		{
			if (!numElements)
				return NULL;

			int tmpIndex = rootIndex;

			while (tmpIndex!=-1)
			{
				if (key == dataList[tmpIndex].first)
				{
					//The key exists
					return &(dataList[tmpIndex].second);
				}
				else if ( key < dataList[tmpIndex].first )
				{
					//Go down the left branch
					tmpindex = nodeList[tmpIndex].left;
				}
				else
				{
					//Go down the right branch
					tmpindex = nodeList[tmpIndex].right;
				}
			}

			return NULL;
		}

		const Data *getData(const Key &key) const
		{
			if (!numElements)
				return NULL;

			int tmpIndex = rootIndex;

			while (tmpIndex!=-1)
			{
				if (key == dataList[tmpIndex].first)
				{
					//The key exists
					return &(dataList[tmpIndex].second);
				}
				else if ( key < dataList[tmpIndex].first )
				{
					//Go down the left branch
					tmpindex = nodeList[tmpIndex].left;
				}
				else
				{
					//Go down the right branch
					tmpindex = nodeList[tmpIndex].right;
				}
			}

			return NULL;
		}

		const Data &getDataRef(const Key &key) const
		{
			if (!numElements)
			{
				throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to get a null reference!");
			}

			int tmpIndex = rootIndex;

			while (tmpIndex!=-1)
			{
				if (key == dataList[tmpIndex].first)
				{
					//The key exists
					return dataList[tmpIndex].second;
				}
				else if ( key < dataList[tmpIndex].first )
				{
					//Go down the left branch
					tmpindex = nodeList[tmpIndex].left;
				}
				else
				{
					//Go down the right branch
					tmpindex = nodeList[tmpIndex].right;
				}
			}

			throw CREATE_LOCATEDEXCEPTION_INFO("ERROR: Tried to get a null reference!");
		}

		void erase(const_iterator iter)
		{
			using namespace std;

			int index = (iter-dataList);
			dataList[index].~pair<Key,Data>();

			memmove(
				dataList+index,
				dataList+index+1,
				sizeof(TreeItem)*(numElements-(index+1))
				);

			numElements--;
		}

		inline const Data &getIndexData(int index) const
		{
			return dataList[index].second;
		}

		inline Data *getIndexDataPtr(int index)
		{
			return &dataList[index].second;
		}

		inline const Data *getIndexDataPtr(int index) const
		{
			return &dataList[index].second;
		}

	protected:

	};

}

#endif // MAPINTERFACE_H_INCLUDED

