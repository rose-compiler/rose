
// Templated class declaration used in template parameter example code
template <typename T>
class templateClass
   {
     public:
          int x;

          void foo(int);
          void foo(double);
   };

// Overloaded functions for testing overloaded function resolution
void foo(int);
void foo(double)
   {
     int x = 1;
     int y;

  // Added to allow non-trivial CFG
     if (x)
        y = 2;
     else
        y = 3;
   }

int main()
   {
     foo(42);
     foo(3.14159265);

     templateClass<char> instantiatedClass;
     instantiatedClass.foo(7);
     instantiatedClass.foo(7.0);

     for (int i=0; i < 4; i++)
        {
          int x;
        }

     return 0;
   }
