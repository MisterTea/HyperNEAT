#ifndef __JGTL_QUADRATIC_H__
#define __JGTL_QUADRATIC_H__

namespace JGTL
{
	template<class T>
	class QuadraticSolution
	{
	public:
		int numSolutions;
		T t1,t2;

		QuadraticSolution()
			:
			numSolutions(0)
		{
		}
	};

	template<class T, class TT, class TTT, class TTTT>
	QuadraticSolution<T> solveQuadratic(TT a, TTT b, TTTT c)
	{
		QuadraticSolution<T> solution;

		if(fabs(a)<1e-6)
		{
			//In this case, there is no squared term, 
			//so the equation becomes linear.

			solution.numSolutions=1;
			solution.t1 = (-c)/b;
		}
		else
		{
			if( (b*b - 4*a*c) < 0)
			{
				solution.numSolutions = 0;
			}
			else
			{
				solution.numSolutions = 2;
				solution.t1 = (-b - sqrtf(b*b - 4*a*c)) / (2*a);
				solution.t2 = (-b + sqrtf(b*b - 4*a*c)) / (2*a);
			}
		}

		return solution;
	}
}

#endif

