// These functions were introduced in GCC 3.3
// You need to add -mmmx and -msse to the compiler 
// configuration (for gcc to enable these).

#if 0
// Builtin functions specific to GNU 4.4.1 (likely 4.4.x)
int __builtin_ia32_psllwi(short,int);
int __builtin_ia32_pslldi(int,int);
int __builtin_ia32_psllqi(long long,int);
int __builtin_ia32_psrawi(short,int);
int __builtin_ia32_psradi(int,int);
int __builtin_ia32_psrlwi(int,int);
int __builtin_ia32_psrldi(int,int);
int __builtin_ia32_psrlqi(long long,int);
#endif




#ifdef __MMX__
  #include<emmintrin.h>
#endif

int main()
   {
     return 0;
   }
