
#define MACRO_B() \
        dx1 = 42; \
        for (int i=0; i<42; i++) { int x; } \
        va += 1;

void foobar()
   {
     int k,i,dx1,va;
     for (k=2; k<12; k++) 
        {
          va = 0;
          MACRO_B();
        }

  // MACRO_B();
   }


