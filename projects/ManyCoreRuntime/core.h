
const int DIM = 3;

// Forward declaration so that the Core<T> class can reference the MulticoreArray<T> class.
// This is to support functions that require access to data in other cores (used reluctantly).
template <typename T> class MulticoreArray;


// Simplify using macro to support linearized indexing of array.
#define index1D(i)     (i)
#define index2D(i,j)   (((j)*coreArrayNeighborhoodSizes_2D[1][1][0])+(i))

// I think this should be in terms of the size for X and Y, not X, Y, and Z!
// #define index3D(i,j,k) (((k)*coreArrayNeighborhoodSizes_3D[1][1][1][2]*coreArrayNeighborhoodSizes_3D[1][1][1][1])+((j)*coreArrayNeighborhoodSizes_3D[1][1][1][0])+(i))
#define index3D(i,j,k) (((k)*coreArrayNeighborhoodSizes_3D[1][1][1][0]*coreArrayNeighborhoodSizes_3D[1][1][1][1])+((j)*coreArrayNeighborhoodSizes_3D[1][1][1][0])+(i))

// And we need another macro for the general case where the memory segment is a different size than coreArrayNeighborhoodSizes_2D[1][1][0] in the X (and Y axis)
// since the lenght of the data in each axis can be different along the same axis of the core array.  These macros take the length of the array in the requires
// axis to properly referne the element on the associated "other core".
#define otherCore_index2D(i,j,sizeX)   (((j)*sizeX)+(i))
#define otherCore_index3D(i,j,k,sizeX,sizeY) (((k)*sizeX*sizeY)+((j)*sizeX)+(i))


template <typename T>
class Core
   {
  // This data is used to express the operations on each core.

  // Notes:
  //    1) It is not clear if we should have the pointer type be a template parameter for the class.
  //       Doing so might make it difficult to do operations between arrays of different types.
  //    2) What dimensions of multidimensional core arrays should we support.
  //    3) How should multidimensional data be represented?  I currently explicitly represent 
  //       1D, 2D, and 3D data.

  // private:
     public:
          const MulticoreArray<T>* multicoreArray;

       // Pointer to the local data for each core.
          T* arraySectionPointer;

          typedef  int index_set_type[3];
          index_set_type coreArrayNeighborhood_1D[3];
          index_set_type coreArrayNeighborhood_2D[3][3];
          index_set_type coreArrayNeighborhood_3D[3][3][3];

       // Mapping from multi-dimensional core array to linear array of cores.
          int coreArrayNeighborhoodLinearized_1D[3];
          int coreArrayNeighborhoodLinearized_2D[3][3];
          int coreArrayNeighborhoodLinearized_3D[3][3][3];

       // Arrays up to 3D can be distributed over 1D, 2D and 3D arrays of cores (last index is data array dimension).
       // int coreArrayNeighborhoodSizes_1D[3][3];
       // int coreArrayNeighborhoodSizes_2D[3][3][3];
       // int coreArrayNeighborhoodSizes_3D[3][3][3][3];
          index_set_type coreArrayNeighborhoodSizes_1D[3];
          index_set_type coreArrayNeighborhoodSizes_2D[3][3];
          index_set_type coreArrayNeighborhoodSizes_3D[3][3][3];

       // Is this a core on the boundary of the multidimensional core array (for each axis).
       // 3D data array support and 3D core array support for this concept is not yet implemented.
          bool bounaryCore_1D[2];
          bool bounaryCore_2D[2][2];

       // Padding between array sections (allows us to avoid false sharing (of cache lines), 
       // or memory can be allocated using libnuma).  Padding is to be arrranged along each 
       // access to support alignment of the data on each core so that data starting each 
       // dimension is aligned on either cache or page boundaries.  Different cache lines 
       // sizes for L1-L3 make also be a reason for having difference sizes of passing for 
       // each dimension.
          int padding[3];

       // This is a table to support arbitrary (but restricted to each axis) distributions of data onto NUMA-based processors (algorithmic distrbutions don't require this table).
          pair<size_t,size_t> arraySectionRanges[DIM];

       // Storing the size of each section explicitly simplifies the table-based implementation (algorithmic distrbutions don't require this table).
          size_t arraySectionSize[DIM];

     public:

       // This constructor must initialize the reference (pointer) to the MulticoreArray<T> object.
          Core( const MulticoreArray<T>* input_multicoreArray );

       // int coreArrayElement(int p, int q, int r) const;

          inline T & operator()(const int & i, const int & j, const int & k);

       // Compute the index for the adjacent memory segment (these wrap at the boundary or the array abstraction).
       // This should be renamed to: "get_lower_memory_section_index_in_core_array(int,int)" and associated "upper" version.
          int get_left_section_index( int dim, int p ) const;
          int get_right_section_index( int dim, int p ) const;

          int get_arraySectionSize(int dim) const;

       // Initialization functions.
          void initializeCoreArrayNeighborhoods( int p );
          void initializeLinearizedCoreArrayNeighborhoods( int p );
          void initializeCoreArraySizeNeighborhoods( int p );
          void initializeCoreArrayBoundaryFlags( int p );

       // This should be renames to: "is_lower_processor_core_array_boundary(int,int)" and associated "upper" version.
          bool is_left_edge_section( int p , int previous_p ) const;
          bool is_right_edge_section( int p, int next_p) const;

         void relax( int core, MulticoreArray<T> & array,  MulticoreArray<T> & old_array );

         void relax_on_boundary( int core, MulticoreArray<T> & array,  MulticoreArray<T> & old_array );
   };


// We have to see the templated code.
#include "core.C"



