// This is a subset of test2004_162.C

void foo()
   {
     int *iptr;

     typedef int* intPointer;

  // Unparsed as: "(&iptr) -> ::intPointer::~intPointer();"
     iptr.intPointer::~intPointer();
  // (&iptr)->intPointer::~intPointer();
   }
