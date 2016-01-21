// #include "Multigrid.H"


typedef unsigned long size_t;

// #include <memory>
#include "simple_RectMDArray.H"
#include "simple_LevelData.H"
#include "simple_BLIterator.H"
// #include "CH_Timer.H"
#include "simple_Stencil.H"

/// Multigrid: solve a FV discretization of Poisson's equation on a union of rectangles.
class Multigrid
{
public:
  /// Default constructor.
  Multigrid();
  /// Not assuming strong construction, so we have constructor and define.
  Multigrid(
            const BoxLayout& a_bl,
            double a_dx,
            int a_level
            );
  void define(
         const BoxLayout& a_bl,
         double a_dx,
         int a_level
         );

#if 0
// DQ: Commented out unused code to make the AST smaller and easier to debug.

  ///  Destructor.
  ~Multigrid();
  /// Compute residual.
  void residual(
                LevelData<double >& a_res,
                LevelData<double >& a_phi,
                const LevelData<double >& a_rhs
                );
  /// Compute max norm of the residual.
  double resnorm(
                LevelData<double >& a_phi,
                const LevelData<double >& a_rhs
                );
#endif

  /// Point relaxation. Currently point Jacobi.
  void pointRelax(
                  LevelData<double >& a_phi,
                  const LevelData<double >& a_rhs,
                  int a_numIter
                  );
#if 0
// DQ: Commented out unused code to make the AST smaller and easier to debug.

  /// average down residual to next coarser level. Assumes exact averaging on control volumes.
  void avgDown(
               LevelData<double >& a_resc,
               const LevelData<double >& a_res
               );
  /// Piecewise constant interpolation of coarse correction to increment fine solution.
  void fineInterp(
                  LevelData<double >& a_phiFine,
                  const LevelData<double >& a_deltaCoarse
                  );
#endif

  /// Multigrid v cycle.
  void vCycle(
              LevelData<double >& a_phi,
              const LevelData<double >& a_rhs
              );
private:
#if 1
  // This are required as part of the API.
  LevelData<double > m_res;
  LevelData<double > m_rescReAligned;
  LevelData<double > m_deltaReAligned;
  LevelData<double > m_resc;
  LevelData<double > m_delta;
  BoxLayout m_bl;
  Multigrid* m_coarsePtr;
  double m_dx;
  double m_lambda;
  int m_level;
  int m_preRelax = 2*DIM;
  int m_postRelax = 2*DIM;
  int m_bottomRelax = 20;
  long long m_flops = 0;
  bool m_isAligned;
  Stencil<double> m_Laplacian;
#endif

};



// DQ: the file "CH_TIMER.H" does not exist, likely we mean to use: "CH_Timer.H"
// #include "CH_TIMER.H"
// #include "CH_Timer.H"

// void LDWrite(LevelData<double>& a_phi);

using namespace std;

Multigrid::Multigrid()
{
}

Multigrid::Multigrid(
                     const BoxLayout& a_bl,
                     double a_dx,
                     int a_level
                     )
{
#if 0
// Simplifying the AST for debugging.
  this->define(a_bl,a_dx,a_level);
#endif
}

