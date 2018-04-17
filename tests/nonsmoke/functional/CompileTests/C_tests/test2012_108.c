#if 0
void foo()
   {
     for (pn = jobtab[jn->other].procs; pn; pn = pn->next)
          if (((((__extension__ ({ union { __typeof(pn->status) __in; int __i; } __u; __u.__in = (pn->status); __u.__i; }))) & 0xff) == 0x7f))
               break;
   }
#endif

#if 0
// This compiles fine and demonstrates the error, but is still too complex.
void foobar_A()
   {
     int i;
     for (i =0; i < 2; i++)
          if (((((__extension__ ({ union { long in; int i; } u; u.in = 42; u.i; }))) & 0xff) == 0x7f))
               break;
   }
#endif

void foo_B (int x);


void foobar()
   {
#if 0
     int i;
  // This is too simple, and fails to demonstrate the error.
     for (i =0; i < 2; i++)
          if (i > 0)
               break;
#endif

#if 0
  // Adding explicit braces fixes the problem...
     while (0)
        {
          if ( ({ union { int i; } u; u.i = 42; }) == 0)
               break;
        }
#endif

     int x;
//   while (0)
     for(;;)
//      if ( ({ union { int i; } u; u.i = 42; }) == 0)
        {
#if 0
          if ( ({ union { int i; } u; u.i = 42; }) == 0)
             foo_B ( ({ union { int i; } u; u.i = 42; }) );
          else
             foo_B ( ({ union { int i; } u; u.i = 42; }) );
#endif
          foo_B ( ({ union { int i; } u; u.i = 42; }) );
          foo_B ( ({ union { int i; } u; u.i = 42; }) );
          foo_B ( ({ union { int i; } u; u.i = 42; }) );
          foo_B ( ({ union { int i; } u; u.i = 42; }) );
          foo_B ( ({ union { int i; } u; u.i = 42; }) );
          foo_B ( ({ union { int i; } u; u.i = 42; }) );
          break;
        }
   }
