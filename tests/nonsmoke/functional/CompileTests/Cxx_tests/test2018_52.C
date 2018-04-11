typedef struct Ctag
   {
     Ctag* next;
   } C;

// This is C++ specific code.
C array[1] = { (Ctag*) 0 };
