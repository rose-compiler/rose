
namespace std
   {
     class string {};
   }

// This template function will not use template arguments in any of its instantiations
// because the template arguments are implicitly represented in the type of the function 
// parameter.
template <class T> void linearIn(T& a_outputT);

// Implicit use of types for the template argument.
template <> void linearIn(std::string& a_outputT);

// This template function will require its template instantiations to explicitly specify
// the template argument since it can't be infered from any function parameter's type.
template <class T> void foo();

// Explicit use of types for the template argument.
template <> void foo<int>();
template <> void foo<float>();
