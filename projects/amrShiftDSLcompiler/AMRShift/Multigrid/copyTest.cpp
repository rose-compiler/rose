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
#include "SPACE.H"
#include "CH_Timer.H"
#include "WriteRectMDArray.H"
#include <iostream>
#include <cstring>
#include <memory>

///
void initialize(LevelData<double >& a_phi,double a_dx)
{
  BoxLayout bl = a_phi.getBoxLayout();
  for (BLIterator blit(bl); blit != blit.end(); ++blit)
    {
      RectMDArray<double >& phi = a_phi[*blit];
      Box bx = bl[*blit];
      for (Point pt = bx.getLowCorner(); bx.notDone(pt); bx.increment(pt))
        {
          phi[pt] = 1.;
          for (int idir = 0; idir < DIM ; idir++)
            {
              phi[pt] = phi[pt]*sin((M_PI*2*pt[idir])*a_dx);
            }
        }
    }
};
void testlap(LevelData<double >& a_phi, double a_dx,char* a_str)
{
  double coef = 1./(a_dx*a_dx);
  Stencil<double> Laplacian(make_pair(getZeros(),-DIM*2*coef));
  BoxLayout bl = a_phi.getBoxLayout();
  
  for (int dir = 0; dir < DIM ; dir++)
    {
      Point edir = getUnitv(dir);
      Stencil<double> plus(make_pair(Shift(edir),coef));
      Stencil<double> minus(make_pair(Shift(edir*(-1)),coef));
      Laplacian = Laplacian + minus + plus;
    }
  
  a_phi.exchange();
  RectMDArray<double> LPhi00(bl.getDomain());
  for (BLIterator blit(bl); blit != blit.end(); ++blit)
    {
      LPhi00 |= Laplacian(a_phi[*blit],bl[*blit]);
    }
   MDWrite(a_str,LPhi00);
};
int main(int argc, char* argv[])
{
  int logDomainSize = MAXBLOCKSIZE_EXP + 2;
  BoxLayout bl(logDomainSize);
  int domainSize = Power(2,logDomainSize);
  int coarsen;
  cout << "input number of levels coarsened by " << endl;
  cin >> coarsen;
  cout << "domainSize = " << domainSize << endl;
  cout << bl << endl;
  double dx = (1.*Power(2,coarsen))/domainSize;
  BoxLayout blCoarseMultiple = bl.coarsen(coarsen);
  BoxLayout blCoarseSingle = bl.deepCoarsen(coarsen);
  LevelData<double> phiSingle(blCoarseSingle,1),phiMultiple(blCoarseMultiple,1);
  phiSingle.setVal(0.);
  phiMultiple.setVal(0.);
  initialize(phiMultiple,dx);
  phiMultiple.copyTo(phiSingle);
  // initialize(phiSingle,dx);
  testlap(phiSingle,dx,"phiSingle");
  testlap(phiMultiple,dx,"phiMultiple");
  
  //cout << bl << endl;

}
///
