// Name qualification test: GNU 4.1.2 does not accept over qualification of names in class definitions.
class A 
   {
      public: A();
   };

A::A() {}
