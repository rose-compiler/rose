class A
   {
     public:
          typedef A typedef_A;
          typedef int typedef_int;
   };

void foo()
   {
  // Name qualification required to reference types in another class.
     typedef A::typedef_A innerClass_A;
     typedef A::typedef_int innerClass_int;

     innerClass_A innerClassObject_A;
     innerClass_int innerClassObject_int;
   }
