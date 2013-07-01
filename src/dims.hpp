#ifndef DIMS_HPP_
#define DIMS_HPP_

#include "lists.hpp"
#include <cmath>
#include <ratio>

namespace dims {

	using namespace lists;

	// type representing the physical dimensions of a quantity
	template<class... Rs>
	using Dimension = typename static_list<Rs...>::elements;

	// convenience using-statement for generating dimensions with integer powers
	template<int... Is>
	using IntDim = Dimension<std::ratio<Is>...>;

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

	// return a simplified ratio
	template<class R>
	struct ratio_simplify {
		using type = std::ratio<R::num,R::den>;
	};

	// operations for modifying dimensions
	template< class Dim1, class Dim2 >
	struct mult_Dimension {
		using result = typename operate<ratio_simplify,typename operate<std::ratio_add,Dim1,Dim2>::result>::result;
	};

	// raising a dimension to a rational power
	template< class Dim, class R >
	struct pow_Dimension {
		using _RList = typename make_list_from_type<list_length<Dim>::value,R>::type;
		using result = typename operate<ratio_simplify,typename operate<std::ratio_multiply,Dim,_RList>::result>::result;
	};

	// get the reciprocal of a dimension
	template< class Dim >
	struct inv_Dimension {
		// negation operator
		template< class R >
		struct ratio_negate {
			using type = std::ratio<-R::num,R::den>;
		};

		using result = typename operate<ratio_simplify,typename operate<ratio_negate,Dim>::result>::result;
	};

	template< class Dim >
	using sqrt_Dimension = pow_Dimension<Dim,std::ratio<1,2>>;

	/*
	 * A wrapper for data which includes information about dimensions.
	 * With compiler optimisations this has no overhead (tested with g++ 4.7 with -O3).
	 */
	template<class Dim, class T=double>
	struct quantity {
		T val;

		typedef Dim dims;

		// basic constructors
		quantity():val(){}
		explicit quantity(T val):val(val){}

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

		/*
		 * We can only multiply-assign and divide-assign by dimensionless numbers
		 */
		template<class Dim2, class T2>
		quantity<Dim,T>& operator*=(quantity<Dim2,T2> rhs) {
			static_assert(std::is_same<Dim2,typename make_list_from_type<list_length<Dim>::value,std::ratio<0>>::type>::value,"Can only *= with dimensionless RHS");
			val *= rhs.val;
			return *this;
		}

		template<class Dim2, class T2>
		quantity<Dim,T>& operator/=(quantity<Dim2,T2> rhs) {
			static_assert(std::is_same<Dim2,typename make_list_from_type<list_length<Dim>::value,std::ratio<0>>::type>::value,"Can only *= with dimensionless RHS");
			val /= rhs.val;
			return *this;
		}

		/*
		 * We can only and or subtract quantities with the same dimension.
		 */

		template<class T2, class T3>
		quantity<Dim,T3> operator+(quantity<Dim,T2> rhs) {
			return quantity<Dim,T3>(val+rhs.val);
		}

		template<class T2, class T3>
		quantity<Dim,T3> operator-(quantity<Dim,T2> rhs) {
			return quantity<Dim,T3>(val+rhs.val);
		}

		template<class T2>
		quantity<Dim,T>& operator+=(quantity<Dim,T2> rhs) {
			val += rhs.val;
			return *this;
		}

		template<class T2>
		quantity<Dim,T>& operator-=(quantity<Dim,T2> rhs) {
			val -= rhs.val;
			return *this;
		}
		friend std::ostream& operator<<(std::ostream& out, const quantity<Dim,T> qty) {
			return out << qty.val;
		}
	};

	/*
	 * Wrappers for functions which change dimension
	 */

	// square root
	template<class Dim, class T>
	quantity< typename sqrt_Dimension<Dim>::result, T> sqrt(const quantity<Dim,T>& qty) {
		return quantity<typename sqrt_Dimension<Dim>::result,T>(::sqrt(qty.val));
	}

	// raise to a rational power
	template<class Dim, class T, intmax_t A, intmax_t B> // (has to be intmax_t to match definition of std::ratio)
	quantity< typename pow_Dimension<Dim,std::ratio<A,B>>::result, T> pow(const quantity<Dim,T>& qty, std::ratio<A,B> R) {
		return quantity<typename pow_Dimension<Dim,decltype(R)>::result,T>(::pow(qty.val,(double)decltype(R)::num/(double)decltype(R)::den));
	}

	// definitions of some useful numbers
	const quantity<number> eulers = 2.7182818284590452353; // e
	const quantity<number> pi     = 3.1415926535897932384; // pi
	const quantity<number> phi    = 1.6180339887498948482; // golden-ratio

}; // namespace dims

#endif /* DIMS_HPP_ */
