template <typename T>
class X
   {
     public:
          friend void foo( X<T> & i )
             {
             }
   };

#if 1
void foobar()
   {
     X<int> x;

     foo(x);
   }
#endif
