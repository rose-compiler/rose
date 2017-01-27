#include <cstdarg>

#define BUG 1
#define DEMO_BUG 1

#if BUG
// I think this may be incorrect code (non-conforming with respect to the standard) that requires a reference.
// The reason is that va_copy can't use "args" as a source (second arg) in the copy to the destiniation (first arg).
// This is enforced in EDG, but not in GNU or Intel (which makes it a difficult case to make).
#if DEMO_BUG
void foobar ( std::va_list args)
#else
// This is the correct version of the code.
void foobar ( std::va_list & args)
#endif
#else
void foobar ()
#endif
   {
#if !BUG
     std::va_list args;
#endif
     std::va_list args_copy;

  // void va_copy (std::va_list dest, std::va_list src )
     va_copy(args_copy, args);
   }


