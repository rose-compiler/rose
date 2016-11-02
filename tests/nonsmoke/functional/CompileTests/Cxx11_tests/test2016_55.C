double XXX__builtin_fabs(double) __attribute__((nothrow, const));
float XXX__builtin_acosf(float) __attribute__((nothrow, const));

double __builtin_fabs(double) __attribute__((nothrow, const));

#if 0
inline constexpr double
abs(double __x)
   {
     return XXX__builtin_fabs(__x);
   }

inline constexpr float
acos(float __x)
   { 
     return XXX__builtin_acosf(__x); 
   }
#endif

inline constexpr double
my_abs(double __x)
   {
     return __builtin_fabs(__x);
   }

