// Alias templates

// In C++03, it is possible to define a typedef only as a synonym for another type, including a synonym 
// for a template specialization with all actual template arguments specified. It is not possible to 
// create a typedef template. For example:

class OtherType;

template <typename First, typename Second, int Third>
class SomeType;
 
// template <typename Second>
// typedef SomeType<OtherType, Second, 5> TypedefName; //Illegal in C++03

// This will not compile.

// C++11 adds this ability with the following syntax:

template <typename First, typename Second, int Third>
class SomeType2;

template <typename Second>
using TypedefName = SomeType2<OtherType, Second, 5>;

// The using syntax can be also used as type aliasing in C++11:

typedef void (*Type)(double);           // Old style
using OtherType2 = void (*)(double);             // New introduced syntax

