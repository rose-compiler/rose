// Type inference
// In C++03 (and C), the type of a variable must be explicitly specified in order to use it. However, with the 
// advent of template types and template metaprogramming techniques, the type of something, particularly the 
// well-defined return value of a function, may not be easily expressed. Therefore, storing intermediates in 
// variables is difficult, possibly requiring knowledge of the internals of a particular metaprogramming library.

#if 0
// DQ (1/2/2019): This is an older version of the test code that was incomplete.
#include<vector>
#include<functional>
// #include<boost>


// C++11 allows this to be mitigated in two ways. First, the definition of a variable with an explicit initialization 
// can use the auto keyword. This creates a variable of the specific type of the initializer:

void some_function(int i, int j);
class some_object{};

// auto some_strange_callable_type = boost::bind(&some_function, _2, _1, some_object);
auto some_strange_callable_type = std::bind(&some_function, _2, _1, some_object);
auto other_variable = 5;

// The type of some_strange_callable_type is simply whatever the particular template function override of boost::bind 
// returns for those particular arguments. This type is easily determined procedurally by the compiler as part of its 
// semantic analysis duties, but is not easy for the user to determine upon inspection.

// The type of other_variable is also well-defined, but it is easier for the user to determine. It is an int, which 
// is the same type as the integer literal.

// Additionally, the keyword decltype can be used to determine the type of an expression at compile-time. For example:

int some_int;
decltype(some_int) other_integer_variable = 5;

// This is more useful in conjunction with auto, since the type of an auto variable is known only to the compiler. 
// However, decltype can also be very useful for expressions in code that makes heavy use of operator overloading and specialized types.

// auto is also useful for reducing the verbosity of the code. For instance, instead of writing

for (std::vector<int>::const_iterator itr = myvec.cbegin(); itr != myvec.cend(); ++itr)

// the programmer can use the shorter

for (auto itr = myvec.cbegin(); itr != myvec.cend(); ++itr)

// This difference grows as the programmer begins to nest containers, though in such cases typedefs are a good way to decrease the amount of code.

// The type denoted by decltype can be different from the type deduced by auto.

#include <vector>
int main()
{
    const std::vector<int> v(1);
    auto a = v[0];        // a has type int
    decltype(v[0]) b = 1; // b has type const int&, the return type of
                        // std::vector<int>::operator[](size_type) const
    auto c = 0;           // c has type int
    auto d = c;           // d has type int
    decltype(c) e;        // e has type int, the type of the entity named by c
    decltype((c)) f = c;  // f has type int&, because (c) is an lvalue
    decltype(0) g;        // g has type int, because 0 is an rvalue
}
#endif


// DQ (1/2/2019): This is a later version that demonstrates teh same features, but which is more complete.
#include <random>
#include <iostream>
#include <memory>
#include <functional>

void f(int n1, int n2, int n3, const int& n4, int n5)
{
    std::cout << n1 << ' ' << n2 << ' ' << n3 << ' ' << n4 << ' ' << n5 << '\n';
}
 
int g(int n1)
{
    return n1;
}
 
struct Foo {
    void print_sum(int n1, int n2)
    {
        std::cout << n1+n2 << '\n';
    }
    int data = 10;
};
 
int main()
{
    using namespace std::placeholders;  // for _1, _2, _3...
 
    // demonstrates argument reordering and pass-by-reference
    int n = 7;
    // (_1 and _2 are from std::placeholders, and represent future
    // arguments that will be passed to f1)
    auto f1 = std::bind(f, _2, _1, 42, std::cref(n), n);
    n = 10;
    f1(1, 2, 1001); // 1 is bound by _1, 2 is bound by _2, 1001 is unused
                    // makes a call to f(2, 1, 42, n, 7)
 
    // nested bind subexpressions share the placeholders
    auto f2 = std::bind(f, _3, std::bind(g, _3), _3, 4, 5);
    f2(10, 11, 12); // makes a call to f(12, g(12), 12, 4, 5);
 
    // common use case: binding a RNG with a distribution
    std::default_random_engine e;
    std::uniform_int_distribution<> d(0, 10);
    auto rnd = std::bind(d, e); // a copy of e is stored in rnd
    for(int n=0; n<10; ++n)
        std::cout << rnd() << ' ';
    std::cout << '\n';
 
    // bind to a pointer to member function
    Foo foo;
    auto f3 = std::bind(&Foo::print_sum, &foo, 95, _1);
    f3(5);
 
    // bind to a pointer to data member
    auto f4 = std::bind(&Foo::data, _1);
    std::cout << f4(foo) << '\n';
 
    // smart pointers can be used to call members of the referenced objects, too
#if 0
 // DQ (1/2/2019): This is C++14 specific code.
    std::cout << f4(std::make_shared<Foo>(foo)) << '\n'
              << f4(std::make_unique<Foo>(foo)) << '\n';
#else
    std::cout << f4(std::make_shared<Foo>(foo)) << '\n';
#endif
}
