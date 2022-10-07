
// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Note: This is a simple CUDA/MPI example
// The program generates an arbitrarily (very) large array of floats, it
// initializes it only locally on the master MPI rank (rank 0) to the value 998.
// It then splits it into chunks that the master MPI rank sends to the slave
// ranks. Each rank will run individually a simple CUDA kernel that will add 1
// to each element of their respective local array chunks and sends the
// modified array back to the master MPI rank, that will update the dataset.
// If all elements of the initial large array contain the value 999, the
// program has been completed successfully. If not, it will give you a
// detailed error.
//
// Technical notes:
// - This program uses hwloc for determining CPU and GPU affinities.
// - This program does NOT currently support MPI sends above INT_MAX elements.
// - Tested successfully on multiGPU/multiNode configurations.
// - MPI calls are in C99 style, although it is a C++ source code
//
// Code design notes:
// This program is a realistic example of what the skeleton of production
// codes using CUDA and MPI look like.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//-----------------------------------------------------------------------------
//
// file: test_cuda_mpi.cpp
//
//-----------------------------------------------------------------------------
#include <hwloc.h>
#include <hwloc/cudart.h>
#include <mpi.h>
#include <sched.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <cuda_runtime.h>

// This macro is to report the CUDA errors it can detect in a nice readable form.
#define cuChk(call)                                                               \
  {                                                                               \
    const cudaError_t cudaError = call;                                           \
    if (cudaError != cudaSuccess)                                                 \
    {                                                                             \
      printf("\nError %s: %d, ", __FILE__, __LINE__ );                            \
      printf("Code %d, reason %s\n", cudaError, cudaGetErrorString(cudaError));   \
      exit(1);                                                                    \
    }                                                                             \
  }                                                                               \

// Simple CUDA kernel adding 1 to each element of an existing array
__global__ void addOne(float* a, int bufsize)
{
  // The size_t type is needed to avoid memory address overflows with large arrays
  size_t index = blockIdx.x * blockDim.x + threadIdx.x;
  size_t stride = blockDim.x * gridDim.x;
  size_t sbufsize = (size_t) bufsize;
  for (size_t i = index; i < sbufsize; i+=stride)
  {
    a[i]+=1.f;
  }
}

