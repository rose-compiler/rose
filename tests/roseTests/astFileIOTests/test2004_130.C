
#include <string>

// Assignment using an explicit cast
std::string s = std::basic_string< char , std::char_traits< char >  , std::allocator< char >  > ("");

// Declaration without an initializer
std::string t;

// Assignment that can use or not use an explicit cast
std::string *p1 = NULL;
std::string *p2 = (std::string*) NULL;

// Generate an assignment that requires an explicit cast!
void* v1 = NULL;
std::string *p3 = (std::string*) v1;

// function declarations with and without const parameters
void foobar1( std::string  s );
void foobar2( std::string & s );
void foobar3( const std::string & s );
void foobar4( const std::string & s );

void foo()
   {
  // Initilization with explicit constructor initializer
     t = std::basic_string< char , std::char_traits< char >  , std::allocator< char >  > ("");

  // Initilization with non-explicit constructor initializer
     t = "";

     foobar1 ( std::basic_string< char , std::char_traits< char >  , std::allocator< char >  > ("") );
  // foobar2 ("");
  // foobar2 ( std::basic_string< char , std::char_traits< char >  , std::allocator< char >  > ("") );
     foobar3 ( std::basic_string< char , std::char_traits< char >  , std::allocator< char >  > ("") );
     foobar4 ("");
     foobar4 (std::string(""));
   }
