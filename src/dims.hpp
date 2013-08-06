#ifndef DIMS_HPP_
#define DIMS_HPP_

#include "lists.hpp"
#include <cmath>
#include <ratio>
#include <type_traits>

namespace dims {

	using namespace lists;

	// type representing the physical dimensions of a quantity
	template<class... Rs>
	using Dimension = typename static_list<Rs...>::elements;

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

		typedef T value_type;

		// all quantities are friends (e.g. quantity<mass> is friends with quantity<time> and vice-versa)
		template<class Dim2, class T2> friend class quantity;

		// basic constructors
		constexpr quantity():val(){}
		explicit quantity(T val):val(val){}

		// forward constructor (allows one to use constructor arguments of underlying value type)
		template<typename... Ts>
		constexpr quantity(Ts... vs) :val(vs...) {
		}

		// copy constructor
		template<class Dim2>
		constexpr quantity(const quantity<Dim2,T>& rhs):val(rhs.val){
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
		quantity<new_dim<Dim2>,mult_type<T2>> operator* (quantity<Dim2,T2> rhs) const {
			return quantity<new_dim<Dim2>,mult_type<T2>>(val*rhs.val);
		}

		template<class Dim2, class T2>
		quantity<new_dim<typename inv_Dimension<Dim2>::result>,div_type<T2>> operator/(quantity<Dim2,T2> rhs) const {
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
		 * We can only add or subtract quantities with the same dimension.
		 */

		template<class T2, class T3=decltype(std::declval<T>()+std::declval<T2>())>
		quantity<Dim,T3> operator+(quantity<Dim,T2> rhs) const {
			return quantity<Dim,T3>(val+rhs.val);
		}

		template<class T2, class T3=decltype(std::declval<T>()-std::declval<T2>())>
		quantity<Dim,T3> operator-(quantity<Dim,T2> rhs) const {
			return quantity<Dim,T3>(val-rhs.val);
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

		// comparison operators
		template<class T2>
		bool operator<(quantity<Dim,T2> rhs) const {
			return val < rhs.val;
		}

		template<class T2>
		bool operator<=(quantity<Dim,T2> rhs) const {
			return val <= rhs.val;
		}

		// for accessing member functions of val
		T* operator->() {
			return &val;
		}

		template<typename U=T,typename R=decltype(std::declval<U>().operator[](0))>
		quantity<Dim,R> operator[](int i) const {
			return quantity<Dim,R>(val[i]);
		}

		// delegate printing to the value type
		friend std::ostream& operator<<(std::ostream& out, const quantity<Dim,T> qty) {
			return out << qty.val;
		}

		// discard dimensional saftey and get the raw value
		friend T discard_dims(const quantity<Dim,T>& qty) {
			return qty.val;
		}

		/*
		 * Wrappers for functions which change dimension
		 */

		// square root
		friend quantity< typename sqrt_Dimension<Dim>::result, T> sqrt(const quantity<Dim,T>& qty) {
			return quantity<typename sqrt_Dimension<Dim>::result,T>(::sqrt(qty.val));
		}

		// raise to a rational power
		template<typename R> // R must be a type of std::ratio
		friend quantity< typename pow_Dimension<Dim,R>::result, T> pow(const quantity<Dim,T>& qty) {
			return quantity<typename pow_Dimension<Dim,R>::result,T>(::pow(qty.val,(double)R::num/(double)R::den));
		}

		template<intmax_t A>
		friend quantity< typename pow_Dimension<Dim,std::ratio<A>>::result,T> pow(const quantity<Dim,T>& qty) {
			return quantity<typename pow_Dimension<Dim,std::ratio<A>>::result,T>(::pow(qty.val,(double)A));
		}


	private:
		T val;
	};

	/*
	 * Some common dimensions and dimensional quantities.
	 */

	// convenience using-statement for generating dimensions with integer powers
	template<int... Is>
	using IntDim = Dimension<std::ratio<Is>...>;

	// basic dimension typedefs
	typedef IntDim<0,0,0>	number;
	typedef IntDim<1,0,0>	mass;
	typedef IntDim<0,1,0>	length;
	typedef IntDim<0,1,0>	position;
	typedef IntDim<0,0,1>	time;
	typedef IntDim<0,1,-1>	velocity;
	typedef IntDim<1,1,-1>  momentum;
	typedef IntDim<0,1,-2>	acceleration;
	typedef IntDim<1,1,-2>	force;
	typedef IntDim<1,2,-2>	work;
	typedef IntDim<0,2,0>	area;
	typedef IntDim<0,3,0>	volume;
	typedef IntDim<0,0,-1>	frequency;
	typedef IntDim<1,-1,-2> pressure;
	typedef IntDim<1,-3,0>  density;
	typedef IntDim<0,-3,0>  number_density;
	typedef IntDim<1,-1,-1> viscosity; // dynamic

#define CQ_IMPL(a) template<typename T=double> using a ## _ ## t = quantity<a,T>;

	// typedefs for common quantities - saves typing
	CQ_IMPL(number)
	CQ_IMPL(mass)
	CQ_IMPL(length)
	CQ_IMPL(time)
	CQ_IMPL(velocity)
	CQ_IMPL(momentum)
	CQ_IMPL(acceleration)
	CQ_IMPL(force)
	CQ_IMPL(work)
	CQ_IMPL(area)
	CQ_IMPL(volume)
	CQ_IMPL(frequency)

#define UDL_IMPL(a) constexpr quantity<a,double> operator"" _ ## a (long double d) { return quantity<a,double>(d); };

	// user-defined literals for creating dimensional doubles quickly
	UDL_IMPL(number)
	UDL_IMPL(mass)
	UDL_IMPL(length)
	UDL_IMPL(time)
	UDL_IMPL(velocity)
	UDL_IMPL(momentum)
	UDL_IMPL(acceleration)
	UDL_IMPL(force)
	UDL_IMPL(work)
	UDL_IMPL(area)
	UDL_IMPL(volume)
	UDL_IMPL(frequency)

	/*
	 * Definitions of some useful numbers
	 */
	constexpr number_t<> eulers = 2.7182818284590452353; // e
	constexpr number_t<> pi     = 3.1415926535897932384; // pi
	constexpr number_t<> phi    = 1.6180339887498948482; // golden-ratio

}; // namespace dims

#endif /* DIMS_HPP_ */
