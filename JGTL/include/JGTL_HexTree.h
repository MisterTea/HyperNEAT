#ifndef HEXTREE_H_INCLUDED
#define HEXTREE_H_INCLUDED

#include <iostream>
#include <string>
using namespace std;

#include "Vector4.h"

#include "boost/pool/pool.hpp"

namespace JGTL
{

	template<class T>
	class HexTreeNode
	{
	protected:
	public:
		HexTreeNode()
		{}

		virtual ~HexTreeNode<T>()
		{
			cout << string("You must use the custom memory manager for HexTrees!");
			string line;
			getline(cin,line);
		}

		virtual T getValue(Vector4<int> topLeftVector4,int size,const Vector4<int> &location) const = 0;

		virtual T getValue() const = 0;

		virtual bool setValue(
			pool<> &branchPool,
			pool<> &stubPool,
			Vector4<int> topLeftVector4,
			int size,
			const Vector4<int> &location,
			const T &value
			) = 0;

		virtual bool isStub() const
		{
			return false;
		}

		virtual void destroy(
			pool<> &branchPool,
			pool<> &stubPool
			)
		{}

		virtual void display(int level) const = 0;

		virtual int getMemUsage() const = 0;
	};

	template<class T>
	class HexTreeStub : public HexTreeNode<T>
	{
	protected:
		T value;

	public:
		HexTreeStub(T _value)
			:
		HexTreeNode<T>(),
			value(_value)
		{}

		virtual ~HexTreeStub()
		{
			cout << string("You must use the custom memory manager for HexTrees!");
			string line;
			getline(cin,line);
		}

		virtual T getValue(Vector4<int> topLeftVector4,int size,const Vector4<int> &location) const
		{
			return value;
		}

		virtual T getValue() const
		{
			return value;
		}

		virtual bool setValue(
			pool<> &branchPool,
			pool<> &stubPool,
			Vector4<int> topLeftVector4,
			int size,
			const Vector4<int> &location,
			const T &value
			)
		{
			this->value = value;

			return false;
		}

		virtual bool setValue(const T &_value)
		{
			value = _value;

			return false;
		}

		virtual bool isStub() const
		{
			return true;
		}


		virtual void display(int level) const
		{
			for (int a=0;a<level;a++)
				printf(">");

			if (value)
				printf("+\n");
			else
				printf(" \n");
		}

		virtual int getMemUsage() const
		{
			return sizeof(HexTreeStub<T>);
		}
	};

	template<class T>
	class HexTreeBranch : public HexTreeNode<T>
	{
		HexTreeNode<T> *children[16];

	public:
		/*HexTreeBranch(HexTreeStub<T> *stub)
		:
		HexTreeNode<T>()
		{
		if (!stub)
		{
		topLeft = topRight = bottomLeft = bottomRight = NULL;
		}
		else
		{
		topLeft = new HexTreeStub<T>(*stub);
		topRight = new HexTreeStub<T>(*stub);
		bottomLeft = new HexTreeStub<T>(*stub);
		bottomRight = new HexTreeStub<T>(*stub);
		}

		}*/

		HexTreeBranch(
			pool<> &branchPool,
			pool<> &stubPool,
			const T &value
			)
			:
		HexTreeNode<T>()
		{
			for (int a=0;a<16;a++)
			{
				children[a] = new(stubPool.malloc()) HexTreeStub<T>(value);
			}
		}

		/*
		HexTreeBranch()
		:
		HexTreeNode<T>()
		{
		topLeft = topRight = bottomLeft = bottomRight = NULL;
		}
		*/

		virtual ~HexTreeBranch()
		{
			cout << string("You must use the custom memory manager for HexTrees!");
			string line;
			getline(cin,line);
		}

		virtual void destroy(
			pool<> &branchPool,
			pool<> &stubPool
			)
		{
			for (int a=0;a<16;a++)
			{
				if (children[a])
				{
					children[a]->destroy(branchPool,stubPool);
					if (children[a]->isStub())
					{
						stubPool.free(children[a]);
					}
					else
					{
						branchPool.free(children[a]);
					}
				}
			}
		}

