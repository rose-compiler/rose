/******************************************************************/
/* 09/17/2015 Manually modified by Pei-Hung Lin (lin32@llnl.gov)  */
/* A manual implementation for the shiftCalculus DSL              */
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
#define SIZE 511
#include "mpi.h"
#include <ctime>
#include "laplacian_lite_v3.h"

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


int master(int rank, int nprocs)
{
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
// source and destination data containers
  class RectMDArray< double  , 1 , 1 , 1 > Asrc(bxsrc);
  class RectMDArray< double  , 1 , 1 , 1 > Adest(bxdest);
// all the coefficients I need for this operation
  const double ident = 1.0;
// DQ (2/18/2015): I need the simpler version because the current constant folding does not operate on floating point values.
// const double C0    = -2.0 * DIM;
  const double C0 = -6.00000;
  initialize(Asrc);
  initialize(Adest);
#if debug
 cout <<" The source Box" << endl;
 Asrc.print();
 cout << endl;
#endif
// build the stencil, and the stencil operator
// Stencil<double> laplace(wt,shft);
  const std::array< Shift  , 3 > S = getShiftVec();
// This calls: template <class T> Stencil<T> operator*(T a_coef, Shift a_shift);
  class Stencil< double  > laplace = C0*((S)^(zero));
  for (int dir = 0; dir < 3; dir++) {
    const class Point thishft = getUnitv(dir);
// DQ (2/15/2015): Added operator+=() to support clearer updates of an existing object for compile-time analysis.
// laplace = laplace + ident*(S^thishft);
// laplace = laplace + ident*(S^(thishft*(-1)));
    laplace += ident*((S)^(thishft));
    laplace += ident*((S)^thishft * -1);
  }
// laplace.stencilDump();
// StencilOperator<double,double, double> op;
  double begin = MPI_Wtime();
  int lb2src = bxsrc .  getLowCorner ()[2];
  int k = 0;
  int ub2src = bxsrc .  getHighCorner ()[2];
  int arraySize_X_src = bxsrc .  size (0);
  int lb1src = bxsrc .  getLowCorner ()[1];
  int j = 0;
  int ub1src = bxsrc .  getHighCorner ()[1];
  int arraySize_Y_src = bxsrc .  size (1);
  int lb0src = bxsrc .  getLowCorner ()[0];
  int i = 0;
  int ub0src = bxsrc .  getHighCorner ()[0];
  int arraySize_Z_src = bxsrc .  size (2);
  int lb2dest = bxdest .  getLowCorner ()[2];
  int ub2dest = bxdest .  getHighCorner ()[2];
  int arraySize_X_dest = bxdest .  size (0);
  int lb1dest = bxdest .  getLowCorner ()[1];
  int ub1dest = bxdest .  getHighCorner ()[1];
  int arraySize_Y_dest = bxdest .  size (1);
  int lb0dest = bxdest .  getLowCorner ()[0];
  int ub0dest = bxdest .  getHighCorner ()[0];
  int arraySize_Z_dest = bxdest .  size (2);
  double *sourceDataPointer = Asrc . getPointer();
  double *destinationDataPointer = Adest . getPointer();


  for (k = lb2dest; k <= ub2dest; ++k) {
    for (j = lb1dest; j <= ub1dest; ++j) {
      for (i = lb0dest; i <= ub0dest; ++i) {
        destinationDataPointer[arraySize_X_dest * (arraySize_Y_dest * k + j) + i] = sourceDataPointer[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + -1) + (j-lb1src)) + (i-lb0src)] + sourceDataPointer[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + 1) + (j-lb1src)) + (i-lb0src)] + sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + -1)) + (i-lb0src)] + sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + 1)) + (i-lb0src)] + sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + -1)] + sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + 1)] + sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + (i-lb0src)] * -6.00000;
