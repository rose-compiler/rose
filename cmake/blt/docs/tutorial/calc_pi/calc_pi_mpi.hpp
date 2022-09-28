///////////////////////////////////////////////////////////////////////////////
///
/// \file calc_pi_mpi.hpp
/// 
/// Header file for calc_pi_mpi library example.
///
/// Adapted from:
///  https://www.mcs.anl.gov/research/projects/mpi/usingmpi/examples-usingmpi/simplempi/cpi_c.html
///////////////////////////////////////////////////////////////////////////////

#ifndef CALC_PI_MPI_HPP
#define CALC_PI_MPI_HPP
#include "calc_pi_mpi_exports.h"
///
/// \brief calculate pi using mpi 
///
///  Estimate pi by integrating f(x) = 4/(1+x^2) from 0 to 1 using 
///  numerical integration over a given number of intervals.
///
double CALC_PI_MPI_API calc_pi_mpi(int num_intervals);

#endif
