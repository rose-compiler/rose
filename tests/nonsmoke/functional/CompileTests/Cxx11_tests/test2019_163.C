class OtherType;

template <typename First, typename Second, int Third>
class SomeType;
 
template <typename First, typename Second, int Third>
class SomeType2;

template <typename Second>
using TypedefName = SomeType2<OtherType, Second, 5>;

