void
ngx_process_get_status(void)
   {


#if 0
  // Original failing code:
     int status;
     if ((((__extension__ (((union ABC { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0x7f))
        {
        }
#endif

#if 0
     int status;
     int x = ((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0x7f));
#endif

     if ( (((union ABC { int __in; int __i; }) { .__in = 42 }).__i) == 0)
        {
        }

#if 0
  // This works.
     int x = (((union { int __in; int __i; }) { .__in = 42 }).__i);
#endif

#if 0
  // int status;
  // int x = ((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0x7f));
  // int x = (((union { int __in; int __i; }) { .__in = 42 }).__i);

  // Original code: int x = (((union { int __in; int __i; }) { .__in = 42 }).__i);
  // int x = (((union { int __in; int __i; }) { .__in = 42 }).__i);
  // int x = ((__extension__ (((union ABC { int __in; int __i; }) { .__in = 42 }).__i) ));
     int x = (((union ABC { int __in; int __i; }) { .__in = 42 }).__i);
#endif
   }
