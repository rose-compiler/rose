// Test use of default in switch in non-typical ordering
// plus extra non-case statements.

void foo( int x)
   {
     int y = 0;
     switch(x)
        {
          int z;
          case 1:
               x = 1;
               z = 5;
               break;
          y = 1;
          default:
             {
               x = 0;
               break;
             }
          case -1:
               x = -1;
               break;
        }
   }
