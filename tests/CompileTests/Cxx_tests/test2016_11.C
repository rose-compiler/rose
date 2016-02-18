
template <class T> 
class X
   {
     public:
         void foobar ( void (*globalFunctionPointer) (void) )
            {
              void foo(void);
              foobar(foo);
            }
   };

void foo(void)
   {
     X<int> Xint;
     Xint.foobar(foo);
   }
