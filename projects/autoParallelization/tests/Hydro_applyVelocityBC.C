// Test case extracted from Hydro code
// using of array of classes or data structures
// Liao 7/6/2016
//

class VectorXY
{
  public:
    VectorXY(){x=0.0; y=0.0;}
    VectorXY(double xx, double yy){x=xx; y=yy;}
    double x;
    double y;

};


class Hydro
{
  public:
    VectorXY * bcVelocity; // velocity BC values
    int * bc3Nodes; // left boundary nodes
    int numBC3Nodes;

    void applyVelocityBC(VectorXY * u);
};
void Hydro::applyVelocityBC(VectorXY * u)
{
  if (bcVelocity[3].y != 0xdeadbeef)
  {  
    for (int in = 0; in < numBC3Nodes; in++)
      u[bc3Nodes[in]].y = bcVelocity[3].y;
  }

}
