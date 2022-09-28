// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Note: Parts of this are a CUDA Hello world example from NVIDIA:
// Obtained from here: https://developer.nvidia.com/cuda-education
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//-----------------------------------------------------------------------------
//
// file: blt_cuda_mpi_smoke.cpp
//
//-----------------------------------------------------------------------------

#include <mpi.h>
#include <iostream>
#include <stdio.h>

__device__ const char *STR = "HELLO WORLD!";
const char STR_LENGTH = 12;

__global__ void hello()
{
  printf("%c\n", STR[threadIdx.x % STR_LENGTH]);
}

int main(int argc, char** argv)
{
  // CUDA smoke test
  int num_threads = STR_LENGTH;
  int num_blocks = 1;
  hello<<<num_blocks,num_threads>>>();
  cudaDeviceSynchronize();

  // MPI smoke test
  // Initialize MPI and get rank and comm size
  MPI_Init(&argc, &argv);

  int commRank = -1;
  MPI_Comm_rank(MPI_COMM_WORLD, &commRank);
  int commSize = -1;
  MPI_Comm_size(MPI_COMM_WORLD, &commSize);

  // Do a basic mpi reduce to determine this actually works
  int globalValue = 0;
  int valueToSend = 1;
  MPI_Reduce(&valueToSend, &globalValue, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  // Finalize MPI
  MPI_Finalize();

  if (commRank == 0)
  {
    std::cout << "Count should be equal to rank size" << std::endl;
    std::cout << "Count = " << globalValue << ", Size = " << commSize << std::endl;

    if (globalValue != commSize)
    {
      return 1;
    }
  }

  return 0;
}


