// Strongly typed enumerations
// In C++03, enumerations are not type-safe. They are effectively integers, even when the enumeration 
// types are distinct. This allows the comparison between two enum values of different enumeration types. 
// The only safety that C++03 provides is that an integer or a value of one enum type does not convert 
// implicitly to another enum type. Additionally, the underlying integral type is implementation-defined; 
// code that depends on the size of the enumeration is therefore non-portable. Lastly, enumeration values 
// are scoped to the enclosing scope. Thus, it is not possible for two separate enumerations to have 
// matching member names.

// C++11 allows a special classification of enumeration that has none of these issues. This is expressed 
// using the enum class (enum struct is also accepted as a synonym) declaration:

enum class Enumeration {
    Val1,
    Val2,
    Val3 = 100,
    Val4 // = 101
};

// This enumeration is type-safe. Enum class values are not implicitly converted to integers; therefore, 
// they cannot be compared to integers either (the expression Enumeration::Val4 == 101 gives a compiler error).

// The underlying type of enum classes is always known. The default type is int, this can be overridden to 
// a different integral type as can be seen in the following example:

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

