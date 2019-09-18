namespace N
   {
     struct X {};
   }

namespace M
   {
     extern int N::X::*pointer_to_data;
   }

// Unclear how we can refer to the pointer_to_data in namespace M.
// int M::pointer_to_data = 0L;
