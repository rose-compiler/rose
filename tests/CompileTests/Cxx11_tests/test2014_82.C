#include <iostream>
 
template<class... Args>
std::size_t f()
   {
     return sizeof...(Args);
   }

int main()
   {
  // original code: std::cout << f<>() << '\n' << f<int>() << '\n' << f<char, int, double>() << '\n';
     std::cout << f<>() << '\n'
               << f<int>() << '\n'
               << f<char, int, double>() << '\n';
   }
