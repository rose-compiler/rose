
namespace N
   {
     struct X {};
   }

#if 0
namespace M
   {
     struct Y {};
   }
#endif

// M::Y N::X::*pointer_to_data;

// M::Y (N::X::* ptfptr) (int) = 0L; // &N::X::f;
// M::Y (N::X::* ptfptr) (int);

// int (N::X::* ptfptr) (int);
int (N::X::* ptfptr) (int);

