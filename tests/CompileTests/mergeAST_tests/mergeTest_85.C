class A
   {
     public:
     A();
   };

A::A()
   {
     const A* pointer = this;
   }

int 
main()
   {
  // Build object so that we can call the constructor
  // A objectA1;
     A objectA2 = A();
  // A objectA3 = objectA1;

  // printf ("Program Terminated Normally! \n");
     return 0;
   }

