#include "rational.hpp"
#include <cmath>

namespace dims {

	using namespace rational;

	// type representing the physical dimensions of a quantity
	template< class M, class L, class T >
	struct Dimension {
		using mass = M;
		using length = L;
		using time = T;
	};

	// convenience using-statement for generating dimensions with integer powers
	template<int M, int L, int T>
	using IntDim = Dimension<Rational<M>,Rational<L>,Rational<T>>;

	// basic dimension typedefs
	typedef IntDim<0,0,0>	number;
	typedef IntDim<1,0,0>	mass;
	typedef IntDim<0,1,0>	length;
	typedef IntDim<0,0,1>	time;
	typedef IntDim<0,1,-1>	velocity;
	typedef IntDim<0,1,-2>	acceleration;
	typedef IntDim<1,1,-2>	force;
	typedef IntDim<1,2,-2>	work;
	typedef IntDim<0,2,0>	area;
	typedef IntDim<0,3,0>	volume;
	typedef IntDim<0,0,-1>	frequency;

	// operations for modifying dimensions
	template< class Dim1, class Dim2 >
	struct mult_Dimension {
		using result = Dimension<
					 typename simplify<typename add_Rational<typename Dim1::mass, typename Dim2::mass>::result>::result,
					 typename simplify<typename add_Rational<typename Dim1::length, typename Dim2::length>::result>::result,
					 typename simplify<typename add_Rational<typename Dim1::time, typename Dim2::time>::result>::result
					   >;
	};

	// raising a dimension to a rational power
	template< class Dim1, class R1 >
	struct pow_Dimension {
		using result = Dimension<
					 typename simplify<typename mult_Rational<typename Dim1::mass,   R1>::result>::result,
					 typename simplify<typename mult_Rational<typename Dim1::length, R1>::result>::result,
					 typename simplify<typename mult_Rational<typename Dim1::time,   R1>::result>::result
					   >;
	};

	// get the reciprocal of a dimension
	template< class Dim1 >
	struct inv_Dimension {
		using result = Dimension<
				 Rational<-Dim1::mass::numerator,	Dim1::mass::denominator>,
				 Rational<-Dim1::length::numerator,	Dim1::length::denominator>,
				 Rational<-Dim1::time::numerator,	Dim1::time::denominator>
				>;
	};

	template< class Dim1 >
	using sqrt_Dimension = pow_Dimension<Dim1,Rational<1,2>>;

	/*
	 * A wrapper for data which includes information about dimensions.
	 * With compiler optimisations this has no overhead (tested with g++ 4.7 at 03).
	 */
	template<class Dim, class T=double>
	struct quantity {
		T val;

		// basic constructors
		quantity():val(){}
		quantity(T val):val(val){}

		// forward constructor (allows one to use constructor arguments of underlying value type)
		template<typename... Ts>
		quantity(Ts... vs) :val(vs...) {
		}

		// copy constructor
		template<class Dim2>
		quantity(const quantity<Dim2,T>& rhs):val(rhs.val){
			static_assert(std::is_same<Dim,Dim2>::value,"Cannot copy quantity with different dimensions.");
		}

		// assignment
		template<class Dim2>
		quantity<Dim,T>& operator=(const quantity<Dim2,T>& rhs){
			static_assert(std::is_same<Dim,Dim2>::value,"Cannot assign quantities with different dimensions.");
			return val = rhs.val;
		}

		// template alias for return type and new dimensions
		template<class T2> using mult_type = decltype(std::declval<T>()*std::declval<T2>());
		template<class T2> using div_type  = decltype(std::declval<T>()/std::declval<T2>());
		template<class Dim2> using new_dim = typename mult_Dimension<Dim,Dim2>::result;

		/*
		 * arithmetic operators calls the 'raw' operator for T and T2 then wraps the result
		 * in a quantity with the correct type and dimensions before returning.
		 */
		template<class Dim2, class T2>
		quantity<new_dim<Dim2>,mult_type<T2>> operator* (quantity<Dim2,T2> rhs) {
			return quantity<new_dim<Dim2>,mult_type<T2>>(val*rhs.val);
		}

		template<class Dim2, class T2>
		quantity<new_dim<typename inv_Dimension<Dim2>::result>,div_type<T2>> operator/(quantity<Dim2,T2> rhs) {
			return quantity<new_dim<typename inv_Dimension<Dim2>::result>,div_type<T2>>(val/rhs.val);
		}

		friend std::ostream& operator<<(std::ostream& out, const quantity<Dim,T> qty) {
			return out << qty.val;
		}
	};

	// wrappers for functions which change dimension
	template<class Dim, class T>
	quantity< typename sqrt_Dimension<Dim>::result, T> sqrt(const quantity<Dim,T>& qty) {
		return quantity<typename sqrt_Dimension<Dim>::result,T>(::sqrt(qty.val));
	}

	template<class Dim, class T, int N>
	quantity< typename pow_Dimension<Dim,Rational<N>>::result, T> pow(const quantity<Dim,T>& qty) {
		return quantity<typename pow_Dimension<Dim,Rational<N>>::result,T>(::pow(qty.val,N));
	}

}; // namespace dims
