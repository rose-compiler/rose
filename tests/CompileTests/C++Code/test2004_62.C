// Exploring PIVOT
// PIVOT specifies the IR so that a statement is derived from an expression ???
// Their motivation is to handle the follwoing, how do we handle this?
//      if (std::cin >> c) { }

// This code works for g++, but does not work for EDG!

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
     string c;
//   if (std::cin >> c) { /* .... */ }
     return 0;
   }
#endif
