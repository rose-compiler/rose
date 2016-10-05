#define myAssert \
     if (1) \
          1; \
       else \
          0;


int 
main()
   {
     int x = 42;

     myAssert;
     myAssert;
     myAssert;
     myAssert;

     switch (x)
        {
          case 0: 
#if 1
             {
               myAssert;
               int y = x;
             }
#else
             {
               int y = x;
             }
#endif
          break;
#if 1
          case 1: 
#if 1
             {
               myAssert;
               int y = x;
             }
#else
             {
               int y = x;
             }
#endif
          break;
#endif
#if 1
          case 2: 
#if 1
             {
               myAssert;
               int y = x;
             }
#else
             {
               int y = x;
             }
#endif
          break;
#endif
#if 1
          case 3: 
#if 1
             {
               myAssert;
               int y = x;
             }
#else
             {
               int y = x;
             }
#endif
          break;
#endif
        }

  // printf ("Program Terminated Normally! \n");
     return 0;
   }
