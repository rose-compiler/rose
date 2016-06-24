// is_trivial example
#include <iostream>

#if __cplusplus >= 201103L
#include <type_traits>

class A {};
class B { B() {} };
class C : B {};
class D { virtual void fn() {} };

int main() {
  std::cout << std::boolalpha;
  std::cout << "is_trivial:" << std::endl;
  std::cout << "int: " << std::is_trivial<int>::value << std::endl;
  std::cout << "A: " << std::is_trivial<A>::value << std::endl;
  std::cout << "B: " << std::is_trivial<B>::value << std::endl;
  std::cout << "C: " << std::is_trivial<C>::value << std::endl;
  std::cout << "D: " << std::is_trivial<D>::value << std::endl;
  return 0;
}
#endif

