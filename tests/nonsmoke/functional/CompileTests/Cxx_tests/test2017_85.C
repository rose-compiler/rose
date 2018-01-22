// This is not legal C++ code (fails for GNU (for good reason), but passes for EDG (which is usually more strict).
void foobar() 
   {
     switch (int i = 42)
        {
          int x = i;
          case 1: 
               x = 2;
               break;
          int y = 4;
          case 2: 
               x = 2;
               int z = 5;
          case 3:
          default:
        }

     7;
   }
