
void foo(double a)
   {
     int x = 1;
     int y;

  // Added to allow non-trivial CFG
     if (x)
        y = 2;
     else
        y = 3;

  // Added to allow non-trivial CFG
     int array[10];
     for (int i=0; i < 10; i++)
        {
          array[i] = y;
        }
   }

