// Exploring PIVOT
// PIVOT specifies the IR so that a statement is derived from an expression ???
// Their motivation is to handle the following, how do we handle this?
//      if (std::cin >> c) { }

// This code works for g++, but does not work for EDG!

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include <iostream>

// #define USING_GNU true
#define USING_GNU false

#if USING_GNU
// This code works fine for gnu g++, but fails to compile with EDG
int main()
   {
     std::string c;
     if (std::cin >> c) { /* .... */ }
     return 0;
   }
#else

#include <string>

int main()
   {
     std::string c;
//   if (std::cin >> c) { /* .... */ }
     return 0;
   }
#endif

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

