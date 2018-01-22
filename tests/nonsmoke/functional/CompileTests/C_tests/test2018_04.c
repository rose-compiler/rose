void foo_1() 
   {
     switch(2) 
        {
          case 42:
          case 43:
               break;
          case 5:
               break;
          default:
            // case of a default that falls through to a case option statment.
            // break;
          case 6:
               break;
        }
   }

