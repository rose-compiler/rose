template <typename T>
class X
   {
     public:
          void foo();

       // The existence of the friend function is essential to reproducing the bug.
          template <typename S>
          friend 
          void foobar( S y )
             {
               T t = 0;
             }
   };

int
main()
   {
     X<int> x;

  // DQ (7/7/2014): The friend function need not be called to demonstrate the problem.
  // foobar(7);
   }
