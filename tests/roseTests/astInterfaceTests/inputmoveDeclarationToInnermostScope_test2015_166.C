// Token-based unparsing requires that the expression macro not contain a closing ";".
// #define MACRO_A() ( 42 + 42 );
#define MACRO_A() ( 42 + 42 )

void foobar()
   {
     int k,dx2;
     for (k=2; k<5; k++) 
        {
          dx2 = MACRO_A();
        }
   }


