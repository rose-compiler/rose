// uchar.h does not apear to be available within GNU 4.8 (and so is not available to EDG).
// uchar.h defines the types char16_t, char32_t and declares the functions mbrtoc16, c16rtomb, mbrtoc32, c32rtomb. 
// It is not possible to support this header file with GNU currently.

//#if __GNUC__ == 4 && __GNUC_MINOR__ > 8 || __GNUC__ > 4
// Pei-Hung (10/11/18) uchar.h is supported from glibc 2.16
// Reference at https://gcc.gnu.org/wiki/C11Status
#if __GLIBC__ == 2 && __GLIBC_MINOR__ > 15 || __GLIBC__ > 2
  #include <uchar.h>
#endif


