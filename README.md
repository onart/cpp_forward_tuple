# cpp_forward_tuple
Reverse version of std::tuple, which is already reversed\
ftuple is a class like std::tuple, but its order in memory is forward\
C++17 or later required
* ISSUE: CURRENTLY CAN'T USE WITH OVERLAPPING TYPE e.g. `ftuple<int, float, int>`

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
This is available(tested) on MSVC, Clang, and GCC

Intended usage: extend this class to generate some code automatically at compile time
```C++
// F version:
constexpr void dosth2(){
    std::cout<<first;
}
    
// F, T... version:
constexpr void dosth2(){
    firstType::dosth2();    // something overloaded or template
    lastType::dosth2();
}
```