#if debug
        cout << i << " " << j << " " << k << " " << destinationDataPointer[arraySize_X_dest * (arraySize_Y_dest * k + j) + i] << "= " << sourceDataPointer[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + -1) + (j-lb1src)) + (i-lb0src)] << "+" << sourceDataPointer[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + 1) + (j-lb1src)) + (i-lb0src)] << "+" << sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + -1)) + (i-lb0src)] << "+" << sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + 1)) + (i-lb0src)] << "+" << sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + -1)] << "+" << sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + 1)] << "+" << sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + (i-lb0src)] << "* -6.00000" << endl;
#endif
      }
    }
  }
  double end = MPI_Wtime();
  double elapsed_secs = (end - begin);
  cout << "Exec. time for serial code: " << elapsed_secs << endl;
#if debug
 cout <<" The serail result" << endl;
 Adest.print();
 cout << endl;
#endif
// real MPI in the following
  class RectMDArray< double  , 1 , 1 , 1 > Adest_new(bxdest);
  initialize(Adest_new);
  double *destinationDataPointer_new = Adest_new . getPointer();
  begin = MPI_Wtime();

  MPI_Bcast( &lb0src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb1src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb2src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub0src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub1src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub2src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_X_src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_Y_src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_Z_src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb0dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb1dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb2dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub0dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub1dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub2dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_X_dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_Y_dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_Z_dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 

  double *distsrc;
  double *distdest;
  int nghost = 1;
  int distsrcsize = arraySize_Z_src / nprocs;
  int offsetsrc = rank * distsrcsize;
  if(rank < arraySize_Z_src%nprocs)
  {
    distsrcsize++;
  }
  if(rank >= arraySize_Z_src%nprocs)
    offsetsrc += arraySize_Z_src%nprocs;
  else
    offsetsrc += rank;
  int distdestsize = arraySize_Z_dest / nprocs;
  int offsetdest = rank * distdestsize;
  if(rank < arraySize_Z_dest%nprocs)
  {
    distdestsize++;
  }
  if(rank >= arraySize_Z_dest%nprocs)
    offsetdest += arraySize_Z_dest%nprocs;
  else
    offsetdest += rank;

  distsrc = (double*)calloc(1,sizeof(double)*(distdestsize+2*nghost)*arraySize_X_src*arraySize_Y_src);
  distdest = (double*)calloc(1,sizeof(double)*(distdestsize)*arraySize_X_dest*arraySize_Y_dest);

   // team leader send data to all members
   int copyOffset = offsetdest * arraySize_X_src*arraySize_Y_src;
   int copySize = (distdestsize + 2 * nghost) * arraySize_X_src*arraySize_Y_src;
   if(nprocs > 1)    
   { 
     int dest, send_tag=1;
     MPI_Request send_reqs[nprocs-1];
     MPI_Status send_status[nprocs-1];
     for(dest = 1; dest < nprocs; ++dest)
     {
       int sendSize = arraySize_Z_dest / nprocs;
       int sendOffset = dest * sendSize;
       if(dest < arraySize_Z_dest%nprocs)
       {
         sendSize++;
       }
       sendSize = (sendSize+2)*arraySize_X_src*arraySize_Y_src;
       if(dest >= arraySize_Z_dest%nprocs)
         sendOffset += arraySize_Z_dest%nprocs;
       else
         sendOffset += dest;
       sendOffset = sendOffset*arraySize_X_src*arraySize_Y_src;
#if debug
cout << "Master send size " << sendSize<< " from offset " << sendOffset << " "  << " to " << dest << endl;
#endif
        MPI_Isend(sourceDataPointer+sendOffset, sendSize,MPI_DOUBLE, dest, send_tag, MPI_COMM_WORLD,&send_reqs[dest-1]);
//     int idx;
//     for(idx = 0; idx < sendSize; ++idx)
//      printf("Source send to dest:%d result %d: %f\n",dest, idx, sourceDataPointer[offsetsrc+sendOffset+idx]); 
     }  
     MPI_Waitall(nprocs-1,send_reqs,send_status);
   }
   // local copy (this is optional, but simplier for transformation)
   memcpy(distsrc,sourceDataPointer+copyOffset,copySize*sizeof(double));

// computation
  for (k = lb2dest; k < distdestsize; ++k) {
    for (j = lb1dest; j <= ub1dest; ++j) {
      for (i = lb0dest; i <= ub0dest; ++i) {
        distdest[arraySize_X_dest * (arraySize_Y_dest * k + j) + i] = 
distsrc[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + -1) + (j-lb1src)) + (i-lb0src)] + \
distsrc[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + 1) + (j-lb1src)) + (i-lb0src)]  + \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + -1)) + (i-lb0src)] + \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + 1)) + (i-lb0src)] + \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + -1)] + \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + 1)] + 
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + (i-lb0src)] * -6.00000;
#if debug
        cout << "rank0 " <<  i << " " << j << " " << k << " " <<  distdest[arraySize_X_dest * (arraySize_Y_dest * k + j) + i] << "= " << \
