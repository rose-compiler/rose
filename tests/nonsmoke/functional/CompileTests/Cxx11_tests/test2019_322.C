namespace N
   {
     struct X {};
   }

namespace M
   {
     struct Y {};
   }

M::Y N::X::*pointer_to_data;

