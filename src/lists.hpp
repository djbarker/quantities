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
	struct static_list;


	template<class T, class... Ts>
	struct static_list {
		using elements = list_element<T,typename static_list<Ts...>::elements>;
	};

	// handle the last element
	template<class T>
	struct static_list<T> {
		using elements = list_element<T,end_element>;
	};

	/*
	 * Performs an operation on N lists. The lists must have the same length.
	 * The operation is specified as a class template with a return typedef named
	 * 'result'.
	 */
	template<template<class...> class Op, class... Lists>
	struct operate {
		using value = typename Op<typename Lists::value...>::result;
		using result = list_element<value,typename operate<Op, typename Lists::tail...>::result>;
		//using result = list_element<value,typename Op<operate<Op,typename Lists::tail...>::value>::result>;
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
