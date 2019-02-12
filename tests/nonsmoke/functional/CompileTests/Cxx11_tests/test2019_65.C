
int foo_wrapper(int);

struct B {};

typedef const int B::*pointer_to_member_integer_type;

template<pointer_to_member_integer_type Pointer>
int foo();

void foobar()
   {
     pointer_to_member_integer_type pointer_to_member = nullptr;

     foo_wrapper((foo<nullptr> ()));
   }
