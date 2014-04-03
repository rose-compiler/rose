// Note that the class "X" must be templated and the "operator+" must be a friend function.

template <typename T>
class X
   {
     public:
#if 1
       // friend X<T> & operator+( X<T> & i, X<T> & j)
          friend void foo( X<T> & i)
       // friend X & operator+( X & i, X & j)
#else
          X<T> & operator+( X<T> & i)
#endif
             {
            // return i;
               i;
             }
   };

int main()
   {
     X<int> y;
  // X y,z;

#if 1
  // y + z;
     foo(y);
#endif

     return 0;
   }

