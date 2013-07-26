#ifndef VECT_HPP_
#define VECT_HPP_

#include <iostream>
#include <memory>
#include <array>

template<int N, typename T>
class nvect {

	static_assert(N>0,"Cannot create nvect<int N, class T> with N<1");
	using this_type = nvect<N,T>;

public:
	nvect(){}

	nvect(std::array<T,N>&& vals) :values(vals) {
	}

	nvect(const this_type& v) :values(v.values) {
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

	// scalar product
	nvect operator* (const this_type& vect){
		this_type out;
		for(int i=0;i<N;++i)
			out.values[i] = values[i]*vect.values[i];
		return out;
	}

	nvect operator* (T scalar){
		this_type out;
		for(int i=0; i<N; ++i)
			out.values[i] = values[i]*scalar;
		return out;
	}

	nvect operator/ (T scalar){
		this_type out;
		for(int i=0; i<N; ++i)
			out.values[i] = values[i]/scalar;
	}

	bool operator< (const this_type& vect)
	{
		bool out = true;
		for(int i=0;i<N;++i)
			out = out && values[i]<vect[i];
		return out;
	}

	bool operator<= (const this_type& vect)
	{
		bool out = true;
		for(int i=0;i<N;++i)
			out = out && values[i]<=vect[i];
		return out;
	}

	friend std::ostream& operator<<(std::ostream& out,const nvect<N,T>& v){
		out << "(" << v.values[0];
		for(int i=1; i<N; ++i)
			out << "," << v.values[i];
		out << ")";
		return out;
	}

private:
	std::array<T,N> values;
};

template<int N> using dvect<N,double>;

#endif /* VECT_HPP_ */
