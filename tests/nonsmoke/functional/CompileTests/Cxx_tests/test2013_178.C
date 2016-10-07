void foobar();

template <typename T>
class X
   {
     public:
          void foo();

       // The existence of the friend function is essential to reproducing the bug.
#if 1
          friend 
#endif
          void foobar()
             {
             }
   };

int
main()
   {
     X<int> x;
     x.foo();

     foobar();
   }
