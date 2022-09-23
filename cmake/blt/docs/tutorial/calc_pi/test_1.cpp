///////////////////////////////////////////////////////////////////////////////
//
// file: test_1.cpp
// 
// Simple example that calculates pi via simple integration.
//
// Adapted from:
// https://www.mcs.anl.gov/research/projects/mpi/usingmpi/examples-usingmpi/simplempi/cpi_c.html
///////////////////////////////////////////////////////////////////////////////

// _blt_tutorial_calpi_test1_start
#include <gtest/gtest.h>

#include "calc_pi.hpp"

TEST(calc_pi, serial_example)
{
    double PI_REF = 3.141592653589793238462643;
    ASSERT_NEAR(calc_pi(1000),PI_REF,1e-6);
}
// _blt_tutorial_calpi_test1_end
