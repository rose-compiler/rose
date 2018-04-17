// This code demonstrates 2 different bugs.

void globalfoo () {};

typedef void (*GlobalPointerToFunctionType)();

class A
   {
     public:
          A(GlobalPointerToFunctionType fptr ) { globalFunctionPointer = fptr; };
          void foo () {};
          GlobalPointerToFunctionType globalFunctionPointer;
   };

typedef void (A::*PointerToMemberFunctionType)();

int
main ()
   {
#if 0
  // bug 1
     A* a = new A(globalfoo);

     (*(a->globalFunctionPointer))();
#endif

#if 0
  // bug 2
     A* a = new A();
     PointerToMemberFunctionType memberFunctionPointer = &A::foo;
     (a->*memberFunctionPointer)();
#endif

     return 0;
   }

