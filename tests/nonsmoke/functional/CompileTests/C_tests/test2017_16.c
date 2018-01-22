void foo()
   {
     int i;
     switch (i)
        {
          case 1:
              goto xxx;
          case 2:
               break;
       // Bug: unparses as:
       // xxx:
       //      break; 
       // default:;
xxx:      default:
               break;
       }
   }
