// A version using runtime support
// Identical to laplacian_mpi_v4_wrt.cpp but with a more intuitive file name
//
/******************************************************************/
/* 09/17/2015 Manually modified by Pei-Hung Lin (lin32@llnl.gov)  */
/* A manual implementation for the shiftCalculus DSL              */
/* Have unified function for both master and members              */
/* Have separate functions for MPI Master and members             */
/* Overall data is allocated only at master                       */
/* Each member gets a distributed data from master for computation*/
/* Change SIZE for a different problem setup                      */
/* Turn on debug to print out array content                       */
/******************************************************************/


// This is the simple version of laplacian.cpp that builds 
// the smallest reasonable AST.  This simpler AST make it 
// easier to work with in the context of understanding 
// the use of the DSL and the interpretation of it's semantics 
// to implement the code generation for the DSL.
// This is the second simple test that applies a Laplacian operator
// with 2*DIM+1 size stencil with one ghost cell along each face
// of the data box. Thus the size of the destination box is 
//BLOCKSIZE^DIM and the size of the source box is (BLOCKSIZE+2)^DIM
//FOR these tests BLOCKSIZE=DOMAINSIZE because we are working with 
//a single box. BLOCKSIZE, DOMAINSIZE and DIM are set in the Makefile
//#define BLOCKSIZE 32
//#define DIM       3
// #define DIM       2
#define debug 0
#define SIZE 511  // SPACE.H defines BLOCKSIZE
#include "mpi.h"
#include "libxomp_mpi.h"
#include <ctime>
#include "laplacian_lite_v3.h"
#include <assert.h>

#define USE_XOMP_MPI 1
void initialize(class RectMDArray< double  , 1 , 1 , 1 > &patch)
{
  class Box D0 = patch . getBox();
  int k = 1;
  for (class Point pt(D0 .  getLowCorner ()); D0 .  notDone (pt); D0 .  increment (pt)) 
    patch[pt] = (k++);
}

int checksum(class RectMDArray< double  , 1 , 1 , 1 > &orig, class RectMDArray< double  , 1 , 1 , 1 > &result)
{
  if(orig.dataSize() != result.dataSize()) 
    return 1;
  class Box D0 = orig . getBox();
  for (class Point pt(D0 .  getLowCorner ()); D0 .  notDone (pt); D0 .  increment (pt))
  {
//    cout << orig[pt] << ":" << result[pt] << endl; 
    if(abs(orig[pt] - result[pt]) > .0001)
      return 1; 
  } 
  return 0; 
}

