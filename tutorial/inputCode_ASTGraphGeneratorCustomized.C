
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

