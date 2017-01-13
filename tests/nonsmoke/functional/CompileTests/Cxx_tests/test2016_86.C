void foo()
   {
     int i;

     i = 1;

     int *iptr;

     iptr = &i;

     typedef int* intPointer;

  // DQ (10/16/2016): Failes in EDG 4.11 (wrong number of operands).
     iptr.intPointer::~intPointer();
   }
