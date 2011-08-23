#define __USE_MATH_DEFINES

#define BOOST_UBLAS_TYPE_CHECK (0)

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#include "JGTL_Ray2.h"
#include "JGTL_Vector2.h"
#include "JGTL_Quadratic.h"
#include "JGTL_StringConverter.h"

#include <boost/algorithm/string.hpp>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
using namespace std;
using namespace JGTL;
using namespace boost;
using namespace boost::numeric;

    typedef float (*regressionEquation2)(float,float,float);
    typedef void (*regressionEquationDerivative2)(float,float,float,float&,float&);

    typedef float (*regressionEquation3)(float,float,float,float);
    typedef void (*regressionEquationDerivative3)(float,float,float,float,float&,float&,float&);

	template<class T>
bool InvertMatrix(const ublas::matrix<T>& input, ublas::matrix<T>& inverse) 
{
	using namespace boost::numeric::ublas;
	typedef permutation_matrix<std::size_t> pmatrix;
	// create a working copy of the input
	matrix<T> A(input);
	// create a permutation matrix for the LU-factorization
	pmatrix pm(A.size1());


	// perform LU-factorization
	int res = lu_factorize(A,pm);
	if( res != 0 ) return false;


	// create identity matrix of "inverse"
	inverse.assign(ublas::identity_matrix<T>(A.size1()));


	// backsubstitute to get the inverse
	lu_substitute(A, pm, inverse);


	return true;
}

void LoadPoints(string ptsString,vector< JGTL::Vector2<float> > &points)
{
	vector<string> splits;
	split( splits, ptsString, is_any_of(",") );

	for(size_t a=0;a<splits.size();a+=2)
	{
		if(splits[a].size()==0)
		{
			return;
		}

		points.push_back(
				JGTL::Vector2<float>(stringTo<float>(splits[a]),stringTo<float>(splits[a+1]))
				);

		//cout << "Adding point: " << points.back() << endl;
	}
}

float linear(float a,float b,float x)
{
	return a + b*x;
}
void linearDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	deriv_a = 1;
	deriv_b = x;
}

float power(float a,float b,float x)
{
	return a*pow(x,b);
}
void powerDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	float tmp = pow(x,b);
	deriv_a = tmp;
	deriv_b = a*tmp*log(x);
}

float exponential(float a,float b,float x)
{
	return a*exp(b*x);
}
void exponentialDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	float tmp = exp(b*x);
	deriv_a = tmp;
	deriv_b = a*x*tmp;
}

float logarithmic(float a,float b,float x)
{
	return a + b*log(x);
}
void logarithmicDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	deriv_a = 1;
	deriv_b = log(x);
}

float hyperbolic(float a,float b,float x)
{
	return a + b/x;
}
void hyperbolicDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	deriv_a = 1;
	deriv_b = 1.0f/x;
}

float squared(float a,float b,float x)
{
	return a + b*x*x;
}
void squaredDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	deriv_a = 1;
	deriv_b = x*x;
}

float taylor(float a,float b,float x)
{
	return exp(a + b*x*x);
}
void taylorDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	float tmp = exp(a + b*x*x);
	deriv_a = tmp;
	deriv_b = x*x*tmp;
}

float expE(float a,float b,float x)
{
	return a + b*pow(x,float(M_E));
}
void expEDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	deriv_a = 1;
	deriv_b = pow(x,float(M_E));
}

float taylor2(float a,float b,float x)
{
	return exp(a + b*sqrt(x));
}
void taylor2Derivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	float tmp = exp(a + b*sqrt(x));
	deriv_a = tmp;
	deriv_b = sqrt(x)*tmp;
}

float hyperb2(float a,float b,float x)
{
	return a + (b / (x*x) );
}
void hyperb2Derivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	deriv_a = 1;
	deriv_b = 1/(x*x);
}

