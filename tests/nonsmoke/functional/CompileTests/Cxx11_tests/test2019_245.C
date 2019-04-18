
// New C++11 form of function declaration.
// Should be: auto c_( char c() ) -> decltype(c);
// Unparse as: char (*c_(char c());
auto c_( char c() ) -> decltype(c);


