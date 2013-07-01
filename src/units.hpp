#ifndef UNITS_HPP_
#define UNITS_HPP_

namespace units
{

	/*
	 * Unit systems - these are represented as static lists of types.
	 * Some standard systems are predefined but the user can build their
	 * own if they wish.
	 */
	namespace si
	{
		struct kg_t {};
		struct m_t {};
		struct s_t {};
	}

	namespace cgs
	{
		struct g_t {};
		struct cm_t {};
	}

	namespace imperial
	{
		struct ft_t {};
		struct in_t {};
		struct lb_t {};
	}

	using si_system = lists::static_list<si::kg_t,si::m_t,si::s_t>::elements;
	using cgs_system = lists::static_list<cgs::g_t,cgs::cm_t,si::s_t>::elements;
	using imperial_system = lists::static_list<imperial::lb_t,imperial::ft_t,si::s_t>::elements;

	/*
	 * Conversion factors for fundamental units. Returns a unit value 1.0 originally in
	 * unit U2 converted to U1. This is done as a static member of a template struct to
	 * allow us to partially specialize for the case where the units are the same
	 */

	template<class U1, class U2>
	struct convert {
		static constexpr double factor() {
			return 1.0/convert<U2,U1>::factor(); // allows us to define conversions in one direction only but have it work for both
		}
	};

	template<> struct convert<si::kg_t,cgs::g_t> { static constexpr double factor() { return 0.001; } };
	template<> struct convert<si::m_t,cgs::cm_t> { static constexpr double factor() { return 0.01; } };
	template<> struct convert<si::kg_t,imperial::lb_t> { static constexpr double factor() { return 0.453592; } };
	template<> struct convert<si::m_t,imperial::ft_t> { static constexpr double factor() { return 0.3048; } };
	template<> struct convert<cgs::g_t,imperial::lb_t> { static constexpr double factor() { return convert<cgs::g_t,si::kg_t>::factor()*convert<si::kg_t,imperial::lb_t>::factor(); } };

	template<class U1> struct convert<U1,U1> { static constexpr double factor() { return 1.0; } };

	/*
	 * A recursive function which computes compound conversion factors from
	 * fundamental conversion factors and the rational powers of a quantities
	 * dimensions.
	 */
	template<class Dim, class System1, class System2>
	constexpr double conversion_factor() {
		using ratpow = typename Dim::value;
		return pow(convert<typename System1::value,typename System2::value>::factor(),(double)ratpow::num/(double)ratpow::den)
				*conversion_factor<typename Dim::tail,typename System1::tail,typename System2::tail>();
	}

	template<>
	constexpr double conversion_factor<lists::end_element,lists::end_element,lists::end_element>() {
		return 1.0;
	}

	/*
	 * Class for representing quantities with units as well as dimensions.
	 */
	template<class Dim, class System, class T = double>
	class unit {
		T val;
		//unit(const T& t) : val(t) {}
		unit(T t) : val(t) {}

		using this_type = unit<Dim,System,T>;

	public:
		unit() :val(1.0) {}

		// create a unit from a quantity with different units (but same dimensions)
		template<class System2>
		unit(const unit<Dim,System2,T>& u) :val(u.val*conversion_factor<Dim,System,System2>()) {}

		// create a unit from another quantity with the same units - no conversion necessary
		unit(const this_type& u) :val(u.val) {}

		/*
		 * Multiply units - produces a unit object with the correct dimensions
		 * and conversion factors applied, the result is in the same system as
		 * the left hand side.
		 */

		template<class Dim2, class System2, class T2>
		using mult_type = unit<typename dims::mult_Dimension<Dim,Dim2>::result,System,decltype(std::declval<T>()*std::declval<T2>())>;

		template<class Dim2, class System2, class T2>
		mult_type<Dim2,System2,T2> operator*(const unit<Dim2,System2,T2>& u){
			return mult_type<Dim2,System2,T2>
							(
								unit<Dim2,System,T2>(u) // convert u to the correct system
							 	.val * val              // multiply by the value
							);							// create the return value directly from the raw value
		}

		// divide two units - see multiply for explanation
		template<class Dim2, class System2, class T2>
		using div_type = unit<typename dims::mult_Dimension<Dim,typename dims::inv_Dimension<Dim2>::result>::result,System,decltype(std::declval<T>()/std::declval<T2>())>;

		template<class Dim2, class System2, class T2>
		div_type<Dim2,System2,T2> operator/(const unit<Dim2,System2,T2>& u){
			return div_type<Dim2,System2,T2>(val / unit<Dim2,System,T2>(u).val);
		}

		// create a unit from a raw data type - declaration
		template<class Dim2, class System2, class T2>
		friend unit<Dim2,System2,T2> operator*(T2 t,const unit<Dim2,System2,T2>& u);

		friend std::ostream& operator<<(std::ostream& out, this_type u) {
			return out << u.val;
		}

		// friend declaration so we can access private members from other unit types
		template<class Dim2, class System2, class T2> friend class unit;
	};

	// create a unit from a raw data type - defintion
	template<class Dim2, class System2, class T2>
	unit<Dim2,System2,T2> operator*(T2 t,const unit<Dim2,System2,T2>& u){
		return unit<Dim2,System2,T2>(t); // utilise friendship
	}

	/*
	 * Unit objects
	 */
	unit<dims::length,si_system> meter;
	unit<dims::mass,si_system> kilogram;
	unit<dims::time,si_system> second;
	unit<dims::length,cgs_system> cm;
	unit<dims::mass,cgs_system> gram;

} // namespace units

#endif /* UNITS_HPP_ */
