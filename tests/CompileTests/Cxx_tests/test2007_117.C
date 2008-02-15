class A;

// This a pointer to a data member for a class that does not have a definition!
typedef int (A::*A_dataMemberPointerType);

void foo()
   {
     A_dataMemberPointerType dataMemberPointer;

     A* a;
  // a->dataMemberPointer = 0;
   }
