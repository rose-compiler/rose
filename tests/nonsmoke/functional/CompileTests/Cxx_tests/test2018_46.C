#if 0
// DQ (4/7/2018): This is a C++11 test, moved to the Cxx11_tests directory.

#include <iostream>
#include <vector>
#include <algorithm>
 
struct Printer 
   { // generic functor
     std::ostream& os;
     Printer(std::ostream& os) : os(os) {}
     template<typename T>
     void operator()(const T& obj) { os << obj << ' '; } // member template
   };

void foobar()
   {
     std::vector<int> v = {1,2,3};
     std::for_each(v.begin(), v.end(), Printer(std::cout));
     std::string s = "abc";
     std::for_each(s.begin(), s.end(), Printer(std::cout));
   }
#endif
