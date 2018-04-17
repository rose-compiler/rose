#if 0
Compiling the following code:
#include <stddef.h>

struct foo {
     char a[13];
     long b;
     char c[7];
     short d;
     char e[3];
};

main ()
{
     int A, B, C, D, E;

     A = offsetof(struct foo, a[0]);
     B = offsetof(struct foo, b);
     C = offsetof(struct foo, c[0]);
     D = offsetof(struct foo, d);
     E = offsetof(struct foo, e[0]);

}

gives the follolwing error from ROSE:
"test3.C", line 15: error: identifier "__offsetof__" is undefined
       A = offsetof(struct foo, a[0]);

#endif

// #undef offsetof
// #undef __offsetof__
// #undef __builtin_offsetof

#if 1
#include <stddef.h>
#else
typedef unsigned long size_t;
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

struct foo
   {
     char a[13];
     long b;
     char c[7];
     short d;
     char e[3];
   };

int 
main ()
   {
     int A, B, C, D, E;

#if ( (__GNUC__ == 3) || (__GNUC__ == 4) && (__GNUC_MINOR__ < 1) )
     A = offsetof(struct foo,a[0]);
  // B = offsetof(struct foo, b);
  // C = offsetof(struct foo, c[0]);
  // D = offsetof(struct foo, d);
  // E = offsetof(struct foo, e[0]);
#else
#warning "offsetof macro or builtin function not defined in g++ 4.1.2 compiler."
#endif

     return 0;
   }

