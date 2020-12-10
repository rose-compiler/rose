
// This will have three non-defining functions instead of just two (unclear if that is correct).

struct A
   {
     int foo(char i);
     int foo(int i);
   };

typedef int (A::*pointer_to_function_char)(char);
