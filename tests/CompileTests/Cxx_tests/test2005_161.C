// This test code show how "string::npos" is output as a constant value instead of
// as a variable name.  In addition to this problem, the value output is
// the evaluation of "-1" cast to an "unsigned int" (size_t, which I think is 
// "unsigned int").

#include "string"

using namespace std;

void foo()
   {
  // Code as it appears in hdrs1/bits/basic_string.h
  // static const size_type local_npos = static_cast<size_type>(-1);

  // This is unparsed as:
  // static const size_t local_npos = 4294967295;
     static const size_t local_npos = static_cast<size_t>(-1);

     string s = "abcdefg";
     bool found =  s.find("debug") != string::npos;
   }

