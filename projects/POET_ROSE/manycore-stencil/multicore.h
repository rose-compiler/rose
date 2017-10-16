#ifndef __MULTICORE__
#define __MULTICORE__
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <assert.h>
#include <omp.h>
#include <unistd.h>
#include <string> 
#include <vector>
#include <utility>
#if HAVE_NUMA_H
   #include<numa.h>
#endif
#include "core.h"

template <typename T>
class MulticoreArray
   {
  // This class supports the abstraction of a single array, internally it separates the array into pieces (one per core)
  // and allocates the memory in a way to be as local ass possible for each core on a NUMA architecture.

     private:
       // Size of the array to be spread over multiple cores.
          int arraySize[DIM];

       // Array of array pointers for each separate memory section (one per core; 3D array using STL).
       // T** arraySectionPointers;
       // vector<vector<vector<T*> > > arraySectionPointers;
          T** arraySectionPointers;

       // Number of cores over which to spread this array (or are defined by the processor architecture).
          int numberOfCores;

       // Padding between array sections (allows us to avoid false sharing, or memory can be allocated using libnuma).
          int padding;
       // const int interArrayPadding[DIM-1];

       // Size of halo region in each dimension.
          int haloSize[DIM];
       // Storing the size of each halo region in each individual Core
          std::vector<size_t> haloRegionsSizes[DIM];
       // Array of halo pointers for each separate memory section (up to 6 per core; 3D array using STL).
       //                         XYZ U/L
          T** haloSectionPointers[DIM][2];

       // Sizes of multi-dimensional array of cores to define the distribution of the aray abstraction onto the many-core processor.
          int coreArraySize[DIM];
#if HAVE_NUMA_H
          int NUMAcoreArraySize[DIM];
#endif
       // This is a table to support arbitrary (but restricted to each axis) distributions of data onto NUMA-based processors (algorithmic distrbutions don't require this table).
          std::vector<std::pair<size_t,size_t> > arraySectionRanges[DIM];

       // Storing the size of each section explicitly simplifies the table-based implementation (algorithmic distrbutions don't require this table).
          std::vector<size_t> arraySectionSizes[DIM];

       // Specifies either table-based or algorithm-based distribution of array into memory sections per core.
          bool useArraySectionRanges;

     public:
       // Support for pre-computed data for each core.
          Core<T>** coreArray;

          enum haloType {attached, detached, remote};
          haloType boundaryType;

     public:
       // Constructor
          MulticoreArray(int arraySize_I, int arraySize_J, int arraySize_K, 
                         const int numberOfCores_X, const int numberOfCores_Y, const int numberOfCores_Z, 
                         const int padding, bool useTableBasedDistribution = false);
       // Constructor
          MulticoreArray(int arraySize_I, int arraySize_J, int arraySize_K, 
                         const int numberOfCores_X, const int numberOfCores_Y, const int numberOfCores_Z,
                         const int numberOfHalos_X, const int numberOfHalos_Y, const int numberOfHalos_Z,
                         haloType haloDataType,
                         const int padding, bool useTableBasedDistribution = false);

       // Destructor
         ~MulticoreArray();

#if HAVE_NUMA_H
          void computeNUMACoreArray();
#endif

       // Example of 2D relaxation operator.
          void relax2D( MulticoreArray<T> & array,  MulticoreArray<T> & old_array );
 
       // Example of 2D relaxation operator (for core boundaries).
          void relax2D_on_boundary( MulticoreArray<T> & array,  MulticoreArray<T> & old_array );

       // Return reference to the ith element of the array (make sure this is an inlined function for performance).
          //inline const T & operator()(const int & i, const int & j, const int & k) const;
          //inline T & operator()(const int & i, const int & j, const int & k);
          const T & operator()(const int & i, const int & j, const int & k) const;
          T & operator()(const int & i, const int & j, const int & k);

       // Access to cores for 2D array of cores
       // inline Core<T> & operator()(const int & i, const int & j, const int & k);
          //inline Core<T> & getCore(const int & i, const int & j, const int & k) const;
          Core<T> & getCore(const int & i, const int & j, const int & k) const;

       // Encapsulation of logic to support algorithm for computing the size of the pth array section of an array over p cores.
         int algorithmicComputationOfSize(int dim, int p) const;

       // Setup table for table-based distribution of array sections over cores (algorithmic distrbutions don't require this step and are a NOP).
          void computeArraySectionDistribution();

       // Compute the actual halo region size
          void computeHaloRegionSize();

       // Debugging (informational) support
          size_t distanceBetweenMemoryAllocatedPerCore();

       // Allocate memory for each memory section (per core).
          void allocateMemorySectionsPerCore();

       // Support for indexing.
          void computeMemorySectionAndOffset(const int & i, const int & j, const int & k, int & core, int & element_index) const;

          int mappingFromMultidimentionalMemorySegmentArray(int i, int j, int k, int coreIndex) const;

       // Initialization of all allocated data (for debugging).
          void initializeDataPlusPadding(const T & x = 0);

       // Scalar initialization operator.
          MulticoreArray & operator=(const T & x);

       // Initialized the 3D array to a sequence of values (index*x)
          void sequenceFill3D(const T & x);

       // Initialized the array to a sequence of values (index*x)
          void sequenceFill(const T & x);

       // Equality operator.
          bool operator==(const MulticoreArray<T> & X) const;

       // assertion can all the local pieces add up to the exact array size.
          void verifyArraySize() const;

       // More verification functions for internal unit testing.
          void verifyMultidimensionalCoreArray() const;

       // Access functions.
          int get_arraySize(int dim) const;
          int numberOfArrayElements() const;
          int get_numberOfCores() const;
          int get_padding() const;
#if HAVE_NUMA_H
          int get_NUMAcoreArraySize(int dim) const;
#endif
          int get_coreArraySize(int dim) const;
          T** get_arraySectionPointers() const;
          T** get_haloSectionPointers(int dim, int boundary) const;
          const std::vector<std::pair<size_t,size_t> > & get_arraySectionRanges(int dim) const;
          const std::vector<size_t> & get_arraySectionSizes(int dim) const;
          const std::vector<size_t> & get_haloSectionSizes(int dim) const;
          int get_haloWidth(int dim) const;
          bool get_tableBasedDistribution() const;

          int coreArrayElement(int p, int q, int r) const;

          int memorySectionSize(int p, int q, int r) const;
          int memorySectionSize(int core) const;
          int indexIntoCoreArray(int dim, int core) const;

       // Compute the index for the adjacent memory segment (these wrap at the boundary or the array abstraction).
          int get_left_section_index( int dim, int p ) const;
          int get_right_section_index( int dim, int p ) const;

       // Detect if the pth memory segment is the left or right physical boundary in the array abstraction.
          bool is_left_edge_section( int p , int previous_p ) const;
          bool is_right_edge_section( int p, int next_p) const;

       // Display array elements (for debugging).
          void display( const std::string & label ) const;

       // Support for data initialization to aid debugging.
          void initializeDataWithCoreArrayIndex();

       // Initialize the boundary to a single value.
          void initializeBoundary( const T & x );

       // Initialization of core array neighborhoods to support stencil operations (indexing on memory allocated to individual cores).
          void initializeCoreArrayNeighborhoods();

       // Initialize the mapping of the core array neighborhoods to the linearized array of cores.
       // This provides a simple API used for the compiler generated code.
          void initializeLinearizedCoreArrayNeighborhoods();

       // This is the size information for each element in the core array neighborhoods (and for each axis direction of multidimensional arrays).
          void initializeCoreArraySizeNeighborhoods();

       // Mark where core array sections represent boundaries of the many-core distributed array (along each axis).
          void initializeCoreArrayBoundaryFlags();

       // Access function for core array of Core<T> pointers.
          Core<T>** get_coreArray() const;

       // Explicit update halo regions
          void haloExchange();

       // Check if there is any Halo region 
          bool isHaloExist();
          bool hasAttachedHalo();
          bool hasDetachedHalo();
       // Define the copy constructor and operator=() and make them private to avoid them being used (for now, while I debug).
          MulticoreArray(const MulticoreArray & X);
          MulticoreArray & operator=(const MulticoreArray & X);

     private:
   };

#endif
