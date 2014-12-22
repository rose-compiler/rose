void foobar()
   {
     int x;
     switch(x)
        {
          case 0:
             {
               x = 76;
               break;
             }
#if 1
          case 1:
          case 2:
             {
               x = 76;
               break;
             }
#endif
        }
   }
