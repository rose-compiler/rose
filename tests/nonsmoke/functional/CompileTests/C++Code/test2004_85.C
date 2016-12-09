
#include <string>

// This is currently required for ROSE to compile this file because typedefed names are not qualified properly!
// using namespace std;

// This does not work!
// std::string x;

// When unparsed to this, g++ will compile this properly
// std::basic_string< char , std::char_traits< char >  , std::allocator< char >  > x;
// but ROSE unparses the code to:
// std::basic_string< char , struct char_traits< char >  , class allocator< char >  > x;
// Which does not compile!
// ROSE needs to unparse qualified names in unparsing template arguments!
std::basic_string < char, std::char_traits<char>, std::allocator<char> > x;


// This unparses to just:
// string y; 
// which lacks the namespace qualifier, and so it does not compile
// ROSE needs to use qualified names in unparsing typedefs!
std::string y;
