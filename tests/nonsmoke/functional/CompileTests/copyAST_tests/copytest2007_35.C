
int 
main()
   {
     int x = 42;
#if 0
     if (true)
        {
          int y = x;
        }
#endif
#if 1
     switch (x)
        {
          case 0: 
             {
               int y = x;
             }
          break;
        }
#endif
   }
