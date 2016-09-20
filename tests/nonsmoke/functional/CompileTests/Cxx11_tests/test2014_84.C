// #include <iostream>

typedef unsigned long size_t;

template<class... Args>
size_t f()
   {
     return sizeof...(Args);
   }

size_t foo()
   {
  // original code: std::cout << f<>() << '\n';
  // generated code: (std::cout << ::f< /* varadic template argument */ ... > ())<<'\n';
     return f<>();
   }
