#if 0
#include <stdio.h>                      
#include <stdlib.h>
#include <stdarg.h>
// #include <varargs.h>
#endif

#if 0
#include "irs.h"
#include "irsctl.h"
#include "AEStackElm.h"
#include "Rgst.h"
#endif

#if 0
double type;
int i;

void foo(...)
{
  va_list ap;
  va_start(ap, type);
  i = va_arg(ap,int);
}
#endif

// Include portable var arg mechanism
#include <stdio.h>                      
#include <stdlib.h>
#include <stdarg.h>
// #include <varargs.h>

// typedef int __builtin_va_alist_t __attribute__((__mode__(__word__)));
// #define va_start(v)   __builtin_varargs_start((v))

// #define __builtin_va_list (void*)
// #define va_start(AP)  AP=(char *) &__builtin_va_alist
// #define va_end(AP)	((void)0)
// #define __va_rounded_size(TYPE) (sizeof (TYPE)

// #define va_arg(AP, TYPE)						\
//  (AP = (__builtin_va_list) ((char *) (AP) + __va_rounded_size (TYPE)),	\
//   *((TYPE *) (void *) ((char *) (AP) - __va_rounded_size (TYPE))))

#if 0
#if 0
#define __builtin_stdarg_start(a,b) (void*)(a,b)
#define __builtin_va_end(a) (void*)(a)
#else
// #define __builtin_stdarg_start(a,b) va_start(a,b)
// #define __builtin_va_end(a) va_end(a)
#define va_start(a,b) (void*)(a,b)
#define va_end(a) (void*)(a)
#define va_arg(a,b) (void*)((a),sizeof(b))
#endif
#endif

void foo(char *fmt, ...)
   {
     va_list args;
     va_start(args,fmt);
     va_arg(args,int);
     va_end(args);
   }
