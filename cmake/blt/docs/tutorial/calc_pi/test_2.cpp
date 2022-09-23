///////////////////////////////////////////////////////////////////////////////
//
// file: test_2.cpp
// 
// Simple example that calculates pi via simple integration.
//
// Adapted from:
// https://www.mcs.anl.gov/research/projects/mpi/usingmpi/examples-usingmpi/simplempi/cpi_c.html
///////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "mpi.h"

#include "calc_pi.hpp"
#include "calc_pi_mpi.hpp"

const double PI_REF = 3.141592653589793238462643;

// test serial lib
TEST(calc_pi_mpi, serial_example)
{
    ASSERT_NEAR(calc_pi(1000),PI_REF,1e-6);
}


// test mpi lib
TEST(calc_pi_mpi, mpi_example)
{
    ASSERT_NEAR(calc_pi_mpi(1000),PI_REF,1e-6);
}

// compare mpi and serial
TEST(calc_pi_mpi, compare_mpi_serial)
{
    ASSERT_NEAR(calc_pi(1000),calc_pi_mpi(1000),1e-12);
}

// _blt_tutorial_calcpi_test2_main_start
// main driver that allows using mpi w/ GoogleTest
int main(int argc, char * argv[])
{
    int result = 0;

    ::testing::InitGoogleTest(&argc, argv);

    MPI_Init(&argc, &argv);

    result = RUN_ALL_TESTS();

    MPI_Finalize();

    return result;
}
//_blt_tutorial_calcpi_test2_main_end
