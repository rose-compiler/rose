// uchar.h does not apear to be available within GNU 4.8 (and so is not available to EDG).
// uchar.h defines the types char16_t, char32_t and declares the functions mbrtoc16, c16rtomb, mbrtoc32, c32rtomb. 
// It is not possible to support this header file with GNU currently.

#if __GNUC__ == 4 && __GNUC_MINOR__ > 8 || __GNUC__ > 4
  #include <uchar.h>
#endif


