int x = 123;
struct B 
   {
     template<class T> operator T&() 
        {
          const T *p = 0;  // T is "const int", NOT "int"
          T *q = p;        // so "const T" is the same as "T"
          T t = 0;         // T is "const int", NOT "const"  - see SR00667
          return x;
        }
   };

void foobar()
   {
     B b;
     int n = (const int &) b;
   }
