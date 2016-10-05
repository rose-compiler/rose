#include <math.h>
#include <stdlib.h>

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
    
    tSquared = pow(t,2.0);
    
    i = 1000;
    
    for(j=abs(1);j<abs(i);j+=abs(2))
    {
        tSquared += 1.0;
        tSquared += pow(t,1.0);
        tSquared += sin(t);
        tSquared += sqrt(t);
        tSquared += foo(2.2);
    }
    return 0;
}
