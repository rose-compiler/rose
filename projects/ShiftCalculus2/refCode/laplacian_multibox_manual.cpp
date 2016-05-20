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
void getError(LevelData<double, 1> & a_error,
              double               & a_maxError,
              const double         & a_dx)
{

  BoxLayout layout = a_error.getBoxLayout();

  LevelData<double, 1> phi(layout, s_nghost);
  LevelData<double, 1> lphcalc(layout, 0);
  LevelData<double, 1> lphexac(layout, 0);

  //  cout << "initializing phi to sum_dir(sin 2*pi*xdir)" << endl;
  initialize(phi,lphexac, a_dx);

  //set ghost cells of phi
  phi.exchange();
  //  dumpLevelRDA(&phi);
  Stencil<double> laplace;
  setStencil(laplace, a_dx);

  //apply stencil operator independently on each box
  a_maxError = 0;
  
  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {
      RectMDArray<double>& phiex =     phi[*blit];
      RectMDArray<double>& lphca = lphcalc[*blit];
      RectMDArray<double>& lphex = lphexac[*blit];
      RectMDArray<double>& error = a_error[*blit];

      //apply is set as an increment so need to set this to zero initially
      lphca.setVal(0.);

      Box bxdst=layout[*blit];
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
  double *sourceDataPointer = phiex . getPointer();
  double *destinationDataPointer = lphca . getPointer();
  for (k = iter_lb2; k < iter_ub2; ++k) {
    for (j = iter_lb1; j < iter_ub1; ++j) {
      for (i = iter_lb0; i < iter_ub0; ++i) {
        destinationDataPointer[arraySize_X * (arraySize_Y * (k - dest_lb2) + (j - dest_lb1)) + (i - dest_lb0)] = 
          (1.0*sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k - src_lb2 + -1) + (j - src_lb1)) + (i - src_lb0)] + 
           1.0*sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k - src_lb2 + 1) + (j - src_lb1)) + (i - src_lb0)] + 
           1.0*sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k - src_lb2) + (j - src_lb1 + -1)) + (i - src_lb0)] + 
           1.0*sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k - src_lb2) + (j - src_lb1 + 1)) + (i - src_lb0)] + 
           1.0*sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k - src_lb2) + (j - src_lb1)) + (i - src_lb0 + -1)] + 
           1.0*sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k - src_lb2) + (j - src_lb1)) + (i - src_lb0 + 1)] + 
          (-2.0*DIM)*sourceDataPointer[arraySize_X_src * (arraySize_Y_src * (k - src_lb2) + (j - src_lb1)) + (i - src_lb0)]) * (1.0/a_dx/a_dx);
//cout << destinationDataPointer[arraySize_X * (arraySize_Y * (k - dest_lb2) + (j - dest_lb1)) + (i - dest_lb0)] << " " << lphex.getPointer()[arraySize_X * (arraySize_Y * (k - dest_lb2) + (j - dest_lb1)) + (i - dest_lb0)] << endl;
      }
    }
  }        
      //error = lphicalc -lphiexac
      lphca.copyTo(error);
      //here err holds lphi calc
      double maxbox = forall_max(error, lphex, &errorF, bxdst);
cout << "maxbox= " << maxbox << endl;

      a_maxError = max(maxbox, a_maxError);
    }
}

int main(int argc, char* argv[])
{

  BoxLayout layoutCoar(s_numblockpower);
  BoxLayout layoutFine(s_numblockpower+1);
  double dxCoar = s_dx   ;
  double dxFine = s_dx/2.;
  LevelData<double, 1> errorFine(layoutFine, 0);
  LevelData<double, 1> errorCoar(layoutCoar, 0);

  double maxErrorFine, maxErrorCoar;
  getError(errorFine, maxErrorFine, dxFine);
  getError(errorCoar, maxErrorCoar, dxCoar);
  
  double order = log(maxErrorCoar/maxErrorFine)/log(2.);
  cout << "L_inf(Fine error) = " << maxErrorFine << endl;
  cout << "L_inf(Coar error) = " << maxErrorCoar << endl;

  cout << "order of laplacian operator = " <<  order << endl;

  return 0;
}
