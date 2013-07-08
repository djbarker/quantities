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

	// reverse the list
	template<class T>
	struct list_reverse {

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

std::ostream& operator<<(std::ostream& out, const lists::end_element& el)
{
	return out << "END";
}

template<class... Ts>
std::ostream& operator<<(std::ostream& out, const lists::static_list<Ts...>& list)
{
	return out << lists::static_list<Ts...>::lists::elements;
}

#endif /* LISTS_HPP */
