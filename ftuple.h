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
#include <cstddef>
#include <vector>

namespace onart {

    // something useful with the class below
    template <class, class...>
    inline constexpr bool is_one_of = false;

    template <class A, class B, class... T>
    inline constexpr bool is_one_of<A, B, T...> = std::is_same_v<A, B> || is_one_of<A, T...>;
    // something useful with the class below

    template <class, class...>
    inline constexpr size_t count_of = 0;

    template <class A, class B, class... T>
    inline constexpr size_t count_of <A, B, T...> = (int)std::is_same_v<A, B> + count_of<A, T...>;

    template<class... T>
    struct ftuple;

    template<class F, size_t i = 0>
    struct ftupleBase {
    public:
        F first;
        ftupleBase() = default;
        ftupleBase(const ftupleBase&) = default;
        ftupleBase& operator=(const ftupleBase&) = default;
    protected:
        using Type = F;
        using firstType = ftupleBase<F, i>;
    };

    template<class F>
    struct ftuple<F> {
    public:
        F first;
        template<unsigned POS>
        constexpr auto& get() {
            static_assert(POS == 0, "Index exceeded");
            return first;
        }

        template<class C>
        constexpr C* get() {
            if constexpr (std::is_same_v<C, F>) { return &first; }
            else { return nullptr; }
        }

        template<class C>
        constexpr std::vector<C*> gets() {
            if constexpr (std::is_same_v<C, F>) { return { &first }; }
            else { return {}; }
        }

        template<unsigned POS>
        constexpr static size_t offset() {
            static_assert(POS == 0, "Index exceeded");
            return 0;
        }
        ftuple() = default;
        ftuple(const ftuple&) = default;
        ftuple(const F& f) : first(f) {}
        ftuple& operator=(const ftuple&) = default;
    protected:
        using Type = F;
        using firstType = ftuple<F>;
        using lastType = void;
    };

    template<class F, class... T>
    struct ftuple<F, T...> : ftupleBase<F, sizeof...(T)>, ftuple<T...> {
    protected:
        using Type = F;
        using firstType = ftupleBase<F, sizeof...(T)>;
        using lastType = ftuple<T...>;
    private:
        template<unsigned POS, class FT>
        constexpr auto& get() {
            if constexpr (POS == 0) return FT::firstType::first;
            else return get<POS - 1, typename FT::lastType>();
        }

        template <class FT, class C>
        constexpr C* get() {
            if constexpr (std::is_same_v<typename FT::Type, C>) { return &(FT::firstType::first); }
            else { return get<typename FT::lastType, C>(); }
        }

        template <class FT, class C>
        constexpr void gets(std::vector<C*>& ret) {
            if constexpr (std::is_same_v<typename FT::Type, C>) { ret.push_back(&(FT::firstType::first)); }
            if constexpr (!std::is_same_v<typename FT::lastType, void>) gets<typename FT::lastType, C>(ret);
        }

        template <unsigned POS, class FT>
        constexpr static size_t offset() {
            using thisType = ftuple<F, T...>;
            if constexpr (POS == 0) return (::size_t)reinterpret_cast<char const volatile*>(&(((thisType*)0)->FT::firstType::first));   // warning: this one can't be calculated in compile time
            else return offset<POS - 1, typename FT::lastType>();
        }
    public:
        template<unsigned POS>
        constexpr auto& get() {
            static_assert(POS <= sizeof...(T), "Index exceeded");
            return get<POS, ftuple<F, T...>>();
        }

        template <class C>
        constexpr C* get() {
            if constexpr (!is_one_of<C, F, T...>) { return nullptr; }
            else return get<ftuple<F, T...>, C>();
        }

        template <class C>
        constexpr std::vector<C*> gets() {
            if constexpr (0 == count_of<C, F, T...>) { return {}; }
            else {
                std::vector<C*> ret;
                ret.reserve(count_of<C, F, T...>);
                gets<ftuple<F, T...>, C>(ret);
                return ret;
            }
        }

        template<unsigned POS>
        constexpr static size_t offset() {
            static_assert(POS <= sizeof...(T), "Index exceeded");
            return offset<POS, ftuple<F, T...>>();
        }

        ftuple() = default;
        ftuple(const ftuple&) = default;
        ftuple(const F& f, const T&... t) : firstType{ f }, lastType(t...) {}
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
