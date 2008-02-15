int x;

void foo()
   {
     int x;
     ::x = 1;

        {
          int x; x = 1;
        }
   }
