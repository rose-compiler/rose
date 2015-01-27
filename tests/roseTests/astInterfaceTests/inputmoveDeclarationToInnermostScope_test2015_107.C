#define COPYNODAL(A,B) \
  for (int local_i=0; local_i<=42; local_i++) { \
    B = A ; \
  }

void foobar()
   {
     int i;
     int j;

     COPYNODAL(i,j);
   }
