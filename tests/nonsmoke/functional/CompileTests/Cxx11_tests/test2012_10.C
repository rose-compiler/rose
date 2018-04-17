// Type inference
// In C++03 (and C), the type of a variable must be explicitly specified in order to use it. However, with the 
// advent of template types and template metaprogramming techniques, the type of something, particularly the 
// well-defined return value of a function, may not be easily expressed. Therefore, storing intermediates in 
// variables is difficult, possibly requiring knowledge of the internals of a particular metaprogramming library.

// C++11 allows this to be mitigated in two ways. First, the definition of a variable with an explicit initialization 
// can use the auto keyword. This creates a variable of the specific type of the initializer:

auto some_strange_callable_type = boost::bind(&some_function, _2, _1, some_object);
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