int main(int argc,char *argv[])
{
// MPI setup
// ------------------------------------------------------------------done 
  int rank, nprocs;

#if USE_XOMP_MPI
  xomp_init_mpi (&argc, &argv, &rank, &nprocs);
#else
  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
#endif

  const class Point zero = getZeros();
  const class Point ones = getOnes();
  const class Point negones = ones * -1;
  const class Point lo(zero);
// DQ (2/7/2015): Fixup for error yet to be fixed in ROSE (or fixed on alternative branch not yet merged).
// Point hi = getOnes()*(BLOCKSIZE-1);
  const int adjustedBlockSize = SIZE;
  const class Point hi = getOnes() * adjustedBlockSize;
//box low and high corners for destination
  const class Box bxdest(lo,hi);
// This will grow the box by one ghost
// along each face and become the box for
// the source box. 
  const class Box bxsrc = bxdest .  grow (1);

//MPI specific code: each array dimension boundary information : [lower: size]
// upper bounds can be omitted
// ---------------------------------------------------------------------
// no need to handle them specially, Can we reuse existing variables ??
  int lb0src, lb1src, lb2src; 
//  int ub0src, ub1src, ub2src;

  int arraySize_X_src, arraySize_Y_src, arraySize_Z_src;

  int lb0dest, lb1dest, lb2dest, ub0dest, ub1dest, ub2dest;
  int arraySize_X_dest, arraySize_Y_dest, arraySize_Z_dest;

  int i,j,k;

// MPI specific code: for mapped arrays, 
//          to direction: one copy
//          from direction: one copy also

  double *sourceDataPointer;
  double *destinationDataPointer;
  double *destinationDataPointer_ref;

// WHY changed to pointer types?? necessary to have global scope, not limited by if(rank0)
  class RectMDArray< double  , 1 , 1 , 1 > *Asrc;
  class RectMDArray< double  , 1 , 1 , 1 > *Adest;

  class RectMDArray< double  , 1 , 1 , 1 > *Adest_ref;
  
  double begin, end, elapsed_secs; 

  if(rank == 0)
  {

// Only master process allocate all data
    Asrc = new RectMDArray< double  , 1 , 1 , 1 >(bxsrc);
    Adest= new RectMDArray< double  , 1 , 1 , 1 >(bxdest);
    Adest_ref = new RectMDArray< double  , 1 , 1 , 1 >(bxdest);

    initialize(*Asrc);
    initialize(*Adest);
    initialize(*Adest_ref);

// these initialization can happen in every process, or not?
// No, seg fault if move into all processes
    destinationDataPointer= Adest-> getPointer();

    lb2src = bxsrc .  getLowCorner ()[2];
    lb1src = bxsrc .  getLowCorner ()[1];

    arraySize_X_src = bxsrc .  size (0);
    arraySize_Y_src = bxsrc .  size (1);
    arraySize_Z_src = bxsrc .  size (2);

    lb0src = bxsrc .  getLowCorner ()[0];

//    ub2src = bxsrc .  getHighCorner ()[2];
//    ub1src = bxsrc .  getHighCorner ()[1];
//    ub0src = bxsrc .  getHighCorner ()[0];


    lb2dest = bxdest .  getLowCorner ()[2];
    ub2dest = bxdest .  getHighCorner ()[2];
    arraySize_X_dest = bxdest .  size (0);
    assert (lb2dest == 0);

    lb1dest = bxdest .  getLowCorner ()[1];
    ub1dest = bxdest .  getHighCorner ()[1];
    arraySize_Y_dest = bxdest .  size (1);
    assert (lb1dest == 0);

    lb0dest = bxdest .  getLowCorner ()[0];
    ub0dest = bxdest .  getHighCorner ()[0];
    arraySize_Z_dest = bxdest .  size (2);
    assert (lb0dest == 0);
    //They are different! one with halo, the other does not.
    assert (arraySize_Z_src == arraySize_Z_dest +2 );
    assert (arraySize_X_src == arraySize_X_dest +2 );
    assert (arraySize_Y_src == arraySize_Y_dest +2 );

    sourceDataPointer = Asrc->getPointer();
    destinationDataPointer_ref = Adest_ref->getPointer();

    begin = MPI_Wtime();
//----------------------------------------------------------    
//   reference sequential version
    for (k = lb2dest; k <= ub2dest; ++k) {
      for (j = lb1dest; j <= ub1dest; ++j) {
        for (i = lb0dest; i <= ub0dest; ++i) {
          destinationDataPointer_ref[arraySize_X_dest * (arraySize_Y_dest * k + j) + i] = 
                 sourceDataPointer[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + -1) + (j-lb1src)) + (i-lb0src)] + 
                 sourceDataPointer[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + 1) + (j-lb1src)) + (i-lb0src)] + 
                 sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + -1)) + (i-lb0src)] + 
                 sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + 1)) + (i-lb0src)] + 
                 sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + -1)] + 
                 sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + 1)] + 
                 sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + (i-lb0src)] * -6.00000;

#if debug
        cout << i << " " << j << " " << k << " " << destinationDataPointer_ref[arraySize_X_dest * (arraySize_Y_dest * k + j) + i] << "= " << sourceDataPointer[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + -1) + (j-lb1src)) + (i-lb0src)] << "+" << sourceDataPointer[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + 1) + (j-lb1src)) + (i-lb0src)] << "+" << sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + -1)) + (i-lb0src)] << "+" << sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + 1)) + (i-lb0src)] << "+" << sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + -1)] << "+" << sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + 1)] << "+" << sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + (i-lb0src)] << "* -6.00000" << endl;
