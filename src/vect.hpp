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
	nvect(){}

	nvect(std::array<T,N>&& vals) :values(vals) {
	}

	nvect(const this_type& v) :values(v.values) {
	}

	// can be used to explicitly convert between nvects storing different types.
	template<typename U>
	explicit nvect(const nvect<N,U>& v) {
		for(size_t i=0;i<N;++i)
			values[i] = T(v.values[i]);
	}

	template<typename U>
	explicit nvect(const U& val) {
		for(size_t i=0;i<N;++i)
			values[i] = T(val);
	}

	template<typename... Ts>
	nvect(T t, Ts&&... vs) :values{{std::forward<T>(t),std::forward<Ts>(vs)...}} {
		static_assert(sizeof...(Ts)==N-1,"Not enough args supplied!");
	}

	~nvect(){}

	nvect& operator=(const nvect& v){
		values = v.values;
		return *this;
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
		nvect<N,decltype(std::declval<T>()*std::declval<U>())> out;
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

	friend std::ostream& operator<<(std::ostream& out,const nvect<N,T>& v){
		out << "(" << v.values[0];
		for(size_t i=1; i<N; ++i)
			out << "," << v.values[i];
		out << ")";
		return out;
	}

private:
	std::array<T,N> values;
};

template<size_t N> using dvect = nvect<N,double>;

#endif /* VECT_HPP_ */