float loglog(float a,float b,float x)
{
	return a + b*log(x)*log(x);
}
void loglogDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	deriv_a = 1;
	deriv_b = log(x)*log(x);
}

float ehyperb(float a,float b,float x)
{
	return a*exp(b/x);
}
void ehyperbDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	float tmp = exp(b/x);
	deriv_a = tmp;
	deriv_b = (a*tmp)/x;
}

float hyperbSqrt(float a,float b,float x)
{
	return a + b/sqrt(x);
}
void hyperbSqrtDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	deriv_a = 1;
	deriv_b = 1/sqrt(x);
}

float hyperbLn(float a,float b,float x)
{
	return a + b/log(x);
}
void hyperbLnDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	deriv_a = 1;
	deriv_b = 1/log(x);
}

float logPower(float a,float b,float x)
{
	return a * pow(log(x),b);
}
void logPowerDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	float tmp = pow(log(x),b);
	deriv_a = tmp;
	deriv_b = a*tmp*log(log(x));
}

float sqrt(float a,float b,float x)
{
	return a + b*sqrt(x);
}
void sqrtDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	deriv_a = 1;
	deriv_b = sqrt(x);
}

float xlogx(float a,float b,float x)
{
	return a + b*pow(x,log(x));
}
void xlogxDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	deriv_a = 1;
	deriv_b = exp(log(x)*log(x));
}

float hblog(float a,float b,float x)
{
	return a + b/pow(x,log(x));
}
void hblogDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	deriv_a = 1;
	deriv_b = exp(-1*log(x)*log(x));
}

float hbe(float a,float b,float x)
{
	return a + b/pow(x,float(M_E));
}
void hbeDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	deriv_a = 1;
	deriv_b = 1/pow(x,float(M_E));
}

float ehyperbsqrt(float a,float b,float x)
{
	return a*exp( b / sqrt(x) );
}
void ehyperbsqrtDerivatives(float a,float b,float x,float &deriv_a,float &deriv_b)
{
	float tmp = exp( b / sqrt(x) );
	deriv_a = tmp;
	deriv_b = a*tmp/sqrt(x);
}

float quadratic(float a,float b,float c,float x)
{
	return a*x*x + b*x + c;
}
void quadraticDerivatives(float a,float b,float c,float x,float &deriv_a,float &deriv_b,float &deriv_c)
{
	deriv_a = x*x;
	deriv_b = x;
	deriv_c = 1;
}

class Result2
{
public:
	float A,B;
	float r2;

	Result2(float Aset,float Bset,float r2set)
		:
			A(Aset),
			B(Bset),
			r2(r2set)
	{
	}

	Result2()
	{
	}
};

class Result3
{
public:
	float A,B,C;
	float r2;

	Result3(float Aset,float Bset,float Cset,float r2set)
		:
			A(Aset),
			B(Bset),
			C(Cset),
			r2(r2set)
	{
	}

	Result3()
	{
	}
};

Result2 tryRegression2(
		const vector< JGTL::Vector2<float> > &points,
		regressionEquation2 equation2,
		regressionEquationDerivative2 derivative2
		);

Result3 tryRegression3(
		const vector< JGTL::Vector2<float> > &points,
		regressionEquation3 equation3,
		regressionEquationDerivative3 derivative3
		);

