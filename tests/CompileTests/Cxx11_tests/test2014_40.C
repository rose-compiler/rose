template <typename First, typename Second, int Third>
class SomeType {};

class OtherType {};

// Example of C++11 support for template typedefs.
// This is not unparsed, but should be unparsed as:
// template <typename Second> using TypedefName = SomeType<OtherType, Second, 5>;
template <typename Second>
using TypedefName = SomeType<OtherType, Second, 5>;

void foo(TypedefName<int> x )
   {
   }

#if 0
// This is not allowed becasue it is a redefinition.
void foo(SomeType<OtherType, int, 5> x )
   {
   }
#endif
