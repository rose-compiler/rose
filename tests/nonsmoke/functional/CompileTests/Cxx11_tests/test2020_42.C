
struct A
   {
     int foo(char i);
     int foo(int i);
   };

typedef int (A::*pointer_to_function_char)(char);

void foobar()
   {
     A a;

  // Original code::
  // (a.*pointer_to_function_char(&A::foo))(1);
  // Unparsed as:
  // (a .* &A::foo)(1);
     (a.*pointer_to_function_char(&A::foo))(1);
   }

