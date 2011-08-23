#ifndef TE_INDEX2_H_INCLUDED
#define TE_INDEX2_H_INCLUDED

namespace JGTL
{

	class Index2
	{
	public:
		int x,y;
		Index2();
		Index2(int _x,int _y);

		Index2(std::string s)
		{
			std::istringstream istr(s);

			istr >> x >> y;
		}

		std::string toString()
		{
			std::ostringstream ostr;
			ostr << x << " " << y;
			return ostr.str();
		}

		bool operator==(const Index2 &index)
		{
			return (x==index.x&&y==index.y);
		}

		bool operator!=(const Index2 &index)
		{
			return (x!=index.x||y!=index.y);
		}
	};

}


#endif // TE_INDEX2_H_INCLUDED
