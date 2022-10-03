void foobar();

template <typename T>
class X
   {
     public:
       // void foo();

       // The existence of the friend keyword to the function is essential to reproducing the bug.
          friend 
          void foobar() {}
   };

int
main()
   {
     X<int> x;
  // x.foo();

     foobar();
   }
