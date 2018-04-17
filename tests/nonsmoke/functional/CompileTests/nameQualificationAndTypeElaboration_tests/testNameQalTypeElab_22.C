// number #22

namespace X
   {
     class X1;
   }

using namespace X;

class X1 {};

// This fails to unparse with "X::" qualified name!
class X::X1 {};

// This fails to generate the name qualification because the hidden list does not
// record the global scope X1 ("::X1") as hiding the namespace X class X1 ("X::X1").
// ::X1 a;

X::X1 b;
