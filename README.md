# cpp_forward_tuple
Reverse version of std::tuple, which is already reversed
ftuple is a class like std::tuple, but its order in memory is forward\

example:\
type `ftuple<int, float, double, char>` is like:
```C++
struct{
    int _1;
    float _2;
    double _3;
    char _4;
};
```
while `std::tuple<int, float, double, char>` is like:
```C++
struct{
    char _4;
    double _3;
    float _2;
    int _1;
};
```
This is available on MSVC, Clang / Not tested on GCC yet

Intended usage: extend this class to generate some code automatically at compile time
```C++
// F version:
template<class FT>
constexpr void dosth2(){
    std::cout<<first;
}
    
// F, T... version:
template<class FT>
constexpr void dosth2(){
    dosth2<FT::firstType>();    // something overloaded or template
    dosth2<FT::lastType>();
}
```
