template <const int & T1>
class A
   {
     public:
          A();
   };

namespace B
   {
     extern const int SPECIAL_VALUE;
     typedef A<SPECIAL_VALUE> SpecialType;
   }

// const int B::SPECIAL_VALUE = 42;

using namespace B;

const int B::SPECIAL_VALUE = 42;


// SpecialType foobar()
void foobar()
   {
     int x = SPECIAL_VALUE;
  // return SpecialType();
   }