int main()
{
	vector< JGTL::Vector2<float> > points;

	LoadPoints("2,59.49,3,40.81,4,32.64,5,26.10,6,24.18,7,22.01,8,17.13,9,16.77,10,18.26,11,13.38,12,13.93,13,10.99,14,11.21,15,10.61,16,13.16,17,12.14,18,11.53,19,8.54,20,8.16,21,7.21,22,7.28,23,10.15,24,7.21,25,8.31,26,8.44,27,6.19,28,6.29,29,4.34,30,7.13,31,5.51,32,7.54,33,5.16,34,6.98,35,4.76,36,5.67,37,7.85,38,2.94,39,6.54,40,3.22,41,5.59,42,6.51,43,2.52,44,7.07,45,4.32,46,6.88,47,6.97,48,3.53,49,2.51,50,4.38,51,2.56,",points);

	 vector<regressionEquation2> equations2;
	 vector<regressionEquationDerivative2> derivatives2;

    /* the ampersand is actually optional */
    equations2.push_back(&linear);
    derivatives2.push_back(&linearDerivatives);
    equations2.push_back(&power);
    derivatives2.push_back(&powerDerivatives);
    equations2.push_back(&exponential);
    derivatives2.push_back(&exponentialDerivatives);
    equations2.push_back(&logarithmic);
    derivatives2.push_back(&logarithmicDerivatives);
    equations2.push_back(&hyperbolic);
    derivatives2.push_back(&hyperbolicDerivatives);
    equations2.push_back(&squared);
    derivatives2.push_back(&squaredDerivatives);
    equations2.push_back(&taylor);
    derivatives2.push_back(&taylorDerivatives);
    equations2.push_back(&expE);
    derivatives2.push_back(&expEDerivatives);
    equations2.push_back(&taylor2);
    derivatives2.push_back(&taylor2Derivatives);
    equations2.push_back(&hyperb2);
    derivatives2.push_back(&hyperb2Derivatives);
    equations2.push_back(&loglog);
    derivatives2.push_back(&loglogDerivatives);
    equations2.push_back(&ehyperb);
    derivatives2.push_back(&ehyperbDerivatives);
    equations2.push_back(&hyperbSqrt);
    derivatives2.push_back(&hyperbSqrtDerivatives);
    equations2.push_back(&hyperbLn);
    derivatives2.push_back(&hyperbLnDerivatives);
    equations2.push_back(&logPower);
    derivatives2.push_back(&logPowerDerivatives);
    equations2.push_back(&sqrt);
    derivatives2.push_back(&sqrtDerivatives);
    equations2.push_back(&xlogx);
    derivatives2.push_back(&xlogxDerivatives);
    equations2.push_back(&hblog);
    derivatives2.push_back(&hblogDerivatives);
    equations2.push_back(&hbe);
    derivatives2.push_back(&hbeDerivatives);
    equations2.push_back(&ehyperbsqrt);
    derivatives2.push_back(&ehyperbsqrtDerivatives);

	 vector<regressionEquation3> equations3;
	 vector<regressionEquationDerivative3> derivatives3;
	 
    equations3.push_back(&quadratic);
    derivatives3.push_back(&quadraticDerivatives);

	 Result3 bestResult;
	 int resultIndex;
	 int resultDim;

 	 for(size_t regressionTypes2=0;regressionTypes2<equations2.size();regressionTypes2++)
	{
		cout << "ON CASE: " << regressionTypes2 << endl;
		Result2 result = tryRegression2(
				points,
				equations2[regressionTypes2],
				derivatives2[regressionTypes2]
				);

		cout << "A: " << result.A << ", B: " << result.B
			<< ", r2: " << result.r2 << endl;

		if(!regressionTypes2 || result.r2>bestResult.r2)
		{
			bestResult.A = result.A;
			bestResult.B = result.B;
			bestResult.C = 0;
			resultIndex = regressionTypes2;
			resultDim=2;
		}
	}

 	 for(size_t regressionTypes3=0;regressionTypes3<equations3.size();regressionTypes3++)
	{
		cout << "ON CASE: " << regressionTypes3 << endl;
		Result3 result = tryRegression3(
				points,
				equations3[regressionTypes3],
				derivatives3[regressionTypes3]
				);

		cout << "A: " << result.A << ", B: " << result.B
			<< ", C: " << result.C 
			<< ", r2: " << result.r2 << endl;

		if(!regressionTypes3 || result.r2>bestResult.r2)
		{
			bestResult = result;
			resultIndex = regressionTypes3;
			resultDim=3;
		}
	}

	 if(resultDim==2)
	 {
	 }
	 else //resultDim==3
	 {
	 }

	return 0;
}

