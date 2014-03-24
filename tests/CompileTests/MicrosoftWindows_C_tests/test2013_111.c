void foo()
   {
     if (1)
        {
          int status;
          if (status)
               foobar1((__extension__ (((union { int __in; int __i; }) { .__in = (status) }).__i)));

          if (status)
               foobar2();
        }
   }


