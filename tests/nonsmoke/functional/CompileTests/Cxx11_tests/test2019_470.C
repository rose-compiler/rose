namespace B
   {
     extern const int SPECIAL_VALUE;
   }

using namespace B;

// Bug (generated code): const int SPECIAL_VALUE = 42;
// Original code:        const int B::SPECIAL_VALUE = 42;
const int B::SPECIAL_VALUE = 42;

