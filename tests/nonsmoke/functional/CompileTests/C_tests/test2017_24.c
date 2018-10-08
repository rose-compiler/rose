
void foo_1() 
   {
     switch(2) 
        {
          case 42:
            goto xxx;
               break;
       // label after the case statement.
          case 5:
               xxx: break;
          case 7:
               break;
        }
   }

