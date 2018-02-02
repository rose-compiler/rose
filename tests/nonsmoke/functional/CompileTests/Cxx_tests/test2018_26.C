namespace X
   {
   }

namespace Y
   {
     namespace Z = X;
   }

void foobar()
   {
     namespace A = ::Y::Z;
   }

