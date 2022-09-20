/* LICENSE: NONE (public domain)
ftuple is a class like std::tuple, but its order in memory is forward

example:

type ftuple<int, float, double, char> is like:
struct{
    int _1;
    float _2;
    double _3;
    char _4;
};

while std::tuple<int, float, double, char> is like:
struct{
    char _4;
    double _3;
    float _2;
    int _1;
};

This is available on MSVC, Clang, and GCC
*/

#include <type_traits>

namespace onart{
    
    // something useful with the class below
    template <class, class...>
    inline constexpr bool is_one_of = false;

    template <class A, class B, class... T>
    inline constexpr bool is_one_of<A, B, T...> = std::is_same_v<A, B> || is_one_of<A, T...>;
    // something useful with the class below
    
    template<class... T>
    struct ftuple;
    
    template<class F>
    struct ftuple<F>{
    public:
        F first;
        template<unsigned POS>
        constexpr auto& get(){
            static_assert(POS == 0, "Index exceeded");
            return first;
        }
        
        template<unsigned POS>
        constexpr static size_t offset(){
            static_assert(POS == 0, "Index exceeded");
            return 0;
        }
        ftuple() = default;
	ftuple(const ftuple&) = default;
	ftuple(const F& f): first(f){}
	ftuple& operator=(const ftuple&) = default;
    protected:
        using Type = F;
        using firstType = ftuple<F>;
    };
    
    template<class F, class... T>
    struct ftuple<F, T...>: ftuple<F>, ftuple<T...>{
    protected:
        using Type = F;
        using firstType = ftuple<F>;
        using lastType = ftuple<T...>;
    private:
        template<unsigned POS, class FT>
        constexpr auto& get(){
            if constexpr(POS == 0) return FT::firstType::first;
            else return get<POS - 1, typename FT::lastType>();
        }
        
        template<unsigned POS, class FT>
        constexpr static size_t offset(){
            using thisType = ftuple<F, T...>;
            if constexpr (POS == 0) return (::size_t)reinterpret_cast<char const volatile*>(&(((thisType*)0)->FT::firstType::first));   // warning: this one can't be calculated in compile time
            else return offset<POS - 1, typename FT::lastType>();
        }
    public:
        template<unsigned POS>
        constexpr auto& get(){
            static_assert(POS <= sizeof...(T), "Index exceeded");
            return get<POS, ftuple<F, T...>>();
        }
        
        template<unsigned POS>
        constexpr static size_t offset(){
            static_assert(POS <= sizeof...(T), "Index exceeded");
            return offset<POS, ftuple<F, T...>>();
        }
        ftuple() = default;
	ftuple(const ftuple&) = default;
	ftuple(const F& f, const T&... t): firstType{f}, lastType(t...){}
	ftuple& operator=(const ftuple&) = default;
    };
}

/*
    Intended usage:
    extend this class to generate something automatically at compile time
    
    static function extension may be like this:
    // F version:
    template<unsigned D>
    constexpr static void dosth1(){
        if constexpr (is_same_v<Type, float>) {
			// some global things like "vertex attrib pointer" on openGL...
		}
		else if constexpr (is_same_v<Type, int>) {
			// ...
		}
        ...
    }
    
    // F, T... version:
    template<unsigned D>
    constexpr static void dosth1() {
        firstType::dosth1<D>();
		lastType::dosth1<D+1>();
	}
    
    non static function extension may be like this:
    // F version:
	constexpr void dosth2(){
	    std::cout<<first;
	}
    
	// F, T... version:
	constexpr void dosth2(){
   	 firstType::dosth2();    // something overloaded or template
  	  lastType::dosth2();
	}
*/
