#include <iostream>

#include "vect.hpp"
#include "dims.hpp"
#include "lists.hpp"
#include "units.hpp"
#include "rational.hpp"

using namespace std;
using namespace dims;
using namespace lists;
using namespace units;
using namespace rational;

int main(int argc, char* argv[])
{
	/*
	 * Example of static lists
	 */

	// create two static lists
	using list1 = static_list<Rational<1>,Rational<2>>::elements;
	using list2 = static_list<Rational<2>,Rational<4>>::elements;

	// print them
	cout << list1() << endl;
	cout << list2() << endl;

	// apply an operation to them and print
	using plus12 = operate<add_Rational,list1,list2>::result;

	cout << plus12() << endl;

	// add items to the first list
	using list3 = push_back<list1,Rational<6>>::type;
	using list4 = push_front<list3,Rational<0>>::type;

	cout << list4() << endl;

	/*
	 * Examples of compile time rational calculations
	 */

	// add two rationals
	add_Rational<Rational<1,2>,Rational<3,4>>::type res;
	cout << res << endl;

	// check for a common factor
	cout << std::boolalpha << is_common_factor<10,8,2>::value << endl;

	// find the highest common divisor
	cout << highest_common_factor<10,5>::value << endl;

	// simplify the result of the previous addition
	rational::simplify<decltype(res)>::type simpl1;
	cout << simpl1 << endl;

	// simplify a negative rational
	rational::simplify<Rational<-2,4>>::type simpl2;
	cout << simpl2 << endl;

	/*
	 * Example of using dimensional analysis with single types
	 */
	quantity<length> l = 1.0;
	quantity<force> f = 2.0;
	quantity<work> w = l*f;
	cout << "W = " << w << endl;
	//quantity<force> f_error = l*f; // compile error: dimension of l*f does not match 'force'
	
	/*
	 * Example of using dimensional analysis with mixed types
	 */
	quantity<acceleration,real3> a(1.0,2.0,3.0);
	quantity<mass> m(1.4);
	quantity<force,real3> f2 = a*m;
	cout << "F = " << f2 << endl;

	/*
	 * Example of using functions which change dimensions
	 */
	quantity<area> A = 4.0;
	quantity<length> L = sqrt(A);
	cout << "L = " << L << endl;
	quantity<volume> V = pow(L,ratio<3,1>());

	/*
	 * Assignment operators
	 */

	quantity<length> dist = 2.4;
	// dist += quantity<mass>(12.4); // compilation error - cannot add quantities with different dimensions
	dist += quantity<length>(1.6);
	// dist *= quantity<mass>(1.0); // compilation error - multiplying changes dimension
	dist *= quantity<number>(2.0);

	/*
	 * Examples with units
	 */

	unit<length,si_system> L2 = 4.0*meter;
	unit<area,si_system> A2 = L2*(1.0*cm);
	cout << A2 << endl; // should be 0.04 m^2
	unit<area,si_system> A3 = (1.0*cm)*L2; // check reverse conversion
	cout << A3 << endl;
	unit<area,imperial_system> A4 = A3; // automatic conversion to ft^2: should be 0.4306
	cout << A4 << endl;

	return 0;
}
