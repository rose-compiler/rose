class A
   {
  // This has to be a forward declaration!
     public: int a();

  // This has to be a defining declaration!
     private: int b(){};

  // This has to be a forward declaration! (current bug is that "public:" is not output in code generation)
     public: int c();
   };

void foo()
   {
     A x;

  // If this is NOT public then we will generate an error (current bug)
     x.c();
   }

