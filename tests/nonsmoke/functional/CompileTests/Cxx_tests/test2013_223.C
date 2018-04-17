template<typename T>
void foo();

class bar 
   {
     void foo() 
        {
       // The issue here is that that has not yet been an instnatiation.
          ::foo<int>();
        }
   };

// experiment with class elaboration.
// int bar;
// class bar barbar;
