# cpp_forward_tuple
Reverse version of std::tuple, which is already reversed\
ftuple is a class like std::tuple, but its order in memory is forward\
C++17 or later required

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
(Actually the offsets are different from the pure `struct`; the struct is 24 bytes long / std::tuple and ftuple are 32 bytes long for this example in MSVC)

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

### Provided public functions
* `get<N>(void)`: the reference of the Nth component. (N: 0-base)
* `get<T>(void)`: the pointer to the first member among the type T components. If there is no T component, it returns `nullptr`
* `gets<T>(void)`: std::vector<T*> containing all of type T component. Of course the life span of this vector is equal to the tuple.
* `offset<N>(void)`: this is a static function; the offset of the Nth component. this works like the `offsetof` macro. (N: 0-base)

** `get<T>`, `gets<T>` are intended to be used with polymorphism
```C++
onart::ftuple<int, float, double, char> obj(1, 2.0f, 3.0, '4'); // {1, 2.0f, 3.0, '4'} also possible
obj.get<3>(); // '4'. also possible: obj.get<3>() = 'a';
obj.get<float>(); // ===> 3.0
obj.get<bool>(); // nullptr
obj.gets<int>(); // {===> 1}
obj.offset<3>(); // 24
sizeof(obj) // 32
```
