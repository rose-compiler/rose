
void foo_1() 
   {
     switch(2) 
        {
          case 42:
            goto xxx;
               break;
  // label before the case statement.
     xxx: case 5:
               break;
          case 7:
               break;
        }
   }