		int getChildIndex(
			const Vector4<int> &location,
			const Vector4<int> &center,
			const Vector4<int> &topLeftVector4,
			Vector4<int> &newTopLeftVector4
			) const
		{
			int index=0;

			if (location.x1>=center.x1)
			{
				index |= (1<<3);
				newTopLeftVector4.x1 = center.x1;
			}
			else
			{
				newTopLeftVector4.x1 = topLeftVector4.x1;
			}

			if (location.y1>=center.y1)
			{
				index |= (1<<2);
				newTopLeftVector4.y1 = center.y1;
			}
			else
			{
				newTopLeftVector4.y1 = topLeftVector4.y1;
			}

			if (location.x2>=center.x2)
			{
				index |= (1<<1);
				newTopLeftVector4.x2 = center.x2;
			}
			else
			{
				newTopLeftVector4.x2 = topLeftVector4.x2;
			}

			if (location.y2>=center.y2)
			{
				index |= (1<<0);
				newTopLeftVector4.y2 = center.y2;
			}
			else
			{
				newTopLeftVector4.y2 = topLeftVector4.y2;
			}

			return index;
		}

		virtual T getValue(Vector4<int> topLeftVector4,int size,const Vector4<int> &location) const
		{
			Vector4<int> center = topLeftVector4;

			int sizeOverTwo = size/2;

			center.x1 += sizeOverTwo;
			center.y1 += sizeOverTwo;
			center.x2 += sizeOverTwo;
			center.y2 += sizeOverTwo;

			/*if(location.x<center.x)
			{
			if(location.y<center.y)
			{
			return topLeft->getValue(topLeftVector4,size,location);
			}
			else
			{
			return bottomLeft->getValue(topLeftVector4,size,location);
			}
			}
			else
			{
			if(location.y<center.y)
			{
			return topRight->getValue(topLeftVector4,size,location);
			}
			else
			{
			return bottomRight->getValue(topLeftVector4,size,location);
			}
			}*/

			HexTreeNode<T> *node;
			Vector4<int> newTopLeftVector4;

			int childIndex = getChildIndex(location,center,topLeftVector4,newTopLeftVector4);

			node = children[childIndex];

			/*
			if (location.x<center.x)
			{
			if (location.y<center.y)
			{
			node = topLeft;
			tmpTopLeftVector4 = topLeftVector4;
			}
			else
			{
			node = bottomLeft;
			tmpTopLeftVector4 = Vector4<int>(topLeftVector4.x,center.y);
			}
			}
			else
			{
			if (location.y<center.y)
			{
			node = topRight;
			tmpTopLeftVector4 = Vector4<int>(center.x,topLeftVector4.y);
			}
			else
			{
			node = bottomRight;
			tmpTopLeftVector4 = Vector4<int>(center.x,center.y);
			}
			}
			*/

			return node->getValue(newTopLeftVector4,sizeOverTwo,location);
		}

		virtual T getValue() const
		{
			return children[0]->getValue();
		}

		void setAll(
			pool<> &branchPool,
			pool<> &stubPool,
			T &value
			)
		{
			destroy(branchPool,stubPool);

			for (int a=0;a<16;a++)
			{
				children[a] = new(stubPool.malloc()) HexTreeStub<T>(value);
			}
		}

		//Returns 'true' if we need to collapse this branch
		virtual bool setValue(
			pool<> &branchPool,
			pool<> &stubPool,
			Vector4<int> topLeftVector4,
			int size,
			const Vector4<int> &location,
			const T &value
			)
		{
			Vector4<int> center = topLeftVector4;

			size /=2;

			center.x1+=size;
			center.y1+=size;
			center.x2+=size;
			center.y2+=size;

			HexTreeNode<T> *node;
			Vector4<int> newTopLeftVector4;

			int childIndex = getChildIndex(location,center,topLeftVector4,newTopLeftVector4);

			node = children[childIndex];

			/*
			if (location.x<center.x)
			{
			if (location.y<center.y)
			{
			node = topLeft;
			tmpTopLeftVector4 = topLeftVector4;
			}
			else
			{
			node = bottomLeft;
			tmpTopLeftVector4 = Vector4<int>(topLeftVector4.x,center.y);
			}
			}
			else
			{
			if (location.y<center.y)
			{
			node = topRight;
			tmpTopLeftVector4 = Vector4<int>(center.x,topLeftVector4.y);
			}
			else
			{
			node = bottomRight;
			tmpTopLeftVector4 = Vector4<int>(center.x,center.y);
			}
			}
			*/

			if (node->isStub())
			{
				HexTreeStub<T> *stubNode = static_cast<HexTreeStub<T> *>(node);

				if (size==1) //replace the stub
				{
					stubNode->setValue(value);
				}
				else if (node->getValue()!=value) //This stub needs to be expanded to a branch
				{
					HexTreeBranch<T> *tmpBranch =
						new(branchPool.malloc()) HexTreeBranch<T>(branchPool,stubPool,stubNode->getValue());

					children[childIndex]->destroy(branchPool,stubPool);
					stubPool.free(children[childIndex]);

					children[childIndex] = tmpBranch;

					children[childIndex]->setValue(branchPool,stubPool,newTopLeftVector4,size,location,value);
				}
			}
			else
			{
				//node is a branch

				if (node->setValue(branchPool,stubPool,newTopLeftVector4,size,location,value))
				{
					//We need to collapse (convert a branch to a stub)

					//First, get a value from the branch (all values should be the same)
					T value = node->getValue();

					//Delete the original branch
					children[childIndex]->destroy(branchPool,stubPool);
					branchPool.free(children[childIndex]);

					//Then, create a stub in the appropriate location
					children[childIndex] = new(stubPool.malloc()) HexTreeStub<T>(value);
				}
			}

			//check if we need to collapse
			bool needToCollapse=true;

			for (int a=0;a<16;a++)
			{
				if (!children[a]->isStub())
				{
					needToCollapse=false;
					break;
				}
				else if (
					((HexTreeStub<T> *)children[a])->getValue() !=
					((HexTreeStub<T> *)children[0])->getValue()
					)
				{
					needToCollapse=false;
					break;
				}
			}

			return needToCollapse;

		}

