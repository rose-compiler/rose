
extern const double DEFAULT_VALUE;
// const double DEFAULT_VALUE = 0.0;

template <const double & T1, const double & T2 = DEFAULT_VALUE>
class A
   {
     public:
          A();
   };

namespace B
   {
     extern const double SPECIAL_VALUE;
     typedef A<SPECIAL_VALUE> SpecialType;
   }

class C
   {
     public:
          B::SpecialType getSpecialValue();
   };

using namespace B;

const double B::SPECIAL_VALUE = 42.0;

SpecialType C::getSpecialValue()
   {
     return SpecialType();
   }

