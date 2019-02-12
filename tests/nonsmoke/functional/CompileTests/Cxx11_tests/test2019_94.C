
struct A
   {
     A(int i);

#if (__GNUG__ > 5 || (__GNUC__ == 5 && __GNUC_MINOR__ > 1))
   // GNU g++ 5.1 error: the ellipsis in ‘A::A(int, int, ...)’ is not inherited [-Winherited-variadic-ctor]
      A(int i, int j, ...);
#endif
   };

struct B : A 
   {
     using A::A;
   };

void foobar()
   {
     B ba (1);
#if (__GNUG__ > 6 || (__GNUC__ == 6 && __GNUC_MINOR__ > 1))
     B bb (2, 3);
     B bc (4,5,6);
#endif
   }
