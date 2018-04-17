
#include <string>

namespace std
   {
     int out_of_range( const string & s );
   }

int globalFunction();

// Use this function as a place to call functions which require qualified and unqualified names
void foo()
   {
  // Example from Kull
  // Function calls are not unparsed with there qualifier, so we generate 
  // "out_of_range" instead of "std::out_of_range" as a function name.
     throw std::out_of_range("index out of range");

  // Function calls are not unparsed with there qualifier, so we generate 
  // "out_of_range" instead of "std::out_of_range" as a function name.
     std::out_of_range("index out of range");

  // This function call contains a function reference expression which does not need a qualified name
     globalFunction();
   }


