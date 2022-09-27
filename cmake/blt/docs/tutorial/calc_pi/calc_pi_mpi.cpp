///////////////////////////////////////////////////////////////////////////////
//
// file: calc_pi_mpi.cpp
// 
// Source file for calc_pi_mpi library example.
//
// Adapted from:
//  https://www.mcs.anl.gov/research/projects/mpi/usingmpi/examples-usingmpi/simplempi/cpi_c.html
///////////////////////////////////////////////////////////////////////////////

#include "mpi.h"
#include "calc_pi_mpi.hpp"
// -- calculate pi via simple integration  -- //
double calc_pi_mpi(int num_intervals)
{
    int num_tasks = 0;
    int task_id   = 0;
    
    MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    
    double h   = 1.0 / (double) num_intervals;
    double sum = 0.0;
    
    for(int i = task_id + 1; i <= num_intervals; i+= num_tasks) 
    {
        double x = h * ((double)i - 0.5);
        sum += (4.0 / (1.0 + x*x));
    }
    
    double pi_local = h * sum;
    double pi = 0;
    
    MPI_Allreduce(&pi_local,
                  &pi,
                  1,
                  MPI_DOUBLE,
                  MPI_SUM,
                  MPI_COMM_WORLD);

    
    return pi;
}
