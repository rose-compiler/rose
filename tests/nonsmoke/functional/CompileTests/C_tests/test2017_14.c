void foo()
   {
     int i;
     switch (i)
        {
          case 1:
          case 2:
               switch (i)
                  {
                    case 3:
                    case 4:
                         i = 42;
                  }
               break;
// # 479 "./print-lmp.c"
          case 5:
               break;
       }
   }
