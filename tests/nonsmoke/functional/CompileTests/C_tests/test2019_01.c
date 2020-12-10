void
ngx_process_get_status(void)
   {
  // Original code:  if ( (((union ABC { int __in; int __i; }) { .__in = 42 }).__i) == 0)
  // Generated code: if (  ((union ABC {                    }) {.__in = 42}) . __i == 0)
     if ( (((union ABC { int __in; int __i; }) { .__in = 42 }).__i) == 0)
        {
        }
   }