distsrc[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + -1) + (j-lb1src)) + (i-lb0src)] << "+" << \
distsrc[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + 1) + (j-lb1src)) + (i-lb0src)] << "+" << \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + -1)) + (i-lb0src)] << "+" << \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + 1)) + (i-lb0src)] << "+" << \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + -1)] << "+" << \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + 1)] << "+" << \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + (i-lb0src)] << "* -6.00000" << endl;
#endif
      }
    }
  }

// team leader receives data to all members
    int src, recv_tag=1;
    MPI_Request recv_reqs[nprocs-1];
    MPI_Status recv_status[nprocs-1];
    for(src = 1; src < nprocs; ++src)
    {
      int recvSize = arraySize_Z_dest / nprocs;
      int recvOffset = src * recvSize;
      if(src < arraySize_Z_dest%nprocs)
      {
        recvSize++;
      }
      recvSize *= arraySize_X_dest*arraySize_Y_dest;
      if(src >= arraySize_Z_dest%nprocs)
        recvOffset += arraySize_Z_dest%nprocs;
      else
        recvOffset += src;
       recvOffset = recvOffset*arraySize_X_dest*arraySize_Y_dest;
       MPI_Irecv(destinationDataPointer_new+recvOffset, recvSize, MPI_DOUBLE, src, recv_tag, MPI_COMM_WORLD,&recv_reqs[src-1]);
    }  
    MPI_Waitall(nprocs-1,recv_reqs,recv_status);
    // local copy (this could be optional, but simplier for transformation)
    memcpy(destinationDataPointer_new+offsetdest,distdest,distdestsize*bxdest.size(0)*bxdest.size(1)*sizeof(double));

  end = MPI_Wtime();
  elapsed_secs = (end - begin);
  cout << "Exec. time for MPI code: " << elapsed_secs << endl;

#if debug
 cout <<" MPI result " << endl;
 Adest_new.print();
 cout << endl;
#endif
  assert(checksum(Adest, Adest_new)==0);
  return 0; 
}

