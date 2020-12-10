
namespace N
   {
     struct X;
   }

namespace M
   {
     struct Y;
   }

// BUG: return type on SgPointerMemberType for function pointer is not name qualified.
// Original code: M::Y (N::X::* ptfptr) (int);
// Unparsed as:      Y (N::X::* ptfptr) (int );
M::Y (N::X::* ptfptr) (int);

