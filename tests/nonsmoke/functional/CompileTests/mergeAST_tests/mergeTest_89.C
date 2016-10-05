class X
   {
     void foo();
   };

typedef void (X::*member_function_pointer)(void);
member_function_pointer y;


