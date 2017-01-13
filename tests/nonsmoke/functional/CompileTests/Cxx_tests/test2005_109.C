// 7/12/2005: Example of function name resolution supression in C++
// Peter told me about this interesting problem :-).  ROSE does not 
// handle this properly yet, but could once we move to EDG 3.4 or beyond.

void foo( long x );
void foo( int  x );

void foobar()
   {
  // Using the extra set of "()" treats the function as a pointer and avoids the 
  // use of function parameters to resolve the function at compile time (or something 
  // like that).  Peter will send a more useful example soon.  This information
  // is in EDG 3.4 (but not 3.3) in the flag "arg_dependent_lookup_suppressed_on_call"
  // (see EDG 3.4 il_def.h for more details).
     (foo)(1);
   }
