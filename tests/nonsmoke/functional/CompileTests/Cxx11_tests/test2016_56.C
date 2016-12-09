constexpr double X__builtin_fabs(double); // __attribute__((nothrow, const));
// double X__builtin_fabs(double); // __attribute__((nothrow, const));

inline constexpr double
abs(double __x)
   {
     return X__builtin_fabs(__x);
   }
