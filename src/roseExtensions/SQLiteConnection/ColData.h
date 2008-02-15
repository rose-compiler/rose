#ifndef COLDATA_H
#define COLDATA_H

#include <string>
#include <iostream>

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

class ColData
{
 private:
   string value;
 public:
   ColData (string val) { value = val; }
   string get_string() { return value; }
   
   operator int () { return atoi(value.c_str()); }
   operator long () { return atol(value.c_str()); }
   operator float () { return (float)atof(value.c_str()); }
   operator double () { return atof(value.c_str()); }
   friend ostream& operator<< (ostream& os, const ColData& cd) {
     return os << cd.value;
   }
   operator string () { return value; }
};

#endif
