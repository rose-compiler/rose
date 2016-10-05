// This test code demonstates the use of forward declarations
// it is designed so that an error in how the source position
// will cause a lagitimate erro in the compilation.

// This will build a SgClassDeclaration and a SgClassDefinition
// where the class definition is marked as compiler generated.
template <typename T> class Y;

// forward declaration of template specialization
#if defined(__clang__)
template<>
#endif
class Y<int>;

#include "test2007_48.h"

// Redundant forward class declaration
#if defined(__clang__)
template<>
#endif
class Y<int>;
#if defined(__clang__)
template<>
#endif
class Y<int>;

// This will build a new SgClassDeclaration, and reuse the existing
// SgClassDefinition (???).
// DQ (2/20/2010): This is a error for g++ 4.x compilers (at least g++ 4.2).
// #if (__GNUC__ >= 3)
// #if ( defined(__clang__) || (defined(__clang__) == 0 && __GNUC__ >= 3) )
// #if ( defined(__clang__) || (defined(__clang__) == 0 && __GNUC__ >= 3) )
#if ( defined(__clang__) || __GNUC__ == 4)
template<>
#endif
class Y<int> {};

// Redundant forward class declaration
#if defined(__clang__)
template<>
#endif
class Y<int>;
#if defined(__clang__)
template<>
#endif
class Y<int>;
