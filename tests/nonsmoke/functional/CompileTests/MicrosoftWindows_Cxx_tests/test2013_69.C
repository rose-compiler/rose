
#if 1
template<typename _Tp> _Tp __cmath_power(_Tp, unsigned int);

template<typename _Tp>
inline _Tp
__pow_helper(_Tp __x, int __n)
   {
  // return __n < 0 ? _Tp(1)/__cmath_power(__x, -__n) : __cmath_power(__x, __n);
  // return _Tp(1);
  // return _Tp(1) / 42;
  // return _Tp(1) / __cmath_power(__x, __n);
     return __cmath_power(__x, __n);
   }

#if 1
template<typename _Tp>
inline _Tp
__cmath_power(_Tp __x, unsigned int __n)
   {
     _Tp __y = __n % 2 ? __x : 1;

     while (__n >>= 1)
        {
          __x = __x * __x;
          if (__n % 2)
               __y = __y * __x;
        }

     return __y;
   }
#endif

#else
template<typename _Tp> _Tp __cmath_power(_Tp);

template<typename _Tp>
_Tp
__pow_helper(_Tp __x)
   {
     return __cmath_power(__x);
   }

#endif
