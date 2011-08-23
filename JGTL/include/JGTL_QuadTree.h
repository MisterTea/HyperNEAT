#ifndef QUADTREE_H_INCLUDED
#define QUADTREE_H_INCLUDED

#include <iostream>
#include <string>
#include <cstdio>
using namespace std;

#include "JGTL_Vector2.h"

#include "boost/pool/pool.hpp"

namespace JGTL
{

	template<class T>
	class QuadTreeNode
	{
	protected:
	public:
		QuadTreeNode()
		{}

		virtual ~QuadTreeNode<T>()
		{
		}

		virtual T getValue(Vector2<int> topLeftVector2,int size,const Vector2<int> &location) const = 0;

		virtual T getValue() const = 0;

		virtual bool setValue(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool,
			Vector2<int> topLeftVector2,
			int size,
			const Vector2<int> &location,
			const T &value
			) = 0;

		virtual bool isStub() const
		{
			return false;
		}

		virtual void destroy(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool
			)
		{}

		virtual void display(int level) const = 0;
	};

	template<class T>
	class QuadTreeStub : public QuadTreeNode<T>
	{
	protected:
		T value;

	public:
		QuadTreeStub(T _value)
			:
		QuadTreeNode<T>(),
			value(_value)
		{}

		virtual ~QuadTreeStub()
		{
		}

		virtual T getValue(Vector2<int> topLeftVector2,int size,const Vector2<int> &location) const
		{
			return value;
		}

		virtual T getValue() const
		{
			return value;
		}

		virtual bool setValue(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool,
			Vector2<int> topLeftVector2,
			int size,
			const Vector2<int> &location,
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
	};

	template<class T>
	class QuadTreeBranch : public QuadTreeNode<T>
	{
		QuadTreeNode<T>* childNodes[2][2];

	public:
		QuadTreeBranch(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool,
			const T &value
			)
			:
		QuadTreeNode<T>()
		{
			for(int x=0;x<2;x++)
			{
				for(int y=0;y<2;y++)
				{
					childNodes[x][y] = new(stubPool.malloc()) QuadTreeStub<T>(value);
				}
			}
		}

		virtual ~QuadTreeBranch()
		{
		}

		virtual void destroy(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool
			)
		{
			if(childNodes[0][0])
			{
				//If one exists, they all exist
				for(int x=0;x<2;x++)
				{
					for(int y=0;y<2;y++)
					{
						QuadTreeNode<T>* node = childNodes[x][y];
						node->destroy(branchPool,stubPool);
						if (node->isStub())
						{
							((QuadTreeStub<T>*)node)->~QuadTreeStub<T>();
							stubPool.free(node);
						}
						else
						{
							((QuadTreeBranch<T>*)node)->~QuadTreeBranch<T>();
							branchPool.free(node);
						}
					}
				}
			}
		}

		virtual T getValue(Vector2<int> topLeftVector2,int size,const Vector2<int> &location) const
		{
			Vector2<int> center = topLeftVector2;

			size/=2;

			center.x += size;
			center.y += size;

			Vector2<int> childVec(
				int(location.x>=center.x),
				int(location.y>=center.y)
				);
			QuadTreeNode<T>* node = childNodes[childVec.x][childVec.y];
			Vector2<int> tmpTopLeftVector2 = topLeftVector2 + (center-topLeftVector2).componentMultiply(childVec);

			return node->getValue(tmpTopLeftVector2,size,location);
		}

		virtual T getValue() const
		{
			if(childNodes[0][0])
			{
				return childNodes[0][0]->getValue();
			}
			else
			{
				return (T)0;
			}
		}