		virtual void display(int level) const
		{
			level++;
			for (int a=0;a<16;a++)
			{
				children[a]->display(level);
			}
		}

		virtual int getMemUsage() const
		{
			int size = sizeof(HexTreeBranch<T>);

			for (int a=0;a<16;a++)
			{
				size += children[a]->getMemUsage();
			}

			return size;
		}
	};

	template<class T>
	class HexTree
	{
		int size;
		HexTreeBranch<T> *root;

		pool<> branchPool;
		pool<> stubPool;

	public:
		HexTree(int _size=16,T defaultValue=(T)0)
			:
		size(_size),
			branchPool(sizeof(HexTreeBranch<T>)),
			stubPool(sizeof(HexTreeStub<T>))
		{
			root = new HexTreeBranch<T>(branchPool,stubPool,defaultValue);
		}

		HexTree(const HexTree<T> &other)
		{
			copyFrom(other);
		}

		~HexTree()
		{
			root->destroy(branchPool,stubPool);
			//((HexTreeBranch<T>*)root)->~HexTreeBranch<T>();
			branchPool.free(root);
		}

		HexTree<T>& operator=(const HexTree<T> &other)
		{
			if (this==&other)
			{
				return *this;
			}

			copyFrom(other);

			return *this;
		}

		void copyFrom(const HexTree<T> &other)
		{
			size = other.size;

			for (size_t y1=0;y1<size;y1++)
			{
				for (size_t x1=0;x1<size;x1++)
				{
					for (size_t y2=0;y2<size;y2++)
					{
						for (size_t x2=0;x2<size;x2++)
						{
							setValue(x1,y1,other.getValue(x2,y2));
						}
					}
				}
			}

		}

		inline T getValue(const Vector4<int> &location) const
		{
			Vector4<int> tmpVec(0,0,0,0);
			return root->getValue(tmpVec,size,location);
		}

		inline T getValue(int x1,int y1,int x2,int y2) const
		{
			Vector4<int> tmpVec(x1,y1,x2,y2);
			return getValue(tmpVec);
		}

		inline void setValue(const Vector4<int> &location,T value)
		{
			root->setValue(branchPool,stubPool,Vector4<int>(0,0,0,0),size,location,value);
		}

		inline void setValue(int x1,int y1,int x2,int y2,T value)
		{
			Vector4<int> tmpVec(x1,y1,x2,y2);
			setValue(tmpVec,value);
		}

		inline void setAll(T value)
		{
			root->setAll(branchPool,stubPool,value);
		}

		inline void display() const
		{
			root->display(0);
		}

		inline T operator()(const Vector4<int> &location) const
		{
			return getValue(location);
		}

		inline T operator()(int x1,int y1,int x2,int y2) const
		{
			Vector4<int> tmpVec(x1,y1,x2,y2);
			return getValue(tmpVec);
		}

		int getMemUsage() const
		{
			return root->getMemUsage();
		}

	protected:
	};

}

#endif // HEXTREE_H_INCLUDED
