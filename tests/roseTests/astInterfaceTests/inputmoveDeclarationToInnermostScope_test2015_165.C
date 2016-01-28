#define MACRO_A() ( 42 + 42 );

#define MACRO_B() \
        dx1 = 42; \
        va += 1;

void foobar()
   {
     int k,i,dx1,dx2,va;
     for (k=2; k<12; k++) 
        {
          va = 0;
          dx2 = 42.0 * MACRO_A();
          MACRO_B();
        }
   }


