void
ngx_process_get_status(void)
   {
#if 1
  // Original failing code:
     int status;
     if ((((__extension__ (((union { __typeof(status) __in; int __i; }) { .__in = (status) }).__i))) & 0x7f))
        {
        }
#endif
   }