int member(int rank, int nprocs)
{
  int lb0src, lb1src, lb2src, ub0src, ub1src, ub2src;
  int arraySize_X_src, arraySize_Y_src, arraySize_Z_src;
  int lb0dest, lb1dest, lb2dest, ub0dest, ub1dest, ub2dest;
  int arraySize_X_dest, arraySize_Y_dest, arraySize_Z_dest;
  int i,j,k;
  MPI_Bcast( &lb0src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb1src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb2src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub0src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub1src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub2src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_X_src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_Y_src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_Z_src, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb0dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb1dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb2dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub0dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub1dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub2dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_X_dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_Y_dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_Z_dest, 1, MPI_INT, 0, MPI_COMM_WORLD); 

  double *distsrc;
  double *distdest;
  int nghost = 1;
  int distdestsize = arraySize_Z_dest / nprocs;
  if(rank < arraySize_Z_dest%nprocs)
  {
    distdestsize++;
  }

  distsrc = (double*)calloc(1,sizeof(double)*(distdestsize+2*nghost)*arraySize_X_src*arraySize_Y_src);
  distdest = (double*)calloc(1,sizeof(double)*(distdestsize)*arraySize_X_dest*arraySize_Y_dest);

//cout << "Rank dest: " << rank << " " << offsetdest << " " << distdestsize << " " << arraySize_Z_dest << endl;
// receive data from master
  int src = 0, recv_tag=1;
  MPI_Request recv_reqs[1];
  MPI_Status recv_status[1];

  int recvSize = (distdestsize + 2 * nghost) * arraySize_X_src*arraySize_Y_src;
  // receiving data
  MPI_Irecv(distsrc, recvSize, MPI_DOUBLE, src, recv_tag, MPI_COMM_WORLD, &recv_reqs[0]);
//  MPI_Irecv(distdest, distsrcsize*bxsrc.size(0)*bxsrc.size(1), MPI_DOUBLE, src, tag, MPI_COMM_WORLD, &reqs[1]);
  MPI_Waitall(1,recv_reqs,recv_status);
//  int idx;
//  for(idx = 0; idx < distsrcsize*bxsrc.size(0)*bxsrc.size(1); ++idx)
//   printf("Receiver:%d has result %d: %f\n",rank, idx, distsrc[idx]); 

// computation
  for (k = lb2dest; k < distdestsize; ++k) {
    for (j = lb1dest; j <= ub1dest; ++j) {
      for (i = lb0dest; i <= ub0dest; ++i) {
        distdest[arraySize_X_dest * (arraySize_Y_dest * k + j) + i] = \
distsrc[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + -1) + (j-lb1src)) + (i-lb0src)] + \
distsrc[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + 1) + (j-lb1src)) + (i-lb0src)] + \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + -1)) + (i-lb0src)] + \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + 1)) + (i-lb0src)] + \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + -1)] + \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + 1)] + \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + (i-lb0src)] * -6.00000;
#if debug
       cout << "rank" << rank << " "  << i << " " << j << " " << k << " " << distdest[arraySize_X_dest * (arraySize_Y_dest * k + j) + i] << "= " << \
distsrc[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + -1) + (j-lb1src)) + (i-lb0src)] << "+" << \
distsrc[arraySize_X_src * (arraySize_Y_src * ((k-lb2src) + 1) + (j-lb1src)) + (i-lb0src)] << "+" << \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + -1)) + (i-lb0src)] << "+" << \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + ((j-lb1src) + 1)) + (i-lb0src)] << "+" << \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + -1)] << "+" << \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + ((i-lb0src) + 1)] << "+" << \
distsrc[arraySize_X_src * (arraySize_Y_src * (k-lb2src) + (j-lb1src)) + (i-lb0src)] << "* -6.00000" << endl;
#endif
      }
//cout << rank<<": end of j = " << j << endl;
    }
//cout << "end of k = " << k << endl;
  }


  int dest = 0, send_tag=1;
  MPI_Request send_reqs[1];
  MPI_Status send_status[1];
  // team members send back data
  MPI_Isend(distdest, distdestsize*arraySize_X_dest*arraySize_Y_dest, MPI_DOUBLE, dest, send_tag, MPI_COMM_WORLD, send_reqs);
//  int idx;
//  for(idx = 0; idx < distdestsize*arraySize_X_dest*arraySize_Y_dest; ++idx)
//   printf("rank %d send result %d: %f\n",rank, idx, distdest[idx]); 
  MPI_Waitall(1,send_reqs,send_status);
  return 0;
}

int main(int argc,char *argv[])
{
// MPI setup
  int rank, nprocs;
  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  if(rank == 0)
  {
    std::cout << "I am rank " << rank << " of " <<  nprocs << " processes, I am calling master subroutine" << std::endl;
    master(rank, nprocs);
  }
  else
  {
    std::cout << "I am rank " << rank << " of " <<  nprocs << " processes, I am calling member subroutine" << std::endl;
    member(rank, nprocs);
  }
  MPI_Finalize();
}
