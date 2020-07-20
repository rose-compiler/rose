#include <string>
#include <cstddef>
#include <concepts>
using namespace std::literals;
 
// Declaration of the concept "Hashable", which is satisfied by
// any type 'T' such that for values 'a' of type 'T',
// the expression std::hash<T>{}(a) compiles and its result is convertible to std::size_t
template<typename T>
concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};
 
struct meow {};
 
template<Hashable T>
void f(T); // constrained C++20 function template
 
// Alternative ways to apply the same constraint:
// template<typename T>
//    requires Hashable<T>
// void f(T); 
// 
// template<typename T>
// void f(T) requires Hashable<T>; 
 
int main() {
  f("abc"s); // OK, std::string satisfies Hashable
  f(meow{}); // Error: meow does not satisfy Hashable
}


