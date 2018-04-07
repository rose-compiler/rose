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
