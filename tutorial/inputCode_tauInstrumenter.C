// #include <math.h>
// #include <stdlib.h>

double foo(double x)
   {
     double theValue = x;
     theValue*= x;
     return theValue;
   }

int main(int argc, char* argv[])  
   {
     int j,i;
     double tSquared,t;
     t = 1.0;

     tSquared = t*t;

     i = 1000;
     for( j=1; j < i; j += 2)
        {
          tSquared += 1.0;
          tSquared += foo(2.2);
        }

     return 0;
   }
