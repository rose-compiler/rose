/* Test substitution of varargs with something that would preserve them in the translation through ROSE

// typedef va_list int;
// #define va_start(ap,parmN) char* rose_macro_declaration_vararg = "ROSE-MACRO-CALL:va_start(ap,parmN)"
// #define va_arg(ap,type)    char* rose_macro_declaration_vararg = "ROSE-MACRO-CALL:va_arg(ap,type)"
// #define va_end(ap)         char* rose_macro_declaration_vararg = "ROSE-MACRO-CALL:va_end(ap)"

typedef char* va_list;
#define va_start(ap,parmN) ap = "ROSE-MACRO-CALL:va_start(ap,parmN)";
#define va_arg(ap,type)    (type) "ROSE-MACRO-CALL:va_arg(ap,type)";
#define va_end(ap)         ap = "ROSE-MACRO-CALL:va_end(ap)";

#define va_start(ap,parmN) ap =  va_start_support("ROSE-MACRO-CALL:va_startSTART_PAREN",#ap,",",#parmN,"END_PAREN")
*/

#if 1

#if 0
#include <varargs.h>
#else
#include <stdarg.h>
#endif

#else

#if 1

#if 1
typedef char* va_list;
#define va_start(ap,parmN) "ROSE-MACRO-EXPRESSION:va_start("#ap","#parmN")"
#define va_arg(ap,type)    (type)"ROSE-MACRO-EXPRESSION:va_arg("#ap","#type")"
#define va_end(ap)         "ROSE-MACRO-EXPRESSION:va_end("#ap")"
#else
typedef char* va_list;
#define MID XXX
#define va_start(ap,parmN) ap =  va_start_support(ROSE-MACRO-CALL:va_startSTART_PAREN,ap,COMMA,parmN,END_PAREN)
#define quoteMe(s1) #s1
#define va_start_support(s1,s2,s3,s4,s5) quoteMe(s1##s2##s3##s4##s5)
#define va_arg(ap,type)    (type) "ROSE-MACRO-CALL:va_arg(ap,type)";
#define va_end(ap)         ap = "ROSE-MACRO-CALL:va_end(ap)";
#endif

#else

typedef char *va_list;
#define va_alist __builtin_va_alist

#ifdef __cplusplus
extern "C" char* __builtin_va_arg_incr(...);
extern "C" int va_alist;
#else /* !defined(__cplusplus) */
extern int va_alist;
extern char* __builtin_va_arg_incr();
#endif  /* defined(__cplusplus) */

/* #define va_start(ap, parmN) (void)(ap = (va_list)(&parmN + 1)) */
#define va_start(ap, parmN) (void)(ap = (va_list)(&parmN + 1))
/* #define va_arg(ap, type) ((type *)(ap += sizeof(type)))[-1] */
#define va_arg(ap, type) ((type *)(ap += sizeof(type)))[-1]

#define va_end(ap) ((void)0)
#endif

/* endif for varargs test of hearder file */
#endif

void foo(char *fmt, ...)
   {
     int i;
     va_list args;
     va_start(args,fmt);
     i = va_arg(args,int);
     va_end(args);
   }

