#ifndef RATIONAL_HPP_
#define RATIONAL_HPP_

#include <iostream>

/*
 * Types for compile time rational number calculations. This is
 * reinventing the wheel; really one should prefer std::ratio
 * from <ratio>
 */

#warning "Use std::ratio instead."

namespace rational
{

	template<int A, int B=1>
	struct Rational {
		enum { numerator=A, denominator=B };
	};

	/*
	 * Operations needed for finding the HCF
	 */

	template<int A, int B>
	struct static_min {
		static const int value = (A<B?A:B);
	};

	template<int A, int B, int C>
	struct is_common_factor {
		static const bool value = (!(A%C))&&(!(B%C));
	};

	// 0 is not a common factor of any non-zero number
	template<int A, int B>
	struct is_common_factor<A,B,0> {
		static const bool value = false;
	};

	template<int,int,int,bool>
	struct _hcf;

	template<int A, int B, int N>
	struct _hcf<A,B,N,true> {
		static const int value = N;
	};

	template<int A, int B, int N>
	struct _hcf<A,B,N,false> {
		static const int value = _hcf<A,B,N-1,is_common_factor<A,B,N-1>::value>::value;
	};

	/*
	 * Find the highest common factor of two integers.
	 */
	template<int A, int B>
	struct highest_common_factor {
		static const int _A = abs(A);
		static const int _B = abs(B);
		static const int value = _hcf<_A,_B,static_min<_A,_B>::value,is_common_factor<_A,_B,static_min<_A,_B>::value>::value>::value;
	};

	// Handle zero such that when dividing by HCF 0/B becomes 0/1
	template<int B>
	struct highest_common_factor<0,B> {
		static const int value = B;
	};

	/*
	 * Simplify a rational e.g. 10/5 becomes 2/1
	 */
	template<class R1>
	struct simplify {
		using type = Rational<
								 R1::numerator / highest_common_factor<R1::numerator,R1::denominator>::value,
								 R1::denominator / highest_common_factor<R1::numerator,R1::denominator>::value
								>;
	};

	/*
	 * Simple algebraic operations
	 */

	template< class R1, class R2>
	struct add_Rational {
		using type = Rational<R1::numerator*R2::denominator+R2::numerator*R1::denominator,R1::denominator*R2::denominator>;
	};

	template< class R1, class R2>
	struct sub_Rational {
		using type = Rational<R1::numerator*R2::denominator-R2::numerator*R1::denominator,R1::denominator*R2::denominator>;
	};

	template< class R1, class R2>
	struct mult_Rational {
		using type = Rational<R1::numerator*R2::numerator,R1::denominator*R2::denominator>;
	};

	template< class R1, class R2>
	struct div_Rational {
		using type = Rational<R1::numerator*R2::denominator,R1::denominator*R2::numerator>;
	};

}; // namespace rational

template<int A, int B>
std::ostream& operator<<(std::ostream& out, rational::Rational<A,B> R)
{
	return out << A << '/' << B;
}

#endif /* RATIONAL_HPP_ */
