#define MACRO()           \
   if (true) { }          \
   for (k=0; k < 42; k++) \
      {                   \
        a2 = array[k];    \
        a3 = 42;          \
      }

void foobar()
   {
     int k;
     int array[42];
     int a2,a3;

     for (int j = 0; j <= 42; j++) 
        {
          MACRO();
        }
   }

