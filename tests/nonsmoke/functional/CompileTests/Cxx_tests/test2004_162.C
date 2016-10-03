

void foo()
   {
     int i;

  // DQ (4/12/2005): g++ version 3.4 bug!
  // i.int::~int();
  // Note that "i.int::~int();" generates "(&i) -> int::~int();" which will 
  // compile with the g++ 3.3.2 compiler but not with g++ version 3.4.3.
  // (&i) -> int::~int();

#ifdef __GNUC_PREREQ
#if (__GNUC_PREREQ(3,4) == 0)
  // Works for g++ 3.3.2 compiler but not g++ 3.4.3
     i.int::~int();
#endif
#endif

     i = 1;

     int *iptr;

  // DQ (4/12/2005): g++ version 3.4 bug!
  // iptr->int::~int();
#ifdef __GNUC_PREREQ
#if (__GNUC_PREREQ(3,4) == 0)
  // Works for g++ 3.3.2 compiler but not g++ 3.4.3
     iptr->int::~int();
#endif
#endif

     iptr = &i;

     typedef int* intPointer;

  // Unparsed as: "(&iptr) -> ::intPointer::~intPointer();"
     iptr.intPointer::~intPointer();
   }
