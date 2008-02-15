// Similar to test2006_72.C except using static variable and function
class X
   {
     public:
         static int foo() 
            {
              return x;
            }

         static int x;
   };

int X::x = 0;
