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
void foobar()
   {
     int i;
     for (i =0; i < 2; i++)
          if (((((__extension__ ({ union { long in; int i; } u; u.in = 42; u.i; }))) & 0xff) == 0x7f))
               break;
   }
#endif


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

     while (0)
          if ( ({ union { int i; } u; u.i = 42; }) == 0)
               break;

   }
