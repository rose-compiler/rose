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
#include "Multigrid.H"
#include "CH_Timer.H"
#include "WriteRectMDArray.H"
#include <iostream>
#include <cstring>
#include <memory>
//#include <xmmintrin.h>

///
void LDWrite(LevelData<double>& a_phi)
{
  BoxLayout bl = a_phi.getBoxLayout();
  RectMDArray<double> phi0(bl.getDomain());
  // MDWrite("phi",phi0);
  for (BLIterator blit(bl); blit != blit.end(); ++blit)
    {
      a_phi[*blit].copyTo(phi0);
    }
   string foo = MDWrite(phi0);
};  
int main(int argc, char* argv[])
{
  int logDomainSize;
  int numLevels;
  // _MM_SET_EXCEPTION_MASK(_MM_GET_EXCEPTION_MASK() & ~_MM_MASK_INVALID);
  cout << "input log_2(domainSize)" << endl;
  cin >> logDomainSize;
  cout << "input number of multigrid levels " << endl;
  cin >> numLevels;
  
  BoxLayout bl(logDomainSize);
  int domainSize = Power(2,logDomainSize);
  // cout << "domainSize = " << domainSize << endl;
  // cout << bl << endl;
  double dx = 1./domainSize;
  LevelData<double> rho(bl),phi(bl,1),res(bl);
  rho.setVal(0.);
  phi.setVal(0.);
  for (BLIterator blit(bl); blit != blit.end(); ++blit)
    {
      RectMDArray<double >& rhoBx = rho[*blit];
      Box bx = bl[*blit];
      for (Point pt = bx.getLowCorner(); bx.notDone(pt); bx.increment(pt))
        {
          rhoBx[pt] = 1.;
          for (int idir = 0; idir < DIM ; idir++)
            {
              rhoBx[pt] = rhoBx[pt]*sin((M_PI*2*pt[idir])*dx+.5*dx);
            }
        }
    }
  Multigrid mg(bl,dx,numLevels);
  cout << "input max number of iterations, convergence tolerance " << endl;
  int maxiter;
  double tol;
  cin >> maxiter >> tol;
  double resnorm0 = mg.resnorm(phi,rho);
  cout << "initial residual = " << resnorm0 << endl;
  for (int iter = 0; iter < maxiter; iter++)
    {
      mg.vCycle(phi,rho);
      double resnorm = mg.resnorm(phi,rho);
      cout << "iter = " << iter << ", resnorm = " << resnorm << endl;
      if (resnorm < tol*resnorm0) break;
    }
  LevelData<double > resid(bl);
  phi.exchange();
  mg.residual(resid,phi,rho);
  LDWrite(phi);
}
///
