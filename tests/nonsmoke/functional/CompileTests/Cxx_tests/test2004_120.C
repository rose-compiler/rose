
#include <string>

// std::string a;
// string b = "Test B";

std::string foo()
   {
  // Currently unparses: "return basic_string(("test"));"
  // This could be because it is using the function name rather 
  // than the class name as the operator in the unparser!
     return "test";
   }


