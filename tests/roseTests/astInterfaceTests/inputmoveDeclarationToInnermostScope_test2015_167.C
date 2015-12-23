// Token-based unparsing requires that the expression macro not contain a closing ";".
// #define MACRO_A() ( 42 + 42 );
#define MACRO_A() ( 42 + 42 )

void foobar()
   {
     int kk,dx2;
     for (kk=22; kk<55; kk++) 
        {
          dx2 = MACRO_A();
        }
   }


