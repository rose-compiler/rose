
#define MACRO_B() \
        dx1 = 42; \
        va += 1;

void foobar()
   {
     int k,i,dx1,va;
     for (k=2; k<12; k++) 
        {
          va = 0;
          MACRO_B();
        }
   }


