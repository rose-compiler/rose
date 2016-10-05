#define COPYNODAL(A,B) \
  for (int local_i=0; local_i<=42; local_i++) { \
    B = A ; \
  }

int i;
int j;

void foobar()
   {
     COPYNODAL(i,j);
   }
