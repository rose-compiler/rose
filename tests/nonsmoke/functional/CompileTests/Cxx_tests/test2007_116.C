
// double __builtin_nans   (const char *__builtin__x);
// double __builtin_nans   (const char *__builtin__x);

// void foo( int x) {};

// void foo();

// This declaration has no associated symbol
// void foo();

class A;

// This a pointer to a member function for a class that does not have a definition!
typedef void (A::*A_functionPtrType) (); 
// typedef int (A::*A_dataMemberPointerType);

void foo()
   {
     A_functionPtrType functionPtr;

  // A_dataMemberPointerType dataMemberPointer;

  // void (A::*A_functionPtr) (); 
   }
