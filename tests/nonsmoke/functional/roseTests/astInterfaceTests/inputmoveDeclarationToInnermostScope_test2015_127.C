
int foobar( int n)
   {
     return 42;
   }

void foo()
   {
     const int size = 10;
     int array[size];

     int i, length;

     foo();

     while (true)
       {
         length = 5;
       }

     int x = 2;

     switch (x)
        {
          case 1:
             {
               for (i=0; i < size; i++)
                  {
                    array[i] = 1;
                    array[i] = foobar(array[i+1]);
                  }
               break;
             }

          case 2:
             {
               for (i=0; i < size; i++)
                  {
                    array[i] = 2;
                    array[i] = foobar(array[i+1]);
                  }
               break;
             }

          default:
             {
             }
        }
   }