Result2 tryRegression2(
		const vector< JGTL::Vector2<float> > &points,
		regressionEquation2 equation2,
		regressionEquationDerivative2 derivative2
		)
{
	Result2 result;

	/*
	points.push_back(JGTL::Vector2<float>(0,0));
	points.push_back(JGTL::Vector2<float>(1,1));
	points.push_back(JGTL::Vector2<float>(2,2));
	points.push_back(JGTL::Vector2<float>(3,3));
	points.push_back(JGTL::Vector2<float>(4,4));
	*/

	//Formula: f(x,A,B) = A + Bx
	//Derivative with respect to A: f(x,A,B) = 1
	//Derivative with respect to B: f(x,A,B) = x

	//First, pick an initial guess
	result.A = 1;
	result.B = 1;

	float prev_s_yx;

	for(int trials=0;trials<100;trials++)
	{
		//cout << "On trial: " << trials << endl;
		ublas::matrix<float> delta(int(points.size()),1);

		float s_yx = 0;
		float s_y = 0;
		//cout << "Deltas: ";
		float avg_y=0;
		for(int a=0;a<int(points.size());a++)
		{
			avg_y += points[a].y;
		}
		avg_y /= float(points.size());
		//cout << "Difference: ";
		for(int a=0;a<int(points.size());a++)
		{
			delta(a,0) = points[a].y - (equation2)(result.A,result.B,points[a].x);
			//if(a)
				//cout << ", ";
			//cout << delta(a,0);

			//Compute s_yx and s_y to see if we need to keep going
			s_yx += (delta(a,0)*delta(a,0));
			s_y += (points[a].y - avg_y)*(points[a].y - avg_y);
		}

		result.r2 = max(0.0f,1.0f - (s_yx / s_y));
		//cout << endl;

		//cout << "s_yx: " << s_yx << " s_y: " << s_y << endl;
		//cout << "R2: " << r2 << endl;

		if(trials)
		{
			if(fabs((prev_s_yx - s_yx)/(prev_s_yx)) < 0.0001)
			{
				//cout << "Found premature stopping condition!\n";
				break;
			}
		}
		prev_s_yx = s_yx;

		ublas::matrix<float> deriv(int(points.size()),2);

		//Compute estimate
		//cout << "Derivative: ";
		for(int testPoint=0;testPoint<int(points.size());testPoint++)
		{
			const JGTL::Vector2<float> &curPoint = points[testPoint];

			(derivative2)(result.A,result.B,curPoint.x,deriv(testPoint,0),deriv(testPoint,1));

			//cout << deriv(testPoint,0) << ", " << deriv(testPoint,1) << ", ";
		}
		//cout << endl;

		ublas::matrix<float> derivTranspose = trans(deriv);

		//mat_a = derivTranspose * deriv
		ublas::matrix<float> mat_a = prod(derivTranspose,deriv);

		//mat_b = derivTranspose * db
		ublas::matrix<float> mat_b = prod(derivTranspose,delta);

		//a*(delta_vector) = b, so solve for delta_vector
		ublas::matrix<float> mat_a_inverse(2,2);
		bool retval=false;
		try
		{
		    retval = InvertMatrix(mat_a,mat_a_inverse);
		}
		catch(...)
		{
			cout << "Error computing matrix inverse!\n";
		}

		if(retval)
		{
			ublas::matrix<float> delta_vector = prod(mat_a_inverse,mat_b);

			//cout << "Delta A: " << delta_vector(0,0) << endl;
			//cout << "Delta B: " << delta_vector(1,0) << endl;

			result.A += delta_vector(0,0);
			result.B += delta_vector(1,0);

			//cout << "New A: " << result.A << endl;
			//cout << "New B: " << result.B << endl;
		}
		else
		{
			cout << "INVERSE FAILED!\n";
			break;
		}
	}

	//cout << "R2: " << result.r2 << endl;

	return result;
}

