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

### Polymorphism
We can't make a virtual template member function in C++, so alternatively we can use a macro defining the default operation for getting some type. (No need to use `get<N>` in virtual function, of course. There's no usage for this in dynamic polymorphism context!)

* Example code

```C++
class Entity {
public:
    POSSIBLE_COMPONENT(int) // Extended to virtual int * get_int() { return nullptr; } virtual std::vector<int*> gets_int() { return {}; }
    POSSIBLE_COMPONENT(float)
    virtual void dosth2() {}
};

template <class... T>
class EntityComponent : public Entity {
public:
    TEMPLATE_VIRTUAL(int) // Extended to int * get_int() { return get<int>(); } std::vector<int*> gets_int() { return gets<int>(); }
    TEMPLATE_VIRTUAL(float)
    inline EntityComponent(const T&... t) :components(t...) {}
    template <class T>
    inline T* get() { return components.get<T>(); }
    template <class T>
    inline std::vector<T*> gets() { return components.gets<T>(); }
    void dosth2(){ components.dosth2(); }
private:
    onart::ftuple<T...> components;
};

int main() {
    EntityComponent<int, float, float, int> body(1, 2.0f, 3.0f, 4);
    EntityComponent<float, int, int, int> body2(6.0f, 1, 3, 4);
    Entity* center = &body;
    for (float* f : center->gets_float()) {
        printf("%f\n",*f);
    }
    center = &body2;
    for (int* i : center->gets_int()) {
        printf("%d\n",*i);
    }
}
```

We can implement the similar operation with STL vector and dynamic polymorphism like: `std::vector<Component*> components`. This provides more flexibility since we can add or remove components in runtime. But this one is a little bit harder to achieve spatial locality than `ftuple`, and traversal like `dosth2` or getting the component should use RTTI for each components.\
`ftuple` provides liberal condition for component types thanks to SFINAE. Branch prediction in vtable is likely to be harder in `ftuple`, but RTTI's used less, so it depends.
