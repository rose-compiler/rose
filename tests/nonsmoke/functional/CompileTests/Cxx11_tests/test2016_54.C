double __builtin_fabs(double); // __attribute__((nothrow, const));

inline constexpr double
abs(double __x)
   {
     return __builtin_fabs(__x);
   }
