// DQ (1/22/2013): Test code generated from test2013_15.C (as I recall).

// This test code is part of one generated from "g++ -E" and it 
// contins references to __builtin_va_start() and __builtin_va_end()
// which are a problem for ROSE.  These are converted va_start()
// and va_end() which is a problem for the backend compiler.
// Not clear why this is a problem for g++.

struct __locale_t
   {
   };

typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;

namespace std __attribute__ ((__visibility__ ("default"))) {

  typedef __locale_t __c_locale;





  inline int
  __convert_from_v(const __c_locale& __cloc __attribute__ ((__unused__)),
     char* __out,
     const int __size __attribute__ ((__unused__)),
     const char* __fmt, ...)
  {
     __c_locale __old;

    va_list __args;
    __builtin_va_start(__args,__fmt);

    const int __ret = 7;

    __builtin_va_end(__args);

    return __ret;
  }

}
