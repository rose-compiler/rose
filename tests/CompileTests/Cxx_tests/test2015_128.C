// This demonstrates the output of excessive name qualification.

class X_t
   {
     public:
          int start;
   };

void foobar()
   {
     X_t *X = 0L;
     int j;

     j = X->start;

#if 0
     for (int i=0; i<=42; i++) 
        {
          j = X->start;
        }
#endif
   }

