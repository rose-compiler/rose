
// Outside of the class this test works fine.
// void (*__cancel_routine)();

class __pthread_cleanup_class
   {
  // When the function pointer is a data member this is more difficult.
#if 1
  // This is the problem case of a function call from a data member which is a function pointer.
     void (*__cancel_routine)();
#else
  // This is the simple case of a member function declaration.
     void __cancel_routine();
#endif
     public:
         void foo () { __cancel_routine(); }
   };

