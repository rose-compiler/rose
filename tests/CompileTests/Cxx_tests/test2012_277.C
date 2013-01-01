// This test code is a problem only for the copyAST_tests directory tests.
template <typename T>
class X
   {
     public:
       // void foo();

       // This is required to be the defining declaration.
          friend X<T> & operator+( X<T> & i, X<T> & j)
             {
               return i;
             }
   };

int main()
   {
     X<int> x;

     return 0;
   }

