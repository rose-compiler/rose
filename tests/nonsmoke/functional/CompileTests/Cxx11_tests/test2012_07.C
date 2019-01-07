// Example from C++11 Wiki web page:

#include<vector>

typedef int MyClass;

// Extern template
// In C++03, the compiler must instantiate a template whenever a fully specified template 
// is encountered in a translation unit. If the template is instantiated with the same 
// types in many translation units, this can dramatically increase compile times. There 
// is no way to prevent this in C++03, so C++11 introduced extern template declarations, 
// analogous to extern data declarations.

// C++03 has this syntax to oblige the compiler to instantiate a template:

template class std::vector<MyClass>;

// C++11 now provides this syntax:
extern template class std::vector<MyClass>;

// which tells the compiler not to instantiate the template in this translation unit.
