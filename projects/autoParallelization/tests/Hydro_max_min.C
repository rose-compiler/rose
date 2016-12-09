// HydroC
class VectorXY
{
  public:
    VectorXY(){x=0.0; y=0.0;}
    VectorXY(double xx, double yy){x=xx; y=yy;}

    double x;
    double y;

};

class PolygonMeshXY 
{
  public:
    int nnodes;
    VectorXY * nodePos;

};
#if 1
class Hydro
{
  public:
    Hydro(int c, PolygonMeshXY m);
    VectorXY * bcVelocity; // velocity BC values
    int cycle;
    PolygonMeshXY & mesh;
};

Hydro::Hydro (int c, PolygonMeshXY m): cycle(c), mesh(m)
{
   double minX = 1e99;
   double maxX = -1e99;
   double minY = 1e99;
   double maxY = -1e99;
   for (int n = 0; n < mesh.nnodes; n++)
   {   
      VectorXY & pos = mesh.nodePos[n];
      if (pos.x < minX) minX = pos.x;
      if (pos.x > maxX) maxX = pos.x;
      if (pos.y < minY) minY = pos.y;
      if (pos.y > maxY) maxY = pos.y;
   } 
}

#endif
void foo()
{
}

