void foobar()
   {
     int x = 0;
     switch (x)
        {
          case 0: 
               x = 1;
#if 0
          case 1:
               x = 2;
#endif
#if 1
          default:
               x = 3;
#endif
        }
   }
