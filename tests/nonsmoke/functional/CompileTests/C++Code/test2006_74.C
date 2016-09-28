namespace std 
{
template<typename _Tp>
   _Tp
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
}

void foo()
   {
     int a,b;
     unsigned int c;

     a = std::__cmath_power(b,c);
     a = std::__cmath_power(1,2);
  }
