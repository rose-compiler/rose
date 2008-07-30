#ifndef COLDATA_H
#define COLDATA_H

#include <string>
#include <iostream>
#include <boost/lexical_cast.hpp>

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

class ColData
{
 private:
   std::string value;
 public:
   ColData (std::string val) { value = val; }
   std::string get_string() { return value; }
   
   operator int () { return boost::lexical_cast<int>(value); }
   operator long () { return boost::lexical_cast<long>(value); }
   operator float () { return boost::lexical_cast<float>(value); }
   operator double () { return boost::lexical_cast<double>(value); }
   friend std::ostream& operator<< (std::ostream& os, const ColData& cd) {
     return os << cd.value;
   }
   operator std::string () { return value; }
};

#endif
