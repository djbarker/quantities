#ifndef LISTS_HPP
#define LISTS_HPP

#include <iostream>

/*
 * Structures for forming lists of types and performing operations on the
 * elements are compile time.
 */

namespace lists
{

	// lists are stored recursively (imagine a unary tree)
	template<class V, typename T>
	struct list_element {
		using value = V;
		using tail = T;
	};

	// for ending recursion
	struct end_element {};

	// construct the list using variadic templates
	template<class T, class... Ts>
	struct static_list {
		using elements = list_element<T,typename static_list<Ts...>::elements>;
	};

	// handle the last element
	template<class T>
	struct static_list<T> {
		using elements = list_element<T,end_element>;
	};

	// get list length by counting recursively
	template<class List>
	struct list_length {
		static constexpr int value = list_length<typename List::tail>::value+1;
	};

	template<>
	struct list_length<end_element> {
		static constexpr int value = 0; // end recursion
	};

	// construct a list of length N by duplicating the same type
	template<int N, class T>
	struct make_list_from_type {
		using type = list_element<T, typename make_list_from_type<N-1,T>::type>;
	};

	template<class T>
	struct make_list_from_type<1,T> {
		using type = list_element<T,end_element>; // end recursion
	};

	// add an element to the front of the list
	template<class List, class T>
	struct push_front {
		using type = list_element<T,List>;
	};

	// add an element to the end of the list
	template<class List, class T>
	struct push_back {
		using type = list_element<typename List::value,typename push_back<typename List::tail,T>::type>;
	};

	template<class T>
	struct push_back<end_element,T> {
		using type = list_element<T,end_element>;
	};

	// remove the first element of the list
	template<class T>
	struct pop_front {
		using type = typename T::tail;
	};

	// remove the last item in the list
	template<class T>
	struct pop_back {

		template<class T1, class T2>
		struct pop_back_impl {
			using type = list_element<T1,typename pop_back_impl<typename T2::value, typename T2::tail>::type>;
		};

		template<class T1>
		struct pop_back_impl<T1,end_element> {
			using type = end_element;
		};

		using type = typename pop_back_impl<typename T::value,typename T::tail>::type;
	};

	// remove a particular item from the list
	template<class T, int N>
	struct pop_int {

		template<class T1, int I, int TLength>
		struct pi_impl {
			using type = list_element<typename T1::value, typename pi_impl<typename T1::tail,I+1,TLength>::type>;
		};

		template<class T1, int TLength>
		struct pi_impl<T1,N,TLength> {
			using type = list_element<typename T1::tail::value, typename T1::tail::tail>;
		};

		// handle case when idx is list length - 1
		template<class T1>
		struct pi_impl<T1,N,N+1> {
			using type = end_element;
		};

		using type = typename pi_impl<T,0,list_length<T>::value>::type;
	};

	// gets the last item
	template<class T>
	struct list_back {

		template<class T2, class V>
		struct lb_impl {
			using type = typename lb_impl<typename T2::tail,typename T2::value>::type;
		};

		template<class V>
		struct lb_impl<end_element,V> {
			using type = V;
		};

		using type = typename lb_impl<typename T::tail,typename T::value>::type;
	};

	// reverse the list
	template<class T>
	struct list_reverse {

		template<class T2, int N>
		struct lr_impl {
			using type = list_element<
										typename list_back<T2>::type,
										typename lr_impl<typename pop_back<T2>::type,list_length<T2>::value-1>::type
									 >;
		};

		template<class T2>
		struct lr_impl<T2,2> {
			using type = typename static_list<typename T2::tail::value,typename T2::value>::elements;
		};

		template<class T2>
		struct lr_impl<T2,1> {
			using type = T2;
		};

		using type = typename lr_impl<T,list_length<typename T::tail>::value>::type;
	};

	// get the N-th element
	template<class T, int N>
	struct list_get_int {
		template<class T1, int I>
		struct lgi_impl {
			using type = typename lgi_impl<typename T1::tail,I+1>::type;
		};

		template<class T1>
		struct lgi_impl<T1,N> {
			using type = typename T1::value;
		};

		using type = typename lgi_impl<T,0>::type;
	};

	/*
	 * Performs an operation on N lists. The lists must have the same length.
	 * The operation is specified as a class template with a return typedef named
	 * 'type'.
	 */
	template<template<class...> class Op, class... Lists>
	struct operate {
		using value = typename Op<typename Lists::value...>::type;
		using result = list_element<value,typename operate<Op, typename Lists::tail...>::result>;
	};

	// end recursion - Note this is not perfect; it will stop only when the first list hits its end.
	template<template<class...> class Op, class... Rest>
	struct operate<Op,end_element,Rest...> {
		using result = end_element;
	};

}; // namespace lists

/*
 * Functions for outputting statically computed lists at runtime.
 */

template<class V, class T>
std::ostream& operator<<(std::ostream& out, const lists::list_element<V,T>& el)
{
	return out << "<" << V() << ", " << T() << ">";
}

std::ostream& operator<<(std::ostream& out, const lists::end_element& el);

template<class... Ts>
std::ostream& operator<<(std::ostream& out, const lists::static_list<Ts...>& list)
{
	return out << lists::static_list<Ts...>::lists::elements;
}

#endif /* LISTS_HPP */
