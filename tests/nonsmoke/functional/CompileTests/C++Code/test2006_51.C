// Example from Beata, we should verify that the "y" in the if statement 
// (SgIfStmt) is in the correct scope!

void s()
   {
     int x=10;
     int y=0;
        {
          if(1)
               int y = 1;
 
          x = y;
          printf("x is %i\n", x);
        }
     return ;
   }
 
