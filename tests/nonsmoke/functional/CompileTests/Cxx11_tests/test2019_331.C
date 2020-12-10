
namespace N 
   {
     struct DEF {};
   }

typedef int (N::DEF::*callback_func_ptr)();

void foo(callback_func_ptr)
   {
   }

void foobar( int (N::DEF::*pointer_to_object) )
   {
   }
