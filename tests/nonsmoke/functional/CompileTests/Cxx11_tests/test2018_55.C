class C
   {
     int x=7; //class member initializer

     public:
          C(); //x is initialized to 7 when the default ctor is invoked
          C(int y) : x(y) {} //overrides the class member initializer
   };

C c; //c.x = 7

C c2(5); //c.x = 5

