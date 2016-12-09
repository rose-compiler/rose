// This demonstrates a unparsing problem for the for loop from the macro.
// It also demonstrated the unpasing fo the base class specification that 
// we would like to also fix (name qualification).

class X_t
   {
     public:
          int start;
          int end;
   };

#define COPY_ME(A,B) \
     for (i=X->start; i<=X->end; i++) { \
          B[i] = A[i] ; \
        }

void foobar()
   {
     X_t *X = 0L;
     double *array_A;
     double *array_B;
     int i;

     COPY_ME(array_A,array_B) ;
   }

