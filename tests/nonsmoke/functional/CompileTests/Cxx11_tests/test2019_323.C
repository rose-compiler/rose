
namespace N
   {
     struct X {};
   }

namespace M
   {
     struct Y {};
   }

M::Y N::X::*pointer_to_data;

// M::Y (N::X::* ptfptr) (int) = 0L; // &N::X::f;
M::Y (N::X::* ptfptr) (int);

