#include <tuple>

namespace onart{

template<class... T>
struct __hollow {};

template<class F, class... T>
__hollow<T..., F> append(__hollow<T...>, __hollow<F>) { return {}; }

template<class F, class... T>
constexpr auto __rev(__hollow <F>, __hollow<T...>);

template<class F>
constexpr __hollow<F> __rev(__hollow <F>, __hollow<>) { return {}; }

template<class F, class S>
constexpr __hollow<S, F> __rev(__hollow<F>, __hollow<S>) {	return {}; }

template<class F, class S, class... T>
constexpr auto __rev(__hollow<F>, __hollow<S, T...>) { 
	return append(__rev(__hollow<S>{}, __hollow<T...>{}), __hollow<F>{});
}

template<class F, class... T>
constexpr auto reverse(){
	return __rev(__hollow<F>{}, __hollow<T...>{});
}

template<class... T>
struct ftuple {
	template<class... T2> constexpr static std::tuple<T2...> _(__hollow<T2...>);
	template<unsigned N>
	auto& get() {
		static_assert(N < sizeof...(T), "aaa");
		return std::get<sizeof...(T) - 1 - N>(v);
	}
	ftuple() = default;
	ftuple(T&&... args):v(args...) {};
private:
	decltype(_(reverse<T...>())) v;
};
  
}
