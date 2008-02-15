// cc.in61
// simulation of gcc's vararg stuff


typedef void *__gnuc_va_list;
typedef __gnuc_va_list va_list;

int myprintf(char const *format, ...)
{
  va_list ap;
  
  // sm: nerfed this and moved into in/gnu/g0006.cc
  // since it is gnu-specific
  //
  // update: then I put it back since the mozilla tests refer
  // to __builtin_next_arg and I want to use them in non-gnu
  // mode too... so this symbol is defined even in non-gnu mode now
  ( ap  = ((__gnuc_va_list) __builtin_next_arg (  format ))) ;

  ((void)0) ;
  return 0;
}