Result3 tryRegression3(
		const vector< JGTL::Vector2<float> > &points,
		regressionEquation3 equation3,
		regressionEquationDerivative3 derivative3
		)
{
	Result3 result;

	/*
	points.push_back(JGTL::Vector2<float>(0,0));
	points.push_back(JGTL::Vector2<float>(1,1));
	points.push_back(JGTL::Vector2<float>(2,2));
	points.push_back(JGTL::Vector2<float>(3,3));
	points.push_back(JGTL::Vector2<float>(4,4));
	*/

	//Formula: f(x,A,B) = A + Bx
	//Derivative with respect to A: f(x,A,B) = 1
	//Derivative with respect to B: f(x,A,B) = x

	//First, pick an initial guess
	result.A = 1;
	result.B = 1;
	result.C = 1;

	float prev_s_yx;

	for(int trials=0;trials<100;trials++)
	{
		//cout << "On trial: " << trials << endl;
		ublas::matrix<float> delta(int(points.size()),1);

		float s_yx = 0;
		float s_y = 0;
		//cout << "Deltas: ";
		float avg_y=0;
		for(int a=0;a<int(points.size());a++)
		{
			avg_y += points[a].y;
		}
		avg_y /= float(points.size());
		for(int a=0;a<int(points.size());a++)
		{
			delta(a,0) = points[a].y - (equation3)(result.A,result.B,result.C,points[a].x);
			//if(a)
				//cout << ", ";
			//cout << delta(a,0);

			//Compute s_yx and s_y to see if we need to keep going
			s_yx += (delta(a,0)*delta(a,0));
			s_y += (points[a].y - avg_y)*(points[a].y - avg_y);
		}

		result.r2 = max(0.0f,1.0f - (s_yx / s_y));
		//cout << endl;

		//cout << "s_yx: " << s_yx << " s_y: " << s_y << endl;
		//cout << "R2: " << r2 << endl;

		if(trials)
		{
			if(fabs((prev_s_yx - s_yx)/(prev_s_yx)) < 0.0001)
			{
				//cout << "Found premature stopping condition!\n";
				break;
			}
		}
		prev_s_yx = s_yx;

		ublas::matrix<float> deriv(int(points.size()),3);

		//Compute estimate
		for(int testPoint=0;testPoint<int(points.size());testPoint++)
		{
			const JGTL::Vector2<float> &curPoint = points[testPoint];

			(derivative3)(result.A,result.B,result.C,curPoint.x,deriv(testPoint,0),deriv(testPoint,1),deriv(testPoint,2));
		}

		ublas::matrix<float> derivTranspose = trans(deriv);

		//mat_a = derivTranspose * deriv
		ublas::matrix<float> mat_a = prod(derivTranspose,deriv);

		//mat_b = derivTranspose * db
		ublas::matrix<float> mat_b = prod(derivTranspose,delta);

		//a*(delta_vector) = b, so solve for delta_vector
		ublas::matrix<float> mat_a_inverse(3,3);
		bool retval = InvertMatrix(mat_a,mat_a_inverse);

		if(retval)
		{
			ublas::matrix<float> delta_vector = prod(mat_a_inverse,mat_b);

			//cout << "Delta A: " << delta_vector(0,0) << endl;
			//cout << "Delta B: " << delta_vector(1,0) << endl;
			//cout << "Delta C: " << delta_vector(2,0) << endl;

			result.A += delta_vector(0,0);
			result.B += delta_vector(1,0);
			result.C += delta_vector(2,0);

			//cout << "New A: " << result.A << endl;
			//cout << "New B: " << result.B << endl;
			//cout << "New C: " << result.C << endl;
		}
		else
		{
			//cout << "INVERSE FAILED!\n";
			break;
		}
	}

	//cout << "R2: " << result.r2 << endl;

	return result;
}

