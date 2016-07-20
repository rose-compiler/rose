#include "CH_Timer.H"
#include "MultigridStencils.H"

Stencil<double> g_Laplacian;
Stencil<double> g_PointRelax;
Stencil<double> g_AvgDown[1 << DIM];
Stencil<double> g_FineInterp;

void buildMultigridStencils()
{
  double lambda;

  // Define stencils: g_Laplacian
  g_Laplacian = Stencil<double>(make_pair(getZeros(),-DIM*2));

  for (int dir = 0; dir < DIM ; dir++)
    {
      Point edir = getUnitv(dir);

      Stencil<double> plus(make_pair(Shift(edir),1.0));
      Stencil<double> minus(make_pair(Shift(edir*(-1)),1.0));

      g_Laplacian = g_Laplacian + minus + plus;
    }

#if (DIM==2)
  lambda = 3.0/20.0;

  g_Laplacian = Stencil<double >(make_pair(getZeros(),-20.0/6.0));
  Box unitCube = Box(getOnes()*(-1),getOnes());
  for (Point pt = unitCube.getLowCorner();
       unitCube.notDone(pt);
       unitCube.increment(pt))
    {
      int ptot = abs(pt[0]) + abs(pt[1]);

      if (ptot == 1)
      {
        g_Laplacian = g_Laplacian + Stencil<double >(make_pair(pt,4.0/6.0));
      }

      if (ptot == 2)
      {
        g_Laplacian = g_Laplacian + Stencil<double >(make_pair(pt,1.0/6.0));
      }
    }
#endif

#if (DIM==3)
  lambda = 15.0/128.0;

  g_Laplacian = Stencil<double >(make_pair(getZeros(),-64.0/15.0));

  Box unitCube = Box(getOnes()*(-1),getOnes());
  for (Point pt = unitCube.getLowCorner();
       unitCube.notDone(pt);
       unitCube.increment(pt))
    {
      int ptot = abs(pt[0]) + abs(pt[1]) + abs(pt[2]);

      if (ptot == 1)
      {
        g_Laplacian = g_Laplacian + Stencil<double >(make_pair(pt,7.0/15.0));
      }

      if (ptot == 2)
      {
        g_Laplacian = g_Laplacian + Stencil<double >(make_pair(pt,1.0/10.0));
      }

      if (ptot == 3)
      {
        g_Laplacian = g_Laplacian + Stencil<double >(make_pair(pt,1.0/30.0));
      }
    }
#endif

  g_PointRelax = Stencil<double>(make_pair(getZeros(),lambda));

  Point twoPoint = getOnes()*2;

  Box bxKernel(getZeros(),getOnes());
  double normalize = 1.0 / bxKernel.sizeOf();

  int count = 0;
  for (Point ptsten = getZeros();
       bxKernel.notDone(ptsten);
       bxKernel.increment(ptsten))
  {
    g_AvgDown[count] = Stencil<double>(make_pair(Shift(ptsten),normalize));
    g_AvgDown[count].setSrcRefratio(twoPoint);

    count++;
  }

  g_FineInterp = Stencil<double>(make_pair(Shift(getZeros()),1.0));
  g_FineInterp.setDestRefratio(twoPoint);
}
