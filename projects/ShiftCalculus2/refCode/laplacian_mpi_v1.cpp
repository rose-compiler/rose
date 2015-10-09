#include <mpi.h>
#include "laplacian_lite_v3.h"

void initialize(class RectMDArray< double  , 1 , 1 , 1 > &patch)
{
  class Box D0 = patch . getBox();
  int k = 1;
  for (class Point pt(D0 .  getLowCorner ()); D0 .  notDone (pt); D0 .  increment (pt)) 
    patch[pt] = (k++);
}

int main(int argc,char *argv[])
{
// MPI setup
  int rank, nprocs;
  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  std::cout << "I am rank " << rank << " of " <<  nprocs << " processes" <<  std::endl;

  int lb0, lb1, lb2, ub0, ub1, ub2;
  int arraySize_X, arraySize_Y, arraySize_Z;
  int i,j,k;
  double *sourceDataPointer, *destinationDataPointer;
  const class Point zero = getZeros();
  const class Point ones = getOnes();
  const class Point negones = ones * -1;
  const class Point lo(zero);
// DQ (2/7/2015): Fixup for error yet to be fixed in ROSE (or fixed on alternative branch not yet merged).
// Point hi = getOnes()*(BLOCKSIZE-1);
  const int adjustedBlockSize = 3;
  const class Point hi = getOnes() * adjustedBlockSize;
//box low and high corners for destination
  const class Box bxdest(lo,hi);
// This will grow the box by one ghost
// along each face and become the box for
// the source box. 
  const class Box bxsrc = bxdest .  grow (1);

  if(rank == 0)
  {
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
  lb2 = bxdest .  getLowCorner ()[2];
  ub2 = bxdest .  getHighCorner ()[2];
  arraySize_X = bxdest .  size (0);
  lb1 = bxdest .  getLowCorner ()[1];
  ub1 = bxdest .  getHighCorner ()[1];
  arraySize_Y = bxdest .  size (1);
  lb0 = bxdest .  getLowCorner ()[0];
  ub0 = bxdest .  getHighCorner ()[0];
  arraySize_Z = bxdest .  size (2);
  sourceDataPointer = Asrc . getPointer();
  destinationDataPointer = Adest . getPointer();
  cout << "before: " << endl;
  Adest.print();
  }

  MPI_Bcast( &lb0, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb1, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &lb2, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub0, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub1, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &ub2, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_X, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_Y, 1, MPI_INT, 0, MPI_COMM_WORLD); 
  MPI_Bcast( &arraySize_Z, 1, MPI_INT, 0, MPI_COMM_WORLD); 


    // This appraoch will hook the distribution runtime
    double *distsrc;
    double *distdest;
    int distsrcsize = bxsrc.size (2) / nprocs;
    int offsetsrc = rank * distsrcsize;
    if(rank < bxsrc.size (2)%nprocs)
    {
      distsrcsize++;
    }
    if(rank >= bxsrc.size (2)%nprocs)
      offsetsrc += bxsrc.size (2)%nprocs;
    else
      offsetsrc += rank;
    int distdestsize = bxdest.size (2) / nprocs;
    int offsetdest = rank * distdestsize;
    if(rank < bxdest.size (2)%nprocs)
    {
      distdestsize++;
    }
    if(rank >= bxdest.size (2)%nprocs)
      offsetdest += bxdest.size (2)%nprocs;
    else
      offsetdest += rank;

    distsrc = (double*)calloc(1,sizeof(double)*(distsrcsize+1)*bxsrc.size(0)*bxsrc.size(1));
    distdest = (double*)calloc(1,sizeof(double)*(distdestsize+1)*bxdest.size(0)*bxdest.size(1));
    if(rank == 0)
    {
      // team leader send data to all members
      int dest, tag=1;
      MPI_Request reqs[nprocs-1];
      MPI_Status status[nprocs-1];
      for(dest = 1; dest < nprocs; ++dest)
      {
        int sendSize = bxsrc.size (2) / nprocs;
        int sendOffset = dest * sendSize;
        if(dest < bxsrc.size (2)%nprocs)
        {
          sendSize++;
        }
        sendSize *= bxsrc.size(0)*bxsrc.size(1);
        if(dest >= bxsrc.size (2)%nprocs)
          sendOffset += bxsrc.size (2)%nprocs;
        else
          sendOffset += dest;
         MPI_Isend(sourceDataPointer+sendOffset, sendSize,MPI_DOUBLE, dest, tag, MPI_COMM_WORLD,&reqs[dest-1]);
//         MPI_Isend(destinationDataPointer+sendOffset, sendSize,MPI_DOUBLE, dest, tag, MPI_COMM_WORLD,&reqs[1+dest*2]);
//      int idx;
//      for(idx = 0; idx < sendSize; ++idx)
//       printf("Source send to dest:%d result %d: %f\n",dest, idx, sourceDataPointer[offsetsrc+sendOffset+idx]); 
      }  
      // local copy (this is optional, but simplier for transformation)
      memcpy(distsrc,sourceDataPointer+offsetsrc,distsrcsize*bxsrc.size(0)*bxsrc.size(1)*sizeof(double));
//      memcpy(distdest,destinationDataPointer+offsetdest,distdestsize*sizeof(double));
      MPI_Waitall(nprocs-1,reqs,status);
    }
    else
    {
      int src = 0, tag=1;
      MPI_Request reqs[1];
      MPI_Status status[1];
      // receiving data
      MPI_Irecv(distsrc, distsrcsize*bxsrc.size(0)*bxsrc.size(1), MPI_DOUBLE, src, tag, MPI_COMM_WORLD, &reqs[0]);
//      MPI_Irecv(distdest, distsrcsize*bxsrc.size(0)*bxsrc.size(1), MPI_DOUBLE, src, tag, MPI_COMM_WORLD, &reqs[1]);
      MPI_Waitall(1,reqs,status);
//      int idx;
//      for(idx = 0; idx < distsrcsize*bxsrc.size(0)*bxsrc.size(1); ++idx)
//       printf("Receiver:%d has result %d: %f\n",rank, idx, distsrc[idx]); 
    }

  for (k = lb2; k < distdestsize; ++k) {
    for (j = lb1; j < ub1; ++j) {
      for (i = lb0; i < ub0; ++i) {
        distdest[arraySize_X * (arraySize_Y * k + j) + i] = distsrc[arraySize_X * (arraySize_Y * (k + -1) + j) + i] + distsrc[arraySize_X * (arraySize_Y * (k + 1) + j) + i] + distsrc[arraySize_X * (arraySize_Y * k + (j + -1)) + i] + distsrc[arraySize_X * (arraySize_Y * k + (j + 1)) + i] + distsrc[arraySize_X * (arraySize_Y * k + j) + (i + -1)] + distsrc[arraySize_X * (arraySize_Y * k + j) + (i + 1)] + distsrc[arraySize_X * (arraySize_Y * k + j) + i] * -6.00000;
//        cout << distdest[arraySize_X * (arraySize_Y * k + j) + i] << "= " << distsrc[arraySize_X * (arraySize_Y * (k + -1) + j) + i] << "+" << distsrc[arraySize_X * (arraySize_Y * (k + 1) + j) + i] << "+" << distsrc[arraySize_X * (arraySize_Y * k + (j + -1)) + i] << "+" << distsrc[arraySize_X * (arraySize_Y * k + (j + 1)) + i] << "+" << distsrc[arraySize_X * (arraySize_Y * k + j) + (i + -1)] << "+" << distsrc[arraySize_X * (arraySize_Y * k + j) + (i + 1)] << "+" << distsrc[arraySize_X * (arraySize_Y * k + j) + i] << "* -6.00000" << endl;
      }
//cout << rank<<": end of j = " << j << endl;
    }
//cout << "end of k = " << k << endl;
  }
    if(rank == 0)
    {
      // team leader receives data to all members
      int src, tag=1;
      MPI_Request reqs[2*nprocs];
      for(src = 1; src < nprocs; ++src)
      {
        MPI_Request reqs[nprocs-1];
        MPI_Status status[nprocs-1];
        int recvSize = bxdest.size (2) / nprocs;
        int recvOffset = src * recvSize;
        if(src < bxdest.size (2)%nprocs)
        {
          recvSize++;
        }
        recvSize *= bxdest.size(0)*bxdest.size(1);
        if(src >= bxdest.size (2)%nprocs)
          recvOffset += bxdest.size (2)%nprocs;
        else
          recvOffset += src;
         MPI_Irecv(destinationDataPointer+recvOffset, recvSize, MPI_DOUBLE, src, tag, MPI_COMM_WORLD,&reqs[src-1]);
         MPI_Waitall(nprocs-1,reqs,status);
      }  
      // local copy (this could be optional, but simplier for transformation)
      memcpy(destinationDataPointer+offsetdest,distdest,distdestsize*bxdest.size(0)*bxdest.size(1)*sizeof(double));
    }
    else
    {
      int src = 0, tag=1;
      MPI_Request reqs[1];
      MPI_Status status[1];
      // team members send back data
      MPI_Isend(distdest, distdestsize*bxdest.size(0)*bxdest.size(1), MPI_DOUBLE, src, tag, MPI_COMM_WORLD, reqs);
//      int idx;
//      for(idx = 0; idx < distdestsize*bxdest.size(0)*bxdest.size(1); ++idx)
//       printf("rank %d send result %d: %f\n",rank, idx, distdest[idx]); 
      MPI_Waitall(1,reqs,status);
    }

    MPI_Finalize();
}
