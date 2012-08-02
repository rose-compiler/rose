// Non-template version of test2012_137.C
class A
   {
     public:
       // C++ casting operator definition
          operator int&() const;
   };

void foo()
   {
     A X_result;
   }



