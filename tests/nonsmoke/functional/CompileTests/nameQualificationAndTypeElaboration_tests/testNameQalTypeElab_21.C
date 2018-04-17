// number #21

namespace X
   {
     class X1{};
   }

using X::X1;

// This is suposed to be a C++ error
class X1 {};

::X1 a;
