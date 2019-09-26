
namespace N
   {
     struct X {};
   }

namespace M
   {
     struct Y {};
   }

// Pointer to member function returning name qualified struct.
M::Y (N::X::* pointer_to_data) (int);

