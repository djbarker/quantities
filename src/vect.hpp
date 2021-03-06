#ifndef VECT_HPP_
#define VECT_HPP_

#include <iostream>
#include <memory>
#include <array>

template<size_t N, typename T>
class nvect {

	static_assert(N>0,"Cannot create nvect<size_t N, class T> with N<1");
	using this_type = nvect<N,T>;

	template<size_t M, class U> friend class nvect;

public:
	// ensure nvect is "trivial" as well as standard-layout => POD
	nvect() 								= default;
	nvect(const this_type&) 				= default;
	nvect(this_type&&) 						= default;
	~nvect() 								= default;
	this_type& operator=(const this_type&) 	= default;
	this_type& operator=(this_type&&) 		= default;

	/*
	 * Non-trivial constructors
	 */

	// stop the variadic template constructor hiding copy constructor for non-const objects
	nvect(this_type& v)
	:nvect<N,T>(const_cast<const this_type&>(v))
	{
	}

	// init each component to a different value
	template<typename U, typename... Us>
	nvect(U&& u, Us&&... us) :values{std::forward<U>(u),std::forward<Us>(us)...} {
		static_assert(sizeof...(Us)==N-1,"Not enough args supplied!");
	}

	// access operator
	T& operator[](size_t i){
		return values[i];
	}

	const T& operator[](size_t i) const {
		return values[i];
	}

	// element-wise multiplication
	template<typename U>
	nvect<N,decltype(std::declval<T>()*std::declval<U>())>
	operator* (const nvect<N,U>& vect) const {
		nvect<N,decltype(std::declval<T>()*std::declval<U>())> out;
		for(size_t i=0;i<N;++i)
			out[i] = values[i]*vect[i];
		return out;
	}

	// multiplication by a scalar
	template<typename U>
	nvect<N,decltype(std::declval<T>()*std::declval<U>())>
	operator* (U scalar) const {
		nvect<N,decltype(std::declval<T>()*std::declval<U>())> out;
		for(size_t i=0; i<N; ++i)
			out[i] = values[i]*scalar;
		return out;
	}

	template<typename U, typename V, size_t M>
	friend nvect<M,decltype(std::declval<V>()*std::declval<U>())> operator*(U scalar, const nvect<M,V>& vect);

	T sum() const {
		T out; // default initialise in case T is some non-primitive type
		for(size_t i=0; i<N; ++i)
			out += values[i];
		return out;
	}

	template<typename U>
	decltype(std::declval<T>()*std::declval<U>())
	dot(const nvect<N,U>& vect) const {
		return (vect*(*this)).sum();
	}

	T magnitude() const {
		return sqrt(this->dot(*this));
	}

	// division by a scalar
	template<typename U>
	nvect<N,decltype(std::declval<T>()/std::declval<U>())>
	operator/ (U scalar) const {
		nvect<N,decltype(std::declval<T>()/std::declval<U>())> out;
		for(size_t i=0; i<N; ++i)
			out[i] = values[i]/scalar;
		return out;
	}

	// element wise division
	template<typename U>
	nvect<N,decltype(std::declval<T>()/std::declval<U>())>
	operator/ (const nvect<N,U>& vect) {
		nvect<N,decltype(std::declval<T>()/std::declval<U>())> out;
		for(size_t i=0; i<N; ++i)
			out[i] = values[i]/vect[i];
		return out;
	}

	// addition and subtraction
	nvect operator + (const this_type& vect) const {
		this_type out;
		for(size_t i=0;i<N;++i)
			out.values[i] = values[i] + vect.values[i];
		return out;
	}

	nvect operator - (const this_type& vect) const {
		this_type out;
		for(size_t i=0;i<N;++i)
			out.values[i] = values[i] - vect.values[i];
		return out;
	}

	// assigment operators
	this_type& operator+= (const this_type& vect) {
		for(size_t i=0;i<N;++i)
			values[i] += vect.values[i];
		return *this;
	}

	this_type& operator-= (const this_type& vect) {
		for(size_t i=0;i<N;++i)
			values[i] -= vect.values[i];
		return *this;
	}

	this_type& operator*= (const this_type& vect) {
		for(size_t i=0;i<N;++i)
			values[i] *= vect.values[i];
		return *this;
	}

	this_type& operator/= (const this_type& vect) {
		for(size_t i=0;i<N;++i)
			values[i] /= vect.values[i];
		return *this;
	}

	this_type& operator*= (T scalar) {
		for(size_t i=0;i<N;++i)
			values[i] *= scalar;
		return *this;
	}

	this_type& operator/= (T scalar) {
		for(size_t i=0;i<N;++i)
			values[i] /= scalar;
		return *this;
	}

	// comparison operators - useful for bounding boxes
	bool operator< (const this_type& vect) const {
		bool out = true;
		for(size_t i=0;i<N;++i)
			out = out && values[i]<vect[i];
		return out;
	}

	bool operator<= (const this_type& vect) const {
		bool out = true;
		for(size_t i=0;i<N;++i)
			out = out && values[i]<=vect[i];
		return out;
	}

	void serialize(std::ostream& out) const {
		for(size_t i=0;i<N;++i)
			out.write((char*)(&values[i]),sizeof(T));
	}

	void deserialize(std::istream& in)
	{
		for(size_t i=0;i<N;++i)
			in.read((char*)(&values[i]),sizeof(T));
	}

	// make a vector where each component is the same value
	template<size_t M, typename U, typename V> friend nvect<M,U> make_vect(const V& val);

	// cast a vector to a vector containing a different type
	template<typename U, typename V, size_t M> friend nvect<M,U> vect_cast(const nvect<M,V>& vect);

	friend std::ostream& operator<<(std::ostream& out,const nvect<N,T>& v){
		out << "(" << v.values[0];
		for(size_t i=1; i<N; ++i)
			out << "," << v.values[i];
		out << ")";
		return out;
	}

private:
	T values[N];
};

template<size_t M, typename U, typename V>
nvect<M,U> make_vect(const V& val) {
	nvect<M,U> out;
	for(auto& o_val : out.values)
		o_val = U(val);
	return out;
}

template<typename U, typename V, size_t M>
nvect<M,U> vect_cast(const nvect<M,V>& vect) {
	nvect<M,U> out;
	for(size_t i=0; i<M; ++i)
		out.values[i] = U(vect.values[i]);
	return out;
}

template<typename U, typename V, size_t M>
nvect<M,decltype(std::declval<V>()*std::declval<U>())> operator*(U scalar, const nvect<M,V>& vect)
{
	return vect*scalar;
}

#endif /* VECT_HPP_ */
