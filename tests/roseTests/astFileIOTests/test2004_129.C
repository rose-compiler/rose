
#include <string>

std::string* s = NULL;

void globalFunction( const std::string & s );

void foo()
   {
  // This function call contains a function reference expression which does not need a qualified name
     globalFunction("");
   }

