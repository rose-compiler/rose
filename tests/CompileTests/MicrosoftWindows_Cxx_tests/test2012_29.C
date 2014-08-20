// This is a simple version of what I think it failing in test2004_48.C
namespace X
   {
  // The first nondefining declaration is in one instance of the namesapce.
     template<typename _Tp> _Tp __cmath_power(_Tp, unsigned int);
   }

namespace X
   {
#if 1
   // And the function is defined in another instance of the namespace.
      template<typename _Tp> _Tp __cmath_power(_Tp, unsigned int)
         {
           return _Tp();
         }
#endif
   }
