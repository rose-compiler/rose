///////////////////////////////////////////////////////////////////////////////
//
// file: calc_pi_example_1.cpp
// 
// Simple example that calculates pi via simple integration.
//
// Adapted from:
// https://www.mcs.anl.gov/research/projects/mpi/usingmpi/examples-usingmpi/simplempi/cpi_c.html
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdlib>
#include <math.h>

// -- calculate pi via simple integration  -- //
double calc_pi(int num_intervals)
{
    int i =0;
    
    double h   = 1.0 / (double) num_intervals;
    double sum = 0.0;
    
    while(i != num_intervals)
    {
        double x = h * ((double)i + 0.5);
        sum += (4.0 / (1.0 + x*x));
        i++;
    }
    
    double pi = h * sum;
    
    return pi;
}

// -- main driver -- //
int main(int argc, char * argv[] )
{
    double PI_REF = 3.141592653589793238462643;

    int num_intervals = 100;
    if(argc >1)
    {
        num_intervals = std::atoi(argv[1]);
    }
    
    std::cout << "calculating pi using " 
              << num_intervals
              << " intervals."
              << std::endl;

    double pi = calc_pi(num_intervals);

    std::cout.precision(16);

    std::cout << "pi is approximately "
              << pi 
              << " , Error is "
              << fabs(pi - PI_REF)
              << std::endl;

}