#if 1
// DQ: comment out to make the AST smaller to support debugging.
void
Multigrid::define(
                  const BoxLayout& a_bl,
                  double a_dx,
                  int a_level
                  )
{
  m_bl = a_bl;
  //cout << m_bl << endl;
  m_level = a_level;
  m_res.define(m_bl);
  m_dx = a_dx;

#if 0
// Simplifying the AST for debugging.

  if (m_level > 0)
    {
      BoxLayout blCoarse = m_bl.coarsen();
      m_resc.define(blCoarse);
      m_delta.define(blCoarse,1);
      
      if (m_bl.canCoarsen()) 
        {
          blCoarse = m_bl.coarsen();
          m_isAligned = true;
          //cout << "coarse grid is aligned at level = " << m_level << endl;
          //cout << blCoarse << endl;
        }
      else
        {
          blCoarse = m_bl.deepCoarsen();
          // cout << "coarse grid is not aligned at level = " << m_level << endl;
          // cout << blCoarse << endl;
          m_isAligned = false;
          m_rescReAligned.define(blCoarse);
          m_deltaReAligned.define(blCoarse,1);
        } 
      m_coarsePtr = new Multigrid(blCoarse,2*m_dx,m_level-1);
    }
#endif

  // Define stencil: m_Laplacian, m_lambda.

  // Initial handling is simpler if I can see the declaration more directly (testing this, 
  // we can generalit it later to identify variables instead of variable declarations 
  // (which is better suited to the input test code)).
  // m_Laplacian = Stencil<double>(make_pair(getZeros(),-DIM*2/(m_dx*m_dx)));
  // Stencil<double> m_Laplacian = Stencil<double>(make_pair(getZeros(),-DIM*2/(m_dx*m_dx)));
  // m_Laplacian = Stencil<double>(make_pair(getZeros(),-DIM*2/(m_dx*m_dx)));

  // DQ: Make this a constant expression that we can evaluate (for now).
     const int const_m_dx = 8;
     m_Laplacian = Stencil<double>(make_pair(getZeros(),-DIM*2/(const_m_dx*const_m_dx)));

#if 1
  for (int dir = 0; dir < DIM ; dir++)
    {
   // This needs to be a constant.
      Point edir = getUnitv(dir);

   // DQ: This needs to evaluate to be a constant.
   // Stencil<double> plus(make_pair(Shift(edir),1./(m_dx*m_dx)));
   // Stencil<double> minus(make_pair(Shift(edir*(-1)),1./(m_dx*m_dx)));
      Stencil<double> plus(make_pair(Shift(edir),1/(const_m_dx*const_m_dx)));
      Stencil<double> minus(make_pair(Shift(edir*(-1)),1/(const_m_dx*const_m_dx)));

   // m_Laplacian = m_Laplacian + minus + plus;
      m_Laplacian += minus + plus;
    }
#endif

  // if (MYDEBUGFLAG) {m_Laplacian.stencilDump();};

#if 0
  // DSL error: This should only be set once, set below (and the rhs needs to be a constant expression as well which is a problem here).
  // m_lambda = m_dx*m_dx/(4.*DIM);

#if (DIM==3)
  m_Laplacian = Stencil<double >(make_pair(getZeros(),-64./(15.*m_dx*m_dx)));
  m_lambda = (15.*m_dx*m_dx)/128.;
  Box unitCube = Box(getOnes()*(-1),getOnes());
  for (Point pt = unitCube.getLowCorner();unitCube.notDone(pt);unitCube.increment(pt))
    {
      int ptot = abs(pt[0]) + abs(pt[1]) + abs(pt[2]);
      if (ptot == 1) {m_Laplacian = m_Laplacian + 
          Stencil<double >(make_pair(pt,7./(15.*m_dx*m_dx)));};
      if (ptot == 2) {m_Laplacian = m_Laplacian + 
          Stencil<double >(make_pair(pt,1./(10.*m_dx*m_dx)));};
      if (ptot == 3) {m_Laplacian = m_Laplacian + 
          Stencil<double >(make_pair(pt,1./(30.*m_dx*m_dx)));};
    }
#endif
#if (DIM==2)
  m_Laplacian = Stencil<double >(make_pair(getZeros(),-20./(6.*m_dx*m_dx)));
  m_lambda = (3.*m_dx*m_dx)/20.;
  Box unitCube = Box(getOnes()*(-1),getOnes());
  for (Point pt = unitCube.getLowCorner();unitCube.notDone(pt);unitCube.increment(pt))
    {
      int ptot = abs(pt[0]) + abs(pt[1]);
      if (ptot == 1) {m_Laplacian = m_Laplacian + 
          Stencil<double >(make_pair(pt,4./(6.*m_dx*m_dx)));};
      if (ptot == 2) {m_Laplacian = m_Laplacian + 
          Stencil<double >(make_pair(pt,1./(6.*m_dx*m_dx)));};
    }
#endif

#else
  // This is temporary code for debugging (note that the constant folding will not be done on 
  // floating point values by design, we might want to provide an optional behavior to support 
  // this for DSL expressions).
  // m_lambda = m_dx*m_dx/(4.*DIM);
  // m_lambda = (4.0 * DIM);
     m_lambda = (4 * DIM);
#endif
}
#endif

#if 0
// DQ: Commented out unused code to make the AST smaller and easier to debug.

Multigrid::~Multigrid()
{
  //  cout << "level = " << m_level << ", flopcount = " << m_flops << endl; 
  if (m_level != 0) { delete m_coarsePtr;}
}
#endif


#if 1
// DQ: Commented out unused code to make the AST smaller and easier to debug.

