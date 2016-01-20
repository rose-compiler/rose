/**
      ___           ___           ___     
     /\  \         /\  \         /\  \    
    /::\  \       |::\  \       /::\  \   
   /:/\:\  \      |:|:\  \     /:/\:\__\  
  /:/ /::\  \   __|:|\:\  \   /:/ /:/  /  
 /:/_/:/\:\__\ /::::|_\:\__\ /:/_/:/__/___
 \:\/:/  \/__/ \:\~~\  \/__/ \:\/:::::/  /
  \::/__/       \:\  \        \::/~~/~~~~ 
   \:\  \        \:\  \        \:\~~\     
    \:\__\        \:\__\        \:\__\    
     \/__/         \/__/         \/__/    
      ___           ___                       ___           ___     
     /\  \         /\__\          ___        /\  \         /\  \    
    /::\  \       /:/  /         /\  \      /::\  \        \:\  \   
   /:/\ \  \     /:/__/          \:\  \    /:/\:\  \        \:\  \  
  _\:\~\ \  \   /::\  \ ___      /::\__\  /::\~\:\  \       /::\  \ 
 /\ \:\ \ \__\ /:/\:\  /\__\  __/:/\/__/ /:/\:\ \:\__\     /:/\:\__\
 \:\ \:\ \/__/ \/__\:\/:/  / /\/:/  /    \/__\:\ \/__/    /:/  \/__/
  \:\ \:\__\        \::/  /  \::/__/          \:\__\     /:/  /     
   \:\/:/  /        /:/  /    \:\__\           \/__/     \/__/      
    \::/  /        /:/  /      \/__/                                
     \/__/         \/__/                                            

   Please refer to Copyright.txt in the CODE directory. 
**/
/**
   When in doubt, write a convergence test.
   This is the multibox version of the Laplacian example.
   We compute the Laplacian of  a periodic function at 
   two different refinements and compute the order of convergence.
p**/

#include "mpi.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <memory>
#include <stdio.h>
#include <fstream>
#include "Shift.H"
#include "Stencil.H" 
#include "PowerItoI.H"
#include "RectMDArray.H"
#include "BoxLayout.H"
#include "LevelData.H"

void dumpRDA(const RectMDArray<double>* a_phi)
{
  cout << "=====" << endl;
  Box bx = a_phi->getBox();
  for (Point pt = bx.getLowCorner();bx.notDone(pt);bx.increment(pt))
    {
      double phival = a_phi->getConst(pt);
      pt.print2();
      cout  << " " << phival << endl;;
    }
}
void dumpLevelRDA(const LevelData<double, 1>* a_phi)
{
  const BoxLayout& layout = a_phi->getBoxLayout();

  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {
      RectMDArray<double> fab = (*a_phi)[*blit];
      dumpRDA(&fab);
    }
  cout << "*******" << endl;
}
void dontCallThis()
{
  dumpRDA(NULL);
  dumpLevelRDA(NULL);
}

double s_problo  =  0;
double s_probhi  =  1;
double s_domlen = s_probhi - s_problo;
int    s_numblockpower = 0;
int    s_numblocks = pow(2, s_numblockpower);
int    s_ncell   = s_numblocks*BLOCKSIZE;
double s_dx      = (s_domlen)/s_ncell;
double s_pi = 4.*atan(1.0);
const int    s_nghost = 1;

///
void initialize(RectMDArray<double>& a_phi,
                RectMDArray<double>& a_lphExac,
                const double       & a_dx,
                const Box          & a_bx
)
{
  for (Point pt = a_bx.getLowCorner(); a_bx.notDone(pt);  a_bx.increment(pt))
    {
      double phiVal = 0;
      double lphVal = 0;
      for(int idir = 0; idir < DIM; idir++)
        {
          double x = a_dx*(pt[idir] + 0.5);
          phiVal +=              sin(2.*s_pi*x/s_domlen);
          lphVal -= 4.*s_pi*s_pi*sin(2.*s_pi*x/s_domlen)/s_domlen/s_domlen;
        }
      a_phi    [pt] = phiVal;
      a_lphExac[pt] = lphVal;
    }
}
///
void initialize(LevelData<double, 1> & a_phi,
                LevelData<double, 1> & a_lphExac,
                const     double     & a_dx)
{
  const BoxLayout& layout = a_phi.getBoxLayout();

  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {
      Box bx = layout[*blit];
      initialize(a_phi    [*blit],
                 a_lphExac[*blit],
                 a_dx, bx
                 );
    }
}
///
void setStencil(Stencil<double>& a_laplace,
                const double   & a_dx)
{
  double C0=-2.0*DIM;
  Point zero=getZeros();
  Point ones=getOnes();
  Point negones=ones*(-1);
  double ident=1.0;
  array<Shift,DIM> S=getShiftVec();

  a_laplace = C0*(S^zero);
  for (int dir=0;dir<DIM;dir++)
    {
      Point thishft=getUnitv(dir);
      a_laplace = a_laplace + ident*(S^thishft);
      a_laplace = a_laplace + ident*(S^(thishft*(-1)));
    }
  //cout  << "stencil unscaled by dx = " << endl;
  //a_laplace.stencilDump();
  
  a_laplace *= (1.0/a_dx/a_dx);
}