		//Returns 'true' if we need to collapse this branch
		virtual bool setValue(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool,
			Vector2<int> topLeftVector2,
			int size,
			const Vector2<int> &location,
			const T &value
			)
		{
			Vector2<int> center = topLeftVector2;

			size /=2;

			center+=size;

			Vector2<int> childVec(
				int(location.x>=center.x),
				int(location.y>=center.y)
				);
			QuadTreeNode<T>* node = childNodes[childVec.x][childVec.y];
			Vector2<int> tmpTopLeftVector2 = topLeftVector2 + (center-topLeftVector2).componentMultiply(childVec);

			if (node->isStub())
			{
				QuadTreeStub<T> *stubNode = static_cast<QuadTreeStub<T> *>(node);

				if (size==1) //replace the stub
				{
					stubNode->setValue(value);
				}
				else if (node->getValue()!=value) //This stub needs to be expanded to a branch
				{
					QuadTreeBranch<T> *tmpBranch =
						new(branchPool.malloc()) QuadTreeBranch<T>(branchPool,stubPool,stubNode->getValue());

					node->destroy(branchPool,stubPool);
					((QuadTreeStub<T>*)node)->~QuadTreeStub<T>();
					stubPool.free(node);

					//set the new value that differs from the others
					tmpBranch->setValue(branchPool,stubPool,tmpTopLeftVector2,size,location,value);

					childNodes[childVec.x][childVec.y] = tmpBranch;
				}
			}
			else
			{
				//node is a branch

				if (node->setValue(branchPool,stubPool,tmpTopLeftVector2,size,location,value))
				{
					//We need to collapse (convert a branch to a stub)

					//First, get a value from the branch (all values should be the same)
					T value = node->getValue();

					//Then delete the original branch
					node->destroy(branchPool,stubPool);
					((QuadTreeBranch<T>*)node)->~QuadTreeBranch<T>();
					branchPool.free(node);

					//Then, create a stub in the appropriate location
					childNodes[childVec.x][childVec.y] = new(stubPool.malloc()) QuadTreeStub<T>(value);
				}
			}

			//check if we need to collapse

			bool allStubs=true;
			for(int x=0;allStubs&&x<2;x++)
			{
				for(int y=0;allStubs&&y<2;y++)
				{
					if(childNodes[x][y]->isStub()==false)
						allStubs=false;
				}
			}

			if (allStubs)
			{
				//All branches are stubs, maybe a chance for a collapse?

				bool allEqual=true;
				for(int x=0;allEqual&&x<2;x++)
				{
					for(int y=0;allEqual&&y<2;y++)
					{
						for(int z=0;allEqual&&z<2;z++)
						{
							if(!x&&!y&&!z)
							{
								continue;
							}
							if(
								((QuadTreeStub<T> *)childNodes[0][0])->getValue() != 
								((QuadTreeStub<T> *)childNodes[x][y])->getValue()
								)
							{
								allEqual=false;
							}
						}
					}
				}
				if(allEqual)
				{
					//Tell the higher up branch to convert this instance into one stub
					return true;
				}
			}

			return false;
		}

		virtual void display(int level) const
		{
			level++;
			//topLeft->display(level);
			//topRight->display(level);
			//bottomLeft->display(level);
			//bottomRight->display(level);
		}
	};

	template<class T>
	class QuadTree
	{
		int size;
		T defaultValue;
		QuadTreeBranch<T> *root;

		boost::pool<> branchPool;
		boost::pool<> stubPool;

	public:
		QuadTree(int _size=16,T _defaultValue=(T)0)
			:
			size(_size),
			defaultValue(_defaultValue),
			branchPool(sizeof(QuadTreeBranch<T>)),
			stubPool(sizeof(QuadTreeStub<T>))
		{
			root = new(branchPool.malloc()) QuadTreeBranch<T>(branchPool,stubPool,defaultValue);
		}

		QuadTree(const QuadTree<T> &other)
			:
		branchPool(sizeof(QuadTreeBranch<T>)),
			stubPool(sizeof(QuadTreeStub<T>))
		{
			copyFrom(other);
		}

		~QuadTree()
		{
			root->destroy(branchPool,stubPool);
			((QuadTreeBranch<T>*)root)->~QuadTreeBranch<T>();
			branchPool.free(root);
		}

		QuadTree<T>& operator=(const QuadTree<T> &other)
		{
			if (this==&other)
			{
				return *this;
			}

			copyFrom(other);

			return *this;
		}

		void copyFrom(const QuadTree<T> &other)
		{
			size = other.size;
			defaultValue = other.defaultValue;

			root = new(branchPool.malloc()) QuadTreeBranch<T>(branchPool,stubPool,defaultValue);

			for (int y=0;y<size;y++)
			{
				for (int x=0;x<size;x++)
				{
					setValue(x,y,other.getValue(x,y));
				}
			}

		}

		inline T getValue(const Vector2<int> &location) const
		{
			Vector2<int> tmpVec(0,0);
			return root->getValue(tmpVec,size,location);
		}

		inline T getValue(int x,int y) const
		{
			Vector2<int> tmpVec(x,y);
			return getValue(tmpVec);
		}

		inline void setValue(const Vector2<int> &location,T value)
		{
			root->setValue(branchPool,stubPool,Vector2<int>(0,0),size,location,value);
		}

		inline void setValue(int x,int y,T value)
		{
			Vector2<int> tmpVec(x,y);
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

		inline T operator()(const Vector2<int> &location) const
		{
			return getValue(location);
		}

		inline T operator()(int x,int y) const
		{
			Vector2<int> tmpVec(x,y);
			return getValue(tmpVec);
		}

	protected:
	};

}

#endif // QUADTREE_H_INCLUDED
