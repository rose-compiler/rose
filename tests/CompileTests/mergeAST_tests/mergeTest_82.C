// This example demonstrates that three function symbols are being built for the 
// min function (likely because we don't store them in the EDG AST in a consistant 
// way.  To see this use "make test_small_single".

namespace std
   {

     template<typename _Tp> void min(const _Tp& __a, const _Tp& __b) {}

     void foobar ()
        {
          min(1,2);
        }
   }
