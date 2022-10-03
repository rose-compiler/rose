///////////////////////////////////////////////////////////////////////////////
//
// file: calc_pi.cpp
// 
// Source file for calc_pi library example.
//
// Adapted from:
//  https://www.mcs.anl.gov/research/projects/mpi/usingmpi/examples-usingmpi/simplempi/cpi_c.html
///////////////////////////////////////////////////////////////////////////////
#include "calc_pi.hpp"
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
