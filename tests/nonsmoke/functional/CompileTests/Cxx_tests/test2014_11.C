#include <string>

// Declaration without an initializer
std::string t;

void foo()
   {
  // Initilization with explicit constructor initializer
     t = std::basic_string< char , std::char_traits< char >  , std::allocator< char >  > ("");
   }

