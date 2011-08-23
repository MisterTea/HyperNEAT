#ifndef OCTTREE_H_INCLUDED
#define OCTTREE_H_INCLUDED

#include <iostream>
#include <string>
using namespace std;

#include "JGTL_Vector3.h"

#include "boost/pool/pool.hpp"

namespace JGTL
{

	template<class T>
	class OctTreeNode
	{
	protected:
	public:
		OctTreeNode()
		{}

		virtual ~OctTreeNode<T>()
		{
		}

		virtual T getValue(Vector3<int> topLeftVector3,int size,const Vector3<int> &location) const = 0;

		virtual T getValue() const = 0;

		virtual bool setValue(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool,
			Vector3<int> topLeftVector3,
			int size,
			const Vector3<int> &location,
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
	class OctTreeStub : public OctTreeNode<T>
	{
	protected:
		T value;

	public:
		OctTreeStub(T _value)
			:
		OctTreeNode<T>(),
			value(_value)
		{}

		virtual ~OctTreeStub()
		{
		}

		virtual T getValue(Vector3<int> topLeftVector3,int size,const Vector3<int> &location) const
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
			Vector3<int> topLeftVector3,
			int size,
			const Vector3<int> &location,
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
	class OctTreeBranch : public OctTreeNode<T>
	{
		OctTreeNode<T>* childNodes[2][2][2];

	public:
		OctTreeBranch(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool,
			const T &value
			)
			:
		OctTreeNode<T>()
		{
			for(int x=0;x<2;x++)
			{
				for(int y=0;y<2;y++)
				{
					for(int z=0;z<2;z++)
					{
						childNodes[x][y][z] = new(stubPool.malloc()) OctTreeStub<T>(value);
					}
				}
			}
		}

		virtual ~OctTreeBranch()
		{
		}

		virtual void destroy(
			boost::pool<> &branchPool,
			boost::pool<> &stubPool
			)
		{
			if(childNodes[0][0][0])
			{
				//If one exists, they all exist
				for(int x=0;x<2;x++)
				{
					for(int y=0;y<2;y++)
					{
						for(int z=0;z<2;z++)
						{
							OctTreeNode<T>* node = childNodes[x][y][z];
							node->destroy(branchPool,stubPool);
							if (node->isStub())
							{
								((OctTreeStub<T>*)node)->~OctTreeStub<T>();
								stubPool.free(node);
							}
							else
							{
								((OctTreeBranch<T>*)node)->~OctTreeBranch<T>();
								branchPool.free(node);
							}
						}
					}
				}
			}
		}

		virtual T getValue(Vector3<int> topLeftVector3,int size,const Vector3<int> &location) const
		{
			Vector3<int> center = topLeftVector3;

			size/=2;

			center.x += size;
			center.y += size;
			center.z += size;

			Vector3<int> childVec(
				int(location.x>=center.x),
				int(location.y>=center.y),
				int(location.z>=center.z)
				);
			OctTreeNode<T>* node = childNodes[childVec.x][childVec.y][childVec.z];
			Vector3<int> tmpTopLeftVector3 = topLeftVector3 + (center-topLeftVector3).componentMultiply(childVec);

			return node->getValue(tmpTopLeftVector3,size,location);
		}

		virtual T getValue() const
		{
			if(childNodes[0][0][0])
			{
				return childNodes[0][0][0]->getValue();
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
			Vector3<int> topLeftVector3,
			int size,
			const Vector3<int> &location,
			const T &value
			)
		{
			Vector3<int> center = topLeftVector3;

			size /=2;

			center.x+=size;
			center.y+=size;
			center.z+=size;

			Vector3<int> childVec(
				int(location.x>=center.x),
				int(location.y>=center.y),
				int(location.z>=center.z)
				);
			OctTreeNode<T>* node = childNodes[childVec.x][childVec.y][childVec.z];
			Vector3<int> tmpTopLeftVector3 = topLeftVector3 + (center-topLeftVector3).componentMultiply(childVec);

			if (node->isStub())
			{
				OctTreeStub<T> *stubNode = static_cast<OctTreeStub<T> *>(node);

				if (size==1) //replace the stub
				{
					stubNode->setValue(value);
				}
				else if (node->getValue()!=value) //This stub needs to be expanded to a branch
				{
					OctTreeBranch<T> *tmpBranch =
						new(branchPool.malloc()) OctTreeBranch<T>(branchPool,stubPool,stubNode->getValue());

					node->destroy(branchPool,stubPool);
					((OctTreeStub<T>*)node)->~OctTreeStub<T>();
					stubPool.free(node);

					//set the new value that differs from the others
					tmpBranch->setValue(branchPool,stubPool,tmpTopLeftVector3,size,location,value);

					childNodes[childVec.x][childVec.y][childVec.z] = tmpBranch;
				}
			}
			else
			{
				//node is a branch

				if (node->setValue(branchPool,stubPool,tmpTopLeftVector3,size,location,value))
				{
					//We need to collapse (convert a branch to a stub)

					//First, get a value from the branch (all values should be the same)
					T value = node->getValue();

					//Then delete the original branch
					node->destroy(branchPool,stubPool);
					((OctTreeBranch<T>*)node)->~OctTreeBranch<T>();
					branchPool.free(node);

					//Then, create a stub in the appropriate location
					childNodes[childVec.x][childVec.y][childVec.z] = new(stubPool.malloc()) OctTreeStub<T>(value);
				}
			}

			//check if we need to collapse

			bool allStubs=true;
			for(int x=0;allStubs&&x<2;x++)
			{
				for(int y=0;allStubs&&y<2;y++)
				{
					for(int z=0;allStubs&&z<2;z++)
					{
						if(childNodes[x][y][z]->isStub()==false)
							allStubs=false;
					}
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
								((OctTreeStub<T> *)childNodes[0][0][0])->getValue() != 
								((OctTreeStub<T> *)childNodes[x][y][z])->getValue()
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
	class OctTree
	{
		int size;
		T defaultValue;
		OctTreeBranch<T> *root;

		boost::pool<> branchPool;
		boost::pool<> stubPool;

	public:
		OctTree(int _size=16,T _defaultValue=(T)0)
			:
			size(_size),
			defaultValue(_defaultValue),
			branchPool(sizeof(OctTreeBranch<T>)),
			stubPool(sizeof(OctTreeStub<T>))
		{
			root = new(branchPool.malloc()) OctTreeBranch<T>(branchPool,stubPool,defaultValue);
		}

		OctTree(const OctTree<T> &other)
			:
		branchPool(sizeof(OctTreeBranch<T>)),
			stubPool(sizeof(OctTreeStub<T>))
		{
			copyFrom(other);
		}

		~OctTree()
		{
			root->destroy(branchPool,stubPool);
			((OctTreeBranch<T>*)root)->~OctTreeBranch<T>();
			branchPool.free(root);
		}

		OctTree<T>& operator=(const OctTree<T> &other)
		{
			if (this==&other)
			{
				return *this;
			}

			copyFrom(other);

			return *this;
		}

		void copyFrom(const OctTree<T> &other)
		{
			size = other.size;
			defaultValue = other.defaultValue;

			root = new(branchPool.malloc()) OctTreeBranch<T>(branchPool,stubPool,defaultValue);

			for (int y=0;y<size;y++)
			{
				for (int x=0;x<size;x++)
				{
					for (int z=0;z<size;z++)
					{
						setValue(x,y,z,other.getValue(x,y,z));
					}
				}
			}

		}

		inline T getValue(const Vector3<int> &location) const
		{
			Vector3<int> tmpVec(0,0,0);
			return root->getValue(tmpVec,size,location);
		}

		inline T getValue(int x,int y,int z) const
		{
			Vector3<int> tmpVec(x,y,z);
			return getValue(tmpVec);
		}

		inline void setValue(const Vector3<int> &location,T value)
		{
			root->setValue(branchPool,stubPool,Vector3<int>(0,0,0),size,location,value);
		}

		inline void setValue(int x,int y,int z,T value)
		{
			Vector3<int> tmpVec(x,y,z);
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

		inline T operator()(const Vector3<int> &location) const
		{
			return getValue(location);
		}

		inline T operator()(int x,int y,int z) const
		{
			Vector3<int> tmpVec(x,y,z);
			return getValue(tmpVec);
		}

	protected:
	};

}

#endif // OCTTREE_H_INCLUDED
