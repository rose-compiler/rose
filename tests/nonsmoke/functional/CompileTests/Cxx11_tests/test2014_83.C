#include <iostream>
 
template<class... Args>
std::size_t f()
   {
     return sizeof...(Args);
   }

int main()
   {
  // original code: std::cout << f<>() << '\n';
  // generated code: (std::cout << ::f< /* varadic template argument */ ... > ())<<'\n';
     std::cout << f<>() << '\n';
   }
