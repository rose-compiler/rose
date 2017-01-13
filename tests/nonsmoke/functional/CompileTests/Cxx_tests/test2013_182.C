template <typename T>
class X
   {
     public:
          void foo();

       // The existence of the friend function is essential to reproducing the bug.
          friend 
          void foobar( T f )
             {
               T t = 0;
             }
   };

void foobar(double f);
void foobar(int f);

int
main()
   {
     X<int> x1;
     X<double> x2;
  // x.foo();

     foobar(7);
     foobar(42.0);
   }
