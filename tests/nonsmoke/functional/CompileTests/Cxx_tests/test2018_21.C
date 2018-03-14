
typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;

int ivalue(int);
int iequals(int, int, int);

#define va_start(a,b) __builtin_va_start(a,b)
// #define va_arg(a,b) __builtin_va_arg(a,b)

void foobar(int i, ...)
   {
     va_list ap;
  // va_start(ap, i);
     __builtin_va_start(ap,i);

  // va_arg(ap, int);
     iequals(29, ivalue(__builtin_va_arg(ap,int)), ivalue(132));

   }