///a_error comes in holding lph calc
double errorF(Tensor<double,1>& a_error, CTensor<double, 1>& a_exac)
{
  a_error(0) -= a_exac(0);
  return fabs(a_error(0));
}
///


double doWork(Box& bxdst, double* sourceDataPointer, double* destinationDataPointer, RectMDArray<double>& phiex, RectMDArray<double>& lphca, RectMDArray<double>& lphex, RectMDArray<double>& error, const double & a_dx)
{
      double a_maxError;
      //apply is set as an increment so need to set this to zero initially
//      lphca.setVal(0.);

//      Stencil<double>::apply(laplace, phiex, lphca, bxdst);
  const class Box sourceBoxRef = phiex . getBox();
  const class Box destinationBoxRef = lphca . getBox();
  int iter_lb2 = bxdst .  getLowCorner ()[2];
  int src_lb2 = sourceBoxRef .  getLowCorner ()[2];
  int dest_lb2 = destinationBoxRef .  getLowCorner ()[2];
  int k = 0;
  int iter_ub2 = bxdst .  getHighCorner ()[2];
  int src_ub2 = sourceBoxRef .  getHighCorner ()[2];
  int dest_ub2 = destinationBoxRef .  getHighCorner ()[2];
  int arraySize_X = bxdst .  size (0);
  int arraySize_X_src = sourceBoxRef .  size (0);
  int iter_lb1 = bxdst .  getLowCorner ()[1];
  int src_lb1 = sourceBoxRef .  getLowCorner ()[1];
  int dest_lb1 = destinationBoxRef .  getLowCorner ()[1];
  int j = 0;
  int iter_ub1 = bxdst .  getHighCorner ()[1];
  int src_ub1 = sourceBoxRef .  getHighCorner ()[1];
  int dest_ub1 = destinationBoxRef .  getHighCorner ()[1];
  int arraySize_Y = bxdst .  size (1);
  int arraySize_Y_src = sourceBoxRef .  size (1);
  int iter_lb0 = bxdst .  getLowCorner ()[0];
  int src_lb0 = sourceBoxRef .  getLowCorner ()[0];
  int dest_lb0 = destinationBoxRef .  getLowCorner ()[0];
  int i = 0;
  int iter_ub0 = bxdst .  getHighCorner ()[0];
  int src_ub0 = sourceBoxRef .  getHighCorner ()[0];
  int dest_ub0 = destinationBoxRef .  getHighCorner ()[0];
  int arraySize_Z = bxdst .  size (2);
  int arraySize_Z_src = sourceBoxRef .  size (2);
  for (k = iter_lb2; k < iter_ub2; ++k) {
    for (j = iter_lb1; j < iter_ub1; ++j) {
      for (i = iter_lb0; i < iter_ub0; ++i) {
        destinationDataPointer[arraySize_X * (arraySize_Y * (k - dest_lb2) + (j - dest_lb1)) + (i - dest_lb0)] = (1.0*sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k - src_lb2 + -1) + (j - src_lb1)) + (i - src_lb0)] + 1.0*sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k - src_lb2 + 1) + (j - src_lb1)) + (i - src_lb0)] + 1.0*sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k - src_lb2) + (j - src_lb1 + -1)) + (i - src_lb0)] + 1.0*sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k - src_lb2) + (j - src_lb1 + 1)) + (i - src_lb0)] + 1.0*sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k - src_lb2) + (j - src_lb1)) + (i - src_lb0 + -1)] + 1.0*sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k - src_lb2) + (j - src_lb1)) + (i - src_lb0 + 1)] + (-2.0*DIM)*sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k - src_lb2) + (j - src_lb1)) + (i - src_lb0)]) * (1.0/a_dx/a_dx);
