class A
   {
     public: void foo(int x);
   };

// Error:  This is unparsed as "void A()" within the new interface!
void A::foo(int x) {}
