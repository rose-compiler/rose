template <typename T>
class X
   {
     public:
          void foo();

       // The existence of the friend function is essential to reproducing the bug.
#if 1
          friend 
#endif
       // void foobar( T f )
          void foobar()
             {
               T t = 0;
             }
   };

int
main()
   {
     X<int> x;
     x.foo();
   }
