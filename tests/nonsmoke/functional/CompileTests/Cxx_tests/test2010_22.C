#include <math.h>
#include <stdio.h>
#include <iostream>
#include <complex>

// Entry point
int main(int argc, char *argv[])
{
    float x;
    float y = 0.0;

    for (x = 0.0; x <= 5.0; x+= 1.0)
    {
        // Simple square
        #pragma LUTOPTIMIZE
        y += x * x;
    }

    printf ("y = %f\n", y); 
    
    return 0;
}
