// This code demonstrates 2 different bugs.

// void globalfoo () {};

// typedef void (*GlobalPointerToFunctionType)();

class A
   {
     public:
//        A(GlobalPointerToFunctionType fptr ) { globalFunctionPointer = fptr; };
//        void foo () {};
//        GlobalPointerToFunctionType globalFunctionPointer;
   };

typedef void (A::*PointerToMemberFunctionType)();
