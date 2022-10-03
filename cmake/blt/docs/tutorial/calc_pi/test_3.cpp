///////////////////////////////////////////////////////////////////////////////
//
// file: test_3.cpp
// 
// Simple example that calculates pi via simple integration.
//
// Adapted from:
// https://www.mcs.anl.gov/research/projects/mpi/usingmpi/examples-usingmpi/simplempi/cpi_c.html
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "calc_pi.hpp"
#include "calc_pi_cuda.hpp"

const double PI_REF = 3.141592653589793238462643;

// test serial lib
TEST(calc_pi_cuda, serial_example)
{
    ASSERT_NEAR(calc_pi(1000),PI_REF,1e-6);
}


// test cuda lib
TEST(calc_pi_cuda, cuda_example)
{
    ASSERT_NEAR(calc_pi_cuda(1000),PI_REF,1e-6);
}

// compare serial and cuda
TEST(calc_pi_cuda, compare_serial_cuda)
{
    ASSERT_NEAR(calc_pi(1000),calc_pi_cuda(1000),1e-12);
}

