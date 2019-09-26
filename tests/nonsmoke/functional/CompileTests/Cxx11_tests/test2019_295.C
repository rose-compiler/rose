namespace M 
   {
     struct X {};
   }

namespace N 
   {
     struct X 
        {
          int h(M::X);
        };
   }

// Original code: int (N::X::*pmf_)(M::X) = &N::X::h;
// Unparsed code: int (::N::X::*pmf_)(struct X ) = &N::X::h;
int (N::X::*pmf_)(M::X) = &N::X::h;

