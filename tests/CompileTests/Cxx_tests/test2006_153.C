
// This is not defined by default for GNU unless we include the system headers.
// It is defined by default for ROSE, since it is required for the Plum Hall 
// tests and I think it is required within the C++ standard.  It is OK to
// redeclare it as long as it is done consistantly!
#ifdef __LP64__
// 64 bit case
typedef long unsigned int size_t;
#else
// 32 bit case
typedef unsigned int size_t;
#endif

typedef struct __gconv_info
   {
  // In C this is unparsed as "::size_t", as defined in C++, size_t 
  // has a default definition so this code is valid for C++.  But gcc 
  // and g++ will complain if size_t is not declared explicitly.  
  // ROSE however passes the Plum Hall test suite on this fine point.
  // DQ: This is my understanding as of 10/9/2006.
     size_t __nsteps;
   } *__gconv_t;