//cout << destinationDataPointer[arraySize_X * (arraySize_Y * (k - dest_lb2) + (j - dest_lb1)) + (i - dest_lb0)] << " " << lphex.getPointer()[arraySize_X * (arraySize_Y * (k - dest_lb2) + (j - dest_lb1)) + (i - dest_lb0)] << endl;
      }
    }
  }        
      //error = lphicalc -lphiexac
      lphca.copyTo(error);
      //here err holds lphi calc
//      double maxbox = forall_max(error, lphex, &errorF, bxdst);
//cout << "maxbox= " << maxbox << endl;

  //    a_maxError = max(maxbox, a_maxError);
      return a_maxError;
}


void getError(LevelData<double, 1> & a_error,
              double               & a_maxError,
              const double         & a_dx)
{
  int rank, nprocs;
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
  std::cout << "I am rank " << rank << " of " <<  nprocs << " processes" <<  std::endl;

  BoxLayout* layout = new BoxLayout();
  int npatches;
  int nPatchPerProc, iStartIdx, iEndIdx;
  int patchID = -1;
  double local_maxError;

  LevelData<double, 1> *phi;
  LevelData<double, 1> *lphcalc;
  LevelData<double, 1> *lphexac;

  if(rank == 0)
  {
    *layout = a_error.getBoxLayout();
    npatches = layout->size();
    phi = new LevelData<double, 1>(*layout, s_nghost);
    lphcalc = new LevelData<double, 1>(*layout, 0);
    lphexac = new LevelData<double, 1>(*layout, 0);
    //  cout << "initializing phi to sum_dir(sin 2*pi*xdir)" << endl;
    initialize(*phi,*lphexac, a_dx);
    //set ghost cells of phi
    phi->exchange();
    if(npatches == 1)
     nPatchPerProc = 1;
    else
      nPatchPerProc = npatches/nprocs;
    iStartIdx = rank * nPatchPerProc;
    iEndIdx = (npatches < (rank+1)*nPatchPerProc) ? npatches-1 : ((rank+1)*nPatchPerProc-1);
    nPatchPerProc = iEndIdx - iStartIdx + 1; 
  }

// barrier to sync halo exchange

  if(nprocs > 1)
    MPI_Bcast( &npatches, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
 

  if(rank == 0)
  {
    Box bxdst;
    MPI_Request reqs[5];
    MPI_Status status[5];
    int iwait = 0;
    for(BLIterator blit(*layout); blit != blit.end(); ++blit)
    {
      bxdst= (*layout)[*blit];
      patchID++;
      bool mypatch = patchID >= iStartIdx && patchID <=iEndIdx;
      RectMDArray<double>& phiex =     (*phi)[patchID];
      RectMDArray<double>& lphca = (*lphcalc)[patchID];
      RectMDArray<double>& lphex = (*lphexac)[patchID];
      RectMDArray<double>& error = a_error[patchID];
      lphca.setVal(0.);
      if(mypatch)
      {
cout << "Rank 0 is working on patch " << patchID << endl;
        local_maxError = doWork(bxdst, phiex.getPointer(), lphca.getPointer(), phiex, lphca, lphex, error, a_dx);
      }
      else
      {
        int dest = patchID / nPatchPerProc;
        MPI_Isend(&phiex, sizeof(RectMDArray<double>),MPI_CHAR, dest, 0, MPI_COMM_WORLD,&reqs[0]);
cout << "master send out patch " <<  patchID<< " sourceDataPointer with size " << phiex.getBox().sizeOf() << endl;
        MPI_Isend(&lphca, sizeof(RectMDArray<double>),MPI_CHAR, dest, 1, MPI_COMM_WORLD,&reqs[1]);
cout << "master send out patch " <<  patchID<< " destinationDataPointer with size " << lphca.getBox().sizeOf() << endl;
        double *sourceDataPointer = phiex . getPointer();
        double *destinationDataPointer = lphca . getPointer();
        MPI_Isend(sourceDataPointer, phiex.getBox().sizeOf(),MPI_DOUBLE, dest, 2, MPI_COMM_WORLD,&reqs[2]);
        MPI_Isend(destinationDataPointer, lphca.getBox().sizeOf(),MPI_DOUBLE, dest, 3, MPI_COMM_WORLD,&reqs[3]);
        MPI_Isend(&bxdst, sizeof(Box),MPI_CHAR, dest, 4, MPI_COMM_WORLD,&reqs[4]);
cout << "master send out patch " <<  patchID<< " bxdst Box with size " << sizeof(Box)<< endl;
        iwait++;
        MPI_Waitall(4,reqs,status);
cout << "Rank 0 is sending patch " << patchID << " to rank " << dest << endl;
      }
    }
  }
  else
  {
    if(npatches == 1)
     nPatchPerProc = 0;
    else
      nPatchPerProc = npatches/nprocs;
    int src = 0;
    if(nPatchPerProc > 0)
    {
//      MPI_Request reqs[5];
      MPI_Status status[5];
      Box bxdst;
      RectMDArray<double>* phiex = new RectMDArray<double>();
      RectMDArray<double>* lphca = new RectMDArray<double>();
      RectMDArray<double>* lphex = new RectMDArray<double>();
      RectMDArray<double>* error = new RectMDArray<double>();
      int idx;
      for(idx = 0; idx < nPatchPerProc; idx++)      
      {
//        Box bxdst=((const BoxLayout&) layout)[*blit];
        patchID++;
        MPI_Recv(phiex, sizeof(RectMDArray<double>), MPI_CHAR, src, 0, MPI_COMM_WORLD, &status[0]);
        MPI_Recv(lphca, sizeof(RectMDArray<double>), MPI_CHAR, src, 1, MPI_COMM_WORLD, &status[1]);
        double *sourceDataPointer = (double*) malloc(sizeof(double)*phiex->getBox().sizeOf());
        MPI_Recv(sourceDataPointer, phiex->getBox().sizeOf(), MPI_DOUBLE, src, 2, MPI_COMM_WORLD, &status[2]);
cout << "Rank " << rank << " receive patch " << idx << " sourceDataPointer with size " << phiex->getBox().sizeOf() << endl;
        double *destinationDataPointer = (double*) malloc(sizeof(double)*lphca->getBox().sizeOf());
        MPI_Recv(destinationDataPointer, lphca->getBox().sizeOf(), MPI_DOUBLE, src, 3, MPI_COMM_WORLD, &status[3]);
cout << "Rank " << rank << " receive patch " << idx << " destinationDataPointer with size " << lphca->getBox().sizeOf() << endl;
        MPI_Recv(&bxdst, sizeof(Box), MPI_CHAR, src, 4, MPI_COMM_WORLD, &status[4]);
cout << "Rank " << rank << " receive patch " << idx << " bxdst with size " << sizeof(Box)<< endl;
cout << "Rank " << rank << " is working on patch " << npatches << endl;
        local_maxError = doWork(bxdst, sourceDataPointer, destinationDataPointer, *phiex, *lphca, *lphex, *error, a_dx);
      }
    }
  }
}

int main(int argc, char* argv[])
{

  int rank, nprocs;
  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &rank);
  MPI_Comm_size (MPI_COMM_WORLD, &nprocs);

  BoxLayout *layoutCoar;
  BoxLayout *layoutFine;
  LevelData<double, 1> *errorFine;
  LevelData<double, 1> *errorCoar;
  double dxCoar = s_dx   ;
  double dxFine = s_dx/2.;
  if(rank == 0)
  {
     layoutCoar = new BoxLayout(s_numblockpower);
     layoutFine = new BoxLayout(s_numblockpower+1);
     errorFine = new LevelData<double, 1> (*layoutFine, 0);
     errorCoar = new LevelData<double, 1> (*layoutCoar, 0);
  }

  double maxErrorFine, maxErrorCoar;

// start task 
  if(rank == 0)
    cout << "compute Fine" << endl;
  getError(*errorFine, maxErrorFine, dxFine);
  if(rank == 0)
    cout << "compute Coarse" << endl;
  getError(*errorCoar, maxErrorCoar, dxCoar);
  
  double order = log(maxErrorCoar/maxErrorFine)/log(2.);
  if(rank == 0)
  {
    cout << "L_inf(Fine error) = " << maxErrorFine << endl;
    cout << "L_inf(Coar error) = " << maxErrorCoar << endl;

    cout << "order of laplacian operator = " <<  order << endl;
  }

  MPI_Barrier (MPI_COMM_WORLD);

  // Trans x.03 
  MPI_Finalize();

  return 0;
}
