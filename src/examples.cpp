#include <iostream>

#include "vect.hpp"
#include "dims.hpp"

using namespace std;
using namespace dims;
using namespace rational;

int main(int argc, char* argv[])
{
	/*
	 * Examples of compile time rational calculations
	 */

	// add two rationals
	add_Rational<Rational<1,2>,Rational<3,4>>::result res;
	cout << res << endl;

	// check for a common factor
	cout << std::boolalpha << is_common_factor<10,8,2>::value << endl;

	// find the highest common divisor
	cout << highest_common_factor<10,5>::value << endl;

	// simplify the result of the previous addition
	simplify<decltype(res)>::result simpl1;
	cout << simpl1 << endl;

	// simplify a negative rational
	simplify<Rational<-2,4>>::result simpl2;
	cout << simpl2 << endl;

	/*
	 * Example of using dimensional analysis with single types
	 */
	quantity<length> l = 1.0;
	quantity<force> f = 2.0;
	quantity<work> w = l*f;
	cout << w << endl;
	//quantity<force> f_error = l*f; // compile error: dimension of l*f does not match 'force'
	
	/*
	 * Example of using dimensional analysis with mixed types
	 */
	quantity<acceleration,real3> a(1.0,2.0,3.0);
	quantity<mass> m(1.4);
	quantity<force,real3> f2 = a*m;
	cout << f2 << endl;

	quantity<area> A(4.0);
	quantity<length> L = sqrt(A);
	cout << "L = " << L << endl;

	return 0;
}
