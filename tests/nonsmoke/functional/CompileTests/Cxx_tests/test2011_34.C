// Example of base class qualification.
class A
   {
     public:
         int x;
   };

class B : public A
   {
     public:
         int x;

         void foo()
            {
           // This is base class qualification, but really uses the same rules for normal name qualification.
               x = A::x;
            }
   };




   
