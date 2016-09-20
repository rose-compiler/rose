

void foo()
   {
     int i;
     const int size = 10;
     int array[size];

     foo();

     int x = 2;

     switch (x)
        {
          case 1:
             {
               for (i=0; i < size; i++)
                  {
                    array[i] = 1;
                  }
               break;
             }

          case 2:
             {
               for (i=0; i < size; i++)
                  {
                    array[i] = 2;
                  }
               break;
             }

          default:
             {
             }
        }
   }
