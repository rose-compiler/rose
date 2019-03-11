enum class Enum2 : unsigned int {Val1, Val2};

// The scoping of the enumeration is also defined as the enumeration name's scope. Using the enumerator names 
// requires explicitly scoping. Val1 is undefined, but Enum2::Val1 is defined.

// Additionally, C++11 will allow old-style enumerations to provide explicit scoping as well as the definition 
// of the underlying type:

enum Enum3 : unsigned long {Val1 = 1, Val2};

// The enumerator names are defined in the enumeration's scope (Enum3::Val1), but for backwards compatibility, 
// enumerator names are also placed in the enclosing scope.

// Forward-declaring enums is also possible in C++11. Previously, enum types could not be forward-declared 
// because the size of the enumeration depends on the definition of its members. As long as the size of the 
// enumeration is specified either implicitly or explicitly, it can be forward-declared:

// enum Enum1;                      // Illegal in C++03 and C++11; the underlying type cannot be determined.
enum Enum4 : unsigned int;       // Legal in C++11, the underlying type is explicitly specified.

enum class Enum5;                // Legal in C++11, the underlying type is int.
enum class Enum6 : unsigned int; // Legal in C++11.
// enum Enum2 : unsigned short;     // Illegal in C++11, because Enum2 was previously declared with a different underlying type.

