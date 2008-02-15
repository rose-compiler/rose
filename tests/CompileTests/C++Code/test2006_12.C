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


#include <stddef.h>

struct foo
   {
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