int main(int argc, char **argv)
{
  hwloc_topology_t topology;
  hwloc_cpuset_t cpuset;
  // Initial values are set negative for early error detection.
  int cpu = -1; int ii = -1;
  int rank = -1, local_rank = -1, totalRanks = -1;
  int cuDevice = -1;

  int Npower = 0; // Stores power of 2 for the number of elements

  // Do not mess with the following lines unless you know what you are doing
  // --------------------------------------------------------------------------------------
  #ifdef OPEN_MPI
  rank       = atoi(std::getenv("OMPI_COMM_WORLD_RANK"));
  local_rank = atoi(std::getenv("OMPI_COMM_WORLD_LOCAL_RANK"));
  #endif

  #ifdef MPICH
  rank       = atoi(std::getenv("MV2_COMM_WORLD_RANK"));
  local_rank = atoi(std::getenv("MV2_COMM_WORLD_LOCAL_RANK"));
  #endif

  if (rank < 0 || local_rank < 0)
  {
    std::cout << "FATAL ERROR: MPI LIBRARY NOT SUPPORTED. EXITING ..." << std::endl;
    std::cout << "This program only supports "
              << "Open MPI, MPICH, MVAPICH2 and compatible" << std::endl;
    exit(-1);
  }

  // --------------------------------------------------------------------------------------

  if (rank == 0) { std::cout << "*** Simple CUDA+MPI smoke test for BLT ***" << std::endl << std::endl; }

  hwloc_topology_init(&topology); // Creates a hwloc topology
  // Gets the topology of the system, including attached devices
  hwloc_topology_set_flags(topology, HWLOC_TOPOLOGY_FLAG_WHOLE_IO);
  hwloc_topology_load(topology);

  int cuDevCount = 0;
  cuChk(cudaGetDeviceCount(&cuDevCount));

  // For each local rank (rank running on each node), select CUDA device number matching the rank number
  cuChk(cudaSetDevice(local_rank % cuDevCount));
  cuChk(cudaGetDevice(&cuDevice)); // Get properties of the currently selected GPU

  // Select cores in node and get the logical processors near the selected GPU
  cpuset = hwloc_bitmap_alloc();
  hwloc_cudart_get_device_cpuset(topology, cuDevice, cpuset);

  // Cycle through all logical processors in the cpuset.
  // * NOTE: This is a preprocessor MACRO. No terminating semicolon. *
  int match = 0;
  hwloc_bitmap_foreach_begin(ii, cpuset)

  if (match == local_rank)
  {
    cpu = ii;
    break;
  }
  // * NOTE: This is a preprocessor MACRO too, but needs a terminating semicolon. *
  hwloc_bitmap_foreach_end();

  hwloc_bitmap_t onecpu = hwloc_bitmap_alloc();
  hwloc_bitmap_set(onecpu, cpu);
  hwloc_set_cpubind(topology, onecpu, 0);

  hwloc_bitmap_free(onecpu);
  hwloc_bitmap_free(cpuset);
  hwloc_topology_destroy(topology);

  char hostname[MPI_MAX_PROCESSOR_NAME];
  gethostname(hostname, sizeof(hostname));
  cpu = sched_getcpu();


  // -------------------------------- MPI REGION BEGINS
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &totalRanks);
  char procname[MPI_MAX_PROCESSOR_NAME];
  int length;
  if (MPI_SUCCESS != MPI_Get_processor_name(procname, &length))
  {
    strcpy(procname, "unknown");
  }

  if (argc == 2)   // If we have one exact command line argument ...
  {
    Npower = atoi(argv[1]);
  }
  else             // else ignore everything
  {
    Npower = 10;   // Sets a default value in case no command line argument is specified
  }
  size_t N             = (size_t) 1 << Npower; // Allocates 2^Npower entries
  size_t Nbytes        = N * (size_t) sizeof(float);

  size_t cudaStreams = (size_t) totalRanks;
  size_t sbufsize = (N/cudaStreams) + (N % cudaStreams);

  std::cout << "MPI rank " << rank << " using GPU " << cuDevice << " and CPU " <<
            cpu << " on host " << hostname << std::endl;
  MPI_Barrier(MPI_COMM_WORLD);

  if ( rank == 0 && sbufsize > (size_t) INT_MAX )
  {
    printf("Bufsize requested: %lu\n", sbufsize);
    printf("Maximum bufsize currently supported: %d\n", INT_MAX);
    printf("Retry to run the program with more MPI ranks\n");
    printf("Exiting ...\n\n\n");
    exit(-1);
  }
  int bufsize = (int) sbufsize;

  // CUDA Kernel run parameters
  dim3 block(1024);
  dim3 grid((bufsize+block.x-1)/block.x);
  float *h_master;

  // Gets free and total memory of each GPU device
  size_t devFreeMem, devTotMem;
  cuChk( cudaMemGetInfo(&devFreeMem, &devTotMem) );

  std::cout << "Device " << cuDevice << " on host " << hostname << " has "
            << devFreeMem/(1 << 20) << " MB of free memory" << std::endl;
  MPI_Barrier(MPI_COMM_WORLD);

  // Fills the master array with synthetic data (an array of ones)
  if (rank == 0)
  {
    // This is some eye-candy to have dynamic units for memory
    size_t szkB = (size_t) 1 << 10;
    size_t szMB = (size_t) 1 << 20;
    size_t szGB = (size_t) 1 << 30;

    char NbytesScale[4][3] = {" B", "kB", "MB", "GB"};
    int scaleIdx = 3;
    size_t NbytesScaled = Nbytes/szGB;
    if (NbytesScaled == 0)
    {
      NbytesScaled = Nbytes/szMB;
      scaleIdx = 2;
      if (NbytesScaled == 0)
      {
        NbytesScaled = Nbytes/szkB;
        scaleIdx = 1;
        if (Nbytes/szkB == 0)
        {
          NbytesScaled = Nbytes;
          scaleIdx = 0;
        }
      }
    }
    std::cout << std::endl << "Elements in N: " << N << "\t Required memory: "
              << NbytesScaled << " " << NbytesScale[scaleIdx] << std::endl;
    h_master = (float *) malloc(Nbytes);
    for (size_t i = 0U; i < N; ++i)
    {
      h_master[i] = 998.0f;
      // Displays progress of filling the bigmem array as a percentage.
      // * This eye-candy slows the process down a bit, but for large memory
      // * allocations it will let you know if the program si stuck or is
      // * actually doing something.
      if (i % (1 << 24) == 0 || i > N - (1 << 2))
      {
        size_t iN = ((i+1)*100)/N;
        printf("Filling master array ... %lu%% completed.        \r", iN);
      }
    }
    std::cout << std::endl;
  }

  // This is needed to allow the master array to fill before MPI Communication can occur safely
  MPI_Barrier(MPI_COMM_WORLD);
  // Uncomment below as needed

  if (rank == 0 ) // Some runtime info
  {
    std::cout << "  INT_MAX = " << INT_MAX << std::endl;
    std::cout << "  bufsize = " << bufsize << std::endl;
    std::cout << "  bufByte = " << (size_t) (bufsize * sizeof(float)) << std::endl;
    std::cout << "blockSize = " << block.x << std::endl;
    std::cout << "numBlocks = " << grid.x  << std::endl;
  }


  // rank-local variables for storing chunks of the master array
  float *h_buf=NULL;
  float *d_buf=NULL;


  // No CUDA streams, no party!
  cudaStream_t cuStreams[cudaStreams];
  cuChk(cudaStreamCreate(&cuStreams[rank]));

  // This is needed for pinned memory access
  cuChk(cudaHostAlloc(&h_buf, bufsize * sizeof(float), cudaHostAllocPortable));
  cuChk(cudaMalloc(&d_buf, bufsize * sizeof(float)));

  cuChk(cudaMemGetInfo(&devFreeMem, &devTotMem));
  std::cout << "Device " << cuDevice << " on host " << hostname << " has "
            << devFreeMem/(1<<20) << " MB of free memory" << std::endl;

  MPI_Request sreq, rreq;

  if (rank == 0)
  {
    for (int j = 1; j < totalRanks; j++)
    {
      for (size_t i = 0; i < sbufsize; ++i)
      {
        size_t Nchunk = i + sbufsize * (size_t) j;
        if (Nchunk < N)
        {
          h_buf[i] = h_master[Nchunk];
        }
      }
      std::cout << "MPI_Isend from rank " << rank << " to rank " << j << " ... ";
      MPI_Isend(h_buf, bufsize, MPI_FLOAT, j, 0, MPI_COMM_WORLD, &sreq);
      MPI_Status mpiStatus;
      MPI_Wait(&sreq, &mpiStatus);
      std::cout << "Done!" << std::endl;
    }

    for (size_t i = 0; i < sbufsize; ++i)
    {
      h_buf[i] = h_master[i];
    }
    cuChk(cudaMemcpy(d_buf, h_buf, bufsize*sizeof(float), cudaMemcpyHostToDevice));
    std::cout << "Rank " << rank << ": launching CUDA kernels ... ";
    addOne<<<grid,block,0,cuStreams[rank]>>>(d_buf, bufsize);
    cudaError_t cuErr = cudaGetLastError();
    std::cout << cudaGetErrorString(cuErr) <<std::endl;
    cuChk(cudaStreamSynchronize(cuStreams[rank]));
    cuChk(cudaMemcpy(h_buf, d_buf, bufsize*sizeof(float), cudaMemcpyDeviceToHost));

    for (size_t i = 0; i < sbufsize; i++) // Sanity check
    {
      if (h_buf[i] != 999.0f )
      {
        if (i < 10)
        {
          printf("Rank %d: h_buf[%lu] = %10.5f\n", rank, i, h_buf[i]);
        }
        else
        {
          exit(-1);
        }
      }
      h_master[i] = h_buf[i];
    }
  }
  else
  {
    std::cout << "Rank " << rank << " MPI_Irecv from rank 0 ... ";
    MPI_Irecv(h_buf, bufsize, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &rreq);
    MPI_Status mpiStatus;
    MPI_Wait(&rreq, &mpiStatus);
    std::cout << "Done!" << std::endl;

    std::cout << "Rank " << rank << ": launching CUDA kernels ... ";
    cuChk(cudaMemcpy(d_buf, h_buf, bufsize * sizeof(float),cudaMemcpyHostToDevice));
    addOne<<<grid,block,0,cuStreams[rank]>>>(d_buf, bufsize);
    cudaError_t cuErr = cudaGetLastError();
    std::cout << cudaGetErrorString(cuErr) <<std::endl;
    cuChk(cudaStreamSynchronize(cuStreams[rank]));
    cuChk(cudaMemcpy(h_buf, d_buf, bufsize * sizeof(float),
                     cudaMemcpyDeviceToHost));
  }
  MPI_Barrier(MPI_COMM_WORLD);

  if (rank != 0)
  {
    int iErr = 10;
    for (size_t i = 0; i < (size_t ) bufsize; i++)
    {
      if (h_buf[i] != 999.0)
      {
        if (iErr > 0)
        {
          printf("Rank %d returns h_buf[%lu] = %10.5f\n", rank, i, h_buf[i]);
          iErr--;
        }
        else
        {
          exit(-1);
        }
      }
    }

    MPI_Request sreq;
    MPI_Status mpiStatus;
    std::cout << "Rank " << rank << " MPI_Isend to rank 0 ...";
    MPI_Isend(h_buf, bufsize, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &sreq);
    MPI_Wait(&sreq, &mpiStatus);
    std::cout << "Done!" << std::endl;
  }

  if (rank == 0)
  {
    for (int j = 1; j < totalRanks; j++)
    {
      MPI_Status mpiStatus;
      MPI_Irecv(h_buf, bufsize, MPI_FLOAT, j, 0, MPI_COMM_WORLD, &rreq);
      MPI_Wait(&rreq, &mpiStatus);
      for (int bi = 0; bi < bufsize; bi++)
      {
        size_t bufIdx = (size_t) bi + (size_t) j * (size_t) (bufsize);
        if (bufIdx < N)
        {
          h_master[bufIdx] = h_buf[bi];
        }
        if (bufIdx >= N) { break; }
      }
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0)
  {
    int passedBool = 1;
    int errLines = 10;
    std::cout << "Validating the result ... ";
    for (size_t i = 0; i < N; ++i)
    {
      if (h_master[i] != 999.0)
      {
        std::cout << "Something is wrong: h_master[" << i << "] = " << h_master[i]
                  << " instead of 999.0" << std::endl;
        passedBool = 0;
        errLines--;
        if (errLines == 0)
        {
          MPI_Finalize();
          exit(-1);
        }
      }
    }
    if (passedBool) { std::cout << "PASSED!" << std::endl; }
  }

  cuChk(cudaFree(d_buf));
  cuChk(cudaFreeHost(h_buf));
  if (rank == 0) { free(h_master); }
  MPI_Finalize();
  return 0;
}
