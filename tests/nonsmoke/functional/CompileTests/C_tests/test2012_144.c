

// This appears to be the final bug in the postfix application.
// At the very end of "main()"...

void exit (int __status);

void foobar()
   {
     int status;
     int pid;

     switch (pid)
        {
          default:
            // exit(((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0x7f) == 0) ? ((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0xff00) >> 8) : 1);
            // exit( ((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0x7f) == 0) );
            // exit( ((__extension__ (((union { int __in; int __i; }) { .__in = 84 }).__i))) );
            // status = ((__extension__ (((union { int __in; int __i; }) { .__in = 84 }).__i)));
            // status = ((__extension__ (((struct { int __in; int __i; }) { .__in = 84 }).__i)));

            // status = ((__extension__ (((union { int __inXXXXX; int __i; }) { .__inXXXXX = 84 }).__i)));
            // exit(((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0x7f) == 0) ? ((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0xff00) >> 8) : 1);
            // pid = ((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0x7f) == 0) ? ((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0xff00) >> 8) : 1;

          // This now works...
          // pid = ((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0x7f) == 0); // ? ((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0xff00) >> 8) : 1;

          // But this fails.
             pid = ((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0x7f) == 0) ? (((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0xff00) : 1;

        }
   }

