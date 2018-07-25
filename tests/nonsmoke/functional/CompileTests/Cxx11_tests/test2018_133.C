// Modified: ROSE-40 (Kull)

namespace namespace1 
   {
     class Class1 
        {
          public:
               int func_and_local1();
        };

     int func_and_local1();
   }

// Add another variable to hide it from global scope.
int func_and_local1 = 0;

int namespace1::Class1::func_and_local1() 
   {
     int func_and_local1;
     func_and_local1 = namespace1::func_and_local1();
   }
