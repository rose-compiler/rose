// HydroC
#include <omp.h> 

class VectorXY 
{
  

  public: inline VectorXY()
{
    (this) -> x = 0.0;
    (this) -> y = 0.0;
  }
  

  inline VectorXY(double xx,double yy)
{
    (this) -> x = xx;
    (this) -> y = yy;
  }
  double x;
  double y;
}
;

class PolygonMeshXY 
{
  public: int nnodes;
  class VectorXY *nodePos;
}
;
#if 1

class Hydro 
{
  public: Hydro(int c,class PolygonMeshXY m);
  class VectorXY *bcVelocity;
// velocity BC values
  int cycle;
  class PolygonMeshXY &mesh;
}
;

Hydro::Hydro(int c,class PolygonMeshXY m) : cycle(c), mesh(m)
{
  double minX = 1e99;
  double maxX = - 1e99;
  double minY = 1e99;
  double maxY = - 1e99;
  
#pragma omp parallel for reduction (min:minX,minY) reduction (max:maxX,maxY)
  for (int n = 0; n <= (this) -> mesh . nnodes - 1; n += 1) {
    class VectorXY &pos = (this) -> mesh . nodePos[n];
    if (pos . x < minX) 
      minX = pos . x;
    if (pos . x > maxX) 
      maxX = pos . x;
    if (pos . y < minY) 
      minY = pos . y;
    if (pos . y > maxY) 
      maxY = pos . y;
  }
}
#endif

void foo()
{
}
