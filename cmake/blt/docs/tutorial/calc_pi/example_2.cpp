///////////////////////////////////////////////////////////////////////////////
//
// file: calc_pi_example_2.cpp
// 
// Simple example that calculates pi via simple integration.
//
// Adapted from:
// https://www.mcs.anl.gov/research/projects/mpi/usingmpi/examples-usingmpi/simplempi/cpi_c.html
///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cstdlib>
#include <math.h>

#include "calc_pi.hpp"

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
