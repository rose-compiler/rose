template<typename T>
void foo();

class bar 
   {
     void foo() 
        {
          ::foo<int>();
        }
   };

// experiment with class elaboration.
// int bar;
// class bar barbar;
