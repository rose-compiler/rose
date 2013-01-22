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
