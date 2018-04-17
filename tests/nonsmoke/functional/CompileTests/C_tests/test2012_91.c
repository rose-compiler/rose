
int
ngx_get_options()
   {
     unsigned char *p;
     int i;

     for (i = 1; i < 2; i++) 
        {
  // while(0) {
  //      switch (*p++){}
  //       for (;;){}
  //       if (0){}
  //       if (0);
#if 0
          switch (*p++){}
#endif
#if 1
          switch (*p++) 
             {
          // DQ (12/21/2017): Modified to make legal code for GNU, now that we don't normalize by adding "{}" to the unparsed code.
          // case 12:
             case 12: {}
            // default:
             }
#endif
          7;
          continue;
          8;
        }
     // }

     return 0;
   }
