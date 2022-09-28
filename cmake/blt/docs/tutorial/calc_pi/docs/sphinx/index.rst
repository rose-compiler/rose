.. Calc Pi documentation master file, created by
   sphinx-quickstart on Sun Sep 10 21:47:20 2017.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to Calc Pi's documentation!
===================================


This is a tutorial example for BLT (https://github.com/llnl/blt) that creates
C++ libraries that calculate :math:`\pi` serially and in parallel using MPI 
and CUDA.

These libraries calculate :math:`\pi` by approximating the integral 
:math:`f(x) = \int_0^14/(1+x^2)` using numerical integration.
In the MPI implementation, the intervals are distributed across MPI tasks and 
a MPI_AllReduce calculates the final result. In the CUDA implementation, the
intervals are distributed across CUDA blocks and threads and a tree reduction
calculates the final result.


The method is adapted from:
https://www.mcs.anl.gov/research/projects/mpi/usingmpi/examples-usingmpi/simplempi/cpi_c.html


