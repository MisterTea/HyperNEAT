#ifndef TREELIST_H_INCLUDED
#define TREELIST_H_INCLUDED

namespace JGTL
{

	template<class Data>
	class TreeListNode
	{
	public:
		Data sibling;
		Data child;
	};

	template<class Data>
	class TreeList
	{
	public:
	protected:
		shared_ptr<TreeListNode> root;

	public:
		TreeList()
		{}

		~TreeList()
		{}


	protected:
	};

}

#endif // TREELIST_H_INCLUDED
