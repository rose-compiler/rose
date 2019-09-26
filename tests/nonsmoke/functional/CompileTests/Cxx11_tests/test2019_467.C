template <const int T1>
class A
   {
     public:
          A();
   };

namespace B
   {
  // extern const double SPECIAL_VALUE;
     typedef A<42> SpecialType;
   }

using namespace B;

// const double B::SPECIAL_VALUE = 42.0;

SpecialType foobar()
   {
     return SpecialType();
   }