#endif
       }
     }
   }
   end = MPI_Wtime();
   elapsed_secs = (end - begin);
   cout << "Exec. time for serial code: " << elapsed_secs << endl;
#if debug
   cout <<" The serail result" << endl;
   Adest_ref->print();
   cout << endl;
#endif
//------------------end sequential reference execution----------------------------------------    
  }
//
// Be careful for a needed barrier!!  avoid race condition!!
  MPI_Barrier(MPI_COMM_WORLD);

  if(rank == 0)
  {
    std::cout << "I am rank " << rank << " of " <<  nprocs << " processes, I am calling master subroutine" << std::endl;
#if 0
    Adest= new RectMDArray< double  , 1 , 1 , 1 >(bxdest);
    initialize(*Adest);
    destinationDataPointer= Adest-> getPointer();
#endif
    begin = MPI_Wtime();
  }
  else
  {
    std::cout << "I am rank " << rank << " of " <<  nprocs << " processes, I am calling member subroutine" << std::endl;
  }


// -------------------TODO translate this 2015-10-26 ------------------
// Translate mapped scalar data, communicate to all processes
// Alternatively, each process calculate them??

  MPI_Bcast( &lb0src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb1src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb2src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
// upper bounds can be omitted, calculated by add lower+size
//  MPI_Bcast( &ub0src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
//  MPI_Bcast( &ub1src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
//  MPI_Bcast( &ub2src, 1, MPI_INT, 0, MPI_COMM_WORLD); 

  MPI_Bcast( &arraySize_X_src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_Y_src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_Z_src, 1, MPI_INT, 0, MPI_COMM_WORLD); 

  MPI_Bcast( &lb0dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb1dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb2dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 

// upper bounds can be omitted, calculated by add lower+size
  MPI_Bcast( &ub0dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub1dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub2dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 

  MPI_Bcast( &arraySize_X_dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_Y_dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_Z_dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 


// Translate mapped arrays
// calculate offset
// declare temp arrays for storing mapped arrays
  double *distsrc;
  double *distdest;
  int nghost = 1; // halo region size for source partitions

  // calculate strip size for the distributed dimension Z

  // a runtime function call for distributing data
  int offsetdest; // _mpi_destinationDataPointer_offset_2
  int distdestsize; // _mpi_destinationDataPointer_size_2
  // _xomp_nprocs , _xomp_rank, 
  // for source array,no need?
  // xomp_static_even_divide_start_size(lb2src,arraySize_Z_src,_xomp_nprocs,_xomp_rank,&_mpi_sourceDataPointer_offset_2,&_mpi_sourceDataPointer_size_2);
  // for destination array which needs being sent back

  // xomp_static_even_divide_start_size(lb2,arraySize_Z_dest,_xomp_nprocs,_xomp_rank,&_mpi_destinationDataPointer_offset_2,&_mpi_destinationDataPointer_size_2);
  xomp_static_even_divide_start_size (0, arraySize_Z_dest, nprocs, rank, & offsetdest, & distdestsize);

  //allocate source data partitions for each process
  // source partitions contain halo region elements
  // TODO a runtime allocation function call? too trivial
  //distsrc = (double*)calloc(1,sizeof(double)*(distdestsize+2*nghost)*arraySize_X_src*arraySize_Y_src);
  distsrc = (double*)calloc(sizeof(double), (distdestsize+2*nghost)*arraySize_X_src*arraySize_Y_src);

  // allocate destination data, no need to initialize
  //distdest = (double*)calloc(1,sizeof(double)*(distdestsize)*arraySize_X_dest*arraySize_Y_dest);
  distdest = (double*)calloc(sizeof(double),(distdestsize)*arraySize_X_dest*arraySize_Y_dest);

  // copy source data from source master to each process's local buffer
  // wrap into a runtime function call
  // Parameters: 
  //
  // INPUT
  //    sourceDataPointer: source array address
  //    source array dimension info:  size_x, size_y, size_z
  //    distribution policy: block on z dimension
  //    total processes: nprocs
  //    rank ID, 
  //    halo region size: 
  //    offsetdest: 0, + chunk , +2chunks, ... etc , can be calculated internally
  //    distdestsize: this can be calculated internally 
  //
  // OUTPUT: modified things
  //      distsrc: local copy of master/slave process
// extern void xomp_divide_scatter_array_to_all (void * sourceDataPointer, int element_type_id, int x_dim_size, int y_dim_size, int z_dim_size,
// int distributed_dimension_id, int halo_size, int rank_id, int process_count, int** distsrc);
  xomp_divide_scatter_array_to_all (sourceDataPointer, arraySize_X_src, arraySize_Y_src, arraySize_Z_src, 2, 1, rank, nprocs, &distsrc);
//       arraySize_X_dest, arraySize_Y_dest, arraySize_Z_dest);


// TODO: is a barrier needed here??

// computation, only k loop is distributed
// Also matches the k loop distribution of the nested loop
// another runtime function call for distributing loops 
 // int _lower, _upper; 
  //void xomp_static_even_divide_lower_upper (int start, int end, int thread_count, int thread_id, int* n_lower, int* n_upper);
  // This is wrong since the bounds are relative to the original global data buffer
  // What is needed is the bounds local to local portions. The lower bounds always start from 0. The size is the size of local portion!
  // So the bounds should match the size of a distributed array's local portion!!
  //xomp_static_even_divide_lower_upper (lb2dest, ub2dest, nprocs, rank, &_lower, &_upper);

  assert (lb2dest ==0);
  //assert (_lower ==0);
  //assert (_upper == distdestsize -1 );
  for (k = 0; k < distdestsize; ++k) { // is this correct??,  should lower starts with 0??
  // for (k = _lower; k <= _upper; ++k) { // bounds obtained from runtime calls, inclusive bounds
    for (j = lb1dest; j <= ub1dest; ++j) {
      for (i = lb0dest; i <= ub0dest; ++i) { // the loop's arrays are replaced with distributed version, the rest is intact
        distdest[arraySize_X_dest * (arraySize_Y_dest * k + j) + i] = \
             distsrc[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + -1) + (j-lb1src)) + (i-lb0src)] + \
             distsrc[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + 1) + (j-lb1src)) + (i-lb0src)] + \
             distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + -1)) + (i-lb0src)] + \
             distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + 1)) + (i-lb0src)] + \
             distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + -1)] + \
             distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + 1)] + \
             distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + (i-lb0src)] * -6.00000;
      }
//cout << rank<<": end of j = " << j << endl;
    }
//cout << "end of k = " << k << endl;
  }

// A runtime function to collect data
// void xomp_collect_scattered_array_from_all ()
// Parameters:
// destinationDataPointer // aggregated data on the master process
// distdest // distributed portions on each process
//  arraySize_X_dest
//  arraySize_Y_dest
//  arraySize_Z_dest
//  halo_size // not used for now
//
//  distribution_dimension_id
//  nprocs
//  rank_id
// Calculated one:
//    offsetdest, distdestsize
  // distribution ID is Z (2 of 0,1,2), halo region is size 0 in this case
  xomp_collect_scattered_array_from_all (distdest, arraySize_X_dest, arraySize_Y_dest, arraySize_Z_dest, 2, 0, rank, nprocs, &destinationDataPointer);

  //TODO Is this barrier necessary? Yes before the deletion later
  MPI_Barrier(MPI_COMM_WORLD);

  if(rank == 0)
  {
#if debug
   cout <<" MPI result " << endl;
   Adest->print();
   cout << endl;
#endif
  assert(checksum(*Adest_ref, *Adest)==0);
  delete Adest_ref;
  delete Asrc;
  delete Adest;
  }

  MPI_Finalize();
}
