struct X {};

namespace M
   {
  // struct Y {};
     struct Y;
   }

// M::Y N::X::*pointer_to_data;
// M::Y X::*pointer_to_data;
// M::Y *pointer_to_data;
M::Y X::*pointer_to_data;

