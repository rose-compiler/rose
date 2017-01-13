enum numbers 
   {
     four  = 4,
     seven = 7
   };

class X
   {
     public:
         void foo( int* a_ptr, bool b, numbers c);
      // void foo( int* a_ptr = 0L, bool b = true, numbers c = four);
      // void foobar( int* a_ptr = 0L, bool b = true, numbers c = four) { }
   };

// DQ (4/27/2013): This is a bug if the default arguments are defined in the defining function outside the class.
void X::foo( int* a_ptr= 0L, bool b = true, numbers c = four)
// void X::foo( int* a_ptr, bool b, numbers c)
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
  // x_object.foo(x_ptr,y);

  // Error, too few arguments in function call
  // x_object.foo(x_ptr);

  // Error, too few arguments in function call
     x_object.foo();
   }

