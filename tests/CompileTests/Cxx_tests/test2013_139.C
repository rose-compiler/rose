class X
   {
     public:
         void foo( bool b);
   };

// DQ (4/27/2013): This is a bug if the default arguments are defined in the defining function outside the class.
void X::foo( bool b = true)
   {
   }

void foobar_B()
   {
     X x_object;

     x_object.foo();
   }
