class A
   {
     public: 
          explicit A(int x) {}
   };

class B: public A
   {
  // Inherit all the constructors from A
     using A::A;
   };
