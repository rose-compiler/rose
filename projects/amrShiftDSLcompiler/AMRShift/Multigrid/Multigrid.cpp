#include "Multigrid.H"
#include "CH_Timer.H"

void LDWrite(LevelData<double>& a_phi);
using namespace std;
Multigrid::Multigrid()
{
};
Multigrid::Multigrid(
                     const BoxLayout& a_bl,
                     double a_dx,
                     int a_level
                     )
{
  this->define(a_bl,a_dx,a_level);
};
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
};
Multigrid::~Multigrid()
{
  cout << "level = " << m_level << ", flopcount = " << m_flops << endl; 
  if (m_level != 0) { delete m_coarsePtr;}
};
void
Multigrid::residual(
                    LevelData<double >& a_res,
                    LevelData<double >& a_phi,
                    const LevelData<double >& a_rhs
                    )
{
   CH_TIMERS("Multigrid::residual");
  a_phi.exchange();
  for (BLIterator blit(m_bl); blit != blit.end(); ++blit)
    {
      Point pt = *blit;
      RectMDArray<double >& res = a_res[pt];
      res |= g_Laplacian(a_phi[*blit],res.getBox());
      res /= m_dx*m_dx;
      res *= -1.0;
      res += a_rhs[*blit];
    }
};
double
Multigrid::resnorm(
                    LevelData<double >& a_phi,
                    const LevelData<double >& a_rhs
                    )
{
  CH_TIMERS("Multigrid::resnorm");
  double retval = 0;
  a_phi.exchange();
  // BLIterator blit(m_bl);
  for (BLIterator blit(m_bl); blit != blit.end(); ++blit)
    // for (blit.begin();blit != blit.end();++blit)
    {
      Point pt = *blit;
      RectMDArray<double > res(a_rhs[pt].getBox());
      res.setVal(0.0);
      res |= g_Laplacian(a_phi[*blit],res.getBox());
      res /= m_dx*m_dx;
      res -= a_rhs[*blit];
      double resmax = abs_max(res,res.getBox());
      if (retval < resmax) retval = resmax;
      
    }
  return retval;
};
void
Multigrid::pointRelax(
                      LevelData<double >& a_phi,
                      const LevelData<double >& a_rhs,
                      int a_numIter
                      )
{
  CH_TIMERS("Multigrid::pointRelax");
  CH_TIMER("sec1",t1);
  CH_TIMER("sec2",t2);
  CH_TIMER("sec3",t3);
  BLIterator blit(m_bl);
  for (int iter = 0; iter < a_numIter; iter++)
    {
      a_phi.exchange();
      for (blit.begin();blit != blit.end();++blit)
        {
          RectMDArray<double > res(m_bl[*blit]); 
          RectMDArray<double > tmp(m_bl[*blit]); 
          CH_START(t1);
          res |= g_Laplacian(a_phi[*blit],res.getBox());
          res /= m_dx*m_dx;
          CH_STOP(t1);
          CH_START(t2);
          res -= a_rhs[*blit];
          CH_STOP(t2);
          CH_START(t3);
          tmp |= g_PointRelax(res,res.getBox());
          tmp *= m_dx*m_dx;
          a_phi[*blit] += tmp;
          CH_STOP(t3);
        }
    }
};
 void
   Multigrid::avgDown(
                      LevelData<double >& a_resc,
                      const LevelData<double >& a_res
                      )
{
  CH_TIMERS("Multigrid::avgDown");
  // Conservative, finite-volume averaging from coarse to fine.
  BLIterator blit(m_bl);

  for (blit.begin();blit !=blit.end();++blit)
    {
      RectMDArray<double >& resc = a_resc[*blit];
      resc.setVal(0.0);
      Box bxc = a_resc.getBoxLayout()[*blit];
      const RectMDArray<double >& res = a_res[*blit];

      for (int i = 0; i < (1 << DIM); i++)
      {
        resc += g_AvgDown[i](res,bxc);
      }
    }
};
void
Multigrid::fineInterp(
                   LevelData<double >& a_phi,
                   const LevelData<double >& a_delta
                   )
{
  CH_TIMERS("Multigrid::fineInterp");
  BLIterator blit(m_bl);
  BoxLayout blCoarse = a_delta.getBoxLayout();
  
  Box bxKernel(getZeros(),getOnes());

  for (blit.begin();blit != blit.end();++blit)
    {
      Box bx0 = blCoarse[*blit];
      const RectMDArray<double >& delta = a_delta[*blit];
      Box bxc = a_delta.getBoxLayout()[*blit];
      RectMDArray<double >& phi = a_phi[*blit];  
      for (Point ptsten = getZeros();
           bxKernel.notDone(ptsten);
           bxKernel.increment(ptsten))
        {
          g_FineInterp.setDestShift(ptsten);
          phi += g_FineInterp(delta,bx0);
        }
    }
};
void 
Multigrid::vCycle(
                  LevelData<double >& a_phi,
                  const LevelData<double >& a_rhs
                  )
{
  CH_TIMERS("Multigrid::vCycle");
  pointRelax(a_phi,a_rhs,m_preRelax);
  m_flops += m_preRelax*m_bl.getDomain().sizeOf()*Power(3,DIM)*2;
  if (m_level > 0)
    {
      residual(m_res,a_phi,a_rhs);
      m_flops += m_bl.getDomain().sizeOf()*Power(3,DIM)*2;
      avgDown(m_resc,m_res);
      if (!m_isAligned)
        {
          m_resc.copyTo(m_rescReAligned);
          m_deltaReAligned.setVal(0.0);
          m_coarsePtr->vCycle(m_deltaReAligned,m_rescReAligned);
          m_deltaReAligned.copyTo(m_delta);
        }
      else
        {
          m_delta.setVal(0.0);
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
};
