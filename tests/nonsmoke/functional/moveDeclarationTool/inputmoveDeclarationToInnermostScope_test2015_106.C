#define COPYNODAL(A,B) \
  for (i=0; i<=42; i++) { \
    B = A ; \
  }

void foobar()
   {
     int i;
     int j;
     COPYNODAL(i,j);
   }
