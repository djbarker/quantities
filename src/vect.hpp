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

	T& operator[](size_t i){
		return values[i];
	}

	nvect operator* (T scalar){
		this_type out;
		for(int i=0; i<N; ++i)
			out.values[i] = values[i]*scalar;
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

using real3 = nvect<3,double>;
