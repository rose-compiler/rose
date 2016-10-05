class Test
   {
     public:
       // This is not allowed in C++.
       // Test& operator->();

          Test* operator->();
          Test& operator+=(const Test & x);

          void getFormat();
   };

void foobar()
   {
     Test ref;
#if 1
  // This is two function calls.
     ref->getFormat();
#endif

  // This is one function call.
  // (ref += ref);

#if 1
     (ref += ref).getFormat();
#endif
   }

