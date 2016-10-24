
int offset;

#if 0
void foobar()
   {
     shared int *foo_A;
     (*foo_A);

     int* foo_B;
     (*((int*)(((char*)foo_B)+offset)));

  // The private keyword is not defined for UPC.
  // ((private int*) foo_C)
  // ((int*)(((char*)foo_C)+offset));

     shared int* shared* bar_A;
     int**bar_B;

     (**bar_B);
     (*(int*)(((char*)(*((int**)(((char*)bar_B)+offset))))+offset));
   }
#endif

#if 1
void foobar_A()
   {
  // Expected translation: shared int *foo_A; --> int *foo_A;
     shared int *foo_A;

  // Example of what generated code should be (using different variable name).
     int* foo_B;
   }
#endif

#if 1
void foobar_B()
   {
  // Required declaration to support example expression.
  // int* foo_A;
     shared int *foo_A;

  // Expected translation: (*foo_A); --> (*((int*)(((char*)foo_A)+offset)));
  // This will be handled using a parameterized transformation (parameterized on the base type of foo_A).
     (*foo_A);

  // Example of what generated code should be.
  // (*((int*)(((char*)foo_A)+offset)));
  // (*((int*)(((int*)foo_A)+offset)));
  // (*((int*)(foo_A+offset)));
  // *(foo_A+offset);

  // Example of what generated code should be.
  // Note that the casts to char* are require to allow the increment to be in bytes.
     (*((int*)(((char*)foo_A)+offset)));
   }
#endif

#if 0
void foobar_C()
   {
  // Expected translation: shared int* shared* bar_A; --> int** bar_A;
     shared int* shared* bar_A;

  // Example of what generated code should be (using different variable name).
     int** bar_B;
   }
#endif

#if 0
void foobar_D()
   {
  // Required declaration to support example expression.
     int** bar_D;

  // Expected translation: (**bar_D); --> (*(int*)(((char*)(*((int**)(((char*)bar_D)+offset))))+offset));
     (**bar_D);

  // Example of what generated code should be.
     (*(int*)(((char*)(*((int**)(((char*)bar_D)+offset))))+offset));
   }
#endif

