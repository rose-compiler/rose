
void foobar()
   {
     struct X
        {
          void *operator new(unsigned long n, float f);
        };

  // This call uses the new operator to allocate memory and initialize that memory to the value 4.2
     new (4.2) X;
   }


