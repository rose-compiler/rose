void foo()
   {
     if (1)
        {
          int status;
#if 0
          if (status)
               foobar1((__extension__ (((union { int __in; int __i; }) { .__in = (status) }).__i)));
#endif
#if 0
       /* implicit functions not allowed in MS mode. */
          if (status)
               foobar2();
#endif
        }
   }


