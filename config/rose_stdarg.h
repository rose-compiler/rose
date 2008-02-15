/* #error "Inside of rose_stdarg.h" */

/* 
Can't include rose_varargs.h in rose_stdarg.h since:
    va_start takes 2 parameters in stdarg.h 
and 
    va_start takes 1 parameter in varargs.h 

The definitions are provide once in the rose_varargs.h file 
include <rose_varargs.h> 
*/

typedef char *va_list;
#define va_alist __builtin_va_alist

typedef __builtin_va_list __gnuc_va_list;

#ifdef __cplusplus
/* #error "Inside of rose_varargs.h __cplusplus is defined" */
extern "C" char* __builtin_va_arg_incr(...);
extern "C" int va_alist;
#else /* !defined(__cplusplus) */
/* #error "Inside of rose_varargs.h __cplusplus is NOT defined" */
extern int va_alist;
extern char* __builtin_va_arg_incr();
#endif  /* defined(__cplusplus) */

/* typedef char* va_list; */
#define va_start(ap,parmN) "ROSE-MACRO-EXPRESSION:va_start("#ap","#parmN")"
#define va_arg(ap,type)    (type)"ROSE-MACRO-EXPRESSION:va_arg("#ap","#type")"
#define va_end(ap)         "ROSE-MACRO-EXPRESSION:va_end("#ap")"
