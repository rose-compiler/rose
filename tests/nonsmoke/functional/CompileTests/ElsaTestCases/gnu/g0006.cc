// g0006.cc
// simulation of gcc's vararg stuff


typedef void *__gnuc_va_list;
typedef __gnuc_va_list va_list;

int myprintf(char const *format, ...)
{
  va_list ap;
  ( ap  = ((__gnuc_va_list) __builtin_next_arg (  format ))) ;
  ((void)0) ;
  return 0;
}