void
Multigrid::pointRelax(
                      LevelData<double >& a_phi,
                      const LevelData<double >& a_rhs,
                      int a_numIter
                      )
{
// CH_TIMERS("Multigrid::pointRelax");
// CH_TIMER("sec1",t1);
// CH_TIMER("sec2",t2);
// CH_TIMER("sec3",t3);
  BLIterator blit(m_bl);

// Use this reference to "m_lambda" in this DSL variable declaration to trigger it being 
// setup as a DSL variable (of integer type, not detected in the original accessment).
  Stencil<double> tmp(make_pair(getZeros(),m_lambda));

  for (int iter = 0; iter < a_numIter; iter++)
    {
   // DQ: What is this?
   // a_phi.exchange();
      for (blit.begin();blit != blit.end();++blit)
        {
       // Do we really want to allocate space each iteration? I assume so.
          RectMDArray<double > res(m_bl[*blit]);

       // CH_START(t1);

       // This is the apply function for the stencil operator.
          res |= m_Laplacian(a_phi[*blit],res.getBox());
       // CH_STOP(t1);
#if 1
       // CH_START(t2);
          res -= a_rhs[*blit];
       // CH_STOP(t2);
#endif
#if 1
       // CH_START(t3);
          a_phi[*blit] += tmp(res,res.getBox());
       // CH_STOP(t3);
#endif
        }
    }
}
#endif

#if 0
// DQ: comment out to make the AST smaller to support debugging.
void
Multigrid::vCycle(
                  LevelData<double >& a_phi,
                  const LevelData<double >& a_rhs
                  )
{
// CH_TIMERS("Multigrid::vCycle");
  pointRelax(a_phi,a_rhs,m_preRelax);

#if 0
  m_flops += m_preRelax*m_bl.getDomain().sizeOf()*Power(3,DIM)*2;
  if (m_level > 0)
    {
      residual(m_res,a_phi,a_rhs);
      m_flops += m_bl.getDomain().sizeOf()*Power(3,DIM)*2;
      avgDown(m_resc,m_res);
      if (!m_isAligned)
        {
          m_resc.copyTo(m_rescReAligned);
          m_deltaReAligned.setVal(0.);
          m_coarsePtr->vCycle(m_deltaReAligned,m_rescReAligned);
          m_deltaReAligned.copyTo(m_delta);
        }
      else
        {
          m_delta.setVal(0.);
          m_coarsePtr->vCycle(m_delta,m_resc);
        }
      fineInterp(a_phi,m_delta);
      pointRelax(a_phi,a_rhs,m_postRelax);
      m_flops += m_postRelax*m_bl.getDomain().sizeOf()*Power(3,DIM)*2;
    }
  else
    {
      pointRelax(a_phi,a_rhs,m_bottomRelax);
      m_flops += m_bottomRelax*m_bl.getDomain().sizeOf()*Power(3,DIM)*2;
    }
#endif
}
#endif

#if 0
// DQ: comment out to make the AST smaller to support debugging.
int main(int argc, char* argv[])
{
  int logDomainSize;
  int numLevels;
  // _MM_SET_EXCEPTION_MASK(_MM_GET_EXCEPTION_MASK() & ~_MM_MASK_INVALID);
  // cout << "input log_2(domainSize)" << endl;
  // cin >> logDomainSize;
  // cout << "input number of multigrid levels " << endl;
  // cin >> numLevels;
  logDomainSize = 8;
  numLevels     = 1;
  
  BoxLayout bl(logDomainSize);
  int domainSize = Power(2,logDomainSize);
  // cout << "domainSize = " << domainSize << endl;
  // cout << bl << endl;
  double dx = 1./domainSize;
  LevelData<double> rho(bl),phi(bl,1),res(bl);
  rho.setVal(0.);
  phi.setVal(0.);

#if 0
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
#endif

  Multigrid mg(bl,dx,numLevels);
// cout << "input max number of iterations, convergence tolerance " << endl;
// int maxiter;
// double tol;
// cin >> maxiter >> tol;
  int maxiter = 1;

// double resnorm0 = mg.resnorm(phi,rho);
// cout << "initial residual = " << resnorm0 << endl;

  for (int iter = 0; iter < maxiter; iter++)
    {
      mg.vCycle(phi,rho);

#if 0
   // Simplify code to focus on pointe relaxation first.
      double resnorm = mg.resnorm(phi,rho);
      cout << "iter = " << iter << ", resnorm = " << resnorm << endl;
      if (resnorm < tol*resnorm0) break;
#endif
    }

#if 0
  LevelData<double > resid(bl);
  phi.exchange();
  mg.residual(resid,phi,rho);
  LDWrite(phi);
#endif
}
#endif
