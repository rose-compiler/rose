// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

//----------------------------------------------------------------------

/* 
   fortran_mpi_test manages MPI_Init & MPI_Finalize. Do not put MPI
   calls in this driver file -- put these calls in fortran_mpi_test,
   which should be defined in blt_fruit_mpi_test.f90.
 */
extern "C" int fortran_mpi_test();

int main()
{
  int result = 0;

  // finalized when exiting main scope
  result = fortran_mpi_test();

  return result;
}
