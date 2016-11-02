// Example code from Xen.

#define ADD_BRACE 0

int foobar()
   {
     if (1)
        {
          switch (42)
             {
               default:
#if ADD_BRACE
                  {
#endif
                    return ( {
                               int x;
                               x = 0;
                             } );
#if ADD_BRACE
                  }
#endif
             }
        }
#if 1
       else
        {
        }
#endif

     return 0;
   }
