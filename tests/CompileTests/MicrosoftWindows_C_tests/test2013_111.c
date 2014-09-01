void foo()
   {
     if (1)
        {
          int status;
#if 0
       /* No support for GNU statement expressions (GNU extension) (or maybe also un-named unions) under Windows */
          if (status)
               foobar1((__extension__ (((union { int __in; int __i; }) { .__in = (status) }).__i)));
#endif
#if 1
       /* implicit functions not allowed (or problematic) in MS mode (this appears to be fixed now). */
          if (status)
               foobar2();
#endif
        }
   }


