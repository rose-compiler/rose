// This test is placed into test2003_08.C with the class defined in test2003_08.h

int x;

#if 0
typedef struct Ctag
   {
     struct Ctag* next;
   } C;

// This fails
Ctag array[1];
// This works
C array[1];
#endif

#if 1
typedef struct Ctag
   {
     struct Ctag* next;
   } C;

// The initializer fails to unparse correctly in this case
C array[1] = { (Ctag*) 0 };
#endif

#if 0
typedef struct Ctag
   {
     struct Ctag* next;
   } C;

Ctag array[1] = { { (Ctag*)0 } };

#endif












