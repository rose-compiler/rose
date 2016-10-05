// This test code demonstrates the use of multiple variables 
// in a for initializer declaration.

#define NULL (0)

int x = 0;

void foo()
   {
     int jj;
     for( int iii = 0, jjj; iii < 10; iii++ )
        {
          for( jj = jjj = 0; jj < 5; jj++, jjj += 2)
             {
               x++;
               jjj++;
             }
        }

  // Build an example where the variables are the same base type, but one is a pointer!
     for( int iii = 0, *jjj = NULL; iii < 10; iii++ )
        {
          x++;
          jjj++;
        }

   }


