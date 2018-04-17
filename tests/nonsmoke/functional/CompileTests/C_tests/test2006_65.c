
int foo7 (int x, double y, int* z);

int foo7 (x,y,z)
   int x;
   float y;
   int* z;
   {
     x = 42 + y + *z;
     return x;
   }

int
main ()
   {
     int x = 0;
     foo7 (1,2.0,&x);

     return 0;
   }
