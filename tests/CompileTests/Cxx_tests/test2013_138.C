enum numbers 
   {
     zero = 0,
     one = 1,
     seven = 7
   };

class X
   {
     public:
         void foo( int* a_ptr, bool b, numbers c);
   };

void foobar_A()
   {
     int* x_ptr;
     bool y;
     numbers big_number = seven;

     X x_object;

     x_object.foo(x_ptr,y,big_number);

  // Error, too few arguments in function call
  // x_object.foo(x_ptr,y);

  // Error, too few arguments in function call
  // x_object.foo(x_ptr);

  // Error, too few arguments in function call
  // x_object.foo();
   }

   
void
X::foo( int* a_ptr= 0L, bool b = true, numbers c = one)
   {
   }



void foobar_B()
   {
     int* x_ptr;
     bool y;
     numbers big_number = seven;

     X x_object;

     x_object.foo(x_ptr,y,big_number);

  // Error, too few arguments in function call
     x_object.foo(x_ptr,y);

  // Error, too few arguments in function call
     x_object.foo(x_ptr);

  // Error, too few arguments in function call
     x_object.foo();
   }
