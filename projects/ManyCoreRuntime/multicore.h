
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
          const int numberOfCores;

       // Padding between array sections (allows us to avoid false sharing, or memory can be allocated using libnuma).
          const int padding;
       // const int interArrayPadding[DIM-1];

       // Sizes of multi-dimensional array of cores to define the distribution of the aray abstraction onto the many-core processor.
       // const int coreArray[DIM];
          int coreArraySize[DIM];

       // This is a table to support arbitrary (but restricted to each axis) distributions of data onto NUMA-based processors (algorithmic distrbutions don't require this table).
          vector<pair<size_t,size_t> > arraySectionRanges[DIM];

       // Storing the size of each section explicitly simplifies the table-based implementation (algorithmic distrbutions don't require this table).
          vector<size_t> arraySectionSizes[DIM];

       // Specifies either table-based or algorithm-based distribution of array into memory sections per core.
          bool useArraySectionRanges;

     public:
       // Support for pre-computed data for each core.
       // vector<Core<T> > coreArray;
          Core<T>** coreArray;

#if 0
       // Data the we need on a per-core basis (per memory section, usually one per 
       // core unless latency hiding is possible using multiple threads per core).

       // This is the index of adjacent array sections to the current array section "p"
          typedef  int index_set_type[3];
          vector<index_set_type> coreArrayNeighborhood_1D[3];
          vector<index_set_type> coreArrayNeighborhood_2D[3][3];
          vector<index_set_type> coreArrayNeighborhood_3D[3][3][3];

       // Mapping from multi-dimensional core array to linear array of cores.
          vector<int> coreArrayNeighborhoodLinearized_1D[3];
          vector<int> coreArrayNeighborhoodLinearized_2D[3][3];
          vector<int> coreArrayNeighborhoodLinearized_3D[3][3][3];

       // Arrays up to 3D can be distributed over 1D, 2D and 3D arrays of cores (last index is data array dimension).
       // int coreArrayNeighborhoodSizes_1D[3][3];
       // int coreArrayNeighborhoodSizes_2D[3][3][3];
       // int coreArrayNeighborhoodSizes_3D[3][3][3][3];
          vector<index_set_type> coreArrayNeighborhoodSizes_1D[3];
          vector<index_set_type> coreArrayNeighborhoodSizes_2D[3][3];
          vector<index_set_type> coreArrayNeighborhoodSizes_3D[3][3][3];

          vector<bool> bounaryCore_1D[2];
          vector<bool> bounaryCore_2D[2][2];
#endif

     public:
       // Constructor
          MulticoreArray(int arraySize_I, int arraySize_J, int arraySize_K, 
                         const int numberOfCores_X, const int numberOfCores_Y, const int numberOfCores_Z, 
                         const int padding, bool useTableBasedDistribution = false);

       // Destructor
         ~MulticoreArray();

       // Example of 2D relaxation operator.
          void relax2D( MulticoreArray<T> & array,  MulticoreArray<T> & old_array );
 
       // Example of 2D relaxation operator (for core boundaries).
          void relax2D_on_boundary( MulticoreArray<T> & array,  MulticoreArray<T> & old_array );

       // Return reference to the ith element of the array (make sure this is an inlined function for performance).
          inline const T & operator()(const int & i, const int & j, const int & k) const;
          inline T & operator()(const int & i, const int & j, const int & k);

       // Access to cores for 2D array of cores
       // inline Core<T> & operator()(const int & i, const int & j, const int & k);
          inline Core<T> & getCore(const int & i, const int & j, const int & k) const;

       // Encapsulation of logic to support algorithm for computing the size of the pth array section of an array over p cores.
         int algorithmicComputationOfSize(int dim, int p) const;

       // Setup table for table-based distribution of array sections over cores (algorithmic distrbutions don't require this step and are a NOP).
          void computeArraySectionDistribution();

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
          int get_coreArraySize(int dim) const;
          T** get_arraySectionPointers() const;
          const vector<size_t> & get_arraySectionSizes(int dim) const;
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
          void display( const string & label ) const;

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

     private:
       // Define the copy constructor and operator=() and make them private to avoid them being used (for now, while I debug).
          MulticoreArray(const MulticoreArray & X);
          MulticoreArray & operator=(const MulticoreArray & X);
   };


// We have to see the templated code.
#include "multicore.C"



